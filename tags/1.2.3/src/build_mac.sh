#!/bin/sh
# Simple mac scons builder, getting tired of cd and stuff and so
echo "### SCONS Mac Shortcut ###"
echo "### By Arkox          ###"
EXEC_DIR=`dirname "$0"`
echo ### changing dir to $EXEC_DIR
cd "$EXEC_DIR"
scons
echo "### COMPILING QAGAME AND BUNDLES ###"
sh makebundle.sh qagame
echo "### BUILD DONE ###"
