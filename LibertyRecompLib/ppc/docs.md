 # 1. Boot-Critical Functions

This document is **source-level, conservative** documentation of boot-relevant behavior spanning:

- Host-side startup (`LibertyRecomp/*`), which loads the title image and transfers control into guest code.
- Recompiled guest-side PowerPC functions (`LibertyRecompLib/ppc/ppc_recomp.*.cpp`), which implement state machines, retry loops, and import calls.

Important limitation:

- From the `ppc_recomp.*.cpp` sources alone, it is **not provable** which *exact* guest functions execute during “boot” without a runtime trace (or at least the concrete guest entrypoint address plus call logs). This document therefore:
  - Documents the **host→guest entry plumbing** (provable from host sources).
  - Documents **boot-critical primitives and protocols** that appear in early/initialization-adjacent code paths (async helpers, mount/open primitives, XAM content/device flows).
  - Provides **cross-reference mechanics** to map runtime addresses (LR/PC) to named `sub_XXXXXXXX` functions.

## 1.1 Host boot chain (provable)

The host executable boots the guest by loading the title image and invoking the guest entrypoint as a mapped PPC function:

- `main.cpp`:
  - Calls `KiSystemStartup()`.
  - Calls `entry = LdrLoadModule(modulePath)`.
  - Calls `GuestThread::Start({ entry, 0, 0, 0 })`.

- `KiSystemStartup()` (`main.cpp`):
  - Performs host-side system setup (heap init, content registration/mounts, root path registration).
  - Sets up `game:`/`D:` and roots like `common:`, `platform:`, `audio:`.

- `LdrLoadModule(path)` (`main.cpp`):
  - Loads module bytes.
  - Calls `Image::ParseImage(...)`.
  - Copies `image.data` into guest memory at `g_memory.Translate(image.base)`.
  - Returns `image.entry_point` as the guest entry address.

- `GuestThread::Start(params)` (`cpu/guest_thread.cpp`):
  - Constructs a `GuestThreadContext` which initializes PPC registers (notably `r1` stack pointer and `r13`).
  - Calls `g_memory.FindFunction(params.function)(ctx.ppcContext, g_memory.base)`.

## 1.2 Host↔guest function dispatch and address→symbol mapping

The host maintains a lookup table from guest addresses to host function pointers:

- `Memory::Memory()` (`kernel/memory.cpp`):
  - Iterates `PPCFuncMappings[]` (from `LibertyRecompLib/ppc/ppc_func_mapping.cpp`).
  - Calls `InsertFunction(guest_addr, host_fn_ptr)` for each mapping.
  - This makes `g_memory.FindFunction(guest_addr)` return a callable `PPCFunc*`.

Cross-reference technique:

- If you have a guest address printed by host logs (e.g., `callerAddr = g_ppcContext->lr`), you can locate the corresponding symbol via `ppc_func_mapping.cpp` (search for the exact address). The mapping entries have the form:
  - `{ 0x82120000, sub_82120000 }, ...`

## 1.3 Boot-critical guest-side primitives (currently confirmed by static inspection)

The recompiled PPC functions are emitted as:

- `PPC_FUNC_IMPL(__imp__sub_XXXXXXXX)` (the recompiled implementation body)
- `PPC_WEAK_FUNC(sub_XXXXXXXX) { __imp__sub_XXXXXXXX(ctx, base); }` (a wrapper)

The following guest functions are repeatedly used as boot-critical *primitives* in the inspected regions:

| Function | Kind | Inputs (observed) | Outputs / side effects (observed) | Notes |
|---|---|---|---|---|
| `sub_829A1F00` | async driver / wait helper | Takes a handle-like value in `r3` in some call sites; other args via regs/stack | Returns `0`/nonzero; may call `__imp__NtWaitForSingleObjectEx` when a host call returns `259` | Contains explicit wait barrier (see §3) |
| `sub_829A2590` | operation start / open-like helper | Callers pass pointers/flags; often called with a path buffer base like `ctx+64` | Returns `-1` or non-`-1` value stored by callers into `ctx+96` | Exact semantics unknown (see §7) |
| `sub_829A1A50` | async-status helper | Callers pass `obj+8`, output ptr, and `0` | Returns `996` / `0` / other; `996` is treated as “no progress” | File contains `// PATCHED` logic that forces success (see §5) |
| `sub_829A1CA0` | wrapper around XAM close | Forwards to `__imp__XamContentClose` | Return compared against `997` | Cooperative pending semantics |
| `sub_829A1958` | close-handle helper | Takes a handle value | Close/cleanup | Used in cleanup paths |
| `sub_829A1EF8` | poll/yield-like helper | No obvious args | Returns a value sometimes compared against `997` | Not provably a yield; appears to read globals |
| `sub_829AAD20` | async-read wrapper | Takes a handle-like value loaded from `*(u32*)(obj+44)`; size compared against `*(u32*)(stack+84)` | Calls `__imp__NtReadFile`; if return equals `259`, calls `__imp__NtWaitForSingleObjectEx` then uses `stack+80` | Demonstrates the `259` → wait pattern around file I/O |
| `sub_829AADB8` | async-write wrapper | Takes a handle-like value loaded from `*(u32*)(obj+44)`; size compared against `*(u32*)(stack+84)` | Calls `__imp__NtWriteFile`; if return equals `259`, calls `__imp__NtWaitForSingleObjectEx` then uses `stack+80` | Demonstrates the `259` → wait pattern around file I/O |
| `sub_829A9738` | wait-with-retry helper | Takes an object handle in `r3` and a small control value in `r5` (stored to `r28`) | Calls `__imp__NtWaitForSingleObjectEx`; if return equals `257` and a flag is set, loops and waits again | Adds an additional distinguished wait result (`257`) used as a retry trigger |
| `sub_82169400` | event/wait loop | Uses globals under `-31981` base and waits on an event-like object at a fixed global address | Calls `__imp__KeWaitForSingleObject` in a loop; loop exit depends on a bit computed from `*(u32*)(global+300)` | Tight in-function loop that can spin if the global condition never changes |
| `sub_821694C8` | wait loop (variant) | Similar global/event pattern as `sub_82169400` | Calls `__imp__KeWaitForSingleObject` and loops while `*(u32*)(global+300) != 0` | Another tight wait loop gated by global memory |
| `sub_829DDC90` | wait loop (GPU init-adjacent) | Waits on an object at `r28+32` and compares wait result | Calls `__imp__KeWaitForSingleObject`; if return equals `258`, loops back | Matches host-side special-casing for caller `0x829DDD48` |
| `sub_829AAE50` | wait/dispatch loop | Waits on an object at `obj+16` then may call an indirect function pointer from a table | Calls `__imp__KeWaitForSingleObject`, then (on another path) executes `PPC_CALL_INDIRECT_FUNC` via a loaded `ctr` | Suggests a queue/dispatcher loop; exact target semantics unknown |

The following magic values are used as protocol signals in the inspected code:

- **`996`**: callers treat as “do not advance; return 0/no progress”.
- **`997`**: callers treat as “pending; retry / store pending state”.
- **`258`**: occurs inside `sub_829A1A50` and is mapped to `996`.
- **`259`**: triggers explicit wait path inside `sub_829A1F00`.
- **`257`**: used as a distinguished result in `sub_829A9738` that triggers retrying `__imp__NtWaitForSingleObjectEx`.

