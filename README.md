# linux-tool-forge
A collection of my own custom made binaries for popular Linux distros Arch Linux, Rocky Linux and Debian

## install instructions
Build with gcc:

`g++ <tool>.cpp -o <toolname>`

This can be combined with a linker flag like:

`g++ <tool>.cpp -o <toolname> -lssh`

The binary can be placed in a useful place, like ie. `/usr/local/bin`
