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
GtranslatorDatabase *gtranslator_open_db(gchar *filename)
{
	/**
	* The "private" variables.
	**/
	GtranslatorDatabase	*db;
	xmlDocPtr		doc;
	xmlNodePtr		node;
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
	if(!g_path_is_absolute(filename))
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
	* Get the nodes from the xml-file.
	**/
	node=doc->xmlRootNode->xmlChildrenNode;
	/**
	* Cruise through the nodes.
	**/
	while(node)
	{
		/**
		* Get the msgid nodes.
		**/
		if(!strcmp(node->name, "msgid"))
		{
			/**
			* Get the message entries and the subnodes
			*  (here the msgstr entries).
			**/
			GtrMsg		*msg=g_new0(GtrMsg,1);
			xmlNodePtr 	subnodes;
			/**
			* Get the msgid via the property-tag of
			*  msgid.
			**/
			msg->msgid=xmlGetProp(node, "name");
			/**
			* Get all the subnodes of the <msgid>
			*  tag (here the msgstr tags).
			**/
			subnodes=node->xmlChildrenNode;
			/**
			* Cruise through the msgstr's and _only_
			*  the msgstrs, no other tags, please.
			**/
			while((subnodes) && (!strcmp(subnodes->name, "msgstr")))
			{
				/**
				* Get the content from the node.
				**/
				gchar 	*content;
				content=xmlNodeGetContent(subnodes);
				/**
				* If there's any content, put it to
				*  list of msgstr entries.
				**/
				if(content)
				{
					/**
					* Actually add the current entry
					*  to the list.
					**/
					msg->msgstrlist=g_list_append(
						msg->msgstrlist, 
						(gpointer) content);
				}
				/**
				* Iterate the subnodes.
				**/
				subnodes=subnodes->next;
			}
		}
		/**
		* Iterate the nodes.
		**/
		node=node->next;
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
		/**
		* Do we have multiple translations?
		**/
		if(msg->msgstrlist)
		{
			/**
			* Then write all msgstrs from
			*  the list.
			**/
			while(msg->msgstrlist)
			{
				/**
				* Write the actual entry.
				**/
				fprintf(file, "\t\t<msgstr>%s</msgstr>\n",
	                                (gchar *) (msg->msgstrlist->data));
				/**
				* Iterate the possible translations.
				**/
				msg->msgstrlist=msg->msgstrlist->next;
			}
		}
		/**
		* Or we do have a single translation?
		**/
		else
		{
			fprintf(file, "\t\t<msgstr>%s</msgstr>\n",
				msg->msgstr);
		}		
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
void gtranslator_dump_db(GtranslatorDatabase *database,
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
void gtranslator_move_db(GtranslatorDatabase *database,
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

/**
* Adds the given GtrMsg to the message database.
**/
void gtranslator_add_to_db(GtranslatorDatabase *database,
	GtrMsg *message)
{
	/**
	* The variables used in this context.
	**/
	gchar 		*searchstr=g_new0(gchar,1);
	gboolean	yippee=FALSE;
	gboolean	add_it_mister=TRUE;
	/**
	* Assign the msgid from the GtrMsg.
	**/
	searchstr=message->msgid;
	/**
	* Check if the GtrMsg's msgid is already in the
	*  database.
	**/
	while(GTR_DB_LIST(database))
	{
		/**
		* Search the message entries for the
		*  msgid.
		**/
		if(!strcmp(
			GTR_DB_LIST_MSG(database)->msgid,
			searchstr))
		{
			/**
			* This msgid is already here, so we do
			*  print out a little message for the user.
			**/
			add_it_mister=FALSE;
			g_message(_("Msgid `%s' is already in the database."),
				searchstr);
			/**
			* Check if there are multiple translatins registered
			*  already for the msgid.
			**/
			if(GTR_DB_LIST_MSG(database)->msgstrlist)
			{	
				g_message(
					_("Checking if msgstr `%s' is already declared in context for msgid `%s'..."),
					message->msgstr, searchstr);
				/**
				* Perform the check if thie msgstr is already in the list.
				**/
				while(GTR_DB_LIST_MSG(database)->msgstrlist)
				{
					/**
					* Check for existence of the msgstr.
					**/
					if(!strcmp((gchar *) (GTR_DB_LIST_MSG(database)->msgstrlist->data),
						message->msgstr))
					{
						/**
						* Set yippee to TRUE.
						**/
						yippee=TRUE;
					}
					/**
					* Iterate the list entry.
					**/
					GTR_DB_LIST_MSG(database)->msgstrlist=GTR_DB_LIST_MSG(database)->msgstrlist->next;	
				}
				/**
				* Did it occure in the list?
				**/
				if(yippee)
				{
					/**
					* Print out a warning and exit from the function.
					**/
					g_warning(_("Trying to add double msgstr list entry `%s' for msgid `%s'!"),
						message->msgstr, message->msgid);
					return;	
				}
				else
				{
					/**
					* Or add the msgstr to the list.
					**/
					GTR_DB_LIST_MSG(database)->msgstrlist=g_list_append(
						GTR_DB_LIST_MSG(database)->msgstrlist,
						message->msgstr);
				}
			}
			else
			{
				/**
				* Check if we're trying to add a double entry.
				**/
				if(!strcmp(GTR_DB_LIST_MSG(database)->msgstr,
					message->msgstr))
				{
					/**
					* Print out an error and exit from this routine.
					**/
					g_warning(_("Trying to add double msgstr `%s' for msgid `%s'!"),
						message->msgstr, message->msgid);
					return;	
				}	
				/**
				* If there's only one msgstr yet for this entry
				*  make a list out of this and add the msgstr.
				**/
				/**
				* 1) Add the old sole msgstr.
				**/
				GTR_DB_LIST_MSG(database)->msgstrlist=g_list_append(
					GTR_DB_LIST_MSG(database)->msgstrlist,
					GTR_DB_LIST_MSG(database)->msgstr);
				/**
				* 2) Add the new msgstr from the GtrMsg.
				**/	
				GTR_DB_LIST_MSG(database)->msgstrlist=g_list_append(
					GTR_DB_LIST_MSG(database)->msgstrlist,
					message->msgstr);	
			}		
		}	
		/**
		* Iterate the list.
		**/
		GTR_DB_LIST_ITERATE(database);
	}
	/**
	* Check if the GtrMsg should be added to the database.
	**/
	if(add_it_mister)
	{
		/**
		* Append it to the GtrMsgs list.
		**/
		GTR_DB_LIST(database)=g_list_append(GTR_DB_LIST(database),
			(gpointer) message);
	}
}
