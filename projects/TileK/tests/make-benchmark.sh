#!/bin/bash

tag=$1
first=$2
last=$3
tilek=$4
rose_flags=$5
c_flags=$6
ld_flags=$7
srcdir=$8

echo "tag=$1"
echo "first=$2"
echo "last=$3"
echo "tilek=$4"
echo "rose_flags=$5"
echo "c_flags=$6"
echo "ld_flags=$7"
echo "srcdir=$8"

set -e

gcc $c_flags -c $srcdir/$tag.c -o $tag.o

for version in $(seq $first $last)
do
  input=$tag\_$version
  output=rose_$input

  kernel=$input-kernel
  static=$input-static

  $tilek $rose_flags -c $srcdir/$input.c
  gcc $c_flags -c $output.c -o $output.o
  gcc $c_flags -c $kernel.c -o $kernel.o
  gcc $c_flags -c $static.c -o $static.o
  libtool --mode=link gcc $tag.o $output.o $kernel.o $static.o $ld_flags -o $input
done

