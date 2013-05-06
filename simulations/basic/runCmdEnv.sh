#/bin/bash

RELATIVE_PATH_TO_ROOT="../.."
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$RELATIVE_PATH_TO_ROOT/src:$RELATIVE_PATH_TO_ROOT/inetmanet/src"

if [ "$#" -ge "1" ]; then
    echo "Running simulation $1"
    LOG_NAME="$1"
    CONF_SECTION=" -c $1"
    # Delete old result files
    echo "Removing all files that match './results/$1-*'"
    rm -f "$DIR/results/$1-"*
else
    LOG_NAME="Simulation"
    CONF_SECTION=""
fi

if [ "$#" -ge "2" ]; then
    for ((i=0; $i<$2; i++)); do
        echo "Run $(($i+1))/$2: : Log is saved to results/$LOG_NAME-$i-Log.txt..."
        cd $DIR && $RELATIVE_PATH_TO_ROOT/omnetpp/ara-sim -r $i -u Cmdenv $CONF_SECTION -n $RELATIVE_PATH_TO_ROOT/inetmanet/src:$RELATIVE_PATH_TO_ROOT/inetmanet/examples:.:$RELATIVE_PATH_TO_ROOT/omnetpp omnetpp.ini > $DIR/results/$LOG_NAME-$i-Log.txt
    done
    echo ""
    echo "All simulation runs finished"
else
    echo "Running simulation. Log is saved to $DIR/results/$LOG_NAME-Log.txt..."
    cd $DIR && $RELATIVE_PATH_TO_ROOT/omnetpp/ara-sim -r * -u Cmdenv $CONF_SECTION -n $RELATIVE_PATH_TO_ROOT/inetmanet/src:$RELATIVE_PATH_TO_ROOT/inetmanet/examples:.:$RELATIVE_PATH_TO_ROOT/omnetpp omnetpp.ini > $DIR/results/$LOG_NAME-Log.txt
fi

