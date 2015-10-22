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
  echo " > $0 --stem stem [...]"
  echo "[ script_dir=$script_dir ]"
}

stem=""
srcdir=""
bindir=""
cfgdir=""
configs=""
target=""
opts=""
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

[ -z "$configs" ]    && configs=$cfgdir/$stem.csv
configs=$(readlink -f $configs)
echo " > configs = $configs"

# Target

echo " > targets = $targets"

# Options

options=$(echo $opts | tr ',' ' ')
echo " > options = $options"

# Debug

debug=$(echo $dbg | tr ',' ' ')
echo " > debug = $debug"

#####

[ ! -e $configs ] && echo "Error: $configs does not exist." && exit 3

#####

shopt -s expand_aliases

[ -z "$TILEK" ] && echo "Error: environment variable \$TILEK is not defined!" && exit 2

$TILEK

alias tilek > /dev/null

#####

echo "####################"

KLT_LD_FLAGS="-lrt"
if [ "$targets" = "threads" ]; then
  KLT_LD_FLAGS="$KLT_LD_FLAGS $KLT_RTL_THREADS"
elif [ "$targets" = "opencl" ]; then
  KLT_LD_FLAGS="$KLT_LD_FLAGS $KLT_RTL_OPENCL"
elif [ "$targets" = "threads,opencl" ]; then
  KLT_LD_FLAGS="$KLT_LD_FLAGS $KLT_RTL"
elif [ "$targets" = "opencl,threads" ]; then
  KLT_LD_FLAGS="$KLT_LD_FLAGS $KLT_RTL"
else
  KLT_LD_FLAGS="$KLT_LD_FLAGS $KLT_RTL_HOST"
fi

C_FLAGS="$debug $KLT_INCPATH $options"

cat $configs | while read config; do
  config=$(echo $config | cut -d'|' -f1)

  mkdir -p $bindir/$stem/$config
  pushd $bindir/$stem/$config > /dev/null

  echo -ne "\r                                                   \rCompile \"$config\""

# echo "tilek --tilek-target=$targets $C_FLAGS $srcdir/$stem-$config.c &> $bindir/$stem/tilek-$config.log"
  tilek --tilek-target=$targets $C_FLAGS $srcdir/$stem-$config.c &> $bindir/$stem/tilek-$config.log

  file=rose_$stem-$config
# echo "gcc $C_FLAGS -c $file.c -o $file.o &> $bindir/$stem/gcc-$file.log"
  gcc $C_FLAGS -c $file.c -o $file.o &> $bindir/$stem/gcc-$file.log
  objects="$file.o"

  file=$stem-$config-static
# echo "gcc $C_FLAGS -c $file.c -o $file.o &> $bindir/$stem/gcc-$file.log"
  gcc $C_FLAGS -c $file.c -o $file.o &> $bindir/$stem/gcc-$file.log
  objects=$objects" $file.o "

  file=$stem-$config-host-kernel
  if [ -e $file.c ]; then
#   echo "gcc $C_FLAGS -c $file.c -o $file.o &> $bindir/$stem/gcc-$file.log"
    gcc $C_FLAGS -c $file.c -o $file.o &> $bindir/$stem/gcc-$file.log
    objects=$objects" $file.o "
  fi

  file=$stem-$config-threads-kernel
  if [ -e $file.c ]; then
#   echo "gcc $C_FLAGS -c $file.c -o $file.o &> $bindir/$stem/gcc-$file.log"
    gcc $C_FLAGS -c $file.c -o $file.o &> $bindir/$stem/gcc-$file.log
    objects=$objects" $file.o "
  fi

# echo "libtool --mode=link gcc $debug $objects $KLT_LD_FLAGS -o $stem-$config &> $bindir/$stem/libtool-$config.log"
  libtool --mode=link gcc $debug $objects $KLT_LD_FLAGS -o $stem-$config &> $bindir/$stem/libtool-$config.log

  popd > /dev/null
done

echo
echo "####################"

