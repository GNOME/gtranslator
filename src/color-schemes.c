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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "color-schemes.h"
#include "parse.h"
#include "prefs.h"
#include "preferences.h"

#include <sys/types.h>
#include <dirent.h>

#include <gnome-xml/tree.h>
#include <gnome-xml/parser.h>

#ifdef ENABLE_NLS
#include <libgnome/gnome-i18n.h>
#endif

#include <libgnome/gnome-util.h>

/*
 * Check if the given xml file is a gtranslator color scheme file.
 */
gboolean check_if_color_scheme(xmlNodePtr *node);

/*
 * Just get the informations about the scheme.
 */ 
GtrColorSchemeInformations *get_color_scheme_infos(xmlNodePtr *node);

/*
 * Free up the structures with these functions.
 */
void free_color_scheme_infos(GtrColorSchemeInformations **infos);
void free_color_scheme(GtrColorScheme **scheme);

/*
 * Open the xml file and return the GtrColorScheme equivalent.
 */ 
GtrColorScheme *gtranslator_color_scheme_open(const gchar *filename)
{
	xmlDocPtr 	xmldoc;
	xmlNodePtr 	node;
	GtrColorScheme *scheme;

	#define GetData(x, y); \
	if(!g_strcasecmp(node->name, "color")) \
	{ \
		if(!g_strcasecmp(xmlGetProp(node, "target"), y)) \
		{ \
		gchar *string=xmlNodeListGetString(xmldoc, \
			node->xmlChildrenNode, 1); \
		g_strstrip(string); \
		scheme->x=g_strdup(string); \
		g_free(string); \
		} \
	}

	g_return_val_if_fail(filename!=NULL, NULL);

	xmldoc=xmlParseFile(filename);

	g_return_val_if_fail(xmldoc!=NULL, NULL);

	scheme=g_new0(GtrColorScheme, 1);
	scheme->info=g_new0(GtrColorSchemeInformations, 1);

	node=xmlDocGetRootElement(xmldoc);

	/*
	 * Check if it's a gtranslator color scheme and return with
	 *  NULL if the check fails.
	 */  
	if(check_if_color_scheme(&node)==FALSE)
	{
		return NULL;
	}

	/*
	 * Get the informations about the color scheme.
	 */ 
	scheme->info=get_color_scheme_infos(&node);

	/*
	 * Now extract the color informations from the xml document.
	 */
	while(node!=NULL)
	{
		GetData(fg, "fg");
		GetData(bg, "bg");
		GetData(special_char, "special_char");
		GetData(hotkey, "hotkey");
		GetData(c_format, "c_format");
		GetData(number, "number");
		GetData(punctuation, "punctuation");
		GetData(special, "special");
		GetData(address, "address");
		GetData(keyword, "keyword");
				
		node=node->next;
	}

	xmlFreeDoc(xmldoc);
	
	return scheme;
}

/*
 * Apply the given color scheme as default.
 */
void gtranslator_color_scheme_apply(const gchar *filename)
{
	GtrColorScheme *theme=g_new0(GtrColorScheme, 1);

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
	gtranslator_config_set_string("scheme/name", theme->info->name);
	gtranslator_config_set_string("scheme/version", theme->info->version);
	gtranslator_config_set_string("scheme/author", theme->info->author);
	gtranslator_config_set_string("scheme/author_email",
		theme->info->author_email);
	
	gtranslator_config_close();

	free_color_scheme(&theme);
}

/*
 * Loads the current GtrColorScheme from the preferences.
 */
GtrColorScheme *gtranslator_color_scheme_load_from_prefs()
{
	GtrColorScheme *theme=g_new0(GtrColorScheme, 1);

	theme->info=g_new0(GtrColorSchemeInformations, 1);

	gtranslator_config_init();

	/*
	 * Reget the stored color scheme values from the preferences.
	 */ 
	theme->info->name=gtranslator_config_get_string("scheme/name");
	theme->info->version=gtranslator_config_get_string("scheme/version");
	theme->info->author=gtranslator_config_get_string("scheme/author");
	theme->info->author_email=gtranslator_config_get_string("scheme/author_email");

	theme->fg=gtranslator_config_get_string("colors/fg");
	theme->bg=gtranslator_config_get_string("colors/bg");
	theme->special_char=gtranslator_config_get_string("colors/special_char");
	theme->hotkey=gtranslator_config_get_string("colors/hotkey");
	theme->c_format=gtranslator_config_get_string("colors/c_format");
	theme->number=gtranslator_config_get_string("colors/number");
	theme->punctuation=gtranslator_config_get_string("colors/punctuation");
	theme->special=gtranslator_config_get_string("colors/special");
	theme->address=gtranslator_config_get_string("colors/address");
	theme->keyword=gtranslator_config_get_string("colors/keyword");

	gtranslator_config_close();

	return theme;
}

/*
 * Restore the default syntax highlighting colors.
 */ 
