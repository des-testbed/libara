#/bin/bash

RELATIVE_PATH_TO_ROOT="../.."
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$RELATIVE_PATH_TO_ROOT/src:$RELATIVE_PATH_TO_ROOT/inetmanet/src"
cd $DIR && $RELATIVE_PATH_TO_ROOT/omnetpp/ara-sim -r 0 -n $RELATIVE_PATH_TO_ROOT/inetmanet/src:$RELATIVE_PATH_TO_ROOT/inetmanet/examples:.:$RELATIVE_PATH_TO_ROOT/omnetpp omnetpp.ini
