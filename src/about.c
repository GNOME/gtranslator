/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* the about-box is now also external-placed
*  from the interface.c file ...
*
* -- the source
**/

#include "about.h"

void about_box(GtkWidget *widget,gpointer useless)
{
	static GtkWidget *about=NULL;
	const gchar *authors[] = {
		"Fatih Demir <kabalak@gmx.net>",
		"Gediminas Paulauskas <menesis@delfi.lt>",
		NULL
		};
	raise_and_return_if_exists(about);

	about=gnome_about_new("gtranslator", VERSION,
		"(C) 1999-2000",
		authors,
		_("gtranslator will be a GNOME framework for translating and managing the translation of the important po-files and othe i18/19/20..n issues."),
		NULL);
	show_nice_dialog(&about,"gtranslator -- about");
}

