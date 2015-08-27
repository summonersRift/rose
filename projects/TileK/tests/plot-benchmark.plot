
set datafile separator ","

set term png size 2400, 1200
#set term png size 4800, 1600

set output tag.'.png'

set multiplot layout 3,4 title 'SGEMM on CPU i7-3610QM at 2.30GHz'
#set multiplot layout 4,8

set logscale x 2
#set logscale y 10

set nokey

set title 'Version 1'
plot tag.'-n_2.csv'  using 2:3  title '2 Threads' with points, \
     tag.'-n_4.csv'  using 2:3  title '4 Threads' with points, \
     tag.'-n_8.csv'  using 2:3  title '8 Threads' with points, \
     tag.'-n_16.csv' using 2:3  title '16 Threads' with points

set nokey

set title 'Version 2'
plot tag.'-n_2.csv'  using 2:4  notitle with points, \
     tag.'-n_4.csv'  using 2:4  notitle with points, \
     tag.'-n_8.csv'  using 2:4  notitle with points, \
     tag.'-n_16.csv' using 2:4  notitle with points

set nokey

set title 'Version 3'
plot tag.'-n_2.csv'  using 2:5  notitle with points, \
     tag.'-n_4.csv'  using 2:5  notitle with points, \
     tag.'-n_8.csv'  using 2:5  notitle with points, \
     tag.'-n_16.csv' using 2:5  notitle with points

set nokey

set title 'Version 4'
plot tag.'-n_2.csv'  using 2:6  notitle with points, \
     tag.'-n_4.csv'  using 2:6  notitle with points, \
     tag.'-n_8.csv'  using 2:6  notitle with points, \
     tag.'-n_16.csv' using 2:6  notitle with points

set nokey

set title 'Version 5'
plot tag.'-n_2.csv'  using 2:7  notitle with points, \
     tag.'-n_4.csv'  using 2:7  notitle with points, \
     tag.'-n_8.csv'  using 2:7  notitle with points, \
     tag.'-n_16.csv' using 2:7  notitle with points

set nokey

set title 'Version 6'
plot tag.'-n_2.csv'  using 2:8  notitle with points, \
     tag.'-n_4.csv'  using 2:8  notitle with points, \
     tag.'-n_8.csv'  using 2:8  notitle with points, \
     tag.'-n_16.csv' using 2:8  notitle with points

set nokey

set title 'Version 7'
plot tag.'-n_2.csv'  using 2:9  notitle with points, \
     tag.'-n_4.csv'  using 2:9  notitle with points, \
     tag.'-n_8.csv'  using 2:9  notitle with points, \
     tag.'-n_16.csv' using 2:9  notitle with points

set nokey

set title 'Version 8'
plot tag.'-n_2.csv'  using 2:10 notitle with points, \
     tag.'-n_4.csv'  using 2:10 notitle with points, \
     tag.'-n_8.csv'  using 2:10 notitle with points, \
     tag.'-n_16.csv' using 2:10 notitle with points

set key box linestyle 1
set key outside
set key right top
set title 'For legend'
plot tag.'-n_2.csv'  using 2:3  title '2 Threads' with points, \
     tag.'-n_4.csv'  using 2:3  title '4 Threads' with points, \
     tag.'-n_8.csv'  using 2:3  title '8 Threads' with points, \
     tag.'-n_16.csv' using 2:3  title '16 Threads' with points

unset multiplot


exit

plot tag.'-n_2.csv' using 2:3  notitle with points
plot tag.'-n_2.csv' using 2:4  notitle with points
plot tag.'-n_2.csv' using 2:5  notitle with points
plot tag.'-n_2.csv' using 2:6  notitle with points
plot tag.'-n_2.csv' using 2:7  notitle with points
plot tag.'-n_2.csv' using 2:8  notitle with points
plot tag.'-n_2.csv' using 2:9  notitle with points
plot tag.'-n_2.csv' using 2:10 notitle with points

plot tag.'-n_4.csv' using 2:3  notitle with points
plot tag.'-n_4.csv' using 2:4  notitle with points
plot tag.'-n_4.csv' using 2:5  notitle with points
plot tag.'-n_4.csv' using 2:6  notitle with points
plot tag.'-n_4.csv' using 2:7  notitle with points
plot tag.'-n_4.csv' using 2:8  notitle with points
plot tag.'-n_4.csv' using 2:9  notitle with points
plot tag.'-n_4.csv' using 2:10 notitle with points

plot tag.'-n_8.csv' using 2:3  notitle with points
plot tag.'-n_8.csv' using 2:4  notitle with points
plot tag.'-n_8.csv' using 2:5  notitle with points
plot tag.'-n_8.csv' using 2:6  notitle with points
plot tag.'-n_8.csv' using 2:7  notitle with points
plot tag.'-n_8.csv' using 2:8  notitle with points
plot tag.'-n_8.csv' using 2:9  notitle with points
plot tag.'-n_8.csv' using 2:10 notitle with points

plot tag.'-n_16.csv' using 2:3  notitle with points
plot tag.'-n_16.csv' using 2:4  notitle with points
plot tag.'-n_16.csv' using 2:5  notitle with points
plot tag.'-n_16.csv' using 2:6  notitle with points
plot tag.'-n_16.csv' using 2:7  notitle with points
plot tag.'-n_16.csv' using 2:8  notitle with points
plot tag.'-n_16.csv' using 2:9  notitle with points
plot tag.'-n_16.csv' using 2:10 notitle with points
