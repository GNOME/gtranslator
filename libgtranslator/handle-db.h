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

#ifndef HANDLE_DB_H
#define HANDLE_DB_H 1

#include <libgtranslator/parse-db.h>

/**
* Addes a node to the given doc.
**/
gint gtranslator_add_node_to_doc(gchar *filename, xmlDocPtr doc,gchar *nodename,gchar *nodecontent);

#endif
