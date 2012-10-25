//##############################################################################
// thunar-dropbox
//
// dropbox-communication.c
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
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>

//##############################################################################
// Exported functions
//##############################################################################
gboolean dropbox_connect(int * sock)
{
	int flags = 0;
	int retval = -1;
	struct sockaddr_un address;
	socklen_t address_length = 0;
	struct timeval tv;
	*sock = -1;

	// Initialize address structure
	memset(&address, 0x0, sizeof(address));
	address.sun_family = AF_UNIX;
	g_snprintf(address.sun_path, sizeof(address.sun_path),
		"%s/.dropbox/command_socket", g_get_home_dir());

	// Calculate the length of the address
	address_length = sizeof(address) - sizeof(address.sun_path) +
		strlen(address.sun_path);

	// Create socket
	retval = socket(PF_UNIX, SOCK_STREAM, 0);
	if(retval < 0)
		goto failed;
	*sock = retval;

	// Set connect timeout
	tv.tv_sec = 0;
	tv.tv_usec = 1000 * 50;
	retval = setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if(retval < 0)
		goto failed;
	retval = setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	if(retval < 0)
		goto failed;

	// Set native non-blocking, for connect timeout
	retval = fcntl(*sock, F_GETFL, 0);
	if(retval < 0)
		goto failed;
	flags = retval;
	retval = fcntl(*sock, F_SETFL, flags | O_NONBLOCK);
	if(retval < 0)
		goto failed;

	// Connect
	retval = connect(*sock, (struct sockaddr*)&address, address_length);
	if(retval < 0 && errno == EINPROGRESS)
	{
		fd_set writers;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		FD_ZERO(&writers);
		FD_SET(*sock, &writers);

		// Wait for the socket to be ready
		retval = select((*sock)+1, NULL, &writers, NULL, &tv);
		if(retval == 0)
			goto failed;

		// Try to connect again
		retval = connect(*sock, (struct sockaddr*)&address, address_length);
		if(retval < 0)
			goto failed;
	}
	else if(retval < 0)
	{
		goto failed;
	}

	// Set socket to blocking
	retval = fcntl(*sock, F_SETFL, flags);
	if(retval < 0)
		goto failed;

	return TRUE;

failed:
	if(*sock != -1)
		close(*sock);
	*sock = -1;
	return FALSE;
}

void dropbox_write(GIOChannel * io_channel, char * str)
{
	gsize bytes_written;
	GIOStatus status;

	do
	{
		status = g_io_channel_write_chars(io_channel, str, -1,
			&bytes_written, NULL);
	} while(status == G_IO_STATUS_AGAIN);

	if(status == G_IO_STATUS_ERROR)
	{
		fprintf(stderr, "dropbox_write() - G_IO_STATUS_ERROR\n");
		exit(EXIT_FAILURE);
	}
}


void dropbox_do_verb(gchar * verb, GList * files)
{
	GList * lp;
	int socket = 0;
	GIOChannel * io_channel = NULL;

	if(!dropbox_connect(&socket))
	{
		fprintf(stderr, "Connecting failed\n");
		return;
	}

	io_channel = g_io_channel_unix_new(socket);
	g_io_channel_set_close_on_unref(io_channel, TRUE);
	g_io_channel_set_line_term(io_channel, "\n", -1);

	dropbox_write(io_channel, "icon_overlay_context_action\n");
	dropbox_write(io_channel, "paths");

	for(lp = files; lp != NULL; lp = lp->next)
	{
		dropbox_write(io_channel, "\t");
		dropbox_write(io_channel, lp->data);
	}

	dropbox_write(io_channel, "\nverb\t");
	dropbox_write(io_channel, verb);
	dropbox_write(io_channel, "\ndone\n");

	g_io_channel_flush(io_channel, NULL);

	g_io_channel_unref(io_channel);
}

