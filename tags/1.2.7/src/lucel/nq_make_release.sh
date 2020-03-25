#!/bin/bash

source nq_constants.sh

cd $NQ_PATH/trunk/src
./build_mac.sh &&
cd build &&
zip cgame_mac -r cgame_mac.bundle &&
zip ui_mac -r ui_mac.bundle &&
mv -f cgame_mac.zip cgame_mac &&
mv -f ui_mac.zip ui_mac &&
zip nq_bin.zip cgame_mac ui_mac &&
mv -f nq_bin.zip ..
