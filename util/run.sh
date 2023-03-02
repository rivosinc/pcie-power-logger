#!/usr/bin/sh

# SPDX-FileCopyrightText: 2023 Rivos Inc.
#
# SPDX-License-Identifier: Apache-2.0

# Script to repeatedly retrieve sample data from host and pipe through
# unpack_power.pl to create a single contiguous CSV output file consisting of
# timestamps and observed power at a given time.

CURL_CMD="curl --http0.9 --no-progress-meter"
UNPACK_POWER_CMD="./unpack_power.pl --print_restore_params "

OUTFILE="./output.csv"
HOST="192.0.2.1:8080"

while getopts "o:h:d" option; do
case ${option} in
	o)
		OUTFILE=${OPTARG}
		;;
	h)
		HOST=${OPTARG}
		;;
	*)
		echo "Error args."
		exit 0;
		;;
esac
done

while :
do

next_args="$($CURL_CMD $HOST | $UNPACK_POWER_CMD $next_args 2>&1 >>$OUTFILE)"
echo -n "."
sleep 0.5

done
