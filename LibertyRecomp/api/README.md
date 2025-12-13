# Liberty

## Contribution Guide

### Naming Conventions

- Use `camelCase` for local variables, `SNAKE_CASE` for preprocessor macros, and `PascalCase` for everything else. Liberty-specific types that don't exist in the game should use `snake_case` for better differentiation.
- Class members should be prefixed with `m_`, e.g., `m_Time`. Do not use this prefix for struct members.
- Pointers should be prefixed with `p`, e.g., `pPlayerPed`.
- Shared pointers should be prefixed with `sp`, e.g., `spDatabase`.
- References should be prefixed with `r`, e.g., `rMessage`.
- Static members outside a class should be prefixed with `g_`, e.g., `g_AllocationTracker`.
- Liberty-specific preprocessor macros should start with `LIBERTY_`, e.g., `LIBERTY_INSERT_PADDING`.
- Function pointers should be prefixed with `fp`, e.g., `fpCEntityConstructor`.

Combine prefixes as necessary, e.g., `m_sp` for a shared pointer as a class member.

### Coding Style

- Always place curly brackets on a new line.
- Prefer forward declaring types over including their respective headers.
- Use <> includes relative to the project's root directory path.
- Use C++17's nested namespace feature instead of defining multiple namespaces on separate lines.
- Enum classes are prohibited as they were not available when the game was developed.
- Avoid placing function definitions in .h files, instead, implement functions in the header's respective .inl file, similar to a .cpp file.
- Ensure that all class members are declared as public. Even if you suspect that a class member was private in the original code, having it public is more convenient in a modding API.
- Avoid placing multiple class definitions in a single header file unless you have a good reason to do so.
- Keep function pointers or addresses outside functions, define them as global variables in the corresponding .inl file. Mark these global variables as `inline` and never nest them within class definitions. You do not need to use the `g_` prefix for function pointers, `fp` is sufficient.
- Use primitive types defined in `cstdint` instead of using types that come with the language, e.g., use `uint32_t` instead of `unsigned int`. Using `float`, `double` and `bool` is okay.

### Mapping Rules

- Always include the corresponding `offsetof`/`sizeof` assertions for mapped classes/structs. If you are uncertain about the type's size, you can omit the `sizeof` assertion.
- Use the exact type name from the game if it's available through RTTI, otherwise, you can look for shared pointers that may reveal the original type name.
- If you are unsure about the name of a class/struct member, use `Field` followed by the hexadecimal byte offset (e.g., `m_Field194`). Avoid names like `m_StoresThisThingMaybe`, you can write comments next to the definition for speculations.
- If a portion of the byte range is irrelevant to your research or not mapped yet, use the `LIBERTY_INSERT_PADDING` macro to align class/struct members correctly.
- When the class has a virtual function table, if you don't want to map every function in it, you can map only the virtual destructor.
- The original file locations are likely available in the executable file as assertion file paths. If you cannot find the file path, use your intuition to place the file in a sensible place.