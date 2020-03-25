#!/bin/bash

source nq_constants.sh

cd $NQ_PATH/trunk/src
scons PROFILER=0 && 
./makebundle.sh qagame.mp.ppc.so && 
rm "$NQ_SERVER_PATH/qagame_mac" &&
cp build/qagame_mac.bundle/Contents/MacOS/qagame_mac "$NQ_SERVER_PATH/"
