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
set style increment user

set key maxrows 2

# Rozmiar OX i OY (numerow)
set tics font ", 26"

# Fonty legendy
set key font "Helvetica, 32"

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
plot for [col=2:5] 'ex3_random_per_query.txt' using 1:col with lines title columnheader


set xtics 4
set output "ex3_seq_per_query.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "QUERY"
plot for [col=2:5] 'ex3_seq_per_query.txt' using 1:col with lines title columnheader

set key ins bottom center
set xtics 10
set output "ex3_newkeys_per_query.pdf"
set title ""
set ylabel "TIME [s]"
set xlabel "QUERY"
plot for [col=2:5] 'ex3_newkeys_per_query.txt' using 1:col with lines title columnheader

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
set yrange [0:50]
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
