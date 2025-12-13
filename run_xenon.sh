#!/bin/bash
set -e
cd /Users/Ozordi/Downloads/MarathonRecomp/LibertyRecompLib/config
echo "Current dir: $(pwd)"
echo "XEX file: $(ls -la '../../Grand Theft Auto IV (USA) (En,Fr,De,Es,It)/default.xex')"
echo "Running XenonRecomp..."
time ../../tools/XenonRecomp/build_find/XenonRecomp/XenonRecomp ./GTA4.toml ../../tools/XenonRecomp/XenonUtils/ppc_context.h
echo "Finished with exit code: $?"
echo "PPC directory contents:"
ls -la ../ppc/
