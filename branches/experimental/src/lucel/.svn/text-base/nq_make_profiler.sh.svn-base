#!/bin/bash

source nq_constants.sh

cd $NQ_PATH/trunk/src
scons PROFILER=1 && 
./makebundle.sh profiler.mp.ppc.so
rm "$NQ_SERVER_PATH/profiler_mac"
cp build/profiler_mac.bundle/Contents/MacOS/profiler_mac "$NQ_SERVER_PATH/"
