/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*	future development in direction to better IQ-Test rankings!
*
**/

#ifndef VFS_HANDLE_H
#define VFS_HANDLE_H 1

#include <glib.h>
#include "libgtranslator.h"
#include <libgnomevfs/gnome-vfs.h>

/**
* This simply tries to get the right method to open the given file,
*  whether it's a local file or not...
**/
void gtranslator_open_file(gchar *filestring);

#endif
