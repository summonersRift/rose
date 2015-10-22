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

[ -z "$configs" ]    && configs=$cfgdir/$stem.csv
configs=$(readlink -f $configs)
echo " > configs = $configs"

# Arguments

[ -z "$argdir" ]  && argdir=$(pwd)
echo " > argdir  = $argdir"

[ -z "$args" ]    && args=$argdir/$stem.csv
args=$(readlink -f $args)
echo " > args    = $args"

# Parameters

[ -z "$timeout" ] && timeout="1m"
echo " > timeout = $timeout"

[ -z "$reps" ]    && reps="3"
echo " > reps    = $reps"

#####

[ ! -e $configs ] && echo "Error: $configs does not exist." && exit 3
[ ! -e $args ] && echo "Error: $args does not exist." && exit 3

#####

echo "####################"

cat $configs | while read config; do
config=$(echo $config | cut -d'|' -f1)
for arg in $(cat $args); do

  tag=$(echo $arg | cut -d',' -f1)
  arguments=$(echo $arg | cut -d',' -f2- | tr ',' ' ')

  binary=$(readlink -f $bindir/$stem/$config/$stem-$config)

  test -x $binary

  mkdir -p $rundir/$stem/$config/$tag
  pushd $rundir/$stem/$config/$tag > /dev/null

  export KLT_OPENCL_KERNEL_DIR=$(readlink -f $bindir/$stem/$config)

  for i in $(seq 1 $reps); do
    echo -ne "\r                                                   \rEvaluate \"$config\" \"$tag\" $i/$reps"
    set +e
    timeout $timeout $binary $arguments > $stem-$config-$tag-$i.log
    set -e
  done

  popd > /dev/null
done
done

echo
echo "####################"