These values resemble Win32 constants, but this document **does not assume** that; meanings are inferred only from local branching.

## 1.4 Function-level dossiers (boot-adjacent; selected by import usage scan)

The functions below were selected because they directly invoke boot-critical host imports (filesystem open/read, waits, XAM task scheduling, etc.). This still does **not** prove they execute during boot, but they are high-value candidates.

### 1.4.1 `__imp__sub_829A2D98` (volume/open verification helper)

- **Source**:
  - `ppc_recomp.155.cpp`

- **Inputs (observed)**:
  - `r3`: treated as a pointer-sized value copied into `stack+100` (stored as `stw r11,100(r1)`).
  - `r4`: copied into `r30` and later compared against a computed value derived from volume info.

- **Outputs (observed)**:
  - Returns `r31`.
  - On error paths, returns a negative NTSTATUS-like value constructed as `(-16384<<16) | 335`.

- **Observed behavior**:
  - Calls `__imp__NtOpenFile` using stack structs located at `stack+80`, `stack+88`, `stack+96`, `stack+104`.
  - If open succeeds, calls `__imp__NtQueryVolumeInformationFile`, then closes the handle via `__imp__NtClose`.
  - After query, computes `r11 = *(u32*)(stack+128) * *(u32*)(stack+132)` and compares it against the input `r30`.
    - If equal, it returns without forcing the error code.
    - If not equal, it forces `r31 = (-16384<<16) | 335`.

- **Imports called (provable)**:
  - `__imp__NtOpenFile`
  - `__imp__NtQueryVolumeInformationFile`
  - `__imp__NtClose`

- **Async / retry**:
  - No internal loop; synchronous open→query→close sequence.

### 1.4.2 `__imp__sub_829A2AE0` (filesystem start/open + volume sizing + allocation)

- **Source**:
  - `ppc_recomp.155.cpp`

- **Inputs (observed)**:
  - `r3` saved as `r30` and later passed to a helper (`sub_8298F330`) and stored into memory.
  - `r4` saved as `r29` and later read at `*(u32*)(r29+4)`.
  - `r5` saved as `r31`; later used as a base pointer with fields at offsets `+24`, `+64`, `+80..+82`.
  - `r6` saved as `r25`; used as an output pointer for a page-aligned size stored via `stw ...,0(r25)`.
  - `r7` saved as `r28`; later used as an output pointer or callback argument.

- **Outputs (observed)**:
  - Returns a status in `r3` (checked by callers for `<0`).
  - Writes a page-aligned size to `*(u32*)(r25+0)`.
  - Writes several fields into the `r31` structure:
    - `stb r26,80(r31)` and `stb 1,81(r31)` and `stb 1,82(r31)`.
    - `std 4096,8(r31)` and `std (computed-4096),16(r31)`.

- **Observed behavior (conservative)**:
  - Prepares an ANSI-string-like structure on the stack via `__imp__RtlInitAnsiString`.
  - Calls `__imp__NtCreateFile` and bails out on `<0`.
  - Calls `__imp__NtAllocateVirtualMemory` for `r31+64` after storing a page-aligned size derived from `*(u32*)(r29+4)`.
  - Calls `__imp__NtQueryVolumeInformationFile` on the created/opened handle (`lwz r3,0(r24)` where `r24=r31+24`).
  - Calls at least one indirect function pointer loaded from `*(u32*)(*(u32*)(r29+664)+8)`.
  - No explicit `996/997` comparisons are present in the slice inspected; this function is not obviously using the `996/997` cooperative protocol.

- **Imports called (provable)**:
  - `__imp__RtlInitAnsiString`
  - `__imp__NtCreateFile`
  - `__imp__NtAllocateVirtualMemory`
  - `__imp__NtQueryVolumeInformationFile`

- **Async / retry**:
  - No internal loop was observed in this function; it is a synchronous “do work then return status” helper.

### 1.4.3 `__imp__sub_829A3560` (task + mount/open/read + wait integration)

- **Source**:
  - `ppc_recomp.155.cpp`

- **Inputs (observed)**:
  - `r3` saved as `r25`.
  - `r4` saved as `r29`.
  - `r5` saved as `r22`.
  - `r6` saved as `r21`.
  - `r7` saved as `r23`.

- **Outputs (observed)**:
  - Returns a status in `r3`.
  - Writes `r20` into the global at `*(u32*)(-31981:6964)` on one success path.
  - Performs a loop back to `loc_829A3690` (within this function) after some success/cleanup paths.

- **Observed behavior (conservative)**:
  - Calls `__imp__XexCheckExecutablePrivilege(23)` early; may set a local flag `r24`.
  - Calls `__imp__NtCreateFile` (stack-based structs around `stack+104`, `stack+128`, `stack+144`, etc.).
  - On successful create/open, performs `__imp__NtReadFile` reading `2048` bytes, closes via `__imp__NtClose`, and checks the read status.
  - Includes an in-function retry via `goto loc_829A3690` that is triggered after some conditions; the exact high-level intent cannot be proven from source alone.
  - Schedules work through XAM task APIs:
    - `__imp__XamTaskSchedule`
    - `__imp__XamTaskCloseHandle`
    - `__imp__XamTaskShouldExit` (used as a loop gate elsewhere, see `__imp__sub_829A3318`).
  - Contains a wait step via `__imp__KeWaitForSingleObject` on a fixed global address (`-32087:32604` pattern).

- **Imports called (provable; partial list)**:
  - `__imp__XexCheckExecutablePrivilege`
  - `__imp__NtCreateFile`
  - `__imp__NtReadFile`
  - `__imp__NtClose`
  - `__imp__KeWaitForSingleObject`
  - `__imp__KeResetEvent`
  - `__imp__KeEnterCriticalRegion` / `__imp__KeLeaveCriticalRegion`
  - `__imp__RtlEnterCriticalSection` / `__imp__RtlLeaveCriticalSection`
  - `__imp__XamTaskSchedule` / `__imp__XamTaskCloseHandle`

- **Async / retry**:
  - Contains in-function control-flow loops (label-based), including one that returns to a `NtCreateFile`-adjacent error-handling path.
  - The loop semantics depend on external conditions and imported call results; do not assume eventual convergence.

### 1.4.4 `__imp__sub_829A3178` (wait + critical-section mediated signal)

- **Source**:
  - `ppc_recomp.155.cpp`

- **Inputs/outputs (observed)**:
  - Returns a value in `r3` after calling `__imp__RtlNtStatusToDosError`.

- **Observed behavior (conservative)**:
  - Reads a global flag at `-31981:6964` and branches:
    - If it is `0`, it constructs `r30 = (-16384<<16) | 622` and then returns a translated error.
  - Checks privilege via `__imp__XexCheckExecutablePrivilege(11)`.
    - If privilege is not present, it sets `r30=0`.
  - On the “privilege present” path, it enters a critical region and critical section, sets a global at `-31981:6984` to `1`, signals an event (`__imp__KeSetEvent`), then waits once via `__imp__KeWaitForSingleObject` and resets that event.
  - After the wait, it loads `r30 = *(u32*)(r30+16)` (where `r30` was a pointer derived from `-31981:6984`) before leaving the critical section.

- **Imports called (provable)**:
  - `__imp__XexCheckExecutablePrivilege`
  - `__imp__KeEnterCriticalRegion` / `__imp__KeLeaveCriticalRegion`
  - `__imp__RtlEnterCriticalSection` / `__imp__RtlLeaveCriticalSection`
  - `__imp__KeSetEvent` / `__imp__KeWaitForSingleObject` / `__imp__KeResetEvent`
  - `__imp__RtlNtStatusToDosError`

