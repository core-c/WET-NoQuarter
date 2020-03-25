#!/bin/sh

ZIPPATH="/usr/bin/"

if [ -x $ZIPPATH/zip ]
then
	zip -m mp_bin.pk3 cgame.* ui.*
	zip noquarter-bin.zip mp_bin.pk3 qagame.*
	if [ $# -gt 0 ]
	then
		mv mp_bin.pk3 $1
		mv qagame.* $1
	fi
else
	echo "ZIP command not found"
	exit 2
fi
