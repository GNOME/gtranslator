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

	about=gnome_about_new("gtranslator", VERSION,
		_("(C) 1999-2000 Fatih Demir ( kabalak@gmx.net )"),
		authors,
		_("gtranslator will be a Gnome framework for translating these emminent important po-files.\nIt will be based upon a gtranslatord and a gtranslator client. Wish the best for us translators and for me!"),
		NULL);
	gtk_window_set_modal(GTK_WINDOW (about), TRUE);
	gtk_window_set_wmclass(GTK_WINDOW (about), "gtranslator", "gtranslator");
}

void about_box_show(GtkWidget *widget,gpointer useless)
{
	about_box();
	#ifdef USE_WINDOW_ICON
	gnome_window_icon_set_from_file(GTK_WINDOW(about),WINDOW_ICON);
	#endif
	gtk_widget_show(about);
}
