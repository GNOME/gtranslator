/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
 *
 * libgtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * libgtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef LIBGTRANSLATOR_VFS_HANDLE_H
#define LIBGTRANSLATOR_VFS_HANDLE_H 1

/*
 * The GnomeVFS based routines of libgtranslator are here.
 */ 

#include <glib.h>
#include "libgtranslator.h"
#include <libgnomevfs/gnome-vfs.h>
#include <dirent.h>

/*
 * Opens up the given file via GnomeVFS routines and returns the
 *  local filename of the transferred file.
 */
gchar 	*gtranslator_vfs_handle_open_file(gchar *filestring);

#endif
