#include "open-differently.h"

/**
* Opens the given mo-file...
**/
void gtranslator_open_mo_file(gchar *file)
{
	gchar *cmd;
	gchar *tempfilename;
	/**
	* Set the temporary filename...
	**/
	tempfilename=g_strdup_printf("%s/temp_po_file",
		((g_getenv("TMPDIR")) ? g_getenv("TMPDIR"): "/tmp"));
	/**
	* Build up the command to execute in the shell.
	**/
	cmd=g_strdup_printf("msgunfmt %s -o %s",
		file,
		tempfilename);
	/**
	* Execute the command.
	**/
	if(!system(cmd))
	{
		/**
		* Yes, it worked for us...
		**/
		parse(tempfilename);
	}
	else
	{
		/**
		* "Misuse" cmd
		**/
		cmd=g_strdup_printf(_("Couldn't open mo-file `%s'!"),
			file);
		/**
		* Sorry, didn't work...
		**/
		gnome_app_warning(GNOME_APP(app1), cmd);
	}
	g_free(cmd);
	g_free(tempfilename);
}

/**
* This is mostly only a copy&pasted part of the mo/gmo-file
*  opening function which is just adapted to use gunzip ...
**/
void gtranslator_open_gzipped_po_file(gchar *file)
{
	gchar *cmd;
	gchar *tempfilename;
	/**
	* Set the temporary filename...
	**/
	tempfilename=g_strdup_printf("%s/temp_po_file",
		((g_getenv("TMPDIR")) ? g_getenv("TMPDIR"): "/tmp"));
	/**
	* Build up the command to execute in the shell.
	**/
	cmd=g_strdup_printf("gzip -dc < %s > %s",
		file,
		tempfilename);
	/**
	* Execute the command.
	**/
	if(!system(cmd))
	{
		/**
		* Yes, it worked for us...
		**/
		parse(tempfilename);
	}
	else
	{
		/**
		* "Misuse" cmd
		**/
		cmd=g_strdup_printf(_("Couldn't open gzipped po-file `%s'!"),
			file);
		/**
		* Sorry, didn't work...
		**/
		gnome_app_warning(GNOME_APP(app1), cmd);
	}
	g_free(cmd);
	g_free(tempfilename);
}
