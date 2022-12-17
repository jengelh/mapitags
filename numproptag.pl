#!/usr/bin/perl
# SPDX-License-Identifier: MIT
# written by Jan Engelhardt, 2022
#
# ilspy the NumPropTag API from
# Microsoft.Exchange.Server.Storage.propertyDefinitions.dll,
#
use strict;
use warnings;
&main();

sub main
{
	my $csnames = &read_ds();
	print STDERR "read_ds: ", scalar(keys %$csnames), " names read\n";
	$csnames = &filter_ansi($csnames);
	print STDERR "filter_ansi: ", scalar(keys %$csnames), " names left\n";
	my $cstags = &kv_invert($csnames);
	print STDERR "kv_invert: ", scalar(keys %$cstags), " proptags\n";
	&filter_have($cstags);
	print STDERR "filter_have: ", scalar(keys %$cstags), " proptags left\n";
	&mt_merge($cstags);
	&mt_new($cstags);
}

sub read_ds
{
	my $names = {};
	my $fh;
	open($fh, "<dll.numproptag.txt") || die "dll.numproptag.txt: $!";
	while (<$fh>) {
		my($name, $proptag) = ($_ =~ m{^\s*public const uint (\w+) = (\d+)u;});
		if (!defined($proptag)) {
			next;
		}
		$proptag = sprintf("%08x", $proptag + 0);
		$names->{$name} = $proptag;
	}
	close($fh);
	return $names;
}

#
# Remove `xxxx001e XAnsi` if we also already have a `xxxx001f X`
#
sub filter_ansi
{
	my $in = shift @_;
	my $out = {%$in};
	foreach my $name (keys %$in) {
		next if (substr($name, -4) ne "Ansi");
		next if (!exists($in->{$name}));
		next if (substr($in->{$name}, -3) ne "01e");
		my $unq = substr($name, 0, -4);
		next if (!exists($in->{$unq}));
		next if (substr($in->{$unq}, -3) ne "01f");
		delete $out->{$name};
	}
	return $out;
}

sub kv_invert
{
	my $in = shift @_;
	my $out = {};
	foreach my $name (keys %$in) {
		my $proptag = $in->{$name};
		if (!defined($out->{$proptag})) {
			$out->{$proptag} = [];
		}
		push(@{$out->{$proptag}}, $name);
	}
	return $out;
}

#
# Filter names that mapitags.txt already has
# (Inplace modification)
#
sub filter_have
{
	my $cstags = shift @_;
	my $fh;
	open($fh, "<mapitags.txt") || die "mapitags.txt: $!";
	while (<$fh>) {
		chomp($_);
		my($proptag, $names) = ($_ =~ m{^(\S{8})\S*\s+(.*)});
		if (!defined($names)) {
			die "Unparsed: $_";
		}
		if (!defined($cstags->{$proptag})) {
			next;
		}
		$names =~ s{\s*/\*.*}{};
		my @names = split(m{\s+}, $names);
		my $new_names = [];
		foreach my $kw (@{$cstags->{$proptag}}) {
			if (grep(m{^(PidTag|\.)\Q$kw\E$}i, @names)) {
				next;
			}
			push(@$new_names, $kw);
		}
		$cstags->{$proptag} = $new_names;
	}
}

#
# Emit new copy of mapitags.txt to stdout, with names merged into existing
# proptags.
#
# Ouptut may contain extra lines because this program does not fully understand
# the +X notation used in mapitags.txt.
#
sub mt_merge
{
	my $cstags = shift @_;
	my $fh;
	open($fh, "<mapitags.txt") || die "mapitags.txt: $!";
	while (<$fh>) {
		chomp($_);
		my($proptag, $markers, $rest) = ($_ =~ m{^(\S{8})(\S*)\s+(.*)});
		my($comments) = ($rest =~ m{(\s*/\*.*)});
		my $names = defined($comments) ? $` : $rest;
		my @names = split(m{\s+}, $names);
		if (defined($cstags->{$proptag})) {
			foreach my $kw (@{$cstags->{$proptag}}) {
				if (grep(m{^(PidTag|\.)\Q$kw\E$}i, @names)) {
					next;
				}
				push(@names, ".$kw");
			}
			delete $cstags->{$proptag};
		}
		print "$proptag$markers\t", join(" ", @names);
		if (defined($comments)) {
			die if (length($comments) == 0);
			print $comments;
		}
		print "\n";
	}
}

#
# Emit tags that were completely new
#
sub mt_new
{
	my $cstags = shift @_;
	if (scalar(keys %$cstags) > 0) {
		print "# NEW:\n";
	}
	foreach my $proptag (sort keys %$cstags) {
		print "$proptag\t";
		foreach my $k (@{$cstags->{$proptag}}) {
			$k = ".$k";
		}
		print join(" ", @{$cstags->{$proptag}}), "\n";
	}
}
