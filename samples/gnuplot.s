# SPDX-FileCopyrightText: 2023 Rivos Inc.
#
# SPDX-License-Identifier: Apache-2.0

# Script to produce power plots from CSV.

set term png size 2048,768
set xlabel "Time (10us)"
set ylabel "Power (W)"

set title "PCIE1 Power"
set output "pcie1.png"
plot "< sed -n <sample.csv '/PCIE1/p'" using 2:3 with lines

set title "PCIE2 Power"
set output "pcie2.png"
plot "< sed -n <sample.csv '/PCIE2/p'" using 2:3 with lines
