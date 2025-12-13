#!/usr/bin/env python3
import subprocess
import os
import sys

os.chdir("/Users/Ozordi/Downloads/MarathonRecomp/LibertyRecompLib/config")

result = subprocess.run(
    [
        "../../tools/XenonRecomp/build_find/XenonRecomp/XenonRecomp",
        "./GTA4.toml",
        "../../tools/XenonRecomp/XenonUtils/ppc_context.h"
    ],
    capture_output=True,
    text=True,
    timeout=120
)

print("=== STDOUT ===")
print(result.stdout)
print("=== STDERR ===")
print(result.stderr)
print("=== RETURN CODE ===")
print(result.returncode)
