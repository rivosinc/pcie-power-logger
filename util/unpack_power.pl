#!/usr/bin/perl

# SPDX-FileCopyrightText: 2023 Rivos Inc.
#
# SPDX-License-Identifier: Apache-2.0

# Script to unpack binary data sample format to CSV.
# Input (stdin) is binary data consisting of raw volage / current samples read
# from 12-bit ADC and 20-bit timestamps.
# Output (stdout) is CSV consisting of "PCIE#, <timestamp>, <power (W)>".
#
# The script can optionally save and restore stateful parameters across runs,
# which is useful when dealing with multiple input streams that represent one
# continuous time period when considered in aggregate.

use strict;
use Getopt::Long;

# Timestamps are 20 bits.
my $MAX_TIMESTAMP = 2**20;

# Most recent voltages from each input channel.
my $last_mv1;
my $last_mv2;

my $start_timestamp;
my $previous_timestamp;
my $timestamp_overflow_count = 0;

my $print_restore_params = 0;

# PCIe2 port on PMD-USB is observed to consistently underreport current.
# Taking data points from a fixed load on various currents 1A - 21A @ 12V,
# actual current can be approximated from PMD observed current as:
# Actual_W = Observed_W * 1.107 + 5.14
sub WattsFromPCIe2ObservedWatts {
	my ($observed) = @_;
	# PMD does not accurately measure currents approaching zero, just
	# provide raw value.
	return $observed if $observed < 5.0;

	my $slope = 1.107;
	my $offset = 5.14;
	return $observed * $slope + $offset;
}

# --print_restore_params_to_stderr: If enabled, when the script exits,
# arguments are printed to stderr that can be passed to the script on a
# subsequent invocation when handling data that is contigous.
#
# Restore parameters:
# --start_timestamp: Starting timestamp, which represents TS = 0.
# --timestamp_overflow_count: Number of times the 20-bit timestamp has rolled
# over.
# -- last_mv1: The most recent voltage sample (PCIE1), in millivolts.
# -- last_mv2: The most recent voltage sample (PCIE2), in millivolts.
GetOptions ("print_restore_params_to_stderr" => \$print_restore_params,
			"start_timestamp=i" => \$start_timestamp,
			"timestamp_overflow_count=i" => \$timestamp_overflow_count,
			"last_mv1=f" => \$last_mv1,
			"last_mv2=f" => \$last_mv2)
			or die("Error in command line arguments\n");

while (my $bytes_read = read(STDIN, my $buffer, 8)) {
	(my $packed, my $entry_type) = unpack("LL", $buffer);
	# 20 bit timestamp, 12 bit raw ADC data.
	my $timestamp = $packed >> 12;
	my $adc_data = $packed & 0xfff;

	if (defined $start_timestamp) {
		$timestamp_overflow_count++ if $timestamp < $previous_timestamp;
		$previous_timestamp = $timestamp;
		$timestamp = $timestamp - $start_timestamp  +
			$timestamp_overflow_count * $MAX_TIMESTAMP;
	} else {
		$previous_timestamp = $start_timestamp = $timestamp;
		$timestamp = 0;
	}

	if ($entry_type == ord("T")) {
		# TODO: Implement timestamp clock sync.
	} elsif ($entry_type == ord("V")) {
		$last_mv1 = $adc_data * 7.568;
	} elsif ($entry_type == ord("W")) {
		$last_mv2 = $adc_data * 7.568;
	} elsif ($entry_type == ord("I")) {
		$adc_data = 0 if ($adc_data & 0x800);
		my $ma = $adc_data * 48.8;
		print "PCIE1, $timestamp, " . ($last_mv1 * $ma / 1000000) . "\n"
			if defined $last_mv1;
	} elsif ($entry_type == ord("J")) {
		$adc_data = 0 if ($adc_data & 0x800);
		my $ma = $adc_data * 48.8;
		print "PCIE2, $timestamp, " .
			(WattsFromPCIe2ObservedWatts($last_mv2 * $ma / 1000000)) .
			"\n" if defined $last_mv2;
	} else {
		print STDERR "Invalid entry_type " . $entry_type;
	}
}

# Print restore parameters to STDERR if requested. For continuity, these can be
# passed to a subsequent invocation of this script.
print STDERR "--start_timestamp=$start_timestamp ".
             "--timestamp_overflow_count=$timestamp_overflow_count "
			 if $print_restore_params;
print STDERR "--last_mv1=$last_mv1 "
	if defined $last_mv1 and $print_restore_params;
print STDERR "--last_mv2=$last_mv2 "
	if defined $last_mv2 and $print_restore_params;
print STDERR "\n" if $print_restore_params;
