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

#include "dialogs.h"
#include "gui.h"
#include "nautilus-string.h"
#include "prefs.h"
#include "runtime-config.h"
#include "utils.h"
#include "utils_gui.h"

#include <libgnome/gnome-url.h>
#include <libgnome/gnome-util.h>

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
			str[i]=gtranslator_runtime_config->special_char;
		} else if(str[i]==gtranslator_runtime_config->special_char) {
			str[i]=' ';
		}
	}
}

/*
 * Save the current application main window's geometry.
 */
void gtranslator_utils_save_geometry(void)
{
	if (GtrPreferences.save_geometry == TRUE) {
		gchar *gstr;
		gint x, y, w, h;
		gstr = gnome_geometry_string(gtranslator_application->window);
		gnome_parse_geometry(gstr, &x, &y, &w, &h);
		GTR_FREE(gstr);
		gtranslator_config_set_int("geometry/x", x);
		gtranslator_config_set_int("geometry/y", y);
		gtranslator_config_set_int("geometry/width", w);
		gtranslator_config_set_int("geometry/height", h);
		
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
		if (GtrPreferences.save_geometry == TRUE) {
			x = gtranslator_config_get_int("geometry/x");
			y = gtranslator_config_get_int("geometry/y");
			width = gtranslator_config_get_int("geometry/width");
			height = gtranslator_config_get_int("geometry/height");
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
				   gboolean editable,
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
	
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(combo)->entry), editable);
	
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
	gtk_table_attach_defaults(GTK_TABLE(table), toggle,
	                          0, GTK_TABLE(table)->ncols > 1 ? 2 : 1,
	                          row, row + 1);

	if(callback)
	{
		gtk_signal_connect(GTK_OBJECT(toggle), "toggled",
			   GTK_SIGNAL_FUNC(callback), NULL);
	}

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

	if(callback)
	{
		gtk_signal_connect(GTK_OBJECT(widget), "changed",
			GTK_SIGNAL_FUNC(callback), NULL);
	}

	return widget;
}

GtkWidget *gtranslator_utils_attach_spin_with_label(GtkWidget *table,
	gint row, const gchar *label_text, gfloat minimum, gfloat maximum,
	gfloat value, GtkSignalFunc callback)
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

	gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		GTK_SIGNAL_FUNC(callback), NULL);
	
	return spin_button;
}

/*
 * Adds a GnomeFontPicker to the given table.
 */
GtkWidget *gtranslator_utils_attach_font_with_label(GtkWidget *table,
	gint row, const gchar *label_text, const gchar *title_text,
	const gchar *fontspec, GtkSignalFunc callback)
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
	
	gtk_signal_connect(GTK_OBJECT(font_selector), "font_set",
		GTK_SIGNAL_FUNC(callback), NULL);
	
	return font_selector;
}

/*
 * Adds a GnomeColorPicker to the given table.
 */
GtkWidget *gtranslator_utils_attach_color_with_label(GtkWidget *table,
	gint row, const gchar *label_text, const gchar *title_text,
	ColorType color_type, GtkSignalFunc callback)
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
	
	gtk_signal_connect(GTK_OBJECT(color_selector), "color_set",
		GTK_SIGNAL_FUNC(callback), NULL);	
	
	return color_selector;
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

void gtranslator_parse_main(const gchar *filename);
void open_it_anyway_callback( gint, gpointer );

void open_it_anyway_callback( gint reply, gpointer filename )
{
	extern gboolean open_anyway;
	
	switch( reply ){
		case 0: /* YES */
			open_anyway = TRUE;
			gtranslator_parse_main( (gchar*)filename );
			break;
		case 1: /* NO */
			break;
	}
	g_free( filename );
}

/*
 * Check if the given file is alrady opened by gtranslator.
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
		gchar *error_message;
		gchar *fname;
		
		fname = g_strdup( filename );

		error_message=g_strdup_printf(
		_("The file\n"
		"\n"
		"   %s\n"
		"\n"
		"is already open in another instance of gtranslator!\n"
		"Please close the other instance of gtranslator handling\n"
		"this file currently to re-gain access to this file.\n"
		"\n"
		"Shall fool gtranslator open this file anyway ?"), 
		filename);

		/*
		 * Return with FALSE if the given file is already open. ???
		 * +++++
		 */
		/* gnome_app_error(GNOME_APP(gtranslator_application), error_message); */
		gnome_app_question_modal( GNOME_APP(gtranslator_application), error_message,
			open_it_anyway_callback, (gpointer)fname );
		
		GTR_FREE(resultfilename);
		GTR_FREE(error_message);

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
	
	if(!gnome_is_program_in_path(program_name))
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
