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
* This is an internally used function, so it's declared
*  here and _not_ in the header.
**/
gboolean gtranslator_matches(GtrMsg *message, gchar *matchme);

/**
* Opens the given file to the database returned by this
*  function.
**/
GtranslatorDatabase *gtranslator_open_db(gchar *filename)
{
	/**
	* The "private" variables.
	**/
	GtranslatorDatabase	*db=g_new0(GtranslatorDatabase,1);
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

/**
* Delete the given GtrMsg from the database.
**/
void gtranslator_delete_from_db(GtranslatorDatabase *database,
	GtrMsg *message)
{
	/**
	* The commonly used variables.
	**/
	gchar 		*searchitem=g_new0(gchar,1);
	gboolean	remove_complete_gtr_msg=FALSE;
	/**
	* If there's a msgid but no msgstr, then we'd to delete
	*  all msgstr's connected with the msgid.
	**/
	if((message->msgid) && (!message->msgstr))
	{
		/**
		* The msgid is our item to search for.
		**/
		searchitem=message->msgid;
	}
	/**
	* If there's only a msgstr, but no msgid, then we'd to
	*  delete all msgstr occurences of the given msgstr.
	**/
	if((message->msgstr) && (!message->msgid))
	{
		/**
		* Here we are searching for a msgstr.
		**/
		searchitem=message->msgstr;
	}
	/**
	* If there are both msgid and msgstr defined in the
	*  given GtrMsg, then we'd to delete only this _one_
	*   GtrMsg  pair.
	**/
	if((message->msgid) && (message->msgstr))
	{
		/**
		* Now, we're again using the msgid as the
		*  "searchkey" but also setting the 
		*    remove_complete_gtr_msg gboolean
		*     to TRUE.
		**/
		searchitem=message->msgid;
		remove_complete_gtr_msg=TRUE;
	}
	/**
	* Cruise through the messages list till we find the 
	*  corresponding GtrMsg.
	**/
	while(GTR_DB_LIST(database))
	{
		/**
		* Test if the current GtrMsg contains any
		*  msgstrlist.
		**/
		if(GTR_DB_LIST_MSGSTR_LIST(database))
		{
			/**
			* Is the searched item a msgstr entry?
			**/
			if(!strcmp(searchitem, message->msgstr))
			{
				/**
				* Cruise through the list.
				**/
				while(GTR_DB_LIST_MSGSTR_LIST(database))
				{
					/**
					* Do we have got a match?
					**/
					if(!strcmp(
						GTR_DB_LIST_MSGSTR_LIST(database)->data,
						searchitem))
					{
						/**
						* Then remove the entry from
						*  the list.
						**/
						GTR_DB_LIST_MSGSTR_LIST(database)=g_list_remove(
							GTR_DB_LIST_MSGSTR_LIST(database),
							(gpointer) searchitem);	
					}
					/**
					* Iterate the list entry.
					**/
					GTR_DB_LIST_MSGSTR_LIST_ITERATE(database);
				}	
			}
		}
		/**
		* Or do we search for a single msgid or msgstr?
		**/
		else
		{
			/**
			* Check if the searched item is a msgid.
			**/
			if(!strcmp(searchitem, message->msgid))
			{
				/**
				* Then check the current entry.
				**/
				if(!strcmp(GTR_DB_LIST_MSG(database)->msgid,
					searchitem))
				{
					/**
					* Remove the GtrMsg fully from the
					*  list.
					**/
					GTR_DB_LIST(database)=g_list_remove(
						GTR_DB_LIST(database),
						(gpointer) GTR_DB_LIST_MSG(database));
				}		
			}
			/**
			* Then it's a sole msgstr.
			**/
			else
			{
				if(!strcmp(GTR_DB_LIST_MSG(database)->msgstr,
					searchitem))
				{
					/**
					* Set the msgstr to NULL.
					**/
					GTR_DB_LIST_MSG(database)->msgstr=NULL;
				}		
			}
		}
		/**
		* Iterate the list.
		**/
		GTR_DB_LIST_ITERATE(database);
	}
	/**
	* At least free the gchar.
	**/
	if(searchitem)
	{
		g_free(searchitem);
	}	
}

/**
* Deletes the given msgid from the database, uses the
*  gtranslator_delete_from_db function internally.
**/
void gtranslator_delete_msgid_from_db(GtranslatorDatabase *database,
	gchar *msgid)
{
	/**
	* Create a new GtrMsg and assign the msgid to the
	*  msgid of the new structure and set the msgstr 
	*   to NULL.
	**/
	GtrMsg	*msg=g_new0(GtrMsg,1);
	msg->msgid=msgid;
	msg->msgstr=NULL;
	/**
	* Call the gtranslator_delete_from_db function now.
	**/
	gtranslator_delete_from_db(database, msg);
}

/**
* This function deletes the given msgid from the database.
**/
void gtranslator_delete_msgstr_from_db(GtranslatorDatabase *database,
	gchar *msgstr)
{
	/**
	* Create again a new GtrMsg structure and assign the msgstr
	*  and this time set the msgid to NULL.
	**/
	GtrMsg	*msg=g_new0(GtrMsg,1);
	msg->msgid=NULL;
	msg->msgstr=msgstr;
	/**
	* Call the gtranslator_delete_from_db function now
	*  internally.
	**/
	gtranslator_delete_from_db(database, msg);
}		

/**
* This gonna be usefull, I guess...
**/
GList *gtranslator_lookup_db(GtranslatorDatabase *database,
	gchar *searchitem)
{
	/**
	* The returned GList is freshly allocated.
	**/
	GList	*result=g_list_alloc();
	/**
	* Test if there's any seachitem.
	**/
	if(!searchitem)
	{
		/**
		* Then we don't have any sophisticated
		*  way to get empty strings translated :-)
		**/
		return NULL;
	}
	/**
	* Now search for the results.
	**/
	while(GTR_DB_LIST(database))
	{
		/**
		* Now test the GtrMsgs.
		**/
		if(gtranslator_matches(
			GTR_DB_LIST_MSG(database),
			searchitem))
		{
			/**
			* Then prepend the current GtrMsg
			*  to the list.
			**/
			result=g_list_prepend(result,
				GTR_DB_LIST_MSG(database));
		}
		/**
		* Iterate to the next entry.
		**/
		GTR_DB_LIST_ITERATE(database);
	}
	/**
	* Now reverse the results' list.
	**/
	result=g_list_reverse(result);
	/**
	* Return the build list.
	**/
	return result;
}

/**
* An utility function to determine if the GtrMsg
*  contains the given gchar.
**/
gboolean gtranslator_matches(GtrMsg *message, gchar *matchme)
{
	/**
	* Test if our GtrMsg and the gchar are matching.
	**/
	/**
	* Simpliest case: The msgid match.
	**/
	if(!strcmp(message->msgid, matchme))
	{
		return TRUE;
	}
	/**
	* Is there a msgstrlist ?
	**/
	if(message->msgstrlist)
	{
		/**
		* Then cruise through the list.
		**/
		while(message->msgstrlist)
		{
			/**
			* Is this match-request in the current
			*  list data (aka. msgstr).
			**/
			if(!strcmp(message->msgstrlist->data,
				matchme))
			{
				return TRUE;
			}	
			/**
			* Iterate the list entry.
			**/
			message->msgstrlist=message->msgstrlist->next;
		}	
	}
	/**
	* Or is this only a sole msgstr?
	**/
	else
	{
		/**
		* Test if we've got a match and 
		*  return TRUE.
		**/
		if(!strcmp(message->msgstr, matchme))
		{
			return TRUE;
		}	
	}
	/**
	* Huh, is there any case left?
	**/
	return FALSE;
}
