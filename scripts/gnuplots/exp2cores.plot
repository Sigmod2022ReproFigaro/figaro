#set title "FiGaRo runtime performance on multi-cores \n \ on real-world datasets"

# Deletes all arrows
#set noarrow

# Deletes all Label
#set nolabel

# Do not create a log file
set nolog

set xlabel "number of threads"
set ylabel "speed-up over 1 thread"
#set key at 400,1000000000
set key below
#set format y "2^{%L}"
set pointsize 1.0

#set style line 1 lt rgbcolor "red" lw 3
#set style line 2 lt rgb "black" lw 3
#set style line 3 lt rgb "green" lw 3

#set logscale x
set xrange [1:50]
set xtics (1,2,4,8,16,24,32,48)
#set logscale y
set yrange [1:11.6]
set ytics (1,2,3,4,5,6,7,8,9,10,11)

set terminal pdf color enhanced font "Helvetica"
set output "exp2cores.pdf"
#set out 'exp2cores.eps'
#set terminal postscript eps color enhanced "Helvetica" 14
set size 1, 0.8

plot "exp2cores_speed_up.dat" u ($1):($2) title "R" lt 1 pt 2 linecolor rgb "magenta" with linespoints, \
     "exp2cores_speed_up.dat" u ($1):($3) title "F" lt 1 pt 4 linecolor rgb "black" with linespoints, \
     "exp2cores_speed_up.dat" u ($1):($4) title "Y" lt 1 pt 9 linecolor rgb "green" with linespoints;
