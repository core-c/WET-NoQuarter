#!/bin/sh
# Simple mac scons builder, getting tired of cd and stuff and so
echo "### SCONS Mac Shortcut ###"
echo "### By Arkox          ###"
EXEC_DIR=`dirname "$0"`
echo ### changing dir to $EXEC_DIR
cd "$EXEC_DIR"
scons WITH_OMNIBOT='0' WITH_LUA='0'

echo "### COMPILING QAGAME AND BUNDLES ###"
sh makebundle.sh qagame
echo "### COMPILING CGAME AND BUNDLES ###"
sh makebundle.sh cgame
echo "### COMPILING UI AND BUNDLES ###"
sh makebundle.sh ui
echo "### BUILD DONE ###"
