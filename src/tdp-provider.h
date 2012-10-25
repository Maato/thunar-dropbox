//##############################################################################
// thunar-dropbox
//
// tdp-provider.h
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
#ifndef __TDP_PROVIDER_H__
#define __TDP_PROVIDER_H__

#include <thunarx/thunarx.h>

G_BEGIN_DECLS;

typedef struct _TdpProviderClass TdpProviderClass;
typedef struct _TdpProvider TdpProvider;

#define TDP_TYPE_PROVIDER				(tdp_provider_get_type())
#define TDP_PROVIDER(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),TDP_TYPE_PROVIDER, TdpProvider))
#define TDP_PROVIDER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), TDP_TYPE_PROVIDER, TdpProviderClass))
#define TDP_IS_PROVIDER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), TDP_TYPE_PROVIDER))
#define TDP_IS_PROVIDER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), TDP_TYPE_PROVIDER))
#define TDP_PROVIDER_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS((obj),TDP_TYPE_PROVIDER, TdpProviderClass))

GType tdp_provider_get_type			(void) G_GNUC_CONST G_GNUC_INTERNAL;
void  tdp_provider_register_type	(ThunarxProviderPlugin * plugin) G_GNUC_INTERNAL;

G_END_DECLS;

#endif