void gtranslator_color_scheme_restore_default()
{
	/*
	 * Call the function which is also called during the first
	 *  initialization of gtranslator to get the default colors.
	 */  
	prefs_init_syntax_colors();
}

/*
 * Sniff out the GtrColorSchemeInformations from the xml document.
 */ 
GtrColorSchemeInformations *get_color_scheme_infos(xmlNodePtr *node)
{
	GtrColorSchemeInformations *infos=g_new0(GtrColorSchemeInformations, 1);
	
	g_return_val_if_fail(*node!=NULL, NULL);

	/*
	 * Get the scheme name & version.
	 */ 
	infos->name=xmlGetProp(*node, "name");
	infos->version=xmlGetProp(*node, "version");

	*node=(*node)->xmlChildrenNode;

	/*
	 * Iterate through the nodes till we got the author tag.
	 */ 
	while(*node!=NULL && g_strcasecmp((*node)->name, "author"))
	{
		*node=(*node)->next;
	}

	if(!*node)
	{
		g_warning(_("Couldn't retrieve author informations!"));

		/*
		 * Translators: These strings are used as author informations if
		 *  no author informations could be retrieved:
		 */  
		infos->author=_("Unknown");
		infos->author_email=_("<Not Given>");
	}
	else
	{
		infos->author=xmlGetProp(*node, "name");
		infos->author_email=xmlGetProp(*node, "email");
	}

	return infos;
}

/*
 * Check the given xml document for being a gtranslator color scheme.
 */
gboolean check_if_color_scheme(xmlNodePtr *node)
{
	g_return_val_if_fail(*node!=NULL, FALSE);
	
	/*
	 * Return FALSE whenever a malus is detected .-)
	 */ 
	#define Malif(x) if(x) { return FALSE; }

	Malif(g_strcasecmp((*node)->name, "colorscheme"));
	Malif(!(*node)->xmlChildrenNode);
	
	return TRUE;
}

/*
 * Free the given GtrColorSchemeInformations.
 */
void free_color_scheme_infos(GtrColorSchemeInformations **infos)
{
	if(*infos)
	{
		g_free((*infos)->name);
		g_free((*infos)->version);
		g_free((*infos)->author);
		g_free((*infos)->author_email);
		g_free(*infos);
	}
}

/*
 * Free the given GtrColorScheme.
 */
void free_color_scheme(GtrColorScheme **scheme)
{
	if(*scheme)
	{
		if((*scheme)->info)
		{
			free_color_scheme_infos(&(*scheme)->info);
		}
		
		g_free((*scheme)->fg);
		g_free((*scheme)->bg);
		g_free((*scheme)->special_char);
		g_free((*scheme)->hotkey);
		g_free((*scheme)->c_format);
		g_free((*scheme)->number);
		g_free((*scheme)->punctuation);
		g_free((*scheme)->special);
		g_free((*scheme)->address);
		g_free((*scheme)->keyword);
		g_free(*scheme);
	}
}

/*
 * Return the list of the schemes found in SCHEMESDIR -- and in dir if given.
 */ 
GList *gtranslator_color_scheme_list(const gchar *directory)
{
	GList *schemesinfos=NULL;
	struct dirent *entry;
	DIR *dir;

	dir=opendir(SCHEMESDIR);

	/*
	 * Print out a small warning if there's no directory available.
	 */ 
	if(!dir && !directory)
	{
		g_warning(_("Couldn't open color schemes directory `%s'!"),
			SCHEMESDIR);

		return NULL;
	}

	while((entry=readdir(dir))!=NULL)
	{
		/*
		 * Do not count the "." and ".." directory entries.
		 */
		if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
		{
			GtrColorScheme *theme=g_new0(GtrColorScheme, 1);
			
			GtrColorSchemeInformations *infos=g_new0(
				GtrColorSchemeInformations, 1);

			theme=gtranslator_color_scheme_open(g_strdup_printf("%s/%s",
				SCHEMESDIR, entry->d_name));

			infos=theme->info;

			schemesinfos=g_list_append(schemesinfos, infos);

			free_color_scheme(&theme);
		}
	}
	
	/*
	 * Test if an argument has been given to add additional color schemes from
	 *  (possibly good for personal user-specific color schemes).
	 */  
	if(directory && g_file_test(directory, G_FILE_TEST_ISDIR))
	{
		dir=opendir(directory);

		if(!dir)
		{
			g_warning(_("Couldn't open specific color schemes directory `%s'!"),
				directory);
		}
		else
		{
			while((entry=readdir(dir))!=NULL)
			{
				if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
				{
					GtrColorScheme *theme=g_new0(GtrColorScheme, 1);

					GtrColorSchemeInformations *infos=g_new0(
						GtrColorSchemeInformations, 1);

					theme=gtranslator_color_scheme_open(g_strdup_printf(
						"%s/%s", directory, entry->d_name));

					infos=theme->info;

					schemesinfos=g_list_append(schemesinfos, infos);

					free_color_scheme(&theme);
				}
			}
		}
	}

	closedir(dir);
	
	return schemesinfos;
}