- **Loop type**:
  - No internal loop in this function; single wait.

### 1.4.5 `__imp__sub_829A3238` (signal + wait driven by global state)

- **Source**:
  - `ppc_recomp.155.cpp`

- **Observed behavior (conservative)**:
  - Reads `*(u32*)(-31981:6964+4)`:
    - If nonzero, it enters a critical section, stores `2` to `-31981:6984`, calls `__imp__KeSetEvent`, waits once via `__imp__KeWaitForSingleObject`, resets the event, then leaves the critical section.
  - Else it reads `*(u32*)(-31981:6964+0)`:
    - If nonzero, it calls `sub_829A2FE8` (which in turn calls `__imp__ObDeleteSymbolicLink`, per the import-usage index).
  - In all cases, it stores `0` to `*(u32*)(-31981:6964+0)` before returning.

- **Imports called (provable)**:
  - `__imp__RtlEnterCriticalSection` / `__imp__RtlLeaveCriticalSection`
  - `__imp__KeSetEvent` / `__imp__KeWaitForSingleObject` / `__imp__KeResetEvent`

- **Loop type**:
  - No internal loop in this function; single wait.

### 1.4.6 `__imp__sub_829A3318` (boot-adjacent orchestrator; calls `sub_829A2AE0` and loops on task exit)

- **Source**:
  - `ppc_recomp.155.cpp`

- **Inputs (observed)**:
  - `r3` is treated as a pointer to a state object `r31`, with fields used at offsets:
    - `+8`, `+12`, `+16`, and `+664`.

- **Outputs (observed)**:
  - Writes `r30` (status) into `*(u32*)(r31+16)`.
  - Signals events via `__imp__KeSetEvent`.
  - Performs cleanup: dismount, free virtual memory, close handles, dereference objects.

- **Observed behavior (conservative)**:
  - Calls `sub_829A2AE0(r3 = *(u32*)(r31+8), r4=r31, r5=stack+96, r6=stack+80, r7=stack+272)` and stores its result to `r30`.
  - Registers title terminate notifications and uses multiple events.
  - Contains an explicit in-function loop (`loc_829A3404`) that:
    - waits (`__imp__KeWaitForSingleObject`),
    - invokes `sub_829A2CA8(...)`,
    - checks `__imp__XamTaskShouldExit()`, and
    - loops again when `XamTaskShouldExit` returns `0`.
  - Performs cleanup regardless of status (dismount, free, close, dereference).

- **Imports called (provable; partial list)**:
  - `__imp__KeWaitForSingleObject`
  - `__imp__KeSetEvent` / `__imp__KeResetEvent`
  - `__imp__XamTaskShouldExit`
  - `__imp__ExRegisterTitleTerminateNotification`
  - `__imp__IoDismountVolume`
  - `__imp__NtFreeVirtualMemory`
  - `__imp__NtClose`
  - `__imp__ObDereferenceObject`

- **Loop type**:
  - Tight in-function wait loop gated by `__imp__XamTaskShouldExit`.

### 1.4.7 `__imp__sub_829C4548` (Net init wrapper)

- **Source**:
  - `ppc_recomp.158.cpp`

- **Inputs (observed)**:
  - `r3`: treated as an `XNADDR*`-like pointer, moved to `r4`.

- **Outputs (observed)**:
  - Writes `1` to `r3` before tail-calling the import.
  - Returns whatever `__imp__NetDll_XNetGetTitleXnAddr` returns.

- **Observed behavior**:
  - Thin wrapper: `r4=r3; r3=1; __imp__NetDll_XNetGetTitleXnAddr(ctx, base)`.

- **Imports called (provable)**:
  - `__imp__NetDll_XNetGetTitleXnAddr`

# 2. State Machines

## 2.1 Async-completion wrapper state machine (`sub_827DBF10` and `sub_827DBF90`)

**Primary state field**:

- `*(u32*)(obj + 0x00)` (loaded via `lwz r11,0(r31)`)

**Observed states and meanings (conservative)**

These meanings are inferred only from guards and the immediate stores performed.

- **State `2`**: “waiting to complete phase-2 operation.”
  - Evidence: `sub_827DBF10` only performs the async-status check when `state==2`.
- **State `3`**: “phase-2 completed successfully.”
  - Evidence: `sub_827DBF10` stores `3` when `sub_829A1A50(...)` returns `0`.
- **State `1`**: “phase-2 completed with nonzero status (error or alternate result).”
  - Evidence: `sub_827DBF10` stores `1` when `sub_829A1A50(...)` returns nonzero and non-`996`.

- **State `4`**: “waiting to complete phase-4 operation.”
  - Evidence: `sub_827DBF90` only performs the async-status check when `state==4`.
- **State `5`**: “phase-4 completed successfully; produced a result.”
  - Evidence: `sub_827DBF90` stores `5` on the path where `sub_829A1A50(...) == 0`, and stores `*(u32*)(stack+80)` into the output.

**Transitions and conditions**

- `2 -> (no change)` when `sub_829A1A50(obj+8, out, 0) == 996`.
  - Implementation: `sub_827DBF10` immediately returns `0` via `loc_827DBF30`.
- `2 -> 3` when `sub_829A1A50(...) == 0`.
- `2 -> 1` when `sub_829A1A50(...) != 0` and `!= 996`.

- `4 -> (no change)` when `sub_829A1A50(...) == 996`.
  - Implementation: `sub_827DBF90` immediately returns `0` via `loc_827DBFB0`.
- `4 -> 5` when `sub_829A1A50(...) == 0`.
- `4 -> 1` when `sub_829A1A50(...) != 0` and additional checks interpret the situation as failure.
  - Evidence: path `loc_827DBFF8` stores `1` to state and stores `-1` to an output slot.

**Notes / unknowns**

- `sub_827DBF90` has additional conditional logic involving `*(u32*)(obj+32)` compared against `1223` and against `0x80070012` (constructed as `-2147024896 | 18`). This suggests special-case error treatment, but the exact meaning cannot be proven without knowing what `obj+32` encodes.

## 2.2 Content/device workflow state machine (`sub_827DDE30`)

This one is larger and includes a *secondary* phase field.

**Primary state field**:

- `*(u32*)(ctx + 0x00)` where `ctx` is the `r31` pointer inside `sub_827DDE30`.
  - Guard: `sub_827DDE30` only runs when `state==8`; otherwise returns `0`.

**Secondary sub-state field**:

- `*(u32*)(ctx + 0x04)` (loaded as `lwz r11,4(r31)`).
  - Used to branch among sub-phases `1`, `2`, `3`, `4`.

**Observed state meanings (partial)**

- **Primary state `8`**: “drive/advance workflow.”
  - Evidence: `sub_827DDE30` early-outs unless `state==8`.

- **Secondary state `1`**: “needs to start an operation that returns a handle in `ctx+96`.”
  - Evidence: when `substate==1`, it calls `sub_829A2590(...)` and stores the return value to `*(u32*)(ctx+96)`.
- **Secondary state `2`**: “operation issued; wait/drive completion using `sub_829A1F00` then poll.”
  - Evidence: on the success path after starting, it stores `2` into `ctx+4`, initializes a small struct at `ctx+36`, and calls `sub_829A1F00(handle=ctx+96, ...)`.
