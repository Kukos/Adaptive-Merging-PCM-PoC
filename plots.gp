# Grafika wektorowa jako pdf
set terminal pdfcairo enhanced
set datafile separator "\t"
set termoption font ',16'
# Kolory wykresow, mozna podac kod rgb w hexach po #
# Format: line [numer linii czyli kolumna w .txt $k dostanie $k kolor] lw [szerokosc linii] lc [tryb koloru u nas rgb] [nazwa predefiniowana albo kod]
set style line 1 lw 10 lc rgb "green"
set style line 2 lw 5 dt 7 lc rgb "blue"
set style line 3 lw 3 lc rgb "red"
set style line 4 lw 3 lc rgb "gold"
set style line 5 lw 3 lc rgb "#660033"
set style increment user

set key top right Right maxrows 3

# Rozmiar OX i OY (numerow)
set tics font ", 26"

# Fonty legendy
set key font "Helvetica, 24"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 24"

# Fonty osi OY
set ylabel font "Helvetica, 32"

# Fonty osi OX
set xlabel font "Helvetica, 32"

# EX3 Per Query
set xtics 15
set output "ex3_random_per_query.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "QUERY"
plot for [col=2:6] 'ex3_random_per_query.txt' using 1:col with lines title columnheader

set xtics 4
set output "ex3_seq_per_query.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "QUERY"
plot for [col=2:6] 'ex3_seq_per_query.txt' using 1:col with lines title columnheader

#set key ins bottom center
set xtics 5
set output "ex3_newkeys_per_query.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "QUERY"
plot for [col=2:6] 'ex3_newkeys_per_query.txt' using 1:col with lines title columnheader


# reset
# set terminal pdfcairo enhanced
# set datafile separator "\t"
# set termoption font ',16'

# set style line 1 lw 3 lc rgb "red"
# set style line 2 lw 3 lc rgb "gold"
# set style line 3 lw 3 lc rgb "#660033"
# set style increment user

# set key top right Right maxrows 3

# # Rozmiar OX i OY (numerow)
# set tics font ", 26"

# # Fonty legendy
# set key font "Helvetica, 24"

# # Fonty tytulu czyli tego nad obrazkiem
# set title font "Helvetica, 24"

# # Fonty osi OY
# set ylabel font "Helvetica, 32"

# # Fonty osi OX
# set xlabel font "Helvetica, 32"

# set output "ex4_stress1_per_batch.pdf"
# set title ""
# set ylabel "TIME [s]"
# set xlabel "QUERY"
# plot for [col=2:4] 'ex4_stress1_per_batch.txt' using 1:col with lines title columnheader

# set output "ex4_stress2_per_batch.pdf"
# set title ""
# set ylabel "TIME [s]"
# set xlabel "QUERY"
# plot for [col=2:4] 'ex4_stress2_per_batch.txt' using 1:col with lines title columnheader

# set output "ex4_stress3_per_batch.pdf"
# set title ""
# set ylabel "TIME [s]"
# set xlabel "QUERY"
# plot for [col=2:4] 'ex4_stress3_per_batch.txt' using 1:col with lines title columnheader

# set output "ex4_stress4_per_batch.pdf"
# set title ""
# set ylabel "TIME [s]"
# set xlabel "QUERY"
# plot for [col=2:4] 'ex4_stress4_per_batch.txt' using 1:col with lines title columnheader

# Zmiana na histogram
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
set yrange [0:1]
set y2range [0:3e7]
set xtics nomirror
set ytics nomirror
set y2tics

# Rozmiar OX i OY (numerow)
set xtics font ", 14"
set ytics font ", 18"
set y2tics font ", 18"

# Fonty legendy
set key font "Helvetica, 18"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 28"

# Fonty osi OY
set ylabel font "Helvetica, 22"
set y2label font "Helvetica, 22"

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
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex1_invalidation.txt'  using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex1_invalidation.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2


# Rozmiar OX i OY (numerow)
set xtics font ", 14"
set ytics font ", 20"
set y2tics font ", 20"

# Fonty legendy
set key font "Helvetica, 24"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 28"

