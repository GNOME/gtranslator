/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *		Gediminas Paulauskas <menesis@gtranslator.org>
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

#include "dialogs.h"
#include "gui.h"
#include "languages.h"
#include "nautilus-string.h"
#include "prefs.h"
#include "utils.h"

#include <dirent.h>

#include <libgnome/gnome-url.h>
#include <libgnome/gnome-i18n.h>
#include <libgnomeui/libgnomeui.h>

/*
 * The used ref' count for the language lists.
 */
static gint list_ref=0; 

/*
 * Strip the filename to get a "raw" enough filename.
 */ 
gchar *gtranslator_utils_get_raw_file_name(gchar *filename)
{
	GString *o=g_string_new("");
	gint count=0;

	filename=g_basename(filename);

	/*
	 * Strip all extensions after the _first_ point.
	 */ 
	while(filename[count]!='.')
	{
		o=g_string_append_c(o, filename[count]);

		count++;
	}

	if(o->len > 0)
	{
		return o->str;
	}
	else
	{
		return NULL;
	}

	g_string_free(o, FALSE);
}

/*
 * Execute the whole stuff "function" for all list entries.
 */
void convert_all_messages(GList *list, GFreeFunc function)
{
	g_return_if_fail(list!=NULL);
	
	while(list!=NULL)
	{
		GtrMsg *message=GTR_MSG(list->data);
		
		function(&message);
		
		list=list->next;
	}
}

/*
 * Subsequently filter out all extension containing filename from the directory.
 * 
 * Should be useful for many cases.
 */
GList *gtranslator_utils_file_names_from_directory(const gchar *directory,
	const gchar *extension, gboolean sort, gboolean strip_extension)
{
	GList		*files=NULL;
	DIR 		*dir;
	struct dirent 	*entry;

	/*
	 * Filter out all bad cases ,-)
	 */
	g_return_val_if_fail(directory!=NULL, NULL);
	g_return_val_if_fail(extension!=NULL, NULL);
	g_return_val_if_fail(strlen(directory) > strlen(extension), NULL);

	/*
	 * Operate on/in the given directory and search for out pattern.
	 */
	dir=opendir(directory);
	g_return_val_if_fail(dir!=NULL, NULL);

	while((entry=readdir(dir)) != NULL)
	{
		if(entry->d_name &&
			strcmp(entry->d_name, ".") && 
			strcmp(entry->d_name, "..") &&
			nautilus_istr_has_suffix(entry->d_name, extension))
		{
			if(strip_extension)
			{
				files=g_list_append(files, 
					gtranslator_utils_get_raw_file_name(
						entry->d_name));
			}
			else
			{
				files=g_list_append(files, 
					g_strdup(entry->d_name));
			}
		}
	}

	/*
	 * Test the files list.
	 */
	g_return_val_if_fail(files!=NULL, NULL);

	/*
	 * If the according argument is given, then sort the filenames list.
	 */
	if(sort)
	{
		files=g_list_append(files, (GFunc) g_strcasecmp);
	}

	closedir(dir);

	return files;
}

/*
 * Check for matching of an entry of the list entry and the string -- returns
 *  '-1' on non-matching, else the position in the list.
 */
gint gtranslator_utils_stringlist_strcasecmp(GList *list, const gchar *string)
{
	gint pos=0;
	
	g_return_val_if_fail(list!=NULL, -1);
	g_return_val_if_fail(string!=NULL, -1);

	/*
	 * The list should only consist out of gchar's..
	 */
	g_return_val_if_fail(sizeof(gchar *) == sizeof(list->data), -1);

	while(list)
	{
		if(!g_strcasecmp(list->data, string))
		{
			return pos;
		}

		list=list->next;
		pos++;

		if(!list)
		{
			return -1;
		}
	}

	return -1;
}

/*
 * Get the right localename for the language.
 */
gchar *gtranslator_utils_get_locale_name(void)
{
	if((!file_opened) || !(po->header->language))
	{
		return NULL;
	}
	else
	{
		gint c;

		for(c=0; languages[c].name!=NULL; c++)
		{
			/*
			 * Check for the language in the header -- may it be in 
			 *  English or in the current locale..
			 */
			if(!strcmp(languages[c].name, po->header->language) ||
				!strcmp(_(languages[c].name), po->header->language))
			{
				return languages[c].lcode;
			}
		}

		return NULL;
	}
}

/*
 * Get the right charset/encoding for the language.
 */
gchar *gtranslator_utils_get_locale_charset(void)
{
	if((!file_opened) || !(po->header->language))
	{
		return NULL;
	}
	else
	{
		gint c;

		for(c=0; languages[c].name!=NULL; c++)
		{
			if(!strcmp(languages[c].name, po->header->language) ||
				!strcmp(_(languages[c].name), po->header->language))
			{
				return languages[c].enc;
			}
		}

		return NULL;
	}
}

/*
 * Shows the gtranslator homepage on the web.
 */
void gtranslator_utils_show_home_page(GtkWidget *widget, gpointer useless)
{
	gnome_url_show("http://www.gtranslator.org");
}

