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

/**
* This is probably the main file for gtranslatord.
**/

/**
* The local includes.
**/
#include <gtranslatord.h>
#include <parse-db.h>
#include <team-handle.h>

/**
* The OAF includes.
**/
#include <liboaf/liboaf.h>

/**
* The mainloop.
**/
int main(int argc,char *argv[])
{
	CORBA_ORB orb;
	CORBA_Environment env;
	/**
	* Init the environment.
	**/
	CORBA_exception_init(&env);
	/**
	* Init the NLS if it's necessary.
	**/
	#ifdef ENABLE_NLS
	bindtextdomain("gtranslator", PACKAGE_LOCALE_DIR);
	textdomain("gtranslator");
	#endif
	/**
	* Init OAF.
	**/
	orb=oaf_init(argc,argv);
	/**
	* Print a simply statement till we can do more.
	**/
	if(orb)
	{
		g_print(_("gtranslatord has started successfully and will finish now...\n"));
	}
	/**
	* TODO: first the libgtranslator has to be extended.
	**/
	exit(0);
}	
