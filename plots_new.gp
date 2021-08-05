# Grafika wektorowa jako pdf
# histogram
set terminal pdfcairo enhanced
set datafile separator "\t"
set style data histogram
set style histogram clustered

# Inne ustawienia bo sie nie miesci legenda
set key width -2 height 0
set key inside right top
set key samplen 1
set key spacing 1


set logscale y
#set logscale y2
set xtics nomirror
set ytics nomirror

# Rozmiar OX i OY (numerow)
set xtics font ", 14"
set ytics font ", 14"

# Fonty legendy
set key font "Helvetica, 18"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 28"

# Fonty osi OY
set ylabel font "Helvetica, 18"

# Fonty osi OX
set xlabel font "Helvetica, 22"

# Kolory - rozne odcienie dlatego w HEXACH
set style line 1 lw 1 lc rgb "#232bff"
set style line 2 lw 1 lc rgb "#9a0f02"
set style line 3 lw 3 lc rgb "#239fff"
set style line 4 lw 3 lc rgb "#232bff"
set style increment user
set style fill pattern 9 border

set output "ex1_invalidation.pdf"
set title ""
set ylabel "Time [s]"
plot for [col=2:2] 'ex1_invalidation.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1


reset
set terminal pdfcairo enhanced
set datafile separator "\t"
set style data histogram
set style histogram clustered

# Inne ustawienia bo sie nie miesci legenda
set key width -2 height 0
set key inside right top
set key samplen 1
set key spacing 1

#set logscale y
#set logscale y2
set xtics nomirror
set ytics nomirror
set yrange [0:*]

# Rozmiar OX i OY (numerow)
set xtics font ", 18"
set ytics font ", 18"

# Fonty legendy
set key font "Helvetica, 18"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 28"

# Fonty osi OY
set ylabel font "Helvetica, 18"

# Fonty osi OX
set xlabel font "Helvetica, 22"

# Kolory - rozne odcienie dlatego w HEXACH
set style line 1 lw 1 lc rgb "#232bff"
set style line 2 lw 1 lc rgb "#9a0f02"
set style line 3 lw 3 lc rgb "#239fff"
set style line 4 lw 3 lc rgb "#232bff"
set style increment user
set style fill pattern 9 border

set output "ex2_index_balanced.pdf"
set title ""
set ylabel "Time [s]"
plot for [col=2:2] 'ex2_index_balanced.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1

set output "ex2_index_read_intensive.pdf"
set title ""
set ylabel "Time [s]"
plot for [col=2:2] 'ex2_index_read_intensive.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1

set output "ex2_index_write_intensive.pdf"
set title ""
set ylabel "Time [s]"
plot for [col=2:2] 'ex2_index_write_intensive.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1

reset
set terminal pdfcairo enhanced
set datafile separator "\t"
set style data histogram
set style histogram clustered

# Inne ustawienia bo sie nie miesci legenda
set key width -2 height 0
set key inside right top
set key samplen 1
set key spacing 1

# set logscale y
# set logscale y2
set xtics nomirror
set ytics nomirror
set yrange [0:*]
set y2range [0:*]

# Rozmiar OX i OY (numerow)
set xtics font ", 18"
set ytics font ", 18"
set y2tics font ", 18"

# Fonty legendy
set key font "Helvetica, 18"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 28"

# Fonty osi OY
set ylabel font "Helvetica, 18"
set y2label font "Helvetica, 18"

# Fonty osi OX
set xlabel font "Helvetica, 22"

# Kolory - rozne odcienie dlatego w HEXACH
set style line 1 lw 1 lc rgb "#232bff"
set style line 2 lw 1 lc rgb "#9a0f02"
set style line 3 lw 3 lc rgb "#239fff"
set style line 4 lw 3 lc rgb "#232bff"
set style increment user
set style fill pattern 9 border


set output "ex3_newkeys_total.pdf"
set title ""
set ylabel "Time [s]"
plot for [col=2:2] 'ex3_newkeys_total.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex3_random_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

set output "ex3_random_total.pdf"
set title ""
set ylabel "Time [s]"
plot for [col=2:2] 'ex3_random_total.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex3_random_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

set output "ex3_seq_total.pdf"
set title ""
set ylabel "Time [s]"
plot for [col=2:2] 'ex3_seq_total.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex3_random_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

set yrange [0:25]
set y2range [0:1e9]
set xtics font ", 14"

