/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * Docbook sgml file backend -- converts the paras -> po files and makes them
 *  possible to be translated.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../../header_stuff.h"
#include "../../message.h"
#include "../../nautilus-string.h"
#include "../../parse.h"
#include "../../prefs.h"

#include <gnome-xml/tree.h>
#include <gnome-xml/parser.h>

xmlDocPtr doc;

/*
 * The prototypes:
 */
gboolean backend_open(const gchar *filename);
gboolean backend_save(void);
gboolean backend_save_as(const gchar *filename);
void backend_close(void);

gboolean backend_open(const gchar *filename)
{
	g_return_val_if_fail(filename!=NULL, FALSE);

	doc=xmlParseFile(filename);
	return TRUE;
}

gboolean backend_save(void)
{
	g_message("Write po -> docbook saving functions..");
	return TRUE;
}

gboolean backend_save_as(const gchar *filename)
{
	g_message("Save to %s", filename);
	return TRUE;
}

void backend_close(void)
{
	if(doc)
	{
		xmlFreeDoc(doc);
	}
}
