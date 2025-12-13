#!/bin/bash
exec 2>&1

cd /Users/Ozordi/Downloads/MarathonRecomp/LibertyRecompLib/config

echo "=== Running XenonRecomp ==="
echo "PWD: $(pwd)"
echo "TOML: GTA4.toml"
echo "PPC Context: ../../tools/XenonRecomp/XenonUtils/ppc_context.h"
echo ""

../../tools/XenonRecomp/build_find/XenonRecomp/XenonRecomp \
    ./GTA4.toml \
    ../../tools/XenonRecomp/XenonUtils/ppc_context.h

EXITCODE=$?
echo ""
echo "=== Exit code: $EXITCODE ==="
echo ""
echo "=== PPC directory contents ==="
ls -la ../ppc/
