
set datafile separator ","

set term png size 2400, 800
#set term png size 4800, 1600

set output tag.'.png'

set xrange [100:2200]
set yrange [0:30]

set multiplot layout 2,4
#set multiplot layout 4,8

set logscale x 2
#set logscale y 10

set key left top

set title 'Version 1'

plot tag.'-n_2.csv'  using 2:3  title '2 Threads' with linespoints, \
     tag.'-n_4.csv'  using 2:3  title '4 Threads' with linespoints, \
     tag.'-n_8.csv'  using 2:3  title '8 Threads' with linespoints, \
     tag.'-n_16.csv' using 2:3  title '16 Threads' with linespoints, \
     tag.'-n_32.csv' using 2:3  title '32 Threads' with linespoints

#set nokey
#
#set title 'Version 1'
#plot tag.'-n_2.csv'  using 2:3  notitle with linespoints, \
#     tag.'-n_4.csv'  using 2:3  notitle with linespoints, \
#     tag.'-n_8.csv'  using 2:3  notitle with linespoints, \
#     tag.'-n_16.csv' using 2:3  notitle with linespoints, \
#     tag.'-n_32.csv' using 2:3  notitle with linespoints

set nokey

set title 'Version 2'
plot tag.'-n_2.csv'  using 2:4  notitle with linespoints, \
     tag.'-n_4.csv'  using 2:4  notitle with linespoints, \
     tag.'-n_8.csv'  using 2:4  notitle with linespoints, \
     tag.'-n_16.csv' using 2:4  notitle with linespoints, \
     tag.'-n_32.csv' using 2:4  notitle with linespoints

set nokey

set title 'Version 3'
plot tag.'-n_2.csv'  using 2:5  notitle with linespoints, \
     tag.'-n_4.csv'  using 2:5  notitle with linespoints, \
     tag.'-n_8.csv'  using 2:5  notitle with linespoints, \
     tag.'-n_16.csv' using 2:5  notitle with linespoints, \
     tag.'-n_32.csv' using 2:5  notitle with linespoints

set nokey

set title 'Version 4'
plot tag.'-n_2.csv'  using 2:6  notitle with linespoints, \
     tag.'-n_4.csv'  using 2:6  notitle with linespoints, \
     tag.'-n_8.csv'  using 2:6  notitle with linespoints, \
     tag.'-n_16.csv' using 2:6  notitle with linespoints, \
     tag.'-n_32.csv' using 2:6  notitle with linespoints

set nokey

set title 'Version 5'
plot tag.'-n_2.csv'  using 2:7  notitle with linespoints, \
     tag.'-n_4.csv'  using 2:7  notitle with linespoints, \
     tag.'-n_8.csv'  using 2:7  notitle with linespoints, \
     tag.'-n_16.csv' using 2:7  notitle with linespoints, \
     tag.'-n_32.csv' using 2:7  notitle with linespoints

set nokey

set title 'Version 6'
plot tag.'-n_2.csv'  using 2:8  notitle with linespoints, \
     tag.'-n_4.csv'  using 2:8  notitle with linespoints, \
     tag.'-n_8.csv'  using 2:8  notitle with linespoints, \
     tag.'-n_16.csv' using 2:8  notitle with linespoints, \
     tag.'-n_32.csv' using 2:8  notitle with linespoints

set nokey

set title 'Version 7'
plot tag.'-n_2.csv'  using 2:9  notitle with linespoints, \
     tag.'-n_4.csv'  using 2:9  notitle with linespoints, \
     tag.'-n_8.csv'  using 2:9  notitle with linespoints, \
     tag.'-n_16.csv' using 2:9  notitle with linespoints, \
     tag.'-n_32.csv' using 2:9  notitle with linespoints

set nokey

set title 'Version 8'
plot tag.'-n_2.csv'  using 2:10 notitle with linespoints, \
     tag.'-n_4.csv'  using 2:10 notitle with linespoints, \
     tag.'-n_8.csv'  using 2:10 notitle with linespoints, \
     tag.'-n_16.csv' using 2:10 notitle with linespoints, \
     tag.'-n_32.csv' using 2:10 notitle with linespoints

unset multiplot


exit

plot tag.'-n_2.csv' using 2:3  notitle with linespoints
plot tag.'-n_2.csv' using 2:4  notitle with linespoints
plot tag.'-n_2.csv' using 2:5  notitle with linespoints
plot tag.'-n_2.csv' using 2:6  notitle with linespoints
plot tag.'-n_2.csv' using 2:7  notitle with linespoints
plot tag.'-n_2.csv' using 2:8  notitle with linespoints
plot tag.'-n_2.csv' using 2:9  notitle with linespoints
plot tag.'-n_2.csv' using 2:10 notitle with linespoints

plot tag.'-n_4.csv' using 2:3  notitle with linespoints
plot tag.'-n_4.csv' using 2:4  notitle with linespoints
plot tag.'-n_4.csv' using 2:5  notitle with linespoints
plot tag.'-n_4.csv' using 2:6  notitle with linespoints
plot tag.'-n_4.csv' using 2:7  notitle with linespoints
plot tag.'-n_4.csv' using 2:8  notitle with linespoints
plot tag.'-n_4.csv' using 2:9  notitle with linespoints
plot tag.'-n_4.csv' using 2:10 notitle with linespoints

plot tag.'-n_8.csv' using 2:3  notitle with linespoints
plot tag.'-n_8.csv' using 2:4  notitle with linespoints
plot tag.'-n_8.csv' using 2:5  notitle with linespoints
plot tag.'-n_8.csv' using 2:6  notitle with linespoints
plot tag.'-n_8.csv' using 2:7  notitle with linespoints
plot tag.'-n_8.csv' using 2:8  notitle with linespoints
plot tag.'-n_8.csv' using 2:9  notitle with linespoints
plot tag.'-n_8.csv' using 2:10 notitle with linespoints

plot tag.'-n_16.csv' using 2:3  notitle with linespoints
plot tag.'-n_16.csv' using 2:4  notitle with linespoints
plot tag.'-n_16.csv' using 2:5  notitle with linespoints
plot tag.'-n_16.csv' using 2:6  notitle with linespoints
plot tag.'-n_16.csv' using 2:7  notitle with linespoints
plot tag.'-n_16.csv' using 2:8  notitle with linespoints
plot tag.'-n_16.csv' using 2:9  notitle with linespoints
plot tag.'-n_16.csv' using 2:10 notitle with linespoints
