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

#include "vfs-handle.h"

/*
 * Open up the given file via GnomeVFS routines.
 */
gchar	*gtranslator_vfs_handle_open_file(gchar *filename)
{
	GnomeVFSURI *file;
	GnomeVFSURI *destination;
	gchar *localfilename=g_new0(gchar,1);
	
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
		localfilename=gnome_vfs_uri_to_string(file, 1<<4);
	}
	else
	{
		/*
		 * We do use the GnomeVFSXfer interface of GnomeVFS for getting
		 *  remote files.
		 *
		 * Therefore we do need some extra stuff.
		 *
		 */
		gchar *destdir=g_new0(gchar,1);
		destdir=g_strdup_printf("%s/.gtranslator-%s",
			g_get_tmp_dir(),
			gnome_vfs_uri_get_host_name(file));
		/*
		 * Build this temporary files' dir.
		 */
		if(gnome_vfs_make_directory(destdir, GNOME_VFS_PERM_USER_ALL|
			GNOME_VFS_PERM_GROUP_ALL)!=GNOME_VFS_OK)
		{
			g_warning(_("Couldn't create the temporary directory `%s'."),
				destdir);
			if(destdir)
			{
				g_free(destdir);
			}
			return NULL;
		}
		/*
		 * The destination path.
		 */
		destination=gnome_vfs_uri_new(g_strdup_printf("%s/%s", destdir,
			gnome_vfs_uri_get_basename(file)));
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
				localfilename=gnome_vfs_uri_to_string(destination, 1<<4);
				return localfilename;
				break;
		}
	}
	if(localfilename)
	{
		return localfilename;
	}
	return NULL;
	
}
