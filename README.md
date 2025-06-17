# Standard Masala Plugins

## Description

Standard plugins for distribution with the Masala software suite.  These are released under an AGPLv3 licence, permitting use for any purpose (not-for-profit or commercial), but requiring that any redistribution of this or any derived product retain an AGPL licence (version 3 or later) and include source code.

## Author

Vikram K. Mulligan, Research Scientist, Center for Computational Biology, Flatiron Institute (vmulligan@flatironinstitute.org)

## Setup

1.  Compile Masala's Core library using the buildme.sh script in the masala/ directory.
2.  Clone the Standard Masala Plugins library in a seperate directory (not a subdirectory of Masala's Core library)
   ```
   git clone git@github.com:flatironinstitute/masala_dev_standard_plugins.git .
   ```
3.  Save the path to Masala Core and Standard Masala Plugins for using throught the setup.  
    ```
    export MASALA_PATH=<path-to-masala> # Change this to correct path for Masala Core.
    export MASALA_STANDARD_PLUGINS=<path-to-masala-plugins> # Change this to correct path for the Standard Masala Plugins.
    ```
4.  Set up a symlink to the Masala Core headers directory.
    
    ```
    ln -s $MASALA_PATH/headers/ $MASALA_STANDARD_PLUGINS/headers/masala/
    ```
    
5.  Set up symlinks to the dynamic library files in your Masala Core build directory.

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
    
6.  Set up a symlink to the external/ directory.

  ```
  ln -s $MASALA_PATH/external $MASALA_STANDARD_PLUGINS/
  ```
  
7.  Set up a symlink to the code_templates/ directory.

   ```
   ln -s $MASALA_PATH/code_templates $MASALA_STANDARD_PLUGINS/
   ```

## Compilation

- Note that default compilation is in debug mode.
- To compile in release mode, edit cmake/CMakeLists.txt, comment out the two "debug" lines, and uncomment the two "release" lines.

## Building Doxygen code documentation

All source code (both manually-written sub-libraries and auto-generated API sub-libraries) is documented with Doxygen tags.  To generate Doxygen HTML documentation, first, follow the instructions above to build Masala's Core library and the Standard Masala Plugins library.  Second, build Doxygen documentation with:

```
doxygen Doxyfile.src
```

(Note that Doxygen must be installed.) Documentation will be addded to the `html_docs/` directory.  Delete this directory to recompile documentation from scratch.
