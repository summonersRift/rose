#!/bin/bash

tag=$1
first=$2
last=$3
n=$4
m=$5
p=$6
nthreads=$7

set -e

echo -n "$n,$m,$p,$nthreads"
for version in $(seq $first $last)
do
  input=$tag\_$version

  echo -n ","
  ./$input $n $m $p $nthreads
done
echo