# Fonty osi OY
set ylabel font "Helvetica, 32"
set y2label font "Helvetica, 32"

# Fonty osi OX
set xlabel font "Helvetica, 22"

set output "ex2_index.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex2_BTREE.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex2_BTREE.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2


# Zmiana na inne histogramy
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

set xtics nomirror
set ytics nomirror
set y2tics

#set ytics 50
#set y2tics 4e9
set yrange [0:70]
set y2range [0:4e9]

# Rozmiar OX i OY (numerow)
set xtics font ", 32"
set ytics font ", 18"
set y2tics font ", 18"

# Fonty legendy
set key font "Helvetica, 18"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 28"

# Fonty osi OY
set ylabel font "Helvetica, 32"
set y2label font "Helvetica, 32"

# Fonty osi OX
set xlabel font "Helvetica, 32"

# Kolory - rozne odcienie dlatego w HEXACH
set style line 1 lw 1 lc rgb "#232bff"
set style line 2 lw 1 lc rgb "#9a0f02"
set style line 3 lw 3 lc rgb "#239fff"
set style line 4 lw 3 lc rgb "#232bff"
set style increment user
set style fill pattern 9 border

set output "ex3_random_total.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex3_random_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex3_random_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

set output "ex3_newkeys_total.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex3_newkeys_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex3_newkeys_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

set output "ex3_seq_total.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex3_seq_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex3_seq_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2


# Batch total

set yrange [0:350]
set y2range [0:1.5e10]
set output "ex4_stress1_total.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex4_stress1_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex4_stress1_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2


set output "ex4_stress2_total.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex4_stress2_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex4_stress2_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

set output "ex4_stress3_total.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex4_stress3_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex4_stress3_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

set yrange [0:50]
set y2range [0:3e9]
set output "ex4_stress4_total.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex4_stress4_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex4_stress4_total.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

# Batch Total Normalize
set yrange [0:10]
set y2range [0:20]
set ytics 1
set y2tics 2
set output "ex4_stress1_total_norma.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex4_stress1_total_norma.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex4_stress1_total_norma.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2


set output "ex4_stress2_total_norma.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex4_stress2_total_norma.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex4_stress2_total_norma.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

set output "ex4_stress3_total_norma.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex4_stress3_total_norma.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex4_stress3_total_norma.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

set output "ex4_stress4_total_norma.pdf"
set title ""
set ylabel "Time [s]"
set y2label "Wear-out [B]" rotate by -90
plot for [col=2:2] 'ex4_stress4_total_norma.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y1, for [col=3:3] 'ex4_stress4_total_norma.txt' using col:xticlabels(1) lt -1 title columnheader axis x1y2

reset
set terminal pdfcairo enhanced
set datafile separator "\t"
set termoption font ',16'

set style line 1 lw 3 lc rgb "red"
set style line 2 lw 3 lc rgb "gold"
set style line 3 lw 3 lc rgb "#660033"
set style increment user

set key top right Right maxrows 3

# Rozmiar OX i OY (numerow)
set tics font ", 24"
set xtics 0.01

# Fonty legendy
set key font "Helvetica, 24"

# Fonty tytulu czyli tego nad obrazkiem
set title font "Helvetica, 24"

# Fonty osi OY
set ylabel font "Helvetica, 32"

# Fonty osi OX
set xlabel font "Helvetica, 32"

set output "ex4_stress_step1_total_norma.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "SELECTIVITY"
plot for [col=2:4] 'ex4_stress_step1_total_norma.txt' using 1:col with lines title columnheader

set output "ex4_stress_step2_total_norma.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "SELECTIVITY"
plot for [col=2:4] 'ex4_stress_step2_total_norma.txt' using 1:col with lines title columnheader

set output "ex4_stress_step3_total_norma.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "SELECTIVITY"
plot for [col=2:4] 'ex4_stress_step3_total_norma.txt' using 1:col with lines title columnheader

set output "ex4_stress_step4_total_norma.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "SELECTIVITY"
plot for [col=2:4] 'ex4_stress_step4_total_norma.txt' using 1:col with lines title columnheader