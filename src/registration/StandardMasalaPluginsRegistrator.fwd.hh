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

/// @file src/registration/StandardMasalaPluginsRegistrator.fwd.hh
/// @brief Forward declarations for a static singleton that registers this plugin
/// module on instantiation.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_registration_StandardMasalaPluginsRegistrator_fwd_hh
#define Standard_Masala_Plugins_src_registration_StandardMasalaPluginsRegistrator_fwd_hh

namespace standard_masala_plugins {
namespace registration {

    class StandardMasalaPluginsRegistrator;

    /// Note that singletons define no shared pointers or weak pointers.  Nonconst to allow
    /// lazy loading of data.
    using StandardMasalaPluginsRegistratorHandle = StandardMasalaPluginsRegistrator *;

} // namespace registration
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_registration_StandardMasalaPluginsRegistrator_fwd_hh