set output "ex3_1_random_total.pdf"
set title ""
set ylabel "Time [s]"
plot for [col=2:2] 'ex3_1_random_total.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex3_1_random_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2


# reset
# set terminal pdfcairo enhanced
# set datafile separator "\t"
# set style data histogram
# set style histogram clustered

# # Inne ustawienia bo sie nie miesci legenda
# set key width -2 height 0
# set key inside right top
# set key samplen 1
# set key spacing 1

# #set logscale y
# #set logscale y2
# set xtics nomirror
# set ytics nomirror
# set yrange [0:*]

# # Rozmiar OX i OY (numerow)
# set xtics font ", 18"
# set ytics font ", 18"

# # Fonty legendy
# set key font "Helvetica, 18"

# # Fonty tytulu czyli tego nad obrazkiem
# set title font "Helvetica, 28"

# # Fonty osi OY
# set ylabel font "Helvetica, 18"

# # Fonty osi OX
# set xlabel font "Helvetica, 22"

# # Kolory - rozne odcienie dlatego w HEXACH
# set style line 1 lw 1 lc rgb "#232bff"
# set style line 2 lw 1 lc rgb "#9a0f02"
# set style line 3 lw 3 lc rgb "#239fff"
# set style line 4 lw 3 lc rgb "#232bff"
# set style increment user
# set style fill pattern 9 border


# set output "ex4_stress2_total.pdf"
# set title ""
# set ylabel "Time [s]"
# plot for [col=2:2] 'ex4_stress2_total.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1

# set output "ex4_stress4_total.pdf"
# set title ""
# set ylabel "Time [s]"
# plot for [col=2:2] 'ex4_stress4_total.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1


reset
set terminal pdfcairo enhanced
set datafile separator "\t"
set termoption font ',16'

set style line 1 lw 3 lc rgb "red"
set style line 2 lw 3 lc rgb "gold"
set style line 3 lw 3 lc rgb "green"
set style line 4 lw 3 lc rgb "blue"
set style increment user

set key top right Right maxrows 3

# Rozmiar OX i OY (numerow)
set tics font ", 18"
set xtics 0.01

# Fonty legendy
set key font "Helvetica, 24"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 24"

# Fonty osi OY
set ylabel font "Helvetica, 32"

# Fonty osi OX
set xlabel font "Helvetica, 32"

set output "ex4_stress_step1_time_total.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "SELECTIVITY"
plot for [col=2:3] 'ex4_stress_step1_time_total.txt' using 1:col with lines title columnheader

# set output "ex4_stress_step2_time_total.pdf"
# set title ""
# set ylabel "TIME [s]"
# set xlabel "SELECTIVITY"
# plot for [col=2:3] 'ex4_stress_step2_time_total.txt' using 1:col with lines title columnheader

# set output "ex4_stress_step3_time_total.pdf"
# set title ""
# set ylabel "TIME [s]"
# set xlabel "SELECTIVITY"
# plot for [col=2:3] 'ex4_stress_step3_time_total.txt' using 1:col with lines title columnheader

set output "ex4_stress_step4_time_total.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "SELECTIVITY"
plot for [col=2:3] 'ex4_stress_step4_time_total.txt' using 1:col with lines title columnheader

reset
set terminal pdfcairo enhanced
set datafile separator "\t"
set termoption font ',16'

set style line 1 lw 3 lc rgb "gold"
set style line 2 lw 3 lc rgb "green"
set style line 3 lw 3 lc rgb "blue"
set style line 4 lw 3 lc rgb "red"
set style increment user

set key top right Right maxrows 3

# Rozmiar OX i OY (numerow)
set tics font ", 18"
set xtics 0.01

# Fonty legendy
set key font "Helvetica, 16"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 24"

# Fonty osi OY
set ylabel font "Helvetica, 32"

# Fonty osi OX
set xlabel font "Helvetica, 32"

set output "ex4_1_stress_step1_time_total.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "SELECTIVITY"
plot for [col=2:5] 'ex4_1_stress_step1_time_total.txt' using 1:col with lines title columnheader

set output "ex4_1_stress_step4_time_total.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "SELECTIVITY"
plot for [col=2:5] 'ex4_1_stress_step4_time_total.txt' using 1:col with lines title columnheader

set yrange [*:2300]

set output "ex4_1_stress_step5_time_total.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "SELECTIVITY"
plot for [col=2:5] 'ex4_1_stress_step5_time_total.txt' using 1:col with lines title columnheader