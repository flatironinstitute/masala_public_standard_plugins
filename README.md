# Standard Masala Plugins

## Description

Standard plugins for distribution with the Masala software suite.  These are released under an AGPLv3 licence, permitting use for any purpose (not-for-profit or commercial), but requiring that any redistribution of this or any derived product retain an AGPL licence (version 3 or later) and include source code.

## Author

Vikram K. Mulligan, Research Scientist, Center for Computational Biology, Flatiron Institute (vmulligan@flatironinstitute.org)

## Setup

1.  Compile Masala using the buildme.sh script in the masala/ directory.
2.  Clone Masala-plugins in a seperate directory (not a subdirectory of Masala)
   ```
   git clone git@github.com:flatironinstitute/masala_dev_standard_plugins.git .
   ```
4.  Save the path to Masala and Masala-plugins for using throught the setup.  
    ```
    export MASALA_PATH=<path-to-masala> # Change it to correct path 
    export MASALA_STANDARD_PLUGINS=<path-to-masala-plugins> # Change it to correct path
    ```
5.  Set up a simlink to the Masala headers directory.
    
    ```
    ln -s $MASALA_PATH/headers/ $MASALA_STANDARD_PLUGINS/headers/masala/
    ```
    
6.  Set up simlinks to the dynamic library files in your Masala build directory.

    For Windows:
    ```
    cd $MASALA_STANDARD_PLUGINS/lib
    for f in $MASALA_PATH/build/*.dylib; do ln -s ${f}; done;
    for f in $MASALA_PATH/build/*.dylib.0.3; do ln -s ${f}; done;
    cd ..
    ```

    For Linux:
    ```
    cd $MASALA_STANDARD_PLUGINS/lib
    for f in $MASALA_PATH/build/*.so; do ln -s ${f}; done;
    for f in $MASALA_PATH/build/*.so.0.3; do ln -s ${f}; done;
    cd ..
    ```

    For Mac:
    ```
    cd $MASALA_STANDARD_PLUGINS/lib
    for f in $MASALA_PATH/build/*.dll; do ln -s ${f}; done;
    for f in $MASALA_PATH/build/*.dll.0.3; do ln -s ${f}; done;
    cd ..
    ```
    
7.  Set up a simlink to the external/ directory.

  ```
  ln -s $MASALA_PATH/external $MASALA_STANDARD_PLUGINS/
  ```
  
9.  Set up a simlink to the code_templates/ directory.

   ```
   ln -s $MASALA_PATH/code_templates $MASALA_STANDARD_PLUGINS/
   ```

## Compilation

- Note that default compilation is in debug mode.
- To compile in release mode, edit cmake/CMakeLists.txt, comment out the two "debug" lines, and uncomment the two "release" lines.
