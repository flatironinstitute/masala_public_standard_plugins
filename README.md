# Standard Masala Plugins

## Description

Standard plugins for distribution with the Masala software suite.  These are released under an AGPLv3 licence, permitting use for any purpose (not-for-profit or commercial), but requiring that any redistribution of this or any derived product retain an AGPL licence (version 3 or later) and include source code.

## Author

Vikram K. Mulligan, Research Scientist, Center for Computational Biology, Flatiron Institute (vmulligan@flatironinstitute.org)

## Setup

1.  Compile Masala using the buildme.sh script in the masala/ directory.
2.  Set up a simlink to the headers/masala/headers directory.  For instance: `ln -s ~/my_projects/masala/headers/ ~/my_projects/standard_masala_plugins/headers/masala/`
3.  Set up simlinks to the dylib files or so files in your masala build directory.  For instance `cd ~/myprojects/standard_masala_plugins/lib && for f in \`ls -1 ~/my_projects/masala/build/*.dylib\`; do ln -s $f .; done && cd ..`
4.  Set up a simlink to the external/ directory.  For instance: `ln -s ~/my_projects/masala/external ~/my_projects/standard_masala_plugins/`
5.  Set up a simlink to the code_templates/ directory.  For instance: `ln -s ~/my_projects/masala/code_templates ~/my_projects/standard_masala_plugins/`

## Compilation

- Note that default compilation is in debug mode.
- To compile in release mode, edit cmake/CMakeLists.txt, comment out the two "debug" lines, and uncomment the two "release" lines.
