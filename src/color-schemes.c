/*
 * (C) 2001-2002 	Fatih Demir <kabalak@gtranslator.org>
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

#include <string.h>
#include <sys/param.h>

#include "color-schemes.h"
#include "defines.include"
#include "gui.h"
#include "nautilus-string.h"
#include "parse.h"
#include "preferences.h"
#include "prefs.h"
#include "stylistics.h"
#include "utils.h"

#include <gtk/gtkmenushell.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include <libgnome/gnome-i18n.h>

#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-helper.h>

/*
 * The globally used GtrColorScheme.
 */
GtrColorScheme	*theme=NULL;

/*
 * This list is now the general home of all colorscheme which we can
 *  find -- a central list should avoid too much mem-shuffling for such stuff.
 */
GList		*colorschemes=NULL;

/*
 * A lend function from history.c -- free's some information about the
 *  menu items we're inserting for the colorschemes.
 */
void free_data(GtkWidget *widget, gpointer userdata);

/*
 * Apply the given (gpointer'ized) colorscheme from the colorschemes menu.
 */
void apply_colorscheme(GtkWidget *widget, gchar *scheme_name);

/*
 * Check if the given xml file is a gtranslator color scheme file.
 */
gboolean check_if_color_scheme(xmlNodePtr *node);

/*
 * Just get the informations about the scheme.
 */ 
GtrColorSchemeInformations *get_color_scheme_infos(xmlNodePtr *node);

/*
 * Free up the structure with this function.
 */
void gtranslator_color_scheme_free_infos(GtrColorSchemeInformations **infos);

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
		GTR_FREE(string); \
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
	 * Get the absolute filename.
	 */
	if(!g_path_is_absolute(filename))
	{
		char realfilename[MAXPATHLEN + 1];
		realpath(filename, realfilename);

		scheme->info->filename=g_strdup(realfilename);
	}
	else
	{
		scheme->info->filename=g_strdup(filename);
	}
	
	/*
	 * Now extract the color informations from the xml document.
	 */
	while(node!=NULL)
	{
		GetData(fg, "fg");
		GetData(bg, "bg");
		GetData(text_bg, "text_bg");
		GetData(special_char, "special_char");
		GetData(hotkey, "hotkey");
		GetData(c_format, "c_format");
		GetData(number, "number");
		GetData(punctuation, "punctuation");
		GetData(special, "special");
		GetData(address, "address");
		GetData(keyword, "keyword");
		GetData(spell_error, "spell_error");
				
		GTR_ITER(node);
	}

	xmlFreeDoc(xmldoc);

	/*
	 * Now check if we could get any "text_bg" attribute -- if not (in
	 *  older colorschemes for example), simply use the "bg" attribute
	 *   for the "text_bg" -- this way we don't get any different text
	 *    background but don't end up with black on navy or something like
	 *     that.
	 */
	if(scheme->bg && !scheme->text_bg)
	{
		scheme->text_bg=g_strdup(scheme->bg);
	}
	
	return scheme;
}

/*
 * Apply the given color scheme as default.
 */
void gtranslator_color_scheme_apply(const gchar *filename)
{
  /* GtrColorScheme*/
        theme=g_new0(GtrColorScheme, 1);

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

	/*
	 * Also set up the scheme specific foreground and background colors.
	 */ 
	gtranslator_config_set_string("colors/fg", theme->fg);
	gtranslator_config_set_string("colors/bg", theme->bg);

	if(theme->text_bg)
	{
		gtranslator_config_set_string("colors/text_bg", theme->text_bg);
	}

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
	gtranslator_config_set_string("colors/spell_error", theme->spell_error);

	/*
	 * Set up the scheme information.
	 */
	gtranslator_config_set_string("scheme/name", theme->info->name);
	gtranslator_config_set_string("scheme/version", theme->info->version);
	gtranslator_config_set_string("scheme/author", theme->info->author);
	gtranslator_config_set_string("scheme/author_email",
		theme->info->author_email);
	gtranslator_config_set_string("scheme/filename", 
		theme->info->filename);
	
	//	gtranslator_color_scheme_free(&theme);
}

/*
 * Loads the current GtrColorScheme from the preferences.
 */
