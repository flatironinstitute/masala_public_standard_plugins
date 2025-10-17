# Standard Masala Plugins

![Masala logo](logos/Masala_logo_standard_plugins_sm.png)

## Version

The Standard Masala Plugins library is currently version 1.1.

## Description

Standard plugins for distribution with the Masala software suite.  These are released under an AGPLv3 licence, permitting use for any purpose (not-for-profit or commercial), but requiring that any redistribution of this or any derived product retain an AGPL licence (version 3 or later) and include source code.

## Authors

The Standard Masala Plugins were created by Vikram K. Mulligan, a Research Scientist in the Center for Computational Biology, at the Flatiron Institute.  For questions, e-mail vmulligan@flatironinstitute.org.

In addition, the following individuals contributed to the development of the Standard Masala Plugins:

- Noora Azadvari, a graduate student at the University of Oregon (eazadvar@uoregon.edu).
- Tristan Zaborniak, a graduate student at the University of Victoria (tristanz@uvic.ca).
- Qiyao Zhu, a Flatiron Research Fellow in the Center for Computational Biology, Flatiron Institute (qzhu@flatironinstitute.org).
- S.M. Bargeen A. Turzo, a Flatiron Research Fellow in the Center for Computational Biology, Flatiron Institute (bturzo@flatironinstitute.org).
- Parisa Hossienzadeh, a professor at the University of Oregon (parisah@uoregon.edu).
- P. Douglas Renfrew, a Research Scientist in the Center for Computational Biology, Flatiron Institute (pdrenfrew@flatironinstitute.org).

The Standard Masala Plugins are maintained by the Biomolecular Design (BmD) Group in the Center for Computatonal Biology at the Flatiron Institute.  Vikram K. Mulligan and P. Douglas Renfrew co-head the BmD Group.

## Citing the Standard Masala Plugins library

If you use the Standard Masala Plugins library for work that you publish, please cite the following preprint:

**T. Zaborniak, N. Azadvari, Q. Zhu, S.M.B.A. Turzo, P. Hosseinzadeh, P.D. Renfrew, and V.K. Mulligan.  (2025).  The open-source Masala software suite: Facilitating rapid methods development for synthetic heteropolymer design. _bioRxiv_ [https://www.biorxiv.org/content/10.1101/2025.07.02.662756v2](https://www.biorxiv.org/content/10.1101/2025.07.02.662756v2).**

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

    ```
    cd $MASALA_STANDARD_PLUGINS/lib
    ln -s $MASALA_PATH/build/lib*.* .
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

To compile, run the `buildme.sh` script.

- Note that default compilation is in release mode.
- To compile in debug mode, edit cmake/CMakeLists.txt and change `SET( MODE release )` to `SET( MODE debug )`.

The compilation may be deleted completely by running the `delete_build.sh` script.  Note that this is not necessary when small changes to the source code are made: simply re-running `buildme.sh` will recompile and relink only the subset of files that need recompilation or relinking.

## Building Doxygen code documentation

All source code (both manually-written sub-libraries and auto-generated API sub-libraries) is documented with Doxygen tags.  To generate Doxygen HTML documentation, first, follow the instructions above to build Masala's Core library and the Standard Masala Plugins library.  Second, build Doxygen documentation with:

```
doxygen Doxyfile.src
```

(Note that Doxygen must be installed.) Documentation will be addded to the `html_docs/` directory.  Delete this directory to recompile documentation from scratch.  Prebuild Doxygen documentation for the current version of the Standard Masala Plugins library may be found at [https://users.flatironinstitute.org/~vmulligan/doxygen/masala_standard_plugins_doxygen/index.html](https://users.flatironinstitute.org/~vmulligan/doxygen/masala_standard_plugins_doxygen/index.html).

## Version History

- Version 1.1 made minor efficiency tweaks to the behaviour of the PairwisePrecomputedCostFunctionNetworkOptimizer.
- Version 1.0 was the first public release.

## Licence

Masala and the Standard Masala Plugins are released under an AGPL version 3 licence.  This licence permits anyone to use the software for any purpose (not-for-profit or commercial), as well as to modify the software and incorporate it into derivative works.  The one stipulation is that if any modified version or derivative work is distributed, or users are given access to it over a network, then the source code must be made available under a GPL or AGPL licence (version 3 or later).  This ensures that the software remains free and open-source.  For full details, see the LICENCE file.
