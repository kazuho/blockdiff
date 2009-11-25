package App::Blockdiff;

our $VERSION = 0.01;

1;
__END__

=head1 NAME

App::Blockdiff - collection of scripts for block-based diff (incremental backups) and merge

=head1 SYNOPSIS

  # remote file backup
  % blockdiff_backup backup_name ssh_blockdiff_dump -z root@host remote_file
  % blockdiff_backup backup_name ssh_blockdiff_dump -z root@host remote_file
  ...

  # remote lvm backup (snapshot)
  % blockdiff_backup backup_name ssh_lvm_dump -z root@host remote_lv
  % blockdiff_backup backup_name ssh_lvm_dump -z root@host remote_lv
  ...

  # low level backup (full)
  % blockdiff_dump < file 6> backup_md5.1 > backup.1
  # low level backup (incremental)
  % blockdiff_dump < file 5< backup_md5.1 6> backup_md5.2 > backup.2
  % blockdiff_dump < file 5< backup_md5.2 6> backup_md5.3 > backup.3
  ...


=head1 DESCRIPTION

App::Blockdiff is a collection of script files for taking block-based diffs and merging them.  It is well suited for taking backups of disk images and database files.

=head1 SEE ALSO

L<blockdiff_backup>   - high level wrapper for taking backups

L<ssh_blockdiff_dump> - wrapper script for running blockdiff_backup over ssh

L<ssh_lvm_backup>     - wrapper for taking LVM snapshot-based backups over ssh

L<blockdiff_dump>     - low level backup script

L<blockdiff_merge>    - merge script (used for restoration)

=head1 AUTHOR

Copyright (C) 2009 Cybozu Labs, Inc., written by Kazuho Oku

=head1 LICENSE

This program is free software; you can redistribute it and/or modify it under the same terms as Perl itself.

See http://www.perl.com/perl/misc/Artistic.html

=cut
