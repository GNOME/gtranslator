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

#include "gui.h"
#include "prefs.h"
#include "utils.h"

#include <libgnome/gnome-url.h>
#include <libgnome/gnome-i18n.h>
#include <libgnomeui/libgnomeui.h>

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
