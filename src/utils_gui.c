/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
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
#include "nautilus-string.h"
#include "parse.h"
#include "prefs.h"
#include "runtime-config.h"
#include "utils.h"
#include "utils_gui.h"

#include <libgnome/gnome-url.h>

#include <libgnomeui/libgnomeui.h>

/*
 * Show an error message.
 */
GtkWidget * gtranslator_utils_error_dialog(gchar *format, ...)
{
	gchar *error;
	va_list ap;
	GtkWidget *w;
	
	va_start(ap, format);
	error = g_strdup_vprintf(format, ap);
	va_end(ap);
	w = gnome_app_error(GNOME_APP(gtranslator_application), error);
	GTR_FREE(error);
	return w;
}

/*
 * Shows the gtranslator homepage on the web.
 */
void gtranslator_utils_show_home_page(GtkWidget *widget, gpointer useless)
{
	gnome_url_show("http://www.gtranslator.org", NULL);
}

/*
 * Go through the characters and search for free spaces
 * and replace them with '·''s.
 */
gchar *gtranslator_utils_invert_dot(gchar *str)
{
	GString *newstr;
	gunichar middot;
	gchar *p;

	g_return_val_if_fail(str != NULL, str);
	g_return_val_if_fail(strlen(str) >= 0, str);
	
	newstr = g_string_sized_new(strlen(str)+10);
	middot = g_utf8_get_char("Â·");
	p = str;
	if (*p)	
		do 
		{
			gunichar c = g_utf8_get_char(p);
			if (c == middot)
				g_string_append_c(newstr, ' ');
			else if (g_unichar_break_type(c) == G_UNICODE_BREAK_SPACE)
				g_string_append_unichar(newstr, middot);
			else
				g_string_append_unichar(newstr, c);
			p = g_utf8_next_char(p);
		} while (*p);
	return(g_string_free(newstr, FALSE));
}

/*
 * Save the current application main window's geometry.
 */
void gtranslator_utils_save_geometry(void)
{
	if (GtrPreferences.save_geometry == TRUE) {
		gint x, y, w, h, d;
		
		/*
		 * Use the Gdk functions to get the window typistics and then
		 *  store the data - we're currently stumping the silly "depth"
		 *   data also, but well...
		 */
		gdk_window_get_geometry(GDK_WINDOW(gtranslator_application->window),
			&x, &y, &w, &h, &d);
		
		gtranslator_config_set_int("geometry/x", x);
		gtranslator_config_set_int("geometry/y", y);
		gtranslator_config_set_int("geometry/width", w);
		gtranslator_config_set_int("geometry/height", h);
		gtranslator_config_set_int("geometry/depht", d);
	}
}

/*
 * Restore the geometry.
 */
void gtranslator_utils_restore_geometry(gchar  * gstr)
{
	gint x=0, y=0, width=0, height=0;

	/*
	 * Set the main window's geometry from the preferences.
	 */
	if (gstr == NULL)
	{
		if(GtrPreferences.save_geometry == TRUE)
		{
			x=gtranslator_config_get_int("geometry/x");
			y=gtranslator_config_get_int("geometry/y");
			width=gtranslator_config_get_int("geometry/width");
			height=gtranslator_config_get_int("geometry/height");
		}
		else
		{
			return;
		}
	}
	/*
	 * If a geometry definition had been defined try to parse it.
	 */
	else
	{
		if(!gtk_window_parse_geometry(GTK_WINDOW(gtranslator_application->window), gstr))
		{
			g_warning(_("The geometry string \"%s\" couldn't be parsed!"), gstr);
			return;
		}
	}
	if (x != -1)
		gtk_window_move(GTK_WINDOW(gtranslator_application), x, y);
	if ((width > 0) && (height > 0))
		gtk_window_resize(GTK_WINDOW(gtranslator_application), width, height);
}

