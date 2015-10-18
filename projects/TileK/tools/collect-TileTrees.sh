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

indir=""
binary=""
arguments=""
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
  if   [ "$1" == "-i" ]; then indir=$2 ; shift 2
  elif [ "$1" == "-b" ]; then binary=$2     ; shift 2
  elif [ "$1" == "-a" ]; then arguments=$2 ; shift 2
  elif [ "$1" == "-o" ]; then outdir=$2 ; shift 2
  else
    echo "Error unrecognized option: $1"
    usage
    exit 2
  fi
done

[ -z "$indir" ]     && echo "Error: Need input directory (-i)" && exit 1
[ -z "$binary" ]    && echo "Error: Need input binary (-b)" && exit 1
[ -z "$arguments" ] && echo "Error: Need arguments CSV (-a)" && exit 1
[ -z "$outdir" ]    && echo "Error: Need output directory (-o)" && exit 1

#####



#####

mkdir -p $outdir
pushd $outdir > /dev/null



popd > /dev/null