/*
 * Go through the characters and search for free spaces
 * and replace them with '·''s.
 */
void gtranslator_utils_invert_dot(gchar *str)
{
	guint i;
	g_return_if_fail(str != NULL);

	for(i=0; str[i] != '\0'; i++) {
		if(str[i]==' ') {
			/*
			 * The "·" is the "middle dot" (00B7), it is
			 * used by gtranslator as special char to make
			 * blanks visible.
			 * If your language uses that char for another
			 * purpose or if you use another enocoding
			 * than latin1, feel free to change it to
			 * whatever you think will be better 
			 */
			str[i]=(_("·"))[0];
		} else if(str[i]==(_("·"))[0]) {
			str[i]=' ';
		}
	}
}

/*
 * Save the current application main window's geometry.
 */
void gtranslator_utils_save_geometry(void)
{
	if (wants.save_geometry == TRUE) {
		gchar *gstr;
		gint x, y, w, h;
		gstr = gnome_geometry_string(gtranslator_application->window);
		gnome_parse_geometry(gstr, &x, &y, &w, &h);
		g_free(gstr);
		gtranslator_config_init();
		gtranslator_config_set_int("geometry/x", x);
		gtranslator_config_set_int("geometry/y", y);
		gtranslator_config_set_int("geometry/width", w);
		gtranslator_config_set_int("geometry/height", h);
		gtranslator_config_close();
		
	}
}

/*
 * Restore the geometry.
 */
void gtranslator_utils_restore_geometry(gchar  * gstr)
{
	gint x, y, width, height;
	/*
	 * Set the main window's geometry from the preferences.
	 */
	if (gstr == NULL) {
		if (wants.save_geometry == TRUE) {
			gtranslator_config_init();
			x = gtranslator_config_get_int("geometry/x");
			y = gtranslator_config_get_int("geometry/y");
			width = gtranslator_config_get_int("geometry/width");
			height = gtranslator_config_get_int("geometry/height");
			gtranslator_config_close();
		}
		else return;
	}
	/*
	 * If a geometry definition had been defined try to parse it.
	 */
	else {
		if (!gnome_parse_geometry(gstr, &x, &y, &width, &height)) {
			g_warning(
			    _("The geometry string \"%s\" couldn't be parsed!"),
			    gstr);
			return;
		}
	}
	if (x != -1)
		gtk_widget_set_uposition(gtranslator_application, x, y);
	if ((width > 0) && (height > 0))
		gtk_window_set_default_size(GTK_WINDOW(gtranslator_application), width, height);
}

GtkWidget *gtranslator_utils_attach_combo_with_label(GtkWidget  * table, gint row,
				   const char *label_text,
				   GList  * list, const char *value,
				   GtkSignalFunc callback,
				   gpointer user_data)
{
	GtkWidget *label;
	GtkWidget *combo;
	label = gtk_label_new(label_text);
	combo = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(combo), list);
	if (value)
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(combo)->entry), value);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 2, row, row + 1);
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(combo)->entry), "changed",
			   GTK_SIGNAL_FUNC(callback), user_data);
	return combo;
}

GtkWidget *gtranslator_utils_attach_toggle_with_label(GtkWidget  * table, gint row,
				    const char *label_text,
				    gboolean value,
				    GtkSignalFunc callback)
{
	GtkWidget *toggle;
	toggle = gtk_check_button_new_with_label(label_text);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), value);
	gtk_table_attach_defaults(GTK_TABLE(table), toggle, 0, 1, row, row + 1);
	gtk_signal_connect(GTK_OBJECT(toggle), "toggled",
			   GTK_SIGNAL_FUNC(callback), NULL);
	return toggle;
}

GtkWidget *gtranslator_utils_attach_entry_with_label(GtkWidget  * table, gint row,
				   const char *label_text,
				   const char *value,
				   GtkSignalFunc callback)
{
	GtkWidget *label;
	GtkWidget *entry;
	label = gtk_label_new(label_text);
	entry = gtk_entry_new();
	if (value)
		gtk_entry_set_text(GTK_ENTRY(entry), value);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), entry, 1, 2, row, row + 1);
	gtk_signal_connect(GTK_OBJECT(entry), "changed",
			   GTK_SIGNAL_FUNC(callback), NULL);
	return entry;
}

GtkWidget *gtranslator_utils_attach_text_with_label(GtkWidget  * table, gint row,
				  const char *label_text,
				  const char *value,
				  GtkSignalFunc callback)
{
	GtkWidget *label;
	GtkWidget *widget;
	GtkWidget *scroll;
	label = gtk_label_new(label_text);
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	widget = gtk_text_new(NULL, NULL);
	gtk_text_set_editable(GTK_TEXT(widget), TRUE);
	if (value)
		gtk_text_insert(GTK_TEXT(widget), NULL, NULL, NULL, value, -1);
	gtk_container_add(GTK_CONTAINER(scroll), widget);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), scroll, 1, 2, row, row + 1);
	gtk_signal_connect(GTK_OBJECT(widget), "changed",
			   GTK_SIGNAL_FUNC(callback), NULL);
	return widget;
}

