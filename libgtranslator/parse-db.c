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

#include "parse-db.h"

/**
* If NLS is requested include the headers and set the define and if it's
*  not requested, simply make a foo-define.
*/
#ifndef ENABLE_NLS
        #include <libintl.h>
        #include <locale.h>
        #define _(String) gettext(String)
#else
	        #define _(String) String
#endif

/**
* Parses the lang.xml file.
**/
void parse_db_for_lang(gchar *language)
{
	gchar file[256];
	gboolean lusp=FALSE;
	/**
	* Check if we did get a language to search for ..
	**/
	if(!language)
	{
		g_warning(_("No language defined in the query request!"));
	}
	/**
	* Check if we got a 'tr_TR' alike language request ..
	**/
	if(strchr(language, '_'))
	{
		/**
		* If a 'tr_TR' isn't found then we go to the superclass db: 'tr'.
		* Just show a message that tells this.
		**/
		g_print(_("FYI: Will also lookup the superclass message db.\n"));
		lusp=TRUE;
	}
	sprintf(file,"%s/%s.xml",MESSAGE_DB_DIR,language);
	g_print("DEBUG: using %s .. \n",file);
	/**
	* Parse the xml file.
	**/
	xmldoc=xmlParseFile(file);
	if(xmldoc==NULL)
	{
		if(lusp==FALSE)
		{
			g_error(_("Couldn't open language base file `%s.xml' in %s.\n"),language,MESSAGE_DB_DIR);
		}
		else
		{
			gchar **sarr;
			sarr=g_strsplit(language,"_",1);
			g_print(_("Subclass file `%s.xml' not found.\n"),language);
			g_print(_("Trying superclass-file `%s.xml' ...\n"),sarr[0]);
			parse_db_for_lang(sarr[0]);
			if(sarr)
			{
				g_strfreev(sarr);
			}	
		}
	}
}
