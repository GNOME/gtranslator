/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "nautilus-string.h"
#include "vfs-handle.h"

#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>
#include <libgnome/gnome-util.h>

#include <libgnomevfs/gnome-vfs-types.h>

#include <libgnomevfs/gnome-vfs-init.h>
#include <libgnomevfs/gnome-vfs-uri.h>
#include <libgnomevfs/gnome-vfs-xfer.h>

#include <gal/util/e-util.h>

/*
 * Open up the given file via GnomeVFS routines.
 */
gchar	*gtranslator_vfs_handle_open(gchar *filename)
{
	GnomeVFSURI *file;
	GnomeVFSURI *destination;
	GString *localfilename=g_string_new("");
	
	/*
	 * Init GnomeVFS, if that hasn't already be done.
	 */
	if(!gnome_vfs_initialized())
	{
		gnome_vfs_init();
	}
	
	/*
	 * Get the corresponding URI for the filename.
	 */
	file=gnome_vfs_uri_new(filename);
	
	/*
	 * If an URI is locally available open it with
	 *  the standard methods.
	 */
	if(gnome_vfs_uri_is_local(file))
	{
		gchar *temp;

		temp=gnome_vfs_uri_to_string(file, 
			GNOME_VFS_URI_HIDE_TOPLEVEL_METHOD);

		g_return_val_if_fail(temp!=NULL, NULL);

		switch(temp[0])
		{
			case '.':
				/*
				 * Append the current directory + a '/' +
				 *  the relative filename to a real file
				 *   path.
				 */  
				g_string_sprintf(localfilename,
					"%s/%s",
					g_get_current_dir(),
					temp);
				break;

			default:
				/*
				 * Append the toplevel '/' to the URI.
				 */ 
				g_string_sprintf(localfilename,
					"/%s",
					temp);
				break;
		}
		
		g_free(temp);

		return localfilename->str;
	}
	else
	{
		/*
		 * We do use the GnomeVFSXfer interface of GnomeVFS for getting
		 *  remote files.
		 *
		 * Therefore we do need some extra stuff.
		 */
		gchar *destdir;
		destdir=g_strdup_printf("%s/.gtranslator/%s",
			g_get_home_dir(),
			gnome_vfs_uri_get_host_name(file));
	
		/*
		 * Test if this directory is already existent and
		 *  create the directory if it's existent.
		 */ 
		if(!g_file_test(destdir, G_FILE_TEST_ISDIR))
		{
			if(e_mkdir_hier(destdir, 0755)==-1)
			{
				g_warning(_("Couldn't create temporary directory `%s'!"),
					destdir);

				return NULL;
			}
		}
		
		
		/*
		 * The destination path.
		 */
		destination=gnome_vfs_uri_new(g_strdup_printf("%s/%s", destdir,
			gnome_vfs_uri_get_basename(file)));

		g_free(destdir);
		
		/*
		 * And now the URI/XFER action...
		 */
		switch(gnome_vfs_xfer_uri(file,
				destination,
				GNOME_VFS_XFER_FOLLOW_LINKS,
				GNOME_VFS_XFER_ERROR_MODE_ABORT,
				GNOME_VFS_XFER_OVERWRITE_ACTION_REPLACE,
				NULL,
				NULL))
		{
			case GNOME_VFS_ERROR_NOT_FOUND:
				g_warning(_("File `%s' couldn't be found!"),
					gnome_vfs_uri_to_string(file, 0));
				return NULL;
					break;
			case GNOME_VFS_ERROR_INVALID_URI:
				g_warning(_("Malformed URI `%s' entered!"),
					gnome_vfs_uri_to_string(file, 0));
				return NULL;
					break;
			case GNOME_VFS_ERROR_HOST_NOT_FOUND:
				g_warning(_("Host `%s' couldn't be found!"),
					gnome_vfs_uri_get_host_name(file));
				return NULL;
					break;
			case GNOME_VFS_ERROR_INVALID_HOST_NAME:
				g_warning(_("Hostname `%s' is not valid!"),
					gnome_vfs_uri_get_host_name(file));
				return NULL;
					break;
			case GNOME_VFS_ERROR_HOST_HAS_NO_ADDRESS:
				g_warning(_("Host `%s' has no address!"),
					gnome_vfs_uri_get_host_name(file));
				return NULL;
					break;
			case GNOME_VFS_ERROR_CANCELLED:
				g_warning(_("Transfer interrupted!"));
				return NULL;
					break;
			/*
			 * The last case should be the "OK" case.
			 */
			default:
				/*
				 * Return the local destination filename.
				 */
				localfilename=g_string_append(localfilename,
				gnome_vfs_uri_to_string(
					destination,
					
					/*
					 * Hide all parts of an URI, so that we
					 *  should get a single plain filename
					 *   string.
					 */ 
					GNOME_VFS_URI_HIDE_HOST_NAME |
					GNOME_VFS_URI_HIDE_HOST_PORT |
					GNOME_VFS_URI_HIDE_TOPLEVEL_METHOD |
					GNOME_VFS_URI_HIDE_USER_NAME |
					GNOME_VFS_URI_HIDE_PASSWORD  |
					GNOME_VFS_URI_HIDE_FRAGMENT_IDENTIFIER));
				return localfilename->str;
				break;
		}
	}
	
	g_string_free(localfilename, 1);
}
