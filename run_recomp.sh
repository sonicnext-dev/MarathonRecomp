#!/bin/bash
set -x
cd /Users/Ozordi/Downloads/MarathonRecomp

# Check all files exist
echo "Checking files..."
ls -la "./LibertyRecompLib/config/GTA4.toml"
ls -la "./tools/XenonRecomp/XenonUtils/ppc_context.h"
ls -la "./Grand Theft Auto IV (USA) (En,Fr,De,Es,It)/default.xex"
ls -la "./LibertyRecompLib/config/gta4_switch_tables.toml"

echo ""
echo "Running XenonRecomp..."
./tools/XenonRecomp/build_find/XenonRecomp/XenonRecomp \
    "./LibertyRecompLib/config/GTA4.toml" \
    "./tools/XenonRecomp/XenonUtils/ppc_context.h"
echo "Exit code: $?"
