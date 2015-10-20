#!/bin/bash

#####
# 
# Usage: ./collect-TileTrees.sh -i input_directory -b binary.exe -a arguments.csv -o output_directory
# 
# After evaluation (eval.sh) of 'binary.exe' for 'arguments.csv' in 'input_directory', this script build the TileTrees and store them in 'output_directory'
# 
#####

script_dir=$(readlink -f $(dirname $0))

function usage() {
  echo "Usage:"
  echo " > $0 -h"
  echo " > $0 -i input_directory -b binary.exe -a arguments.csv -o output_directory"
  echo "   -i: "
  echo "   -b: "
  echo "   -a: "
  echo "   -o: "
}

stem=""
config=""
argdir=""
args=""
bindir=""
rundir=""
outdir=""

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
  elif [ "$1" == "--argdir"  ]; then argdir=$2  ; shift 2
  elif [ "$1" == "--args"    ]; then args=$2    ; shift 2
  elif [ "$1" == "--cfgdir"  ]; then cfgdir=$2  ; shift 2
  elif [ "$1" == "--configs" ]; then configs=$2 ; shift 2
  elif [ "$1" == "--bindir"  ]; then bindir=$2  ; shift 2
  elif [ "$1" == "--rundir"  ]; then rundir=$2  ; shift 2
  elif [ "$1" == "--outdir"  ]; then outdir=$2  ; shift 2
  else
    echo "Error unrecognized option: $1"
    usage
    exit 2
  fi
done

#####

echo "####################"

echo "TileK Collect TileTree Script"

[ -z "$stem" ] && usage && exit 1
echo " > stem    = $stem"

# Arguments

[ -z "$argdir" ]  && argdir=$(pwd)
echo " > argdir  = $argdir"

[ -z "$args" ]    && args=$argdir/$stem-args.csv
args=$(readlink -f $args)
echo " > args    = $args"

# Configuration

[ -z "$cfgdir" ]  && cfgdir=$(pwd)
echo " > cfgdir  = $cfgdir"

[ -z "$configs" ]    && configs=$cfgdir/$stem-configs.csv
configs=$(readlink -f $configs)

# bindir

[ -z "$bindir" ]  && bindir=$(pwd)
echo " > bindir  = $bindir"

# Rundir

[ -z "$rundir" ]  && rundir=$(pwd)
echo " > rundir  = $rundir"

# Output

[ -z "$outdir" ]  && outdir=$(pwd)
echo " > outdir  = $outdir"

#####

[ ! -e $args ] && echo "Error: $args does not exist." && exit 3

#####

shopt -s expand_aliases

[ -z "$KLT" ] && echo "Error: environment variable \$KLT is not defined!" && exit 2

$KLT

#####

mkdir -p $outdir

config=$(head -n 1 $configs | cut -d'|' -f1)
dot -Tsvg $bindir/$stem/$config/$stem-$config\_kernel_0_looptree.dot -o $outdir/$stem-$config-looptree.svg
dot -Tpng $bindir/$stem/$config/$stem-$config\_kernel_0_looptree.dot -o $outdir/$stem-$config-looptree.png

cat $configs | while read config; do
config=$(echo $config | cut -d'|' -f1)
for arg in $(cat $args); do
  tag=$(echo $arg | cut -d',' -f1)

  looptree=$bindir/$stem/$config/$stem-$config\_kernel_0_subkernel_0_looptree.json
  loop_ctx=$rundir/$stem/$config/$tag/$stem-$config\_kernel_0_subkernel_0_loop_ctx.json
  tiletree=$outdir/$stem-$config-$tag-tiletree

  klt-build-TileTree-JSONs $looptree $loop_ctx $tiletree.json
  klt-TileTree-JSON-to-GraphViz $tiletree.json > $tiletree.dot
  dot -Tpng $tiletree.dot -o $tiletree.png
done
done