GtrColorScheme *gtranslator_color_scheme_load_from_prefs()
{
  /*GtrColorScheme */theme=g_new0(GtrColorScheme, 1);

	theme->info=g_new0(GtrColorSchemeInformations, 1);

	/*
	 * Reget the stored color scheme values from the preferences.
	 */ 
	theme->info->name=gtranslator_config_get_string("scheme/name");
	theme->info->version=gtranslator_config_get_string("scheme/version");
	theme->info->author=gtranslator_config_get_string("scheme/author");
	theme->info->author_email=gtranslator_config_get_string("scheme/author_email");
	theme->info->filename=gtranslator_config_get_string("scheme/filename");

	theme->fg=gtranslator_config_get_string("colors/fg");
	theme->bg=gtranslator_config_get_string("colors/bg");
	theme->text_bg=gtranslator_config_get_string("colors/text_bg");
	theme->special_char=gtranslator_config_get_string("colors/special_char");
	theme->hotkey=gtranslator_config_get_string("colors/hotkey");
	theme->c_format=gtranslator_config_get_string("colors/c_format");
	theme->number=gtranslator_config_get_string("colors/number");
	theme->punctuation=gtranslator_config_get_string("colors/punctuation");
	theme->special=gtranslator_config_get_string("colors/special");
	theme->address=gtranslator_config_get_string("colors/address");
	theme->keyword=gtranslator_config_get_string("colors/keyword");
	theme->spell_error=gtranslator_config_get_string("colors/spell_error");

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
	gtranslator_colors_init_default_colors();
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

	g_message("name: %s version %s", infos->name, infos->version);
	*node=(*node)->xmlChildrenNode;

	/*
	 * Iterate through the nodes till we got the author tag.
	 */ 
	while(*node!=NULL && g_strcasecmp((*node)->name, "author"))
	{
		GTR_ITER(*node);
	}

	if(!*node)
	{
		g_warning(_("Couldn't retrieve author information!"));

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
void gtranslator_color_scheme_free_infos(GtrColorSchemeInformations **infos)
{
	if(*infos)
	{
		GTR_FREE((*infos)->name);
		GTR_FREE((*infos)->version);
		GTR_FREE((*infos)->author);
		GTR_FREE((*infos)->author_email);
		GTR_FREE(*infos);
	}
}

/*
 * Free the given GtrColorScheme.
 */
void gtranslator_color_scheme_free(GtrColorScheme **scheme)
{
	if(*scheme)
	{
		if((*scheme)->info)
		{
			gtranslator_color_scheme_free_infos(&(*scheme)->info);
		}
		
		GTR_FREE((*scheme)->fg);
		GTR_FREE((*scheme)->bg);

		/*
		 * The "text_bg" attribute for the colorschemes is quite new, therefore
		 *  we don't have it per default -- check for it before free'ing it.
		 */
		if((*scheme)->text_bg)
		{
			GTR_FREE((*scheme)->text_bg);
		}
		
		GTR_FREE((*scheme)->special_char);
		GTR_FREE((*scheme)->hotkey);
		GTR_FREE((*scheme)->c_format);
		GTR_FREE((*scheme)->number);
		GTR_FREE((*scheme)->punctuation);
		GTR_FREE((*scheme)->special);
		GTR_FREE((*scheme)->address);
		GTR_FREE((*scheme)->keyword);
		GTR_FREE((*scheme)->spell_error);
		GTR_FREE(*scheme);
	}
}

/*
 * Create a list of all available color schemes in our common
 *  directories where we are searching for the colorschemes.
 */
void gtranslator_color_scheme_create_schemes_list()
{
	gchar 	*personal_schemes_directory=NULL;

	personal_schemes_directory=g_strdup_printf(
		"%s/.gtranslator/colorschemes", g_get_home_dir());

	/*
	 * First load all colorschemes from ~/.gtranslator/colorschemes.
	 */
	colorschemes=gtranslator_utils_file_names_from_directory(
		personal_schemes_directory, ".xml", TRUE, TRUE, FALSE);
	GTR_FREE(personal_schemes_directory);

	/*
	 * Now append the colorschemes from the global colorschemes reservoire.
	 */
	if(!colorschemes)
	{
		colorschemes=gtranslator_utils_file_names_from_directory(
			SCHEMESDIR, ".xml", TRUE, TRUE, FALSE);
	}
	else
	{
		GList	*global_colorschemes=NULL;

		global_colorschemes=gtranslator_utils_file_names_from_directory(
			SCHEMESDIR, ".xml", TRUE, TRUE, FALSE);

		/*
		 * Append and resort the colorschemes list (now consisting of 
		 *  global + personal colorschemes directory contents).
		 */
		colorschemes=g_list_concat(colorschemes, global_colorschemes);
		colorschemes=g_list_sort(colorschemes, 
			(GCompareFunc) nautilus_strcmp_compare_func);
	}
}

/*
 * Clean the list of the colorschemes up -- free the list and it's values.
 */
void gtranslator_color_scheme_delete_schemes_list()
{
	if(colorschemes)
	{
		gtranslator_utils_free_list(colorschemes, TRUE);
	}
}

/*
 * Creates the menu with the colorschemes as toggle items -- easier
 *  access to the favourites of kabalak :-)
 */
void gtranslator_color_scheme_show_list()
{
	GnomeUIInfo 	*menu;
	
	GList 		*onelist;

	gint		 i=0;
	
	gchar 		*menupath=_("_Settings/_Colorschemes/");

	/*
	 * Delete the old entries.
	 */
	gnome_app_remove_menu_range(GNOME_APP(gtranslator_application), 
		menupath, 0, i);

	/*
	 * Parse the list.
	 */
	for(onelist=g_list_last(colorschemes); onelist!=NULL; onelist=g_list_previous(onelist))
	{
		gchar *colorscheme_name=NULL;

		colorscheme_name=((gchar *) (onelist->data));
		menu=g_new0(GnomeUIInfo, 2);

		/*
		 * Set the label name.
		 */
		if(colorscheme_name)
		{
			menu->label=g_strdup_printf("%s", colorscheme_name);
		}
		
		/*
		 * Set the GnomeUIInfo settings and labels.
		 */
		menu->type=GNOME_APP_UI_ITEM;
		menu->hint=g_strdup_printf(_("Activate colorscheme %s"), colorscheme_name);
		menu->moreinfo=(gpointer)apply_colorscheme;
		menu->user_data=colorscheme_name;
		(menu+1)->type=GNOME_APP_UI_ENDOFINFO;

		/*
		 * Insert this item into menu
		 */
		gnome_app_insert_menus(GNOME_APP(gtranslator_application), menupath, menu);
		gnome_app_install_menu_hints(GNOME_APP(gtranslator_application), menu);

// XXX convert it
//		gtk_signal_connect(GTK_OBJECT(menu->widget), "destroy",
//			   GTK_SIGNAL_FUNC(free_data), menu->hint);

		/*
		 * Free the string and the GnomeUIInfo structure.
		 */
// XXX fix it
// 		GTR_FREE(menu->label);
		GTR_FREE(menu);
	}
}

/*
 * Helper function for free'ing action.
 */
void free_data(GtkWidget *widget, gpointer userdata)
{
	GTR_FREE(userdata);
}

/*
 * Apply the given scheme name from the colorschemes list.
 */
void apply_colorscheme(GtkWidget *widget, gchar *scheme_name)
{
	if(scheme_name)
	{
		/*
		 * First check if there's such a colorscheme in 
		 *  ~/.gtranslator/colorschemes before checking the global 
		 *   colorschemes directory.
		 */
		GtrPreferences.scheme=g_strdup_printf(
			"%s/.gtranslator/colorschemes/%s.xml", g_get_home_dir(), 
				scheme_name);

		/*
		 * If there's no such colorscheme in the 
		 *  ~/.gtranslator/colorschemes directory, try the global 
		 *   colorschemes directory.
		 */
		if(!g_file_test(GtrPreferences.scheme, G_FILE_TEST_EXISTS))
		{
			GtrPreferences.scheme=g_strdup_printf("%s/%s.xml", 
				SCHEMESDIR, scheme_name);
		}

		if(g_file_test(GtrPreferences.scheme, G_FILE_TEST_EXISTS))
		{
			/*
			 * Free the old used colorscheme.
			 */
			gtranslator_color_scheme_free(&theme);
			
			/*
			 * Read in the new colorscheme, initialize the colors.
			 */
			gtranslator_color_scheme_apply(GtrPreferences.scheme);
			theme=gtranslator_color_scheme_load_from_prefs();
			
			gtranslator_colors_initialize();
		}

		GTR_FREE(scheme_name);
	}

	gtranslator_set_style(GTK_WIDGET(text_box), 0);
        gtranslator_set_style(GTK_WIDGET(trans_box), 1);
}

/*
 * Copy the given GtrColorSchemeInformation.
 */
GtrColorSchemeInformations *gtranslator_color_scheme_infos_copy(
	GtrColorSchemeInformations *infos)
{
	GtrColorSchemeInformations *copy=g_new(GtrColorSchemeInformations, 1);

	copy->name=g_strdup(infos->name);
	copy->version=g_strdup(infos->version);
	copy->author=g_strdup(infos->author);
	copy->author_email=g_strdup(infos->author_email);
	copy->filename=g_strdup(infos->filename);

	return copy;
}

/*
 * Copy the given GtrColorScheme.
 */
GtrColorScheme *gtranslator_color_scheme_copy(GtrColorScheme *scheme)
{
	GtrColorScheme *copy=g_new(GtrColorScheme, 1);

	copy->info=gtranslator_color_scheme_infos_copy(scheme->info);

	copy->fg=g_strdup(scheme->fg);
	copy->bg=g_strdup(scheme->bg);

	if(scheme->text_bg)
	{
		copy->text_bg=g_strdup(scheme->text_bg);
	}
	
	copy->special_char=g_strdup(scheme->special_char);
	copy->hotkey=g_strdup(scheme->hotkey);
	copy->c_format=g_strdup(scheme->c_format);
	copy->number=g_strdup(scheme->number);
	copy->punctuation=g_strdup(scheme->punctuation);
	copy->special=g_strdup(scheme->special);
	copy->address=g_strdup(scheme->address);
	copy->keyword=g_strdup(scheme->keyword);
	copy->spell_error=g_strdup(scheme->spell_error);

	return copy;
}
