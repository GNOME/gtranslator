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
* PSC: This has been completely written with vim; the best editor of all.
*
**/

#include <libgtranslator/handle-db.h>
#include <unistd.h>

/**
* Opens the given file to the database returned by this
*  function.
**/
GtranslatorDatabase gtranslator_open_db(gchar *filename)
{
	/**
	* The "private" variables.
	**/
	GtranslatorDatabase	db;
	xmlDocPtr		doc;
	/**
	* Check the filename for existence and 
	*  absolutelyness.
	**/
	if(!filename)
	{
		/**
		* Print out a warning and exit from the
		*  function.
		**/
		g_warning(_("No file specified!"));
		return NULL;
	}
	/**
	* Test the filename for absolutelyness.
	**/
	if(g_path_is_absolute(filename))
	{
		/**
		* Also print out a warning and exit.
		**/
		g_warning(
			_("The given filepath isn't absolute!"));
		return NULL;
	}
	/**
	* Open the file with libxml.
	**/
	doc=xmlParseFile(filename);
	/**
	* Is this file existent/did an error occure?
	**/
	if(!doc)
	{
		g_warning(_("Couldn't open database `%s'!"), filename);
		return NULL;
	}
	/**
	* Return the build GtranslatorDatabase.
	**/
	return db;
}

/**
* This function saves the given database.
**/
void gtranslator_save_db(GtranslatorDatabase *database)
{
	/**
	* The used filestream, the date holding gchar
	*  and the used GtrMsg structure.
	**/
	FILE 		*file;
	gchar 		datestr[10];
	GtrMsg		*msg=g_new0(GtrMsg,1);
	/**
	* The time stuff.
	**/
	time_t		timetick;
	struct	tm	*timebox;
	/**
	* First a check.
	**/
	if(!database)
	{
		g_warning(_("No translation database given to store."));
		return;
	}
	/**
	* Build the date string.
	*
	* 1) Get the current time im UTC.
	**/
	timetick=time(NULL);
	/**
	* 2) Get the timebox :-)
	**/
	timebox=localtime(&timetick);
	/**
	* 3) Build the date string.
	**/
	strftime(datestr, 10, "%Y-%m-%d", timebox);
	/**
	* Open the file.
	**/
	file=fopen(GTR_DB_FILENAME(database), "w");
	/**
	* Test if the stream is still alive :-)=
	**/
	if(!file)
	{
		g_warning(_("Lost file stream for message database `%s'."),
			GTR_DB_FILENAME(database));
		return;
	}
	/**
	* Write the header.
	**/
	fprintf(file, "<?xml version=\"1.0\"?>\n");
	/**
	* The database header fields (like language, author, email address
	*  etc.
	**/
	fprintf(file, "<db language=\"%s\" author=\"%s\" email=\"%s\">\n",
		GTR_DB_LANG(database), GTR_DB_AUTHOR(database), 
		GTR_DB_AUTHOR_EMAIL(database));
	/**
	* The serial fields with the current date and an iterated serial
	*  number.
	**/
	fprintf(file, "<serial date=\"%s\">%i</serial>\n",
		datestr,
		GTR_DB_SERIAL(database)++);	
	/**
	* Now store all the entries.
	**/
	while(GTR_DB_LIST(database)!=NULL)
	{
		/**
		* Get the GtrMsg from the list.
		**/
		msg=GTR_DB_LIST_MSG(database);
		/**
		* Write the msgid/msgstr pairs.
		**/
		fprintf(file, "\t<msgid name=\"%s\">\n",
			msg->msgid);
		fprintf(file, "\t\t<msgstr>%s</msgstr>\n",
			msg->msgstr);
		fprintf(file, "\t</msgid>\n");		
		/**
		* Iterate to the next list element.
		**/
		GTR_DB_LIST_ITERATE(database);
	}
	/**
	* Now write the last closing tag.
	**/
	fprintf(file, "</db>\n");
	/**
	* Check the file stream and close it.
	**/
	if(file)
	{
		fclose(file);
	}
	/**
	* Test the GtrMsg and also free it.
	**/
	if(msg)
	{
		g_free(msg);
	}
}

/**
* This "dumps" the database to the given file.
**/
void gtranslator_dump_db(GtranslatorDatabase database,
	gchar *filename)
{
	/**
	* The new gchar variable.
	**/
	gchar 	*temp=g_new0(gchar,1);
	/**
	* Simple check the filename.
	**/
	if(!filename)
	{
		g_warning(_("No filename given to dump the database."));
		return;
	}
	/**
	* Get the old "original" filename to the new variable.
	**/
	temp=g_strdup(GTR_DB_FILENAME(database));
	/**
	* Now set the "new" filename.
	**/
	GTR_DB_FILENAME(database)=filename;
	/**
	* Save it.
	**/
	gtranslator_save_db(database);
	/**
	* And now "restore" the original filename.
	**/
	GTR_DB_FILENAME(database)=g_strdup(temp);
	/**
	* Free the temporary variable.
	**/
	if(temp)
	{
		g_free(temp);
	}	
}

/**
* Moves the database to the new "location" and unlinks
*  the original location.
**/
void gtranslator_move_db(GtranslatorDatabase database,
	char *moveto)
{
	/**
	* Dump the database to the new file.
	**/
	gtranslator_dump_db(database, moveto);
	/**
	* And now unlink the "old" file.
	**/
	if(!unlink(GTR_DB_FILENAME(database)))
	{
		/**
		* If the action didn't succeed inform
		*  the user about this.
		**/
		g_warning(_("Couldn't remove database file `%s'!"),
			GTR_DB_FILENAME(database));
	}
}
