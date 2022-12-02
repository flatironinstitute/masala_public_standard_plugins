/*
    Masala
    Copyright (C) 2022 Vikram K. Mulligan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/// @file src/registration/StandardMasalaPluginsRegistrator.hh
/// @brief Headers for a static singleton that registers this plugin
/// module on instantiation.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_registration_StandardMasalaPluginsRegistrator_hh
#define Standard_Masala_Plugins_src_registration_StandardMasalaPluginsRegistrator_hh

// Forward declarations:
#include <registration/StandardMasalaPluginsRegistrator.fwd.hh>

namespace standard_masala_plugins {
namespace registration {

/// @brief A static singleton for managing output to the tracer (screen and/or logfile(s)).
class StandardMasalaPluginsRegistrator {

public:

    /// @brief Instantiate the static singleton and get a handle to it.
    static StandardMasalaPluginsRegistratorHandle get_instance();

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTRUCTOR
////////////////////////////////////////////////////////////////////////////////

    /// @brief Private constructor: object can only be instantiated with getInstance().
    /// @details Construction triggers registration of all instantiable classes in this module
    /// that will be managed by the plugin manager.
    StandardMasalaPluginsRegistrator();

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTRUCTORS AND DESTRUCTORS
////////////////////////////////////////////////////////////////////////////////

    /// @brief No copy constructor.
    StandardMasalaPluginsRegistrator( StandardMasalaPluginsRegistrator const & ) = delete;

    /// @brief No assignment operator.
    void operator=( StandardMasalaPluginsRegistrator const & ) = delete;

    /// @brief Default destructor.
    ~StandardMasalaPluginsRegistrator() = default;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

    /// @brief Register all plugins in the standard Masala plugins module with the
    /// Masala plugin manager.
    void register_plugins() const;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

    /// @brief Register all selectors in the standard Masala plugins module with the
    /// Masala plugin manager.
    void register_selectors() const;

}; // class StandardMasalaPluginsRegistrator

} // namespace registration
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_registration_StandardMasalaPluginsRegistrator_hh