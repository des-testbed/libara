#/bin/bash

RELATIVE_PATH_TO_ROOT="../.."
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$RELATIVE_PATH_TO_ROOT/src:$RELATIVE_PATH_TO_ROOT/inetmanet/src"

nedPath="$RELATIVE_PATH_TO_ROOT/inetmanet/src:$RELATIVE_PATH_TO_ROOT/inetmanet/examples:.:$RELATIVE_PATH_TO_ROOT/omnetpp"

if [ "$#" -lt "1" ]; then
	echo "ERROR: too few arguments for rum script"
	echo "Usage: runCmdEnv.sh <experiment_name> <optional_number_of_runs>"
fi

experimentName=$1

# Starts a single OMNeT++ simulation run
# $1: run number or "*" for all
function runSimulation() {
	if [ $1 == "*" ]; then
		logName=$experimentName-Log.txt
		echo "Running simulation. Log is saved to results/$logName..."
	else
		logName=$experimentName-$1-Log.txt
		echo "Run $(($1+1))/$numberOfRuns: Log is saved to results/$logName..."
	fi
	$RELATIVE_PATH_TO_ROOT/omnetpp/ara-sim -r $1 -u Cmdenv -c $experimentName -n "$nedPath" omnetpp.ini > $DIR/results/$logName
}

cd $DIR
echo "Running simulation [$experimentName]"

mkdir --parents "./results"

echo "Removing all files that match './results/$experimentName-*'"
rm -f "$DIR/results/$experimentName-"*

if [ "$#" -ge "2" ]; then
	numberOfRuns=$2
    for ((i=0; $i<$numberOfRuns; i++)); do
        runSimulation $i
    done
    echo ""
    echo "All simulation runs finished"
else
	runSimulation $experimentName *
fi