GtkWidget *gtranslator_utils_attach_combo_with_label(GtkWidget  * table, gint row,
				   const char *label_text,
				   GList  * list, const char *value,
				   gboolean editable,
				   GCallback callback,
				   gpointer user_data)
{
	GtkWidget *label;
	GtkWidget *combo;
	label = gtk_label_new(label_text);
	combo = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(combo), list);
	if (value)
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(combo)->entry), value);
	
	gtk_editable_set_editable(GTK_EDITABLE(GTK_COMBO(combo)->entry), editable);
	
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 2, row, row + 1);
	
	g_signal_connect(G_OBJECT(GTK_COMBO(combo)->entry), "changed",
			 G_CALLBACK(callback), user_data);
	return combo;
}

GtkWidget *gtranslator_utils_attach_toggle_with_label(GtkWidget  * table, gint row,
				    const char *label_text,
				    gboolean value,
				    GCallback callback)
{
	GtkWidget *toggle;
	toggle = gtk_check_button_new_with_label(label_text);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), value);
	gtk_table_attach_defaults(GTK_TABLE(table), toggle,
	                          0, GTK_TABLE(table)->ncols > 1 ? 2 : 1,
	                          row, row + 1);

	if(callback)
	{
		g_signal_connect(G_OBJECT(toggle), "toggled",
			         G_CALLBACK(callback), NULL);
	}

	return toggle;
}

GtkWidget *gtranslator_utils_attach_entry_with_label(GtkWidget  * table, gint row,
				   const char *label_text,
				   const char *value,
				   GCallback callback)
{
	GtkWidget *label;
	GtkWidget *entry;
	label = gtk_label_new(label_text);
	entry = gtk_entry_new();
	if (value)
		gtk_entry_set_text(GTK_ENTRY(entry), value);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), entry, 1, 2, row, row + 1);
	g_signal_connect(G_OBJECT(entry), "changed",
			 G_CALLBACK(callback), NULL);
	return entry;
}

GtkWidget *gtranslator_utils_attach_text_with_label(GtkWidget  * table, gint row,
				  const char *label_text,
				  const char *value,
				  GCallback callback)
{
	GtkWidget *label;
	GtkWidget *widget;
	GtkWidget *scroll;
	GtkTextBuffer *buff;
	GtkTextIter start;

	label = gtk_label_new(label_text);
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	widget = gtk_text_view_new();
	if (value) {
		buff = gtk_text_buffer_new(NULL);
		gtk_text_buffer_get_start_iter(buff, &start);
		gtk_text_buffer_insert(buff, &start, value, strlen(value));
		gtk_text_view_set_buffer(GTK_TEXT_VIEW(widget), buff);
		if(callback)
		{
			g_signal_connect(G_OBJECT(buff), "changed",
					 G_CALLBACK(callback), NULL);
		}

	}
	gtk_container_add(GTK_CONTAINER(scroll), widget);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), scroll, 1, 2, row, row + 1);

	return widget;
}

GtkWidget *gtranslator_utils_attach_spin_with_label(GtkWidget *table,
	gint row, const gchar *label_text, gfloat minimum, gfloat maximum,
	gfloat value, GCallback callback)
{
	GtkWidget *label;
	GtkWidget *spin_button;
	GtkObject *adjustment;

	label=gtk_label_new(label_text);
	adjustment=gtk_adjustment_new(value, minimum, maximum, 1.0, 1.0, 1.0);

	spin_button=gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button), value);

	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), spin_button, 1, 2, row, row + 1);

	g_signal_connect(G_OBJECT(spin_button), "changed",
		G_CALLBACK(callback), NULL);
	
	return spin_button;
}

/*
 * Adds a GnomeFontPicker to the given table.
 */
