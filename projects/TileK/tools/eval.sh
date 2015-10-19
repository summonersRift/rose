#!/bin/bash -e

#####
# 
# Usage: ./eval.sh -d output_directory -b binary.exe -a arguments.csv
# 
# Execute 'binary.exe' for each combinaison of arguments contained in 'arguments.csv'.
# Execute in a subdirectory of 'output_directory' which name is a composite of binary's name and the arguments.
# 
#####

script_dir=$(readlink -f $(dirname $0))
script_name=$(basename $0)

function usage() {
  echo "Usage:"
  echo " > $script_name -h"
  echo " > $script_name --stem stem --config config"
  echo "           [ --bindir \$(pwd) | --binary=\$bindir/\$stem ]"
  echo "           [ --argdir \$(pwd) | --args \$argdir/\$stem.csv ]"
  echo "           [ --outdir \$(pwd) ]"
  echo "           [ --timeout 1m ]"
  echo "           [ --reps 3 ] "
  echo "   -s: "
  echo "   -d: "
  echo "   -b: "
  echo "   -a: "
}

stem=""
config=""
bindir=""
outdir=""
args=""
timeout=""
reps=""

if [ -z $1 ]; then
  usage
  exit 2
fi
if [ "$1" == "-h" ]; then
  usage
  exit 0
fi
while [ ! -z $1 ]; do
  if   [ "$1" == "--stem"    ]; then stem=$2    ; shift 2
  elif [ "$1" == "--bindir"  ]; then bindir=$2  ; shift 2
  elif [ "$1" == "--argdir"  ]; then argdir=$2  ; shift 2
  elif [ "$1" == "--args"    ]; then args=$2    ; shift 2
  elif [ "$1" == "--cfgdir"  ]; then cfgdir=$2  ; shift 2
  elif [ "$1" == "--configs" ]; then configs=$2 ; shift 2
  elif [ "$1" == "--rundir"  ]; then rundir=$2  ; shift 2
  elif [ "$1" == "--timeout" ]; then timeout=$2 ; shift 2
  elif [ "$1" == "--reps"    ]; then reps=$2    ; shift 2
  else
    echo "Error unrecognized option: $1"
    usage
    exit 2
  fi
done

#####

echo "####################"

echo "TileK Eval Script"

[ -z "$stem" ] && usage && exit 1
echo " > stem    = $stem"

# bindir

[ -z "$bindir" ]  && bindir=$(pwd)
echo " > bindir  = $bindir"

# Rundir

[ -z "$rundir" ]  && rundir=$(pwd)
echo " > rundir  = $rundir"

# Configuration

[ -z "$cfgdir" ]  && cfgdir=$(pwd)
echo " > cfgdir  = $cfgdir"

[ -z "$configs" ]    && configs=$cfgdir/$stem-configs.csv
configs=$(readlink -f $configs)

# Arguments

[ -z "$argdir" ]  && argdir=$(pwd)
echo " > argdir  = $argdir"

[ -z "$args" ]    && args=$argdir/$stem-args.csv
args=$(readlink -f $args)
echo " > args    = $args"

# Parameters

[ -z "$timeout" ] && timeout="1m"
echo " > timeout = $timeout"

[ -z "$reps" ]    && reps="3"
echo " > reps    = $reps"

#####

[ ! -e $args ]   && echo "Error: $args does not exist."   && exit 3

#####

echo "####################"

for config in $(cat $configs); do
for arg in $(cat $args); do

  tag=$(echo $arg | cut -d',' -f1)
  arguments=$(echo $arg | cut -d',' -f2- | tr ',' ' ')

  binary=$(readlink -f $bindir/$stem/$config/$stem-$config)

  mkdir -p $rundir/$stem/$config/$tag
  pushd $rundir/$stem/$config/$tag > /dev/null

  echo "Execute \"$config\" \"$tag\": \"$arguments\""

  for i in $(seq 1 $reps); do
    timeout $timeout $binary $arguments
    echo -ne "\r         \r    $i/$reps"
  done
  echo

  popd > /dev/null
done
done

echo "####################"

