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

/**
* This is probably the main file for gtranslatord.
**/

/**
* The local includes.
**/

#include <libgtranslator/gtranslatord.h>
#include <libgtranslator/libgtranslator.h>
#include <popt-gnome.h>

/**
* The OAF includes.
**/
#include <liboaf/liboaf.h>

/**
* Simply undefine and redefine the G_LOG_DOMAIN
*  value just for more logicness as gtranslatord
*   has/will have got also own warning/error
*    messages which do appear with the G_LOG_DOMAIN
*     information.
**/
#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "gtranslatord"
#endif

/**
* The static gchar for the language to parse.
**/
static gchar	*lang=NULL;

/**
* The poptTable for gtranslatord.
**/
static struct poptOption gtranslatord_options[] = {
	{
		"parse-db", 'p', POPT_ARG_STRING, &lang,
		0, N_("The language to parse the db for"), N_("language_code")
	},
	POPT_AUTOHELP{NULL}
};

/**
* The mainloop.
**/
int main(int argc,char *argv[])
{
	CORBA_ORB		orb;
	CORBA_Environment	env;
	poptContext		context;
	#ifdef GCONF_IS_PRESENT
	GError			*error=NULL;
	#endif
	/**
	* Init the environment.
	**/
	CORBA_exception_init(&env);
	/**
	* Hm, bind to gettext...
	**/
	(void) bindtextdomain("gtranslator", GNOMELOCALEDIR);
	(void) textdomain("gtranslator");
	/**
	* Get the arguments and the context.
	**/
	context=poptGetContext("gtranslatord", argc, argv,
		gtranslatord_options, 0);
	while(poptGetNextOpt(context)>=0)
	{
	}
	/**
	* Free the context.
	**/
	poptFreeContext(context);
	/**
	* Init OAF.
	**/
	orb=oaf_init(argc,argv);
	/**
	* Print a simply statement till we can do more.
	**/
	if(orb)
	{
		g_print(_("gtranslatord has started successfully and will do some operations now ...\n"));
		if(lang)
		{
			/**
			* Parse the requested language database.
			**/
			GtranslatorDatabase *db;
			db=parse_db_for_lang(lang);
		}
		/**
		* Again this preliminary GConf stuff.
		**/
		#ifdef GCONF_IS_PRESENT
		if(!(gconf_init(argc,argv, &error)))
		{
			/**
			* Print some more exact informations on the GConf init-error.
			**/
			g_warning(_("GConf initialization error: `%s'"), error->message);
			/**
			* Free the GError.
			**/
			g_clear_error(&error);
		}
		#endif
	}
	exit(0);
}
