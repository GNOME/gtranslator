/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The session-functions of gtranslator
*
* -- header
**/

#ifndef GTR_SESSION_H
#define GTR_SESSION_H 1

#include "interface.h"
#include "parse.h"

/**
* The die-function of gtranslator
**/
gint gtranslator_dies_for_you(GnomeClient *client,gpointer data);

/**
* The SaveYourself-function of gtranslator
**/
gint gtranslator_sleep(GnomeClient *client, gint phase, GnomeSaveStyle s_style,
	gint shutdown, GnomeInteractStyle i_style,gint fast,gpointer data);

#endif // GTR_SESSION_H
