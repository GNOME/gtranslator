/*
 * (C) 2001 	Fatih Demir <kabalak@gmx.net>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "color-schemes.h"
#include "parse.h"
#include "prefs.h"
#include "preferences.h"

#include <gnome-xml/tree.h>
#include <gnome-xml/parser.h>

#ifdef ENABLE_NLS
#include <libgnome/gnome-i18n.h>
#endif

GtrColorScheme *gtranslator_color_scheme_open(const gchar *filename)
{
	xmlDocPtr 	xmldoc;
	xmlNodePtr 	node;
	GtrColorScheme *scheme;

	#define GetData(x, y); \
	if(node && !strcmp(xmlGetProp(node, "target"), y)) \
	{ \
		scheme->x=xmlNodeGetContent(node); \
	}

	g_return_val_if_fail(filename!=NULL, NULL);

	xmldoc=xmlParseFile(filename);

	g_assert(xmldoc);

	scheme = g_new0(GtrColorScheme, 1);
	
	scheme->name=xmlGetProp(xmldoc->xmlRootNode, "name");
	scheme->version=xmlGetProp(xmldoc->xmlRootNode, "version");
	
	node=xmldoc->xmlRootNode->xmlChildrenNode;

	scheme->author=xmlGetProp(node, "name");
	scheme->author_email=xmlGetProp(node, "email");
	
	while(node!=NULL)
	{
		GetData(fg, "fg");
		GetData(bg, "bg");
		GetData(special_char, "special_char");
		GetData(c_format, "c_format");
		GetData(number, "number");
		GetData(punctuation, "punctuation");
		GetData(special, "special");
		GetData(address, "address");
		GetData(keyword, "keyword");
				
		node=node->next;
	}
	
	return scheme;
}

/*
 * Apply the given color scheme as default.
 */
void gtranslator_color_scheme_apply(const gchar *filename)
{
	GtrColorScheme *theme;

	theme=gtranslator_color_scheme_open(filename);

	if(theme==NULL)
	{
		g_warning(_("Can't apply color scheme file `%s'!"),
			filename);

		return;
	}

	/*
	 * Save the color valuesinto the preferences.
	 */
	gtranslator_config_init();

	/*
	 * Also set up the scheme specific foreground and background colors.
	 */ 
	gtranslator_config_set_string("colors/fg", theme->fg);
	gtranslator_config_set_string("colors/bg", theme->bg);

	/*
	 * And all the syntax colors.
	 */
	gtranslator_config_set_string("colors/special_char",
		theme->special_char);
	gtranslator_config_set_string("colors/hotkey", theme->hotkey);
	gtranslator_config_set_string("colors/c_format", theme->c_format);
	gtranslator_config_set_string("colors/number", theme->number);
	gtranslator_config_set_string("colors/punctuation", theme->punctuation);
	gtranslator_config_set_string("colors/special", theme->special);
	gtranslator_config_set_string("colors/address", theme->address);
	gtranslator_config_set_string("colors/keyword", theme->keyword);

	/*
	 * Set up the scheme information.
	 */
	gtranslator_config_set_string("scheme/name", theme->name);
	gtranslator_config_set_string("scheme/version", theme->version);
	gtranslator_config_set_string("scheme/author", theme->author);
	gtranslator_config_set_string("scheme/author_email",
		theme->author_email);
	
	gtranslator_config_close();
}
