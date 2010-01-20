use strict;
use warnings;

use Digest::MD5 qw(md5);
use File::Temp qw(tempdir);
use POSIX qw(:sys_wait_h);
use Test::More;

my $BS_HDR = "\0\x40\0\0"; # 16384 (block size) in little endian

my $tempdir = tempdir(CLEANUP => 1);

# full backup test
my $out = runit("t/assets/hello.txt");
my $status = $?;
ok WIFEXITED($?), 'blockdiff_dump exited';
is WEXITSTATUS($?), 0, 'blockdiff_dump exit code';
is(
    $out,
    $BS_HDR . ("\0"x8) . read_file('t/assets/hello.txt'),
    'blockdiff_dump output',
);
is(
    read_file("$tempdir/md5.out"),
    $BS_HDR . md5(read_file('t/assets/hello.txt')),
    'blockdiff_dump md5 output',
);

# nexist
runit("t/assets/nexist.txt", 1);
$status = $?;
ok WIFEXITED($?), 'blockdiff_dump exitted';
isnt WEXITSTATUS($?), 0, 'blockdiff_dump error on exit';

# TODO: incremental backup

# TODO: -z and --lzop

done_testing;

sub runit {
    my ($fn, $no_stderr) = @_;
    open(
        my $fh,
        '-|',
        "blib/script/blockdiff_dump $fn 6> $tempdir/md5.out"
            . ($no_stderr ? ' 2> /dev/null' : ''),
    ) or die "failed to exec blockdiff:$!";
    my $out = join '', <$fh>;
    close $fh;
    $out;
}

sub read_file {
    my $fn = shift;
    open my $fh, '<', $fn
        or die "failed to open file:$fn:$!";
    return join '', <$fh>;
}
