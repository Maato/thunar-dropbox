//##############################################################################
// thunar-dropbox
//
// tdp-provider.c
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
#include <unistd.h>
#include <gio/gio.h>

#include "tdp-provider.h"

//##############################################################################
// Function prototypes
//##############################################################################
static void tdp_provider_menu_provider_init(ThunarxMenuProviderIface * iface);
static void tdp_provider_finalize(GObject * object);
static GList * tdp_provider_get_file_actions(
	ThunarxMenuProvider * menu_provider, GtkWidget * window, GList * files);

//##############################################################################
// GObject
//##############################################################################
struct _TdpProviderClass
{
	GObjectClass __parent__;
};

struct _TdpProvider
{
	GObject __parent__;
};

//##############################################################################
// Thunarx 
//##############################################################################
THUNARX_DEFINE_TYPE_WITH_CODE (TdpProvider,
	tdp_provider,
	G_TYPE_OBJECT,
	THUNARX_IMPLEMENT_INTERFACE (THUNARX_TYPE_MENU_PROVIDER,
		tdp_provider_menu_provider_init));

//##############################################################################
// Other 
//##############################################################################
static void tdp_provider_class_init(TdpProviderClass * klass)
{
	GObjectClass * gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->finalize = tdp_provider_finalize;
}

static void tdp_provider_menu_provider_init(ThunarxMenuProviderIface * iface)
{
	iface->get_file_actions = tdp_provider_get_file_actions;
}

static void tdp_provider_init(TdpProvider * tdp_provider)
{

}

static void tdp_provider_finalize(GObject * object)
{
	TdpProvider * tdp_provider = TDP_PROVIDER(object);

	(*G_OBJECT_CLASS(tdp_provider_parent_class)->finalize)(object);
}

static void tdp_callback(GtkAction * action, gpointer data)
{
	GList * actioninfo = (GList*)data;
	gchar * verb = NULL;

	if(actioninfo == NULL)
		return;

	verb = actioninfo->data;
	actioninfo = actioninfo->next;

	dropbox_do_verb(verb, actioninfo);
}

static void tdp_closure_destroy_notify(gpointer data, GClosure * closure)
{
	GList * actioninfo = (GList*)data;
	GList * lp;

	for(lp = actioninfo; lp != NULL; lp = lp->next)
	{
		g_free(lp->data);
	}

	g_list_free(actioninfo);
}

static GList * add_action(GList * list, GList * filelist, gchar * str)
{
	GtkAction * action = NULL;
	gchar ** argval;
	guint len;
	GList * actioninfo = NULL;
	GList * iter;

	for(iter = filelist; iter != NULL; iter = iter->next)
	{
		actioninfo = g_list_append(actioninfo, g_strdup(iter->data));
	}

	argval = g_strsplit(str, "~", 0);
	len = g_strv_length(argval);

	if(len == 3)
	{
		gchar unique_name[128];
		g_sprintf(unique_name, "Tdp::%s", argval[2]);

		action = g_object_new(GTK_TYPE_ACTION,
			"name", unique_name,
			"label", argval[0],
			"tooltip", argval[1],
			"icon-name", "thunar-dropbox",
			 NULL);

		actioninfo = g_list_prepend(actioninfo, g_strdup(argval[2]));

		GClosure * closure = g_cclosure_new(
			G_CALLBACK(tdp_callback),
			(gpointer)actioninfo,
			tdp_closure_destroy_notify);

		g_signal_connect_closure(G_OBJECT(action), "activate", closure, TRUE);
	}

	g_strfreev(argval);

	if(action != NULL)
		list = g_list_append(list, action);
	return list;
}

static GList * tdp_provider_get_file_actions(
	ThunarxMenuProvider * menu_provider,
	GtkWidget * window,
	GList * files)
{
	GFile * file;
	GList * actions = NULL;
	GList * lp;
	gchar * path;

	GList * filelist = NULL;

	int socket;
	if(!dropbox_connect(&socket))
		return NULL;

	GIOChannel * io_channel = g_io_channel_unix_new(socket);
	g_io_channel_set_close_on_unref(io_channel, TRUE);
	g_io_channel_set_line_term(io_channel, "\n", -1);

	dropbox_write(io_channel, "icon_overlay_context_options\n");
	dropbox_write(io_channel, "paths");

	for(lp = files; lp != NULL; lp = lp->next)
	{
		file = thunarx_file_info_get_location(lp->data);
		path = g_file_get_path(file);
		g_object_unref (file);
		if(path == NULL)
			continue;

		dropbox_write(io_channel, "\t");
		dropbox_write(io_channel, path);

		filelist = g_list_append(filelist, path);
	}

	dropbox_write(io_channel, "\ndone\n");
	g_io_channel_flush(io_channel, NULL);

	for(;;)
	{
		gchar * line;
		GIOStatus status = g_io_channel_read_line(io_channel, &line,
			NULL, NULL, NULL);

		if(status == G_IO_STATUS_NORMAL)
		{
			if(strcmp(line, "done\n") == 0)
			{
				g_free(line);
				break;
			}
			else if(strcmp(line, "notok\n") == 0)
			{
			}
			else if(strcmp(line, "ok\n") == 0)
			{
			}
			else
			{
				gchar ** argval;
				guint len;

				argval = g_strsplit(line, "\t", 0);
				len = g_strv_length(argval);

				if(len > 1)
				{
					int i;
					for(i = 0; i < len; i++)
					{
						actions = add_action(actions, filelist, argval[i]);
					}
				}

				g_strfreev(argval);
			}

			g_free(line);
		}
		else if(status == G_IO_STATUS_AGAIN)
		{
			break;
		}
		else if(status == G_IO_STATUS_ERROR)
		{
			break;
		}
	}

	for(lp = filelist; lp != NULL; lp = lp->next)
	{
		g_free(lp->data);
	}
	g_list_free(filelist);

	g_io_channel_unref(io_channel);
	return actions;
}
