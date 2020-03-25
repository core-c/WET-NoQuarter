#!/bin/bash

source nq_constants.sh

cd $NQ_PATH/trunk/assets
rm -f $NQ_DATA
zip $NQ_DATA -r * -x models/players/\* -x \*.svn\* \*.DS_Store\*
rm -f "$INSTALLPATH/$NQ_DATA"
mv $NQ_DATA "$NQ_INSTALL_PATH/"  
