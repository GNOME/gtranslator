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

#include "vfs-handle.h"

/**
* Now simply use GnomeVFS to get the file or simply to open it.
**/
void gtranslator_open_file(gchar *filestring)
{
	GnomeVFSURI *file, *destination;
	/**
	* Test the given filestring.
	**/
	g_return_if_fail(filestring!=NULL);
	/**
	* Init GnomeVFS :-)
	**/ 
	gnome_vfs_init();
	/**
	* Flush the vfs/file_to_open configuration setting.
	**/
	gtranslator_config_init();
	gtranslator_config_set_string("vfs/file_to_open", "");
	gtranslator_config_close();
	/**
	* Get the URI for the given filestring.
	**/
	file=gnome_vfs_uri_new(filestring);
	/**
	* If it's a local file or if we didn't get a URI for it,
	*  then simply set the corresponding configuration entry.
	**/
	if((!file)||(gnome_vfs_uri_is_local(file)))
	{
		gtranslator_config_init();
		gtranslator_config_set_string("vfs/file_to_open",
			gnome_vfs_uri_to_string(file, 0));
		gtranslator_config_close();
	}
	else
	{
		/**
		* So, now we've got to get the remote file..
		*
		* We do use the GnomeVFSXfer interface of GnomeVFS for this,
		*  therefore we do need some extra stuff.
		**/
		gchar *destdir=g_new0(gchar,1);
		destdir=g_strdup_printf("%s/%s/%s", g_get_home_dir(), ".gtranslator/remote-po",
			gnome_vfs_uri_get_host_name(file));
		/**
		* Build this temporary files' dir.
		**/	
		if(gnome_vfs_make_directory(destdir, GNOME_VFS_PERM_USER_ALL|
			GNOME_VFS_PERM_GROUP_ALL)!=GNOME_VFS_OK)
		{
			g_warning(_("Couldn't create the temporary directory `%s'."),
				destdir);
			return;	
		}
		/**
		* The destination path.
		**/
		destination=gnome_vfs_uri_new(g_strdup_printf("%s/%s", destdir, 
			gnome_vfs_uri_get_basename(file)));
		/**
		* And now the URI/XFER action...
		**/	
		switch(gnome_vfs_xfer_uri(file, destination, GNOME_VFS_XFER_FOLLOW_LINKS,
			GNOME_VFS_XFER_ERROR_MODE_ABORT, GNOME_VFS_XFER_OVERWRITE_ACTION_REPLACE, 
			NULL, NULL))
		{
			case GNOME_VFS_ERROR_NOT_FOUND:
				g_warning(_("File `%s' couldn't be found!"), gnome_vfs_uri_to_string(
					file, 0));
					break;
			case GNOME_VFS_ERROR_INVALID_URI:
				g_warning(_("Malformed URI `%s' entered!"), gnome_vfs_uri_to_string(
					file, 0));
					break;
			case GNOME_VFS_ERROR_HOST_NOT_FOUND:
				g_warning(_("Host `%s' couldn't be found!"), gnome_vfs_uri_get_host_name(
					file));
					break;
			case GNOME_VFS_ERROR_INVALID_HOST_NAME:
				g_warning(_("Hostname `%s' is not valid!"), gnome_vfs_uri_get_host_name(
					file));
					break;
			case GNOME_VFS_ERROR_HOST_HAS_NO_ADDRESS:
				g_warning(_("Host `%s' has no address!"), gnome_vfs_uri_get_host_name(
					file));
					break;
			case GNOME_VFS_ERROR_CANCELLED:			
				g_warning(_("Transfer interrupted!"));
			/**
			* The last case should be the "OK" case, so we leave it and the
			*  irrelevant cases out here.
			**/
			default:
				/**
				* Now we can again set the name of the file to be opened.
				**/
				gtranslator_config_init();
				gtranslator_config_set_string("vfs/file_to_open",
					gnome_vfs_uri_to_string(destination, 0));
				gtranslator_config_close();
				break;				
		}
	}	
	
}
