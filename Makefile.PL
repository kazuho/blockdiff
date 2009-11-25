use inc::Module::Install;

name 'App-Blockdiff';

all_from 'lib/App/Blockdiff.pm';

configure_requires 'Module::Install::ReadmeFromPod';

install_script 'blockdiff_backup';
install_script 'blockdiff_dump';
install_script 'blockdiff_merge';
install_script 'ssh_blockdiff_dump';
install_script 'ssh_lvm_dump';

auto_include;
auto_install;

WriteAll;
