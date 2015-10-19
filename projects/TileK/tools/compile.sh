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
  if   [ "$1" == "--stem"    ]; then stem=$2    ; shift 2
  elif [ "$1" == "--srcdir" ];  then srcdir=$2  ; shift 2
  elif [ "$1" == "--bindir"  ]; then bindir=$2  ; shift 2
  elif [ "$1" == "--cfgdir"  ]; then cfgdir=$2  ; shift 2
  elif [ "$1" == "--configs" ]; then configs=$2 ; shift 2
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

# Source directory

[ -z "$srcdir" ] && srcdir=$(pwd)
echo " > srcdir  = $srcdir"

# bindir

[ -z "$bindir" ]  && bindir=$(pwd)
echo " > bindir  = $bindir"

# Configuration

[ -z "$cfgdir" ]  && cfgdir=$(pwd)
echo " > cfgdir  = $cfgdir"

[ -z "$configs" ]    && configs=$cfgdir/$stem-configs.csv
configs=$(readlink -f $configs)

# Target

echo " > targets = $targets"

# Options

options=$(echo $opts | tr ',' ' ')
echo " > options = $options"

# Debug

debug=$(echo $dbg | tr ',' ' ')
echo " > debug = $debug"

#####

shopt -s expand_aliases

[ -z "$TILEK" ] && echo "Error: environment variable \$TILEK is not defined!" && exit 2

$TILEK

alias tilek > /dev/null

#####

echo "####################"

KLT_LD_FLAGS="-lrt"
KLT_LD_FLAGS="$KLT_LD_FLAGS $KLT_RTL_HOST"

C_FLAGS="$debug $KLT_INCPATH $options"

for config in $(cat $configs); do
  mkdir -p $bindir/$stem/$config
  pushd $bindir/$stem/$config > /dev/null

  echo "tilek --tilek-target=$targets $C_FLAGS $srcdir/$stem-$config.c &> $bindir/$stem/tilek-$config.log"
  tilek --tilek-target=$targets $C_FLAGS $srcdir/$stem-$config.c &> $bindir/$stem/tilek-$config.log

  echo "gcc $C_FLAGS -c rose_$stem-$config.c -o rose_$stem-$config.o &> $bindir/$stem/gcc-rose_$stem-$config.log"
  gcc $C_FLAGS -c rose_$stem-$config.c -o rose_$stem-$config.o &> $bindir/$stem/gcc-rose_$stem-$config.log

  echo "gcc $C_FLAGS -c $stem-$config-host-kernel.c -o $stem-$config-host-kernel.o &> $bindir/$stem/gcc-$config-host-kernel.log"
  gcc $C_FLAGS -c $stem-$config-host-kernel.c -o $stem-$config-host-kernel.o &> $bindir/$stem/gcc-$config-host-kernel.log

  echo "gcc $C_FLAGS -c $stem-$config-static.c -o $stem-$config-static.o &> $bindir/$stem/gcc-$config-static.log"
  gcc $C_FLAGS -c $stem-$config-static.c -o $stem-$config-static.o &> $bindir/$stem/gcc-$config-static.log

  echo "libtool --mode=link gcc $debug rose_$stem-$config.o $stem-$config-host-kernel.o $stem-$config-static.o $KLT_LD_FLAGS -o $stem-$config &> $bindir/$stem/libtool-$config.log"
  libtool --mode=link gcc $debug rose_$stem-$config.o $stem-$config-host-kernel.o $stem-$config-static.o $KLT_LD_FLAGS -o $stem-$config &> $bindir/$stem/libtool-$config.log

  popd > /dev/null
done

echo "####################"

