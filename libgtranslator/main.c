/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
 *
 * gtranslatord is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * gtranslatord is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * gtranslatord's main source file.
 */

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <libgtranslator/gtranslatord.h>
#include <libgtranslator/libgtranslator.h>
#include <popt-gnome.h>
#include <liboaf/liboaf.h>

/*
 * The static variable used for the popt argument.
 */
static gchar	*lang=NULL;

/*
 * gtranslatord's options as a poptTable.
 */
static struct poptOption gtranslatord_options[] = {
	{
		"parse-db", 'p', POPT_ARG_STRING, &lang,
		0, N_("The language to parse the db for"), N_("language_code")
	},
	POPT_AUTOHELP{NULL}
};

/*
 * gtranslatord's main loop.
 */
int main(int argc,char *argv[])
{
	CORBA_ORB		orb;
	CORBA_Environment	env;
	poptContext		context;
	#ifdef GCONF_IS_PRESENT
	GError			*error=NULL;
	#endif
	
	CORBA_exception_init(&env);
	/*
	 * Hm, bind to gettext...
	 */
	bindtextdomain("gtranslator", GNOMELOCALEDIR);
	textdomain("gtranslator");
	
	/*
	 * Parse the arguments.
	 */
	context=poptGetContext("gtranslatord", argc, argv,
		gtranslatord_options, 0);
	while(poptGetNextOpt(context)>=0)
	{
	}
	
	/*
	 * Free the used poptContext.
	 */
	poptFreeContext(context);
	
	/*
	 * Initialize OAF and do some small pieces of work till we
	 *  can do much more within gtranslatord.
	 */
	orb=oaf_init(argc,argv);
	if(orb)
	{
		g_print(_("gtranslatord has started successfully and will do some operations now ...\n"));
		
		if(lang)
		{
			/*
			 * Now parse the requested language's database via the old
			 *  parse_db interface.
			 */
			GtranslatorDatabase *db;
			db=parse_db_for_lang(lang);
		}
		
		/*
		 * Initialize GConf or print out the occured error.
		 */ 
		#ifdef GCONF_IS_PRESENT
		if(!(gconf_init(argc,argv, &error)))
		{
			g_warning(_("GConf initialization error:\n`%s'"),
				error->message);
			g_clear_error(&error);
		}
		#endif
	}
	
	exit(0);
}
