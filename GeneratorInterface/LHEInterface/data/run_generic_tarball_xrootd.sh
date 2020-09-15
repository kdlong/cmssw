#!/bin/bash

set -e

echo "   ______________________________________     "

if [ $# -lt 1 ]; then
    echo "%MSG-ExternalLHEProducer-subprocess ERROR in external process. At least one argument required with format root://<xrd_path>/<path>"
fi
if [[ $1 != "root://"* ]]; then
    echo "%MSG-ExternalLHEProducer-subprocess ERROR in external process. Path must have format root://<xrd_path>/<path>"
    exit 1
fi 

xrd_path=$1
gridpack=gridpack/$(basename $xrd_path)

if [ ! -d gridpack ]; then
    mkdir gridpack
fi

if [ -e $gridpack ]; then
    echo "%MSG-ExternalLHEProducer-subprocess WARNING: File $gridpack already exists, it will be overwritten."
    rm $gridpack
fi

xrdcp $xrd_path gridpack

echo "%MSG-ExternalLHEProducer-subprocess INFO: Acesssing $path from $xrd_redirector with xrootd"
path=`pwd`/$gridpack
generic_script=$(dirname ${0})/run_generic_tarball_cvmfs.sh 
. $generic_script $path ${@:2}