GtkWidget *gtranslator_utils_append_page_to_preferences_dialog(GtkWidget  * probox, gint rows, gint cols,
			     const char *label_text)
{
	GtkWidget *label;
	GtkWidget *page;
	label = gtk_label_new(label_text);
	page = gtk_table_new(rows, cols, FALSE);
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(probox), page, label);
	return page;
}

/*
 * Set up the lists to use within the combo boxes.
 */
void gtranslator_utils_language_lists_create(void)
{
	gint c = 0;
	list_ref++;
	/*
	 * Create only if it's the first call.
	 */
	if (list_ref > 1) 
		return;
	languages_list = encodings_list = lcodes_list = group_emails_list = 
		bits_list = NULL;
	while (languages[c].name != NULL) {
		languages_list =
		    g_list_prepend(languages_list,
				   (gpointer) _(languages[c].name));
		lcodes_list =
		    g_list_prepend(lcodes_list,
				   (gpointer) languages[c].lcode);
		if (g_list_find_custom
		    (encodings_list, (gpointer) languages[c].enc,
		     (GCompareFunc) strcmp) == NULL)
			encodings_list =
			    g_list_prepend(encodings_list,
					   (gpointer) languages[c].enc);
		if (g_list_find_custom
		    (group_emails_list, (gpointer) languages[c].group,
		     (GCompareFunc) strcmp) == NULL)
			group_emails_list =
			    g_list_prepend(group_emails_list,
					   (gpointer) languages[c].group);
		if (g_list_find_custom
		    (bits_list, (gpointer) languages[c].bits,
		     (GCompareFunc) strcmp) == NULL)
			bits_list =
			    g_list_prepend(bits_list,
					   (gpointer) languages[c].bits);
		c++;
	}
	/*
	 * Arrange the resulting lists.
	 */
	languages_list = g_list_sort(languages_list, (GCompareFunc) strcoll);
	lcodes_list = g_list_reverse(lcodes_list);
	group_emails_list =
	    g_list_sort(group_emails_list, (GCompareFunc) strcmp);
	encodings_list =
	    g_list_sort(encodings_list, (GCompareFunc) strcmp);
	bits_list = g_list_sort(bits_list, (GCompareFunc) strcmp);
}

/*
 * Frees the language list.
 */
gboolean gtranslator_utils_language_lists_free(GtkWidget  * widget, gpointer useless)
{
	list_ref--;
	/*
	 * If something needs them, leave.
	 */
	if (list_ref != 0) return FALSE;
#define free_a_list(list) g_list_free(list); list=NULL;
	free_a_list(languages_list);
	free_a_list(lcodes_list);
	free_a_list(group_emails_list);
	free_a_list(encodings_list);
	free_a_list(bits_list);
#undef free_a_list
	return FALSE;
}

/*
 * Try to convert the < 0.37 color preferences to their new location.
 */
void gtranslator_utils_old_colors_to_new_location()
{
	gchar *value;
	gboolean converted=FALSE;
	
	gtranslator_config_init();

	value=gtranslator_config_get_string("colors/fg");

	if(value && value[0]=='#')
	{
		/* Translators: DO NOT translate these */
		g_warning("Converting old fg color..");
		gtranslator_config_set_string("colors/own_fg", value);
		converted=TRUE;
	}

	value=gtranslator_config_get_string("colors/bg");

	if(value && value[0]=='#')
	{
		/* Translators: DO NOT translate these. */
		g_warning("Converting old bg color");
		gtranslator_config_set_string("colors/own_bg", value);
		converted=TRUE;
	}

	if(converted)
	{
		gtranslator_config_set_string("colors/fg", "black");
		gtranslator_config_set_string("colors/bg", "white");
	}

	gtranslator_config_close();
}


/*
 * Checks the given file for read permissions first and then
 *  for the right write permissions.
 */
gboolean gtranslator_utils_check_file_permissions(GtrPo *po_file)
{
	FILE *file;
	gchar *error_message;

	g_return_val_if_fail(po_file != NULL, FALSE);
	/*
	 * Open the file first for reading.
	 */
	file=fopen(po_file->filename, "r");
	if(file == NULL)
	{
		/*
		 * Create an error box and prevent further reading
		 *  of the file.
		 */  
		gtranslator_error(_("You don't have read permissions on file `%s'"),
			po_file->filename);

		return FALSE;
	}
	else
	{
		/*
		 * Open the same file also for a write-permission check.
		 */ 
		file=fopen(po_file->filename, "r+");
		if(file == NULL)
		{
			/*
			 * Show a warning box to the user and warn him about
			 *  the fact of lacking write permissions.
			 *  FIXME: do this ONLY on file save
			 */  
			error_message=g_strdup_printf(
				_("You don't have write permissions on file `%s'.\n\
This means that you should save it as a copy to a local directory\n\
of your choice."),
				po_file->filename);
			gnome_app_warning(GNOME_APP(gtranslator_application), error_message);

			po_file->no_write_perms=TRUE;
		
			return TRUE;
		}
		else
		{
			po_file->no_write_perms=FALSE;
		}
	}

	fclose(file);

	return TRUE;
}