- **Secondary state `3`**: “cleanup/close path.”
  - Evidence: when `substate==3`, it transitions to primary state `9` and returns a stored value from `ctx+116`.
- **Secondary state `4`**: “terminal / cancel / abort path.”
  - Evidence: when `substate==4`, it forces primary `state=1` and outputs `0`.

**Transitions and conditions (selected, from the visible slice)**

- Gate 0: `if sub_827DC050(ctx, &tmp) == 996` then `return 0` and do not mutate state.
  - This is an explicit “do nothing until ready” barrier.

- `substate==1`:
  - Calls `sub_829A2590(ctx+64, ...)` and stores result in `ctx+96`.
  - If `ctx+96 == -1`, it calls `sub_829A1EF8()` and goes into a cleanup path.
  - Else sets `substate=2`, zeros `ctx+36..+52`, then calls `sub_829A1F00(ctx+96, ctx+112, ctx+116, ctx+36, 0)`.

- `substate==2`:
  - If `sub_829A1F00(...) != 0`, it returns `0` (no further progress in this call).
  - Else it calls `sub_829A1EF8()` and checks if the return equals `997`.
    - If `==997`, it returns `0` (still pending).
    - If `!=997`, it closes `ctx+96` via `sub_829A1958`, sets `ctx+96=-1`, then calls `sub_829A1EF8()` again and goes to cleanup.

- Cleanup loop portion:
  - Multiple paths eventually reach code that calls `sub_829A1CA0(handle)` (imported as `__imp__XamContentClose`) and checks whether it returns `997`.
    - If it returns `997`, it sets `substate=4` and returns `0`.
    - Otherwise it calls `sub_829A1EF8()` and either:
      - sets `state=1` and returns `1` (appears like “finished with failure”), or
      - continues cleanup.

Because the full function body is large, this is a partial reconstruction focused on the visible control flow around `loc_827DDF98`.

## 2.3 UI/selection workflow state machine (`sub_827DC458` and `sub_827DC368`)

These functions manipulate `*(u32*)(ctx+0)` and interact with XAM UI and enumeration APIs.

**Primary state field**:

- `*(u32*)(ctx + 0x00)` where `ctx` is `r31`.

**Key observations**

- `sub_827DC368` can set `state=2` if a call to `sub_829A1288` (imported as `__imp__XamShowDeviceSelectorUI`) returns `997`.
- `sub_827DC458` begins only if `state==0 || state==1 || *(u32*)(ctx+56)==0` (otherwise it returns `0`). It then:
  - calls `sub_829A1CB8` (imported as `__imp__XamContentCreateEnumerator`) and if it returns nonzero, sets `state=1` and returns `0`.
  - calls `sub_829A1A38` (imported as `__imp__XamEnumerate`) and checks `997`:
    - if `997`, sets `state=4` and returns `1` (meaning: “async started / pending”).
    - otherwise treats it as failure: sets `state=1`, closes a handle with `sub_829A1958`, and returns `0`.

## 2.4 Tight retry loop around `sub_829A1F00` (`sub_827F0B20`)

This is a non-state-machine *tight retry loop* (i.e., the loop happens inside a single function call) and is therefore a strong candidate for an observable “spin” if it fails to converge.

**Loop driver**:

- `sub_827F0B20` (in `ppc_recomp.124.cpp`) repeatedly calls `sub_829A1F00(...)`.

**Loop condition**:

- It loops while `sub_829A1F00(...) == 0`.
- It exits when `sub_829A1F00(...) != 0`, returning the value stored at `stack+80`.

**Internal pacing logic (conservative)**:

- A local countdown `r28` is initialized to `10`.
- Each loop iteration may call `sub_829A1EF8()`.
- When the countdown expires (`r28` becomes negative), the function conditionally calls an indirect function pointer loaded from a fixed-address global location (pattern: `lwz r11,-32108(r27)` followed by `mtctr r11; bctrl`). The target and its side effects are not provable from the source shown.

 # 3. Retry Loops / Barriers

This code uses explicit label-based loops (via `goto loc_...`) rather than structured `while` loops.

## 3.1 Cooperative barrier based on return code `996` (non-progress gate)

Pattern:

- Call a helper (`sub_829A1A50` directly, or via `sub_827DC050`).
- If return is `996`, **exit early without changing state**.

Examples:

- `sub_827DBF10`:
  - Loop-equivalent behavior: repeated calls are expected until `sub_829A1A50(...) != 996`.
  - Exit condition: `sub_829A1A50(...) != 996`.
  - Loop body: just the call + compare; no other observable side effects inside the function.

- `sub_827DDE30`:
  - First gate is `if sub_827DC050(...) == 996 return 0;`.

This is a *barrier* in the sense that forward progress is impossible while the helper keeps returning `996`.

## 3.2 Cooperative barrier around `XamContentClose` polling (`loc_827DE030` / `loc_827DDF98`) inside `sub_827DDE30`

From `ppc_recomp.122.cpp`:

- Label `loc_827DE030` performs a “poll” of `sub_829A1CA0(...)` (imported as `__imp__XamContentClose`) and then branches based on whether the return equals `997`.
- Label `loc_827DDF98` **does not loop**: it calls `sub_829A1EF8()` and then forces `*(u32*)(r31+0)=1`, writes `0` to the output pointer, and returns `1`.

Conservative interpretation:

- **Cooperative barrier condition**: `sub_829A1CA0(...) == 997` is treated as a special “pending” result that causes the function to set `*(u32*)(r31+4)=4` and return `0` (no progress).
- **Non-`997` close result**: if `sub_829A1CA0(...) != 997`, the code jumps to `loc_827DDF98` and terminates the workflow with `state=1` and output `0`.

This means that within `sub_827DDE30`, `997` is treated as “still pending; keep waiting,” but the wait is *not an in-function spin*: it returns to the caller in state `substate=4`.

Whether `sub_829A1EF8()` is an intentional “yield” is **not provable** from source: in the current implementation it calls `sub_829A92A0()` which either returns `0` (if a global flag is set) or returns a global error value from `*(u32*)(*(u32*)(r13+256) + 352)`.

## 3.3 Other cooperative polling patterns based on `997`

Several call sites treat `997` as “keep waiting” and return to the caller without changing to a terminal state. For example:

- `sub_827DC458` sets `state=4` and returns `1` when `XamEnumerate` returns `997`.

This is not an in-function busy-spin; it’s a **cooperative polling state machine** where the caller must call again.

## 3.4 Enforced wait barrier in `sub_829A1F00` (`259` → `__imp__NtWaitForSingleObjectEx`)

`sub_829A1F00` (in `ppc_recomp.155.cpp`) contains an explicit barrier:

- It calls an indirect host function pointer (from `*(u32*)(*(u32*)(-32756 + -32086-base) + 16)`), then checks whether the result equals `259`.
- If it equals `259`, it calls `__imp__NtWaitForSingleObjectEx(r3=r30, r4=1, r5=0, r6=0)` and then re-uses a value loaded from `stack+80`.

Static conclusions:

- The code enforces a blocking wait when this `259` result occurs.
- The code does not prove what `259` means, but it is used as a trigger for “wait for single object.”

## 3.5 Tight in-function retry loop that can look like a spin (`sub_827F0B20`)

Unlike the cooperative state machines that return to their caller, `sub_827F0B20` contains an actual in-function retry loop:

