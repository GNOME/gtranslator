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
* This happens to be a test-programm for the libgtranslator routines.
**/

#include <handle-db.h>
#include <preferences.h>

int main(int argc,char *argv[])
{
	gchar *ert = g_new0(gchar,1);
	#ifdef GCONF_IS_PRESENT
	GError	*error = NULL;
	gconf_init(argc,argv, &error);
	#endif
	gtranslator_config_init();
	g_print("Testing the string methods ...");
	gtranslator_config_set_string("Merhaba", "Hello!");
	g_print("Getting the test string .......");
	g_print("\n%s\n",gtranslator_config_get_string("Merhaba"));
	g_print("\nGetting string ... \n");
	gtranslator_config_set_string("ert/er","ERT");
	ert=gtranslator_config_get_string("ert/er");
	g_print("\nLet's look @ %s\n\n",ert);
	gtranslator_config_set_bool("Gidelim", TRUE);
	g_print("Getting the stuff for boolean values ...");
	if(gtranslator_config_get_bool("Gidelim")==TRUE)
	{
		g_print("\nThis works also ..\n");
	}
	g_print("Testing the int methods ...\n");
	gtranslator_config_set_int("Year",2000);
	g_print("This should be true : 2000 == %i\n",gtranslator_config_get_int("Year"));
	gtranslator_config_close();
	parse_db_for_lang("tr");
	if(ert)
	{
		g_free(ert);
	}
	return 0;
}	
