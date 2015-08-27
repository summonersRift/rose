#!/bin/bash

tag=$1
s_list=$2
n_list=$3
srcdir=$4

> $tag-min.csv

for s in $s_list; do
for n in $n_list; do
  awk -F , -v s=$s -v n=$n '{if ($1 == s && $4 == n) for (i = 1; i <= 8; i++) if (v[i] == 0 || v[i] > $(i+4)) v[i] = $(i+4); } END { printf("%d,%d,%d,%d", s, s, s, n) ; for (i = 1; i <= 8; i++) printf(",%d", v[i]) ; printf("\n") ; }' $tag.csv >> $tag-min.csv
done
done

> $tag-norm.csv

for s in $s_list; do
  awk -F , -v s=$s '{if ($1 == s) { if ($4 == 1) for (i = 1; i <= 8; i++) b[i]=$(i+4); else { printf("%d,%d", $4, s); for (i = 1; i <= 8; i++) printf(",%f", b[i]/$(i+4)); printf("\n"); } } }' $tag-min.csv >> $tag-norm.csv
done

for n in $n_list; do
  grep "^$n," $tag-norm.csv > $tag-norm-n_$n.csv
done

gnuplot -e "tag='sgemm-norm'" $srcdir/../plot-benchmark.plot