- First call: `sub_829A1F00(...)`.
- If it returns `0`, the code performs a bounded local countdown and repeatedly retries `sub_829A1F00(...)`.

The loop continues *solely* based on the return value of `sub_829A1F00`.

Therefore, if `sub_829A1F00` continues returning `0` indefinitely, `sub_827F0B20` can execute indefinitely within one invocation.

## 3.6 Tight in-function wait loop around `KeWaitForSingleObject` (`sub_829DDC90`)

From `ppc_recomp.160.cpp`:

- `sub_829DDC90` calls `__imp__KeWaitForSingleObject` and compares the return value against `258`.
- If the return equals `258`, it branches back to an earlier label (`loc_829DDCF0`) and repeats the wait path.

This is an in-function loop whose continuation depends on the return value of the wait call.

## 3.7 Tight in-function wait loops gated by global flags (`sub_82169400` / `sub_821694C8`)

From `ppc_recomp.4.cpp`:

- `sub_82169400` calls `__imp__KeWaitForSingleObject` in a loop and exits only after observing a computed condition derived from a global structure (notably loads at offsets like `+300` and `+304`).
- `sub_821694C8` contains a similar wait-and-check loop, repeatedly waiting and checking a global value at `+300` until it becomes `0`.

Static limitation: the meanings of the probed global offsets are not provable from these snippets alone.

## 3.8 Retry loop around `NtWaitForSingleObjectEx` (`sub_829A9738`)

From `ppc_recomp.156.cpp`:

- `sub_829A9738` calls `__imp__NtWaitForSingleObjectEx` and, on one path, compares the return value against `257`.
- If the value equals `257` and a caller-provided flag value (`r28`) is nonzero, it branches back and waits again.

This introduces an additional distinguished wait result used as a retry trigger.

 # 4. Host Interactions

This section documents host import interactions that are provably involved in the inspected guest-side logic.

## 4.1 Imported calls used by the inspected guest workflows

Observed imported calls and their usage patterns:

