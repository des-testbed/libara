#/bin/bash

RELATIVE_PATH_TO_ROOT="../.."
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$RELATIVE_PATH_TO_ROOT/src:$RELATIVE_PATH_TO_ROOT/inetmanet/src"

nedPath="$RELATIVE_PATH_TO_ROOT/inetmanet/src:$RELATIVE_PATH_TO_ROOT/inetmanet/examples:.:$RELATIVE_PATH_TO_ROOT/omnetpp"
cd $DIR

# Starts a single OMNeT++ simulation run on the Cmdenv
# $1: run number or "*" for all
function runSimulation() {
	if [ "$1" = "*" ]; then
		logName=$experimentName-Log.txt
		echo "Running simulation. Log is saved to $DIR/results/$logName"
		$RELATIVE_PATH_TO_ROOT/omnetpp/ara-sim -u Cmdenv -c $experimentName -n "$nedPath" omnetpp.ini > $DIR/results/$logName
	else
		logName=$experimentName-$1-Log.txt
		echo "Run $(($1+1))/$numberOfRuns: Log is saved to $DIR/results/$logName"
		$RELATIVE_PATH_TO_ROOT/omnetpp/ara-sim -r "$1" -u Cmdenv -c $experimentName -n "$nedPath" omnetpp.ini > $DIR/results/$logName
	fi
}

if [ $# -eq 0 ]; then
	# just run the simulation with the GUI
	$RELATIVE_PATH_TO_ROOT/omnetpp/ara-sim -r 0 -n "$nedPath" omnetpp.ini
else
	if [ $1 == "--help" ]; then
		scriptName=`basename $0`
		echo "libARA simulation run script by Friedrich Große"
		echo "Usage: $scriptName [<experiment_name> [<optional_number_of_runs> | --debug] | --help]"
		echo "Description:"
		echo "    - You can run the GUI if no parameters are supplied."
		echo "    - If you anter an experiment name as first parameter the simulation is run on the command line"
		echo "    - You can additionally define the number of repetitions for more statistically meaningful results"
		echo "    - If you want to run the debugger (nemiver) on the given experiment you can use --debug as second parameter"
		exit 1
	fi
	
	# normally the first parameter contains the experiment (scenario) name to run on Cmdenv
	experimentName=$1
	
	echo "Running simulation [$experimentName]"
	
	# prepare the results directory
	mkdir --parents "./results"
	
	echo "Removing all files that match './results/$experimentName-*'"
	rm -f "$DIR/results/$experimentName-"*
	
	if [ $# -ge 2 ]; then
		if [ $2 == "--debug" ]; then
			echo "Running nemiver debug..."
			nemiver $RELATIVE_PATH_TO_ROOT/omnetpp/ara-sim -r 0 -u Cmdenv -c $experimentName -n "$nedPath" omnetpp.ini
		else
			numberOfRuns=$2
		    for ((i=0; $i<$numberOfRuns; i++)); do
		        runSimulation $i
		    done
		    echo ""
		    echo "All simulation runs finished"
	    fi
	else
		runSimulation "*"
	fi
fi
