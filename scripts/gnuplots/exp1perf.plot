#set title "Runtime performance comparison of FiGaRo-THIN and competitors\n \ on real-world datasets"

# Deletes all arrows
#set noarrow

# Deletes all Label
#set nolabel

# Do not create a log file
set nolog

set xlabel "percentage of one hot encoded column"
set ylabel "wall-clock time (sec)"
#set key at 400,1000000000
set key below
#set format y "2^{%L}"
set pointsize 1.0

#set style line 1 lt rgbcolor "red" lw 3
#set style line 2 lt rgb "black" lw 3
#set style line 3 lt rgb "green" lw 3

#set logscale x
set xrange [8:102]
set xtics (10,20,30,40,50,60,70,80,90,100)
set logscale y
set yrange [0.45:500]


set terminal pdf color enhanced font "Helvetica"
set output "exp1perf.pdf"
#set out 'exp1perf.eps'
#set terminal postscript eps color enhanced "Helvetica" 14
set size 1,0.9

plot "exp1perf-retailer.dat" u ($1):($2) title "FiGaRo-THIN/R" lt 2 pt 2 linecolor rgb "black" with linespoints, \
     "exp1perf-retailer.dat" u ($1):($3) title "MKL/R" lt 1 pt 2 linecolor rgb "magenta" with linespoints, \
     "exp1perf-retailer.dat" u ($1):($4) title "THIN/R" lt 1 pt 2 linecolor rgb "green" with linespoints, \
     "exp1perf-favorita.dat" u ($1):($2) title "FiGaRo-THIN/F" lt 2 pt 4 linecolor rgb "black" with linespoints, \
     "exp1perf-favorita.dat" u ($1):($3) title "MKL/F" lt 1 pt 4 linecolor rgb "magenta" with linespoints, \
     "exp1perf-favorita.dat" u ($1):($4) title "THIN/F" lt 1 pt 4 linecolor rgb "green" with linespoints, \
     "exp1perf-yelp.dat" u ($1):($2) title "FiGaRo-THIN/Y" lt 2 pt 9 linecolor rgb "black" with linespoints, \
     "exp1perf-yelp.dat" u ($1):($3) title "MKL/Y" lt 1 pt 9 linecolor rgb "magenta" with linespoints,\
     "exp1perf-yelp.dat" u ($1):($4) title "THIN/Y" lt 1 pt 9 linecolor rgb "green" with linespoints;