| Import thunk | Where it appears (examples) | How callers interpret return | Notes |
|---|---|---|---|
| `__imp__NtCreateFile` | Multiple call sites in `ppc_recomp.155.cpp` | Compared against `<0` or `==0` in various contexts | Host implementation special-cases `game:\` mount opens (see §4.2) |
| `__imp__NtReadFile` | Used in `ppc_recomp.155.cpp` in file/crypto routines | Return compared `<0` / `>=0` | Semantics depend on host FS |
| `__imp__NtWriteFile` | Used in `ppc_recomp.156.cpp` | Return compared against `259` and may be followed by `__imp__NtWaitForSingleObjectEx` | Appears as part of an async I/O wrapper |
| `__imp__NtClose` | Used to close handles | N/A | Cleanup |
| `__imp__NtWaitForSingleObjectEx` | Called from `sub_829A1F00` when host call returns `259` | Caller assumes this wait contributes to progress | Explicit enforced wait path |
| `__imp__KeWaitForSingleObject` | Used in `ppc_recomp.4.cpp`, `ppc_recomp.155.cpp`, `ppc_recomp.156.cpp`, `ppc_recomp.160.cpp` | Sometimes compared against `258`, sometimes used as a pure wait step in a loop | Host may short-circuit specific callers (see §4.3) |
| `__imp__XamEnumerate` | Used via wrapper `sub_829A1A38` in `sub_827DC458` | `997` treated as “pending” | Cooperative polling |
| `__imp__XamContentClose` | Used via wrapper `sub_829A1CA0` | `997` treated as “pending” | Cleanup barrier |
| `__imp__XamShowDeviceSelectorUI` | Used via wrapper `sub_829A1288` | `997` treated as “pending” | UI/device selection |

The guest also uses at least one **indirect host function pointer** (inside `sub_829A1F00`). The concrete target and semantics are not provable from the guest sources alone.

## 4.2 Host-side `NtCreateFile` special-case relevant to boot stalls (`game:\` open storm)

In `LibertyRecomp/kernel/imports.cpp`, the host implementation of `NtCreateFile` contains explicit logic for mount-style opens of the root paths:

- Detects `game:\` / `d:\` root opens.
- Tracks open count.
- Logs the guest caller address as `callerAddr = g_ppcContext->lr`.

This is a key safe instrumentation point because it can identify which guest code location is driving repeated root opens.

## 4.3 Host-side wait behavior that can affect boot progression

`KeWaitForSingleObject` (host, `imports.cpp`) contains special-case logic:

- Logs waits and records `caller = g_ppcContext->lr`.
- Has explicit "GPU init deadlock prevention" logic for some caller addresses (e.g. `0x829DDD48`, `0x829D8AA8`), and may force internal GPU flags and return `STATUS_SUCCESS`.

This means some guest-side waits may be short-circuited on the host, and some apparent boot progress may depend on these host interventions.

## 4.4 Host-side network stub observed during boot (`__imp__NetDll_XNetGetTitleXnAddr`)

The host provides an implementation for `__imp__NetDll_XNetGetTitleXnAddr` in `imports.cpp` that:

- Optionally writes an `XNADDR`-like structure into guest memory (when pointer appears valid).
- Returns a bitmask value (`0x66` in the current host code).

 This can affect guest-side network initialization state machines, but the corresponding guest call sites have not been mapped in this document.

## 4.5 Guest import usage index (auto-derived)

This table is generated by scanning for direct calls of the form `__imp__Xxx(ctx, base)` inside each `PPC_FUNC_IMPL(__imp__sub_XXXXXXXX)` body in `ppc_recomp.*.cpp`.

Important limitation:

- This is a **call-site index**, not proof that these functions execute during boot.

| Import thunk | # callers | Caller functions (`__imp__sub_XXXXXXXX`) |
|---|---:|---|
| `__imp__KeDelayExecutionThread` | 1 | __imp__sub_829A9620 |
| `__imp__KeWaitForMultipleObjects` | 1 | __imp__sub_82169B00 |
| `__imp__KeWaitForSingleObject` | 8 | __imp__sub_82169400, __imp__sub_821694C8, __imp__sub_829A3178, __imp__sub_829A3238, __imp__sub_829A3318, __imp__sub_829A3560, __imp__sub_829AAE50, __imp__sub_829DDC90 |
| `__imp__NetDll_XNetCleanup` | 1 | __imp__sub_829C4458 |
| `__imp__NetDll_XNetGetConnectStatus` | 1 | __imp__sub_829C44A0 |
| `__imp__NetDll_XNetGetEthernetLinkStatus` | 1 | __imp__sub_829C4558 |
| `__imp__NetDll_XNetGetTitleXnAddr` | 1 | __imp__sub_829C4548 |
| `__imp__NetDll_XNetQosListen` | 1 | __imp__sub_829C44B0 |
| `__imp__NetDll_XNetQosLookup` | 1 | __imp__sub_829C44D0 |
| `__imp__NetDll_XNetQosRelease` | 1 | __imp__sub_829C4538 |
| `__imp__NetDll_XNetServerToInAddr` | 1 | __imp__sub_829C4478 |
| `__imp__NetDll_XNetStartup` | 1 | __imp__sub_829C4390 |
| `__imp__NetDll_XNetUnregisterInAddr` | 1 | __imp__sub_829C4490 |
| `__imp__NetDll_XNetXnAddrToInAddr` | 1 | __imp__sub_829C4460 |
| `__imp__NtClose` | 15 | __imp__sub_829A0538, __imp__sub_829A29A8, __imp__sub_829A2D98, __imp__sub_829A3318, __imp__sub_829A3560, __imp__sub_829A3F10, __imp__sub_829A40F0, __imp__sub_829A41A8, __imp__sub_829A4278, __imp__sub_829A93A8, __imp__sub_829A9580, __imp__sub_829A9CB0, __imp__sub_829AA008, __imp__sub_829AFCA0, __imp__sub_829E8BA0 |
| `__imp__NtCreateFile` | 4 | __imp__sub_829A29A8, __imp__sub_829A2AE0, __imp__sub_829A3560, __imp__sub_829A4278 |
| `__imp__NtOpenFile` | 8 | __imp__sub_829A2D98, __imp__sub_829A3F10, __imp__sub_829A40F0, __imp__sub_829A41A8, __imp__sub_829A93A8, __imp__sub_829A9580, __imp__sub_829AFCA0, __imp__sub_829E8BA0 |
| `__imp__NtReadFile` | 3 | __imp__sub_829A29A8, __imp__sub_829A3560, __imp__sub_829AAD20 |
| `__imp__NtWaitForMultipleObjectsEx` | 1 | __imp__sub_829A22C8 |
| `__imp__NtWaitForSingleObjectEx` | 5 | __imp__sub_829A1F00, __imp__sub_829A3BD0, __imp__sub_829A9738, __imp__sub_829AAD20, __imp__sub_829AADB8 |
| `__imp__NtWriteFile` | 4 | __imp__sub_829A29A8, __imp__sub_829A3BD0, __imp__sub_829AADB8, __imp__sub_829AFCA0 |
| `__imp__XamContentClose` | 1 | __imp__sub_829A1CA0 |
| `__imp__XamContentCreateEnumerator` | 1 | __imp__sub_829A1CB8 |
| `__imp__XamContentCreateEx` | 1 | __imp__sub_829A1C38 |
| `__imp__XamContentGetCreator` | 1 | __imp__sub_829A1CB0 |
| `__imp__XamContentGetDeviceData` | 1 | __imp__sub_829A1CC0 |
| `__imp__XamContentSetThumbnail` | 1 | __imp__sub_829A1CA8 |
| `__imp__XamEnumerate` | 1 | __imp__sub_829A1A38 |
| `__imp__XamLoaderLaunchTitle` | 1 | __imp__sub_829A9208 |
| `__imp__XamLoaderTerminateTitle` | 2 | __imp__sub_829A0858, __imp__sub_829A09E8 |
| `__imp__XamShowDeviceSelectorUI` | 1 | __imp__sub_829A1288 |
| `__imp__XamShowDirtyDiscErrorUI` | 1 | __imp__sub_829A1290 |
| `__imp__XamShowGamerCardUIForXUID` | 1 | __imp__sub_829A1278 |
| `__imp__XamShowMessageBoxUIEx` | 1 | __imp__sub_829A0538 |
| `__imp__XamShowPlayerReviewUI` | 1 | __imp__sub_829A1280 |
| `__imp__XamShowSigninUI` | 1 | __imp__sub_829A1270 |
| `__imp__XexCheckExecutablePrivilege` | 3 | __imp__sub_829A0678, __imp__sub_829A3178, __imp__sub_829A3560 |
| `__imp__XexGetModuleHandle` | 2 | __imp__sub_829C3FE8, __imp__sub_829C4390 |
| `__imp__XexGetProcedureAddress` | 2 | __imp__sub_829C3FE8, __imp__sub_829C4390 |

# 5. Failure / Stall Modes
 
 ## 5.1 Async protocol signals and why they matter for boot
 
 **Where the magic values originate (from source)**:
 
 - `997` originates as:
   - the direct return value of several `__imp__Xam*` calls (e.g. `__imp__XamEnumerate`, `__imp__XamContentClose`).
   - a literal assigned in some functions (direct `li ...,997` patterns exist in the corpus).
 
 - `996` originates as:
   - a literal assigned in at least one function (e.g. `sub_829A4648` has a `li r3,996` path), and
   - a literal used as the “incomplete” result in multiple comparisons.
 
 - `258` originates as:
   - a literal assigned in `sub_829A1A50` on a particular path and then mapped to `996`.
 
 - `259` originates as:
   - a literal compared against the return value of an indirect host call inside `sub_829A1F00`, and is used as the condition to call `__imp__NtWaitForSingleObjectEx`.
 
 **Meaning inferred from control flow**:
 
 - **`997`**: callers often treat `997` as “operation pending; store a pending state and return.”
 - **`996`**: callers treat `996` as “do not advance; return `0` (no progress).”
 - **`258`**: intermediate value that is mapped to `996` inside `sub_829A1A50`.
 - **`259`**: triggers an explicit wait path in `sub_829A1F00`.
 
 **Resolution requirement (conservative)**:
 
 Nothing in the guest sources enforces that `996/997` will ever change. Progress requires external state changes by the host/runtime or other subsystems such that subsequent calls stop returning the “pending” value.
 
 ## 5.2 `sub_829A1A50` never stops yielding `996`
 
 - **Symptom**: callers repeatedly return without mutating state (no forward progress).
 - **Depends on**:
   - **Host API semantics / missing side effects**.
   - **Incorrect async completion handling**.
   - **Incorrect initialization** of the status object passed to the helper.
 
 ## 5.3 APIs keep returning `997` indefinitely
 
 - **Symptom**: “pending” states persist (cooperative polling never resolves).
 - **Depends on**:
   - **Host API semantics** (XAM stubs, async engine).
 
 ## 5.4 Cleanup logic depends on handle semantics
 
 In `sub_827DDE30`, cleanup includes closing handles (`sub_829A1958`) and calling `XamContentClose` (`sub_829A1CA0`). If the host’s rules differ from what the guest expects, it can repeatedly take non-progress branches.
 
 - **Depends on**:
   - **Host API semantics**.
 
 ## 5.5 Source-level patching can create false progress
 
 `sub_829A1A50` currently contains `// PATCHED` logic that forces success and/or converts `996/997` to `0`.
 
 - **Symptom**: barriers that would normally block no longer block; state machines may advance with invariants violated.
 
 ## 5.6 Tight retry loops can spin inside one invocation
 
 - `sub_827F0B20` contains a tight retry loop that continues while `sub_829A1F00(...) == 0`.
 - If `sub_829A1F00` never returns nonzero, the loop can be unbounded within one call.
 
 Additional host-side stall modes (provable from host code):
 
 - Host waits may block indefinitely if an object is never signaled (unless special-cased as in the GPU init path).
 - Host filesystem semantics for mount paths (`game:\`) can cause repeated open attempts if guest expects file-like behavior but host returns directory-like behavior.
 
 # 6. Safe Instrumentation Points
 
 These are instrumentation locations that are (a) guaranteed to execute when the relevant path is exercised and (b) can be implemented as logging only.
 
 ## 6.1 Host-side safe points
 
 - `NtCreateFile` (`imports.cpp`): log `guestPath`, `callerAddr = g_ppcContext->lr`, and status returned, especially for `game:\` root opens.
 - `KeWaitForSingleObject` (`imports.cpp`): log `caller = g_ppcContext->lr`, object type, timeout, and whether the GPU special-case path was taken.
 - `NetDll_XNetGetTitleXnAddr` (`imports.cpp`): log the `XNADDR*` pointer and return value to confirm network init isn’t stuck waiting on “pending”.
 
 ## 6.2 Guest-side safe points (log-only)
 
 - Entry/exit of `sub_829A1F00`: log the input handle value, whether the `259` branch was taken, and whether it returns `0` vs nonzero.
 - Loop backedge in `sub_827F0B20`: log iteration count and `sub_829A1F00` return.
 
 ## 6.3 Address-to-function correlation
 
 - Use `ppc_func_mapping.cpp` to map logged guest addresses (LR/PC) back to `sub_XXXXXXXX`.
 - This is required to convert host-side logs like `caller=0x829DDD48` into concrete guest function identities.
 
 # 7. Open Questions / Unknowns
 
 Static source alone cannot answer the following, but each item is directly relevant to proving boot completeness:
 
 - What concrete structure types correspond to the “context objects” whose fields are accessed at offsets `0`, `4`, `8`, `32`, `56`, `60`, `96`, `112`, `116`, etc.?
 - What does `sub_829A2590` do (it appears to start an operation returning `-1` or a handle)?
 - What does the indirect callback in `sub_827F0B20` (loaded from `lwz r11,-32108(r27)` then called via `bctrl`) do? Does it yield, pump events, or advance async state?
 - For the host-reported `game:\` open storm: which guest function(s) correspond to the logged `callerAddr = g_ppcContext->lr`, and do those call sites reside inside `sub_827F0B20`, `sub_827DDE30`, or a different loop entirely?
 - What are the host/runtime semantics of the imported XAM functions used here:
   - `__imp__XamEnumerate`
   - `__imp__XamContentCreateEnumerator`
   - `__imp__XamContentClose`
   - `__imp__XamShowDeviceSelectorUI`
 - Does `sub_829A1EF8` (via `sub_829A92A0`) have any host-side side effects that would act like a yield/sleep?
 - Are the `// PATCHED` blocks meant to be temporary instrumentation?
 
 Hooking-related questions:
 
 - Are attempted hooks targeting `sub_829A1A50` / `sub_829A1CA0` applied to the correct symbol (e.g. `sub_829A1A50` vs `__imp__sub_829A1A50`), and do call sites actually invoke the symbol being replaced?
 - For XAM-related wrappers (e.g. `sub_829A1CA0`), are call sites using the wrapper or calling the import thunk directly (e.g. `__imp__XamContentClose`)?

# 8. Render Loop Progression Analysis

This section documents the investigation into why the game boots but never enters the render loop (VdSwap never called).

## 8.1 Current State Summary

**What works:**
- Game boots successfully
- Files load from RPF (common.rpf, xbox360.rpf)
- Streaming system functions without corruption (heap aliasing fix applied)
- GPU initialization completes (VdInitializeEngines, VdRetrainEDRAM)
- Worker threads (9) are created and idle correctly
- XXOVERLAPPED async completion is being set up

**What doesn't work:**
- Render loop never starts
- VdSwap is never called
- Main thread is blocked waiting for a condition
- No crash, no spin — clean block

## 8.2 VdSwap Call Chain (Traced)

The VdSwap import is called through this chain:

```
sub_828529B0  (main loop orchestrator)
    └── sub_829CB818  (pre-frame setup)
    └── sub_828507F8  (frame presentation wrapper)
            └── sub_829D5388  (D3D Present wrapper)
                    └── __imp__VdSwap  (kernel import @ 0x82A0310C)
```

**`sub_828507F8`** (`ppc_recomp.130.cpp` line 1665):
- Takes a device context pointer in r3
- Loads frame buffer index from offset 19480
- Calls `sub_829D5388` with frame data
- Increments frame counter

**`sub_829D5388`** (`ppc_recomp.159.cpp` line 30427):
- The actual D3D Present wrapper
- Prepares display parameters
- Calls `__imp__VdSwap(ctx, base)` at address 0x829D55D4

## 8.3 Critical Blocking Issues Identified

### 8.3.1 XamTaskShouldExit Returns 1 (CRITICAL)

**Location:** `imports.cpp` line 5470

```cpp
uint32_t XamTaskShouldExit(uint32_t taskHandle)
{
    // Return 1 to indicate task should exit (no async tasks supported yet)
    return 1;
}
```

**Problem:** This causes ALL XAM async tasks to exit immediately after being scheduled. The game's boot orchestration functions (documented in §1.4.6 `sub_829A3318`) use XamTaskShouldExit in a loop:

```
while (XamTaskShouldExit() == 0) {
    KeWaitForSingleObject(...);
    // do async work
}
```

When XamTaskShouldExit returns 1, the task loop exits immediately without completing its work. This breaks:
- Content mounting completion
- Async I/O completion callbacks
- Background loading tasks
- Any subsystem that depends on XAM task completion signals

**Real Xbox 360 behavior:** XamTaskShouldExit returns 0 while the task should continue running, and returns 1 only when the system requests task termination (e.g., title exit).

**Required fix:**
```cpp
uint32_t XamTaskShouldExit(uint32_t taskHandle)
{
    // Return 0 to indicate task should continue running
    // Only return 1 when system is shutting down
    return 0;
}
```

### 8.3.2 XamTaskSchedule Is a Stub (CRITICAL)

**Location:** `imports.cpp` line 5465

```cpp
void XamTaskSchedule()
{
    LOG_UTILITY("!!! STUB !!!");
}
```

**Problem:** XamTaskSchedule is supposed to:
1. Accept a function pointer and context
2. Create an async task handle
3. Schedule the task for execution
4. Return the task handle to the caller

The current stub does nothing, meaning async tasks are never actually scheduled or executed.

**Real Xbox 360 signature:**
```cpp
DWORD XamTaskSchedule(
    LPVOID lpStartAddress,      // Task function pointer
    LPVOID lpParameter,         // Task context
    DWORD dwProcessId,          // Process ID (usually 0)
    DWORD dwStackSize,          // Stack size
    DWORD dwPriority,           // Thread priority
    DWORD dwFlags,              // Creation flags
    PHANDLE phTask              // Output: task handle
);
```

**Required implementation:** Create a native thread or queue the work to a thread pool, return a valid handle.

### 8.3.3 XamTaskCloseHandle Is a Stub

**Location:** `imports.cpp` line 5476

```cpp
uint32_t XamTaskCloseHandle(uint32_t taskHandle)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}
```

This is less critical but should properly clean up task handles.

## 8.4 Boot-Critical Progression Gates

### 8.4.1 Content/Device Workflow (`sub_827DDE30`)

This function orchestrates content mounting and requires:
1. `sub_829A2590` to start an open operation (returns handle or -1)
2. `sub_829A1F00` to drive async I/O completion
3. `sub_829A1CA0` (XamContentClose wrapper) to return non-997

**Gate condition:** Primary state must be 8, and sub_829A1F00 must return non-zero.

### 8.4.2 Task + Mount/Read Integration (`sub_829A3318`)

This is a boot-adjacent orchestrator that:
1. Calls `sub_829A2AE0` (filesystem open + volume sizing)
2. Registers title terminate notifications
3. Enters a wait loop gated by `XamTaskShouldExit`

**Gate condition:** `XamTaskShouldExit()` must return 0 for the loop to continue processing.

**Current behavior:** Loop exits immediately because XamTaskShouldExit returns 1.

### 8.4.3 Async Status Helper (`sub_829A1A50`)

Returns protocol signals:
- `996`: "no progress, retry later"
- `0`: "operation complete"
- Other: "error or alternate state"

**Gate condition:** Must eventually return 0 for state machines to advance.

## 8.5 Main Thread Block Point Analysis

Based on the call chain and stub analysis, the main thread is likely blocked at:

1. **Early boot:** `sub_829A3318` or similar task orchestrator
2. **Cause:** XamTaskShouldExit returns 1, causing task loops to exit prematurely
3. **Effect:** Subsystem initialization never completes, preventing main loop entry

The game's boot sequence appears to be:
```
main() → KiSystemStartup() → LdrLoadModule() → GuestThread::Start()
    → [Guest entrypoint]
        → System initialization (XAM tasks scheduled)
        → Content mounting (waits for XAM task completion)
        → GPU initialization (completes - special-cased)
        → Main loop entry (NEVER REACHED)
            → sub_828529B0 (main loop)
                → sub_828507F8 (present)
                    → sub_829D5388 (VdSwap wrapper)
```

The block occurs between "Content mounting" and "Main loop entry" because XAM task completion never signals.

## 8.6 Required Kernel Semantics

### 8.6.1 Async I/O Completion (XXOVERLAPPED)

**Current implementation status:** Partially implemented

**XXOVERLAPPED structure:**
```cpp
struct XXOVERLAPPED {
    be<uint32_t> Error;           // 0x00: Result code
    be<uint32_t> Length;          // 0x04: Bytes transferred
    be<uint32_t> InternalContext; // 0x08: Internal
    be<uint32_t> hEvent;          // 0x0C: Event to signal
    be<uint32_t> pCompletionRoutine; // 0x10: APC routine
    be<uint32_t> dwCompletionContext; // 0x14: APC context
    be<uint32_t> dwExtendedError; // 0x18: Extended error
};
```

**What game expects:**
1. Error field set to 0 on success, error code on failure
2. Length field set to bytes transferred
3. hEvent signaled when operation completes
4. pCompletionRoutine called if provided (APC)

**Current behavior:** Error/Length/hEvent are set correctly. pCompletionRoutine (APC) is NOT called.

### 8.6.2 Event Signaling (KeSetEvent, KeWaitForSingleObject)

**Current implementation status:** Implemented

**KeSetEvent:** Correctly signals Event objects and uses atomic generation counter.

**KeWaitForSingleObject:** Implemented with:
- Type-based dispatch (Event, Semaphore, etc.)
- GPU init special-casing for callers 0x829DDD48, 0x829D8AA8
- Infinite wait logging

### 8.6.3 XAM Task Scheduling

**Current implementation status:** NOT IMPLEMENTED (stubs only)

**Required behavior:**

**XamTaskSchedule:**
1. Create a task descriptor with function pointer and context
2. Assign a unique task handle
3. Either:
   - Immediately execute the task on a worker thread, OR
   - Queue it for later execution
4. Return handle to caller

**XamTaskShouldExit:**
1. Return 0 while task should continue running
2. Return 1 only when system requests termination

**XamTaskCloseHandle:**
1. Wait for task completion if still running
2. Release task resources
3. Invalidate handle

### 8.6.4 Thread Barriers and Job Queues

The game creates 9 worker threads that idle on events. These appear to be correctly parked.

The main thread waits for initialization signals from XAM tasks. This is the block point.

### 8.6.5 GPU Sync Primitives

**Current implementation status:** WORKING

GPU initialization is special-cased in KeWaitForSingleObject:
- After 10 poll iterations, GPU flags are forced true
- Ring buffer read pointer writeback is faked
- Graphics interrupt callback is fired

## 8.7 Why VdSwap Is Never Reached (Causal Explanation)

1. **Boot sequence starts XAM tasks** for content mounting and initialization
2. **XamTaskSchedule is a stub** — tasks are never actually scheduled
3. **XamTaskShouldExit returns 1** — any task loops exit immediately
4. **Initialization state machines stall** — they wait for task completion signals that never come
5. **Main loop is gated by initialization completion** — this gate is never opened
6. **sub_828529B0 (main loop) is never called** — therefore sub_828507F8 → sub_829D5388 → VdSwap never executes

The game does not fail loudly because:
- It's designed for cooperative async scheduling
- Stall conditions result in infinite waits, not crashes
- The block is in a waiting state, not an error state

# 9. Minimum Required Implementations for GTA IV to Enter Render Loop

## 9.1 Critical (Blocks Main Thread)

| Function/Subsystem | Required Behavior | What Happens If Missing | Blocks |
|---|---|---|---|
| **XamTaskShouldExit** | Return 0 while task is running; return 1 only on system shutdown | Task loops exit immediately, initialization never completes | Main thread |
| **XamTaskSchedule** | Create async task, assign handle, schedule execution | Async tasks never run, completion signals never fire | Main thread + workers |
| **XamTaskCloseHandle** | Clean up task resources, wait for completion if needed | Resource leaks, handle reuse issues | Both |

## 9.2 High Priority (May Block Subsystems)

| Function/Subsystem | Required Behavior | What Happens If Missing | Blocks |
|---|---|---|---|
| **XXOVERLAPPED.pCompletionRoutine** | Call APC callback when async operation completes | Some async operations may not signal completion | Workers |
| **NtWaitForMultipleObjectsEx** | Wait on multiple kernel objects | Multi-object waits hang forever | Both |
| **Timer objects (NtCreateTimer, NtSetTimerEx)** | Schedule timed callbacks | Timed events never fire | Workers |

## 9.3 Medium Priority (Affects Functionality)

| Function/Subsystem | Required Behavior | What Happens If Missing | Blocks |
|---|---|---|---|
| **XamEnumerate** | Return content enumeration results with proper pending semantics | Content enumeration may hang | Main thread |
| **XamContentClose** | Close content with proper pending/completion semantics | Cleanup may hang | Main thread |
| **ObCreateSymbolicLink / ObDeleteSymbolicLink** | Create/delete symbolic links for mount points | Mount point management broken | Neither |

## 9.4 Immediate Action Items

1. **Fix XamTaskShouldExit** (1 line change):
   ```cpp
   uint32_t XamTaskShouldExit(uint32_t taskHandle)
   {
       return 0;  // Tasks should continue running
   }
   ```

2. **Implement XamTaskSchedule** (requires thread pool or immediate execution):
   ```cpp
   uint32_t XamTaskSchedule(
       uint32_t funcAddr, uint32_t context, uint32_t processId,
       uint32_t stackSize, uint32_t priority, uint32_t flags,
       be<uint32_t>* phTask)
   {
       // Option A: Execute immediately on current thread
       // Option B: Queue to thread pool and return handle
       // Either way, set *phTask to valid handle
   }
   ```

3. **Add logging to main loop entry** to confirm when/if it's reached:
   ```cpp
   // In sub_828529B0 hook or at its entry point
   LOGF_IMPL(Utility, "MainLoop", "MAIN LOOP ENTERED!");
   ```

## 9.5 Verification Steps

After implementing fixes:

1. **Check log for XamTaskSchedule calls** — should see task scheduling
2. **Check log for XamTaskShouldExit calls** — should see many 0 returns
3. **Check log for "VdSwap frame"** — should see frame presentation
4. **Observe window** — should see rendered frames

If VdSwap is still not called after XamTask fixes:
1. Add logging to sub_828529B0 entry
2. Add logging to sub_828507F8 entry
3. Trace what condition prevents main loop entry
