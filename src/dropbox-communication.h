//##############################################################################
// thunar-dropbox
//
// dropbox-communication.h
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
#ifndef __DROPBOX_COMMUNICATION_H__
#define __DROPBOX_COMMUNICATION_H__

#include <glib.h>

extern gboolean dropbox_connect(int * sock);
extern void dropbox_do_verb(gchar * verb, GList * files);
extern void dropbox_write(GIOChannel * io_channel, char * str);

#endif
