#!/bin/bash -e

#####
# 
# Usage: ./compile.sh -d output_directory -i file.c -t TileK_directory
# 
# Compile file.c using TileK and backend chain. The results are stored in 'output_directory'
# 
#####

script_dir=$(readlink -f $(dirname $0))

function usage() {
  echo "Usage:"
  echo " > $0 -h"
  echo " > $0 --stem stem --config config [ --outdir \$(pwd) ] [ --srcdir \$(pwd) ] [ --targets \"\" ] [ --opts \"\" ] [--debug \"\"]"
  echo "   --outdir: "
  echo "   --srcdir: "
  echo "   --targets: "
  echo "   --opts: "
  echo "   --debug: "
  echo "[ script_dir=$script_dir ]"
}

stem=""
config=""
outdir=""
srcdir=""
opts=""
target=""
dbg=""

if [ -z $1 ]; then
  usage
  exit 2
fi
if [ "$1" == "-h" ]; then
  usage
  exit 0
fi
while [ ! -z $1 ]; do
  if   [ "$1" == "--stem" ];    then stem=$2    ; shift 2
  elif [ "$1" == "--config" ];  then config=$2  ; shift 2
  elif [ "$1" == "--outdir" ];  then outdir=$2  ; shift 2
  elif [ "$1" == "--srcdir" ];  then srcdir=$2  ; shift 2
  elif [ "$1" == "--targets" ]; then targets=$2 ; shift 2
  elif [ "$1" == "--opts" ];    then opts=$2    ; shift 2
  elif [ "$1" == "--debug" ];   then dbg=$2     ; shift 2
  else
    echo "Error unrecognized option: $1"
    usage
    exit 2
  fi
done

#####

echo "####################"

echo "TileK Compile Script"

[ -z "$stem" ] && usage && exit 1
echo " > stem    = $stem"

[ -z "$config" ] && usage && exit 1
echo " > config    = $config"

[ -z "$outdir" ] && outdir=$(pwd)/$stem/$config
echo " > outdir  = $directory"

[ -z "$srcdir" ] && srcdir=$(pwd)
echo " > srcdir  = $srcdir"

options=$(echo $opts | tr ',' ' ')
echo " > options = $options"

echo " > targets = $targets"

debug=$(echo $dbg | tr ',' ' ')
echo " > debug = $debug"

#####

shopt -s expand_aliases

[ -z "$TILEK" ] && echo "Error: environment variable \$TILEK is not defined!" && exit 2

echo "$TILEK"
$TILEK

alias tilek > /dev/null

#####

infile=$srcdir/$stem-$config.c
[ ! -e $infile ] && echo "Error: cannot find $infile" && exit 3

echo " > infile  = $infile"

#####

echo "####################"

mkdir -p $outdir
pushd $outdir > /dev/null

KLT_LD_FLAGS="-lrt"
KLT_LD_FLAGS="$KLT_LD_FLAGS $KLT_RTL_HOST"

C_FLAGS="$debug $KLT_INCPATH $options"

echo "tilek --tilek-target=$targets $C_FLAGS $infile &> tilek-$stem-$config.log"
tilek --tilek-target=$targets $C_FLAGS $infile &> tilek-$stem-$config.log

echo "gcc $C_FLAGS -c rose_$stem-$config.c -o rose_$stem-$config.o &> gcc-rose_$stem-$config.log"
gcc $C_FLAGS -c rose_$stem-$config.c -o rose_$stem-$config.o &> gcc-rose_$stem-$config.log

echo "gcc $C_FLAGS -c $stem-$config-host-kernel.c -o $stem-$config-host-kernel.o &> gcc-$stem-$config-host-kernel.log"
gcc $C_FLAGS -c $stem-$config-host-kernel.c -o $stem-$config-host-kernel.o &> gcc-$stem-$config-host-kernel.log

echo "gcc $C_FLAGS -c $stem-$config-static.c -o $stem-$config-static.o &> gcc-$stem-$config-static.log"
gcc $C_FLAGS -c $stem-$config-static.c -o $stem-$config-static.o &> gcc-$stem-$config-static.log

echo "libtool --mode=link gcc $debug rose_$stem-$config.o $stem-$config-host-kernel.o $stem-$config-static.o $KLT_LD_FLAGS -o $stem-$config &> libtool-$stem-$config.log"
libtool --mode=link gcc $debug rose_$stem-$config.o $stem-$config-host-kernel.o $stem-$config-static.o $KLT_LD_FLAGS -o $stem-$config &> libtool-$stem-$config.log

popd > /dev/null

echo "####################"

