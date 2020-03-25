#!/bin/bash

source nq_constants.sh

cd $NQ_PATH/trunk/src
scons BUILD=$NQ_BUILD PROFILER=0 &&
./makebundle.sh cgame.mp.ppc.so &&
./makebundle.sh ui.mp.ppc.so &&
./makebundle.sh qagame.mp.ppc.so &&
cd build &&
zip cgame_mac -r cgame_mac.bundle &&
zip ui_mac -r ui_mac.bundle &&
mv cgame_mac.zip cgame_mac &&
mv ui_mac.zip ui_mac &&
zip nq_bin.zip cgame_mac ui_mac &&
mv nq_bin.zip .. &&
cd .. &&
zip nq_bin.zip cgame.mp.i386.so cgame_mp_x86.dll ui.mp.i386.so ui_mp_x86.dll &&
mv nq_bin.zip $NQ_BIN &&
cd build &&
zip nq_build.zip -r qagame_mac.bundle &&
mv nq_build.zip .. &&
cd .. &&
zip nq_build.zip qagame.mp.i386.so qagame_mp_x86.dll $NQ_BIN &&
mv nq_build.zip nq_build_$NQ_VERSION.zip
