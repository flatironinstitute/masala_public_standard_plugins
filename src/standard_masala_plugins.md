\mainpage Standard Masala Plugins

## Description

The Standard Masala Plugins library version 1.0 contains discrete cost function network (CFN) and continuous real-valued local (RVL) optimizers.  It also contains a framework for defining scoring functions, file interpreters, and selectors.

## Authors

- Vikram K. Mulligan, Ph.D., Research Scientist, Center for Computational Biology, Flatiron Institute (vmulligan@flatironinstitute.org).
- Tristan Zaborniak, Ph.D. Candidate, Department of Computer Science, University of Victoria (tzaborniak@flatironinstitute.org).
- Noora Azadvari, Ph.D. Candidate, Department of Bioengineering, University of Oregon (eazadvar@uoregon.edu).
- P. Douglas Renfrew, Ph.D., Research Scientist, Center for Computatinoal Biology, Flatiron Institute (pdrenfrew@flatironinstitute.org).

## Library organization

This library is divided into hand-written sub-libraries and auto-generated API sub-libraries.  All sub-libraries are in appropriately named sub-directories of the src/ directory; all namespaces start with standard_masala_plugins, and then follow the directory structure.  The namespaces of the hand-written sub-libraries are:

- \link standard_masala_plugins::file_interpreters \endlink
- \link standard_masala_plugins::optimizers \endlink
- \link standard_masala_plugins::registration \endlink
- \link standard_masala_plugins::scoring \endlink
- \link standard_masala_plugins::selectors \endlink

The corresponding auto-generated libraries are in the following namespaces:

- \link standard_masala_plugins::file_interpreters_api \endlink
- \link standard_masala_plugins::optimizers_api \endlink
- \link standard_masala_plugins::registration_api \endlink
- \link standard_masala_plugins::scoring_api \endlink
- \link standard_masala_plugins::selectors_api \endlink

## Using this library as a conventional library linked at compilation time

If you choose the link this library at compilation time, it is _only_ safe to directly call functions or instantiate classes in the auto-generated API namespaces.  These have stable APIs that will be preserved in future versions of the Standard Masala Plugins library, or deprecated in a manner that provides ample warning.  Handwritten libraries are _not_ part of the API, and can change without warning.

## Using this library as a dynamically-linked library only provided at runtime

As an alternative, you may choose only to link the Masala Core library.  In that case, you can use the \link masala::base::managers::plugin_module::MasalaPluginLibraryManager \endlink to load the Standard Masala Plugins at runtime, which will trigger registration of all plugin modules in this library that have API definitions with the \link masala::base::managers::plugin_module::MasalaPluginModuleManager \endlink, which your code may interrogate to obtain Masala plugins.  The interface for plugin modules may be accessed through the \link masala::base::api::MasalaObjectAPIDefinition \endlink for each plugin module.
