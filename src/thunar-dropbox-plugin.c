//##############################################################################
// thunar-dropbox
//
// thunar-dropbox-plugin.c
// 
// Copyright 2010 Maato
//
// Authors:
//    Maato <maato@softwarebakery.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3, as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranties of
// MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
//##############################################################################
#include "tdp-provider.h"

//##############################################################################
// Static variables
//##############################################################################
static GType type_list[1];

//##############################################################################
// Exported functions
//##############################################################################
G_MODULE_EXPORT void thunar_extension_initialize(ThunarxProviderPlugin * plugin)
{
	tdp_provider_register_type (plugin);
	type_list[0] = TDP_TYPE_PROVIDER;
}

G_MODULE_EXPORT void thunar_extension_shutdown(void)
{
}

G_MODULE_EXPORT void thunar_extension_list_types(const GType ** types,
	gint * n_types)
{
	*types = type_list;
	*n_types = G_N_ELEMENTS(type_list);
}
