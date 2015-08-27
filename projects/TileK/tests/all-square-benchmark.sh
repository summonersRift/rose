#!/bin/bash

tag=$1
first=$2
last=$3
s_list=$4
n_list=$5
eval=$6
srcdir=$7

set -e

for s in $s_list; do
for n in $n_list; do
  $srcdir/../eval-benchmark.sh $tag $first $last $s $s $s $n $eval
done
done

