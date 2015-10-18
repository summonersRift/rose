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
  elif [ "$1" == "--config" ];  then config=$2  ; shift 2
  elif [ "$1" == "--bindir"  ]; then bindir=$2  ; shift 2
  elif [ "$1" == "--binary"  ]; then binary=$2  ; shift 2
  elif [ "$1" == "--argdir"  ]; then argdir=$2  ; shift 2
  elif [ "$1" == "--args"    ]; then args=$2    ; shift 2
  elif [ "$1" == "--outdir"  ]; then outdir=$2  ; shift 2
  elif [ "$1" == "--timeout" ]; then timeout=$2 ; shift 2
  elif [ "$1" == "--reps"    ]; then reps=$2    ; shift 2
  else
    echo "Error unrecognized option: $1"
    usage
    exit 2
  fi
done

echo "####################"

echo "TileK Eval Script"

[ -z "$stem" ] && usage && exit 1
echo " > stem    = $stem"

[ -z "$config" ] && usage && exit 1
echo " > config    = $config"

[ -z "$bindir" ]  && bindir=$(pwd)/$stem/$config
echo " > bindir  = $bindir"

[ -z "$binary" ]  && binary=$bindir/$stem-$config
echo " > binary  = $binary"

[ -z "$outdir" ]  && outdir=$(pwd)/$stem/$config
echo " > outdir  = $outdir"

[ -z "$argdir" ]  && argdir=$(pwd)
echo " > argdir  = $argdir"

[ -z "$args" ]    && args=$argdir/$stem.csv
echo " > args    = $args"

[ -z "$timeout" ] && timeout="1m"
echo " > timeout = $timeout"

[ -z "$reps" ]    && reps="3"
echo " > reps    = $reps"

#####

[ ! -e $binary ] && echo "Error: $binary does not exist." && exit 3
[ ! -e $args ]   && echo "Error: $args does not exist."   && exit 3

#####

echo "####################"

mkdir -p $outdir
pushd $outdir > /dev/null

for arg in $(cat $args); do
  tag=$(echo $arg | cut -d',' -f1)
  arguments=$(echo $arg | cut -d',' -f2- | tr ',' ' ')

  mkdir -p $tag
  pushd $tag > /dev/null

  echo "Execute \"$tag\": \"$arguments\""

  for i in $(seq 1 $reps); do
    timeout $timeout $binary $arguments
    echo -ne "\r         \r    $i/$reps"
  done
  echo

  popd > /dev/null
done

popd > /dev/null

echo "####################"

