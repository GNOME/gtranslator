/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The "initial"-file 
*
* -- only source 
**/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <popt-gnome.h>

#include "msg_db.h"
#include "gui.h"
#include "session.h"
#include "prefs.h" 
#include "parse.h"
#include "parse-db.h"
#include "team-handle.h"

static const gchar *team_string = NULL;

/**
* Some static variables for the poptTable in the main routines.
**/
static gchar *gtranslator_geometry = NULL;
static gchar *query_string = NULL;
static guint challen;

/**
* The popt-options table
**/
static struct poptOption gtranslator_options[] = {
	{
		NULL, '\0', POPT_ARG_INTL_DOMAIN, PACKAGE,
	 0, NULL, NULL},
	{
		"challenge-length", 'c', POPT_ARG_INT, &challen,
	 0, N_("Sets the minimum match for query"), "LENGTH"},
	{
                "geometry", 'g', POPT_ARG_STRING, &gtranslator_geometry,
	 0, N_("Specifies the main-window geometry"), "GEOMETRY"},
	{
		"msg-db", 'm', POPT_ARG_STRING, &msg_db,
	 0, N_("The messages db to use"), "MSG_DB"},
	{
		"query", 'q', POPT_ARG_STRING, &query_string,
	 0, N_("Query the messages db & exit"), "QUERY_STRING"},
	{
	 "team", 't', POPT_ARG_STRING, &team_string,
	 0, N_("Team to append"), "TEAMNAME"},
	POPT_AUTOHELP {NULL}
};

int main(int argc, char *argv[])
{
	/**
	* The Sessionmanagement client
	**/
	GnomeClient *client;
	GnomeClientFlags flags;
	/**
	* For the arguments
	**/
	poptContext context;
	const char **args;

	#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain(PACKAGE);
	#endif
	/**
	* Init gtranslator.
	**/
	gnome_init_with_popt_table("gtranslator", VERSION, argc, argv, 
		gtranslator_options, 0, &context);
	/* It is important to say that db was not inited */
	msg_db_inited = FALSE;
	if (!challen) 
		challen = 2;
		set_challenge_length(challen);
	/**
        * If a query has been done.
        **/
	if (query_string) {
		init_msg_db();
		g_print("%s\n", get_from_msg_db(query_string));
			/**
			* Exit within the non-GUI circles.
			**/
			exit(0);
        }
	/* Initialize the regular expression cache */
	rxc = gnome_regex_cache_new_with_size(20);
	read_prefs();
	/* If we use message db */
	if (wants.use_msg_db) {
		/* If the language is set, call the specified 
		 * language.xml file. */
		if (lc)
			parse_db_for_lang(lc);
		else {
			gchar *langs;
			/* If we've got a $LANG, then call the corresponding
			 *  $LANG.xml file as the message db.
			 */
			langs = getenv("LANG");
			if (langs)
				parse_db_for_lang(langs);
			else
				/* It should not crash, so use example.xml */
				parse_db_for_lang("example");
		}
		init_msg_db();
	}
	/*
	if (team_string) { 
		team_handle_new(team_string); 
	} 
	*/
	/**
	* Get the client
	**/
	client = gnome_master_client();
	/**
	* Connect the signals for Sessionmanagement
	**/
	gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
			   GTK_SIGNAL_FUNC(gtranslator_sleep),
			   (gpointer) argv[0]);
	gtk_signal_connect(GTK_OBJECT(client), "die",
		GTK_SIGNAL_FUNC(gtranslator_dies_for_you), NULL);

	/* Create the main app-window */
	create_app1();
	restore_geometry(gtranslator_geometry);

	/**
	* If there are any files given on command line, open them
	* TODO add here loading of all files, when program becomes MDI
	**/
	file_opened = FALSE;
	args = poptGetArgs(context);
	if (args)
		parse(args[0]);
	poptFreeContext(context);
	/* Disable the buttons if no file is opened. */
	if (!file_opened)
		disable_actions_no_file();

	/* Check the session client flags, and restore state if needed */
	flags = gnome_client_get_flags(client);
	if (flags & GNOME_CLIENT_RESTORED)
		restore_session(client);
	/* Show the application window, with icon, if requested */
	#ifdef USE_WINDOW_ICON
	gnome_window_icon_set_from_file(GTK_WINDOW(app1), WINDOW_ICON);
	#endif
	gtk_widget_show_all(app1);
	/* Enter the Gtk+ main-loop */
	gtk_main();
	return 0;
}

