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

void about_box()
{
	const gchar *authors[] = {
		"Fatih Demir",
		NULL
		};

	about = gnome_about_new ("gtranslator", VERSION,
		_("(C) 1999-2000 Fatih Demir ( kabalak@gmx.net )"),
		authors,
		_("gtranslator is a Gnome Application for translating .po-files more comfortable than in (X)Emacs .\nTill now you can't see much , but that will change !"),
		NULL);
	gtk_window_set_modal (GTK_WINDOW (about), TRUE);
	gtk_window_set_wmclass (GTK_WINDOW (about), "gtranslator", "gtranslator");
	gtk_widget_show(about);
}
