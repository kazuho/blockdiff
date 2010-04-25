use strict;
use warnings;

use File::Temp qw(tempdir);
use Test::More;

my $workdir = tempdir(CLEANUP => 1);
my $testfile = "$workdir/out.txt";
my $BS_HDR = "\1" . ("\0"x3); # 1 byte in little endian

# create "hello"
runit(
    join '', map {
        chr($_) . ("\0"x7) . substr "hello", $_, 1
    } (0..4)
);
is read_output(), "hello";

# s/e/a/
runit(
    "\1" . ("\0" x 7) . 'a',
);
is read_output(), "hallo";

done_testing;

sub runit {
    my $dump = shift;
    open my $fh, '|-', "blib/script/blockdiff_merge $testfile"
        or die "failed to exec blockdiff_merge:$!";
    print $fh $BS_HDR, $dump;
    close $fh
        or die "blockdiff_merge returned error:$?";
}

sub read_output {
    open my $fh, '<', $testfile
        or die "failed to open file:$testfile:$!";
    join '', <$fh>;
}
