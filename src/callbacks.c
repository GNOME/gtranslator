/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * It's in a complete rewrite so wait some time ...
 **/

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <gnome.h>

#include "callbacks.h"
#include "about.h"
#include "gtr_dialogs.h"
#include "parse.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>

/**
 * The variables for all that ....
 **/
gchar cmd[256];
gchar fname[768];
static GnomeHelpMenuEntry help_me = { "gtranslator", "index.html" };
int warn_me,show_me_errors,startup_at_last;
