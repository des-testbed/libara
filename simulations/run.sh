#/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR && ../omnetpp/ara-sim -r 0 -n ../inetmanet/src:../inetmanet/examples:.:../omnetpp omnetpp.ini