GtkWidget *gtranslator_utils_attach_font_with_label(GtkWidget *table,
	gint row, const gchar *label_text, const gchar *title_text,
	const gchar *fontspec, GCallback callback)
{
	GtkWidget *label;
	GtkWidget *font_selector;
		
	label=gtk_label_new(label_text);
	
	font_selector=gnome_font_picker_new();
	
	gnome_font_picker_set_title(GNOME_FONT_PICKER(font_selector), 
		title_text);
	
	if(fontspec)
	{
		gnome_font_picker_set_font_name(GNOME_FONT_PICKER(font_selector),
			fontspec);
	}
	
	gnome_font_picker_set_mode(GNOME_FONT_PICKER(font_selector),
		GNOME_FONT_PICKER_MODE_FONT_INFO);
	
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), font_selector, 1, 2, 
		row, row + 1);
	
	g_signal_connect(G_OBJECT(font_selector), "font_set",
		G_CALLBACK(callback), NULL);
	
	return font_selector;
}

/*
 * Adds a GnomeColorPicker to the given table.
 */
GtkWidget *gtranslator_utils_attach_color_with_label(GtkWidget *table,
	gint row, const gchar *label_text, const gchar *title_text,
	ColorType color_type, GCallback callback)
{
	GtkWidget *label;
	GtkWidget *color_selector;
	
	label=gtk_label_new(label_text);

	color_selector=gnome_color_picker_new();
	
	gnome_color_picker_set_title(GNOME_COLOR_PICKER(color_selector),
		title_text);
	
	gtranslator_color_values_get(GNOME_COLOR_PICKER(color_selector), 
		color_type);
	
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, row, row + 1);
	gtk_table_attach_defaults(GTK_TABLE(table), color_selector, 1, 2, 
		row, row + 1);
	
	g_signal_connect(G_OBJECT(color_selector), "color_set",
		G_CALLBACK(callback), NULL);	
	
	return color_selector;
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
		gtranslator_utils_error_dialog(
			_("You are not permitted to access file '%s'."),
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
			 */  
			error_message=g_strdup_printf(
				_("You do not have permission to modify file '%s'.\n\
Please save a new copy of it to a place of your choice and get write\n\
permission for it."),
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

void open_it_anyway_callback( gint, gpointer );

void open_it_anyway_callback( gint reply, gpointer filename )
{
	extern gboolean open_anyway;
	
	switch( reply ){
		case 0: /* YES */
			open_anyway = TRUE;
			break;
		case 1: /* NO */
			break;
	}
	g_free( filename );
}

/*
 * Check if the given file is already opened by gtranslator.
 */
gboolean gtranslator_utils_check_file_being_open(const gchar *filename)
{
	gchar *resultfilename;

	extern gboolean open_anyway;

	if( open_anyway ){
		open_anyway = FALSE;
		return( FALSE );
	}

	g_return_val_if_fail(filename!=NULL, FALSE);

	resultfilename=gtranslator_config_get_string("runtime/filename");

	/*
	 * Test if we've got a filename and then test it for equality with our
	 *  currently in another instance opened po file.
	 */
	if(resultfilename && (!nautilus_strcasecmp(resultfilename, filename)) &&
		(strlen(resultfilename)==strlen(filename)))
	{
		gint reply;
		reply = gtranslator_already_open_dialog(NULL, (gpointer)filename);
		if(reply == GTK_RESPONSE_YES)
			open_anyway = TRUE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * Check for a needed program -- returns FALSE on failure (how logical, not ,-)).
 */
gboolean gtranslator_utils_check_program(const gchar *program_name,
	const gint type_int)
{
	g_return_val_if_fail(program_name!=NULL, FALSE);
	
	if(!g_find_program_in_path(program_name))
	{
		gchar *warning_message;

		if(type_int==0)
		{
			warning_message=g_strdup_printf(
				_("The necessary decompression program `%s' is not installed!"), program_name);
		}
		else
		{
			warning_message=g_strdup_printf(
				_("The necessary compression program `%s' is not installed!"), program_name);
		}

		gnome_app_warning(GNOME_APP(gtranslator_application), warning_message);
		GTR_FREE(warning_message);

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
