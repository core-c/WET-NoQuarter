#!/bin/sh
PREFIX=/opt/mingw-4.0.1/ 
TARGET=i386-mingw32 
#PREFIX=/usr/local/cross-tools 
#TARGET=i386-mingw32msvc 
PATH="$PREFIX/bin:$PREFIX/$TARGET/bin:$PATH"
export PATH
exec scons $*
