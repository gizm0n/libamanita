#!/usr/bin/perl
my $prefix = shift;
my $dir = "$prefix/lib/pkgconfig";
opendir(DIR, $dir) or die $!;
my @files = grep { /\.pc$/ && -f "$dir/$_" } readdir(DIR);
closedir(DIR);
foreach my $fn (@files) {
	my ($file,$p);
	open(INFILE, "<$dir/$fn") or die $!;
	local $/;
	$file = <INFILE>;
	close(INFILE);
	if($file =~ s/\bprefix\=(.*)\b/$p=$1;"\$prefix"/e) {
		$file =~ s/$p/\$\{prefix\}/g;
		$file =~ s/\$prefix/prefix=$prefix/;
		open(OUTFILE,">$dir/$fn") or die $!;
		print OUTFILE $file;
		close(OUTFILE);
	}
}

