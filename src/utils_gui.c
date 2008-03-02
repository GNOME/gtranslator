/*
 * (C) 2001-2007 	Fatih Demir <kabalak@kabalak.net>
 * 			Ignacio Casal <nacho.resa@gmail.com>
 * 			Paolo Maggi 
 *
 * 	Based in gedit utils funcs.
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

#include "utils_gui.h"

#include <string.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <glade/glade.h>
#include <gtk/gtk.h>


GtkWidget *
gtranslator_gtk_button_new_with_stock_icon (const gchar *label,
				      const gchar *stock_id)
{
	GtkWidget *button;

	button = gtk_button_new_with_mnemonic (label);
	gtk_button_set_image (GTK_BUTTON (button),
			      gtk_image_new_from_stock (stock_id,
							GTK_ICON_SIZE_BUTTON));

        return button;
}

void
gtranslator_utils_menu_position_under_widget (GtkMenu  *menu,
					gint     *x,
					gint     *y,
					gboolean *push_in,
					gpointer  user_data)
{
	GtkWidget *w = GTK_WIDGET (user_data);
	GtkRequisition requisition;

	gdk_window_get_origin (w->window, x, y);
	gtk_widget_size_request (GTK_WIDGET (menu), &requisition);

	if (gtk_widget_get_direction (w) == GTK_TEXT_DIR_RTL)
	{
		*x += w->allocation.x + w->allocation.width - requisition.width;
	}
	else
	{
		*x += w->allocation.x;
	}

	*y += w->allocation.y + w->allocation.height;

	*push_in = TRUE;
}

void
gtranslator_utils_menu_position_under_tree_view (GtkMenu  *menu,
					   gint     *x,
					   gint     *y,
					   gboolean *push_in,
					   gpointer  user_data)
{
	GtkTreeView *tree = GTK_TREE_VIEW (user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	
	model = gtk_tree_view_get_model (tree);
	g_return_if_fail (model != NULL);

	selection = gtk_tree_view_get_selection (tree);
	g_return_if_fail (selection != NULL);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter))
	{
		GtkTreePath *path;
		GdkRectangle rect;

		gdk_window_get_origin (GTK_WIDGET (tree)->window, x, y);
			
		path = gtk_tree_model_get_path (model, &iter);
		gtk_tree_view_get_cell_area (tree, path,
					     gtk_tree_view_get_column (tree, 0), /* FIXME 0 for RTL ? */
					     &rect);
		gtk_tree_path_free (path);
		
		*x += rect.x;
		*y += rect.y + rect.height;
		
		if (gtk_widget_get_direction (GTK_WIDGET (tree)) == GTK_TEXT_DIR_RTL)
		{
			GtkRequisition requisition;
			gtk_widget_size_request (GTK_WIDGET (menu), &requisition);
			*x += rect.width - requisition.width;
		}
	}
	else
	{
		/* no selection -> regular "under widget" positioning */
		gtranslator_utils_menu_position_under_widget (menu,
							x, y, push_in,
							tree);
	}
}


/**
 * gtranslator_utils_get_glade_widgets:
 * @filename: the path to the glade file
 * @root_node: the root node in the glade file
 * @error_widget: a pointer were a #GtkLabel
 * @widget_name: the name of the first widget
 * @...: a pointer were the first widget is returned, followed by more
 *       name / widget pairs and terminated by NULL.
 *
 * This function gets the requested widgets from a glade file. In case
 * of error it returns FALSE and sets error_widget to a GtkLabel containing
 * the error message to display.
 *
 * Returns FALSE if an error occurs, TRUE on success.
 */
gboolean
gtranslator_utils_get_glade_widgets (const gchar *filename,
				     const gchar *root_node,
				     GtkWidget **error_widget,
				     const gchar *widget_name,
				     ...)
{
	GtkWidget *label;
	GladeXML *gui;
	va_list args;
	const gchar *name;
	gchar *msg;
	gchar *filename_markup;
	gchar *msg_plain;
	gboolean ret = TRUE;

	g_return_val_if_fail (filename != NULL, FALSE);
	g_return_val_if_fail (error_widget != NULL, FALSE);
	g_return_val_if_fail (widget_name != NULL, FALSE);

	*error_widget = NULL;

	gui = glade_xml_new (filename, root_node, NULL);
	if (!gui)
	{
		filename_markup = g_markup_printf_escaped ("<i>%s</i>", filename);
		msg_plain = g_strdup_printf (_("Unable to find file %s."),
				filename_markup);
		msg = g_strconcat ("<span size=\"large\" weight=\"bold\">",
				msg_plain, "</span>\n\n",
				_("Please check your installation."), NULL);
		label = gtk_label_new (msg);

		gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
		gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
		
		g_free (filename_markup);
		g_free (msg_plain);
		g_free (msg);

		gtk_misc_set_padding (GTK_MISC (label), 5, 5);
 		
		*error_widget = label;

		return FALSE;
	}

	va_start (args, widget_name);
	for (name = widget_name; name; name = va_arg (args, const gchar *) )
	{
		GtkWidget **wid;

		wid = va_arg (args, GtkWidget **);
		*wid = glade_xml_get_widget (gui, name);
		if (*wid == NULL)
		{
			g_warning ("Cannot find widget '%s' inside file '%s'.",
				   name,
				   filename);
				   
			filename_markup = g_markup_printf_escaped ("<i>%s</i>", filename);
			msg_plain = g_strdup_printf (
					_("Unable to find the required widgets inside file %s."),
					filename_markup);
			msg = g_strconcat ("<span size=\"large\" weight=\"bold\">",
					msg_plain, "</span>\n\n",
					_("Please check your installation."), NULL);
			label = gtk_label_new (msg);

			gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
			gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
			
			g_free (filename_markup);
			g_free (msg_plain);
			g_free (msg);

			gtk_misc_set_padding (GTK_MISC (label), 5, 5);
 			
			*error_widget = label;

			ret = FALSE;

			break;
		}
	}
	va_end (args);

	g_object_unref (gui);

	return ret;
}

static gboolean
is_valid_scheme_character (gchar c)
{
	return g_ascii_isalnum (c) || c == '+' || c == '-' || c == '.';
}

static gboolean
has_valid_scheme (const gchar *uri)
{
	const gchar *p;

	p = uri;

	if (!is_valid_scheme_character (*p)) {
		return FALSE;
	}

	do {
		p++;
	} while (is_valid_scheme_character (*p));

	return *p == ':';
}

gboolean
gtranslator_utils_is_valid_uri (const gchar *uri)
{
	const guchar *p;

	if (uri == NULL)
		return FALSE;

	if (!has_valid_scheme (uri))
		return FALSE;

	/* We expect to have a fully valid set of characters */
	for (p = (const guchar *)uri; *p; p++) {
		if (*p == '%')
		{
			++p;
			if (!g_ascii_isxdigit (*p))
				return FALSE;

			++p;		
			if (!g_ascii_isxdigit (*p))
				return FALSE;
		}
		else
		{
			if (*p <= 32 || *p >= 128)
				return FALSE;
		}
	}

	return TRUE;
}

/**
 * gtranslator_utils_drop_get_uris:
 * @selection_data: the #GtkSelectionData from drag_data_received
 * @info: the info from drag_data_received
 *
 * Create a list of valid uri's from a uri-list drop.
 * 
 * Return value: a string array which will hold the uris or NULL if there 
 *		 were no valid uris. g_strfreev should be used when the 
 *		 string array is no longer used
 */
gchar **
gtranslator_utils_drop_get_uris (GtkSelectionData *selection_data)
{
	gchar **uris;
	gint i;
	gint p = 0;
	gchar **uri_list;

	uris = g_uri_list_extract_uris ((gchar *) selection_data->data);
	uri_list = g_new0(gchar *, g_strv_length (uris) + 1);

	for (i = 0; uris[i] != NULL; i++)
	{
		/* Silently ignore malformed URI/filename */
		if (gtranslator_utils_is_valid_uri (uris[i]))
			uri_list[p++] = g_strdup (uris[i]);
	}

	if (*uri_list == NULL)
	{
		g_free(uri_list);
		return NULL;
	}

	return uri_list;
}

gchar *
gtranslator_utils_escape_search_text (const gchar* text)
{
	GString *str;
	gint length;
	const gchar *p;
 	const gchar *end;

	if (text == NULL)
		return NULL;

    	length = strlen (text);

	/* no escape when typing.
	 * The short circuit works only for ascii, but we only
	 * care about not escaping a single '\' */
	if (length == 1)
		return g_strdup (text);

	str = g_string_new ("");

	p = text;
  	end = text + length;

  	while (p != end)
    	{
      		const gchar *next;
      		next = g_utf8_next_char (p);

		switch (*p)
        	{
       			case '\n':
          			g_string_append (str, "\\n");
          			break;
			case '\r':
          			g_string_append (str, "\\r");
          			break;
			case '\t':
          			g_string_append (str, "\\t");
          			break;
			case '\\':
          			g_string_append (str, "\\\\");
          			break;
        		default:
          			g_string_append_len (str, p, next - p);
          			break;
        	}

      		p = next;
    	}

	return g_string_free (str, FALSE);
}

gchar *
gtranslator_utils_unescape_search_text (const gchar *text)
{
	GString *str;
	gint length;
	gboolean drop_prev = FALSE;
	const gchar *cur;
	const gchar *end;
	const gchar *prev;
	
	if (text == NULL)
		return NULL;

	length = strlen (text);

	str = g_string_new ("");

	cur = text;
	end = text + length;
	prev = NULL;
	
	while (cur != end) 
	{
		const gchar *next;
		next = g_utf8_next_char (cur);

		if (prev && (*prev == '\\')) 
		{
			switch (*cur) 
			{
				case 'n':
					str = g_string_append (str, "\n");
				break;
				case 'r':
					str = g_string_append (str, "\r");
				break;
				case 't':
					str = g_string_append (str, "\t");
				break;
				case '\\':
					str = g_string_append (str, "\\");
					drop_prev = TRUE;
				break;
				default:
					str = g_string_append (str, "\\");
					str = g_string_append_len (str, cur, next - cur);
				break;
			}
		} 
		else if (*cur != '\\') 
		{
			str = g_string_append_len (str, cur, next - cur);
		} 
		else if ((next == end) && (*cur == '\\')) 
		{
			str = g_string_append (str, "\\");
		}
		
		if (!drop_prev)
		{
			prev = cur;
		}
		else 
		{
			prev = NULL;
			drop_prev = FALSE;
		}

		cur = next;
	}

	return g_string_free (str, FALSE);
}

/*
 * n: len of the string in bytes
 */
gboolean 
g_utf8_caselessnmatch (const gchar *s1,
		       const gchar *s2,
		       gssize n1,
		       gssize n2)
{
	gchar *casefold;
	gchar *normalized_s1;
      	gchar *normalized_s2;
	gint len_s1;
	gint len_s2;
	gboolean ret = FALSE;

	g_return_val_if_fail (s1 != NULL, FALSE);
	g_return_val_if_fail (s2 != NULL, FALSE);
	g_return_val_if_fail (n1 > 0, FALSE);
	g_return_val_if_fail (n2 > 0, FALSE);

	casefold = g_utf8_casefold (s1, n1);
	normalized_s1 = g_utf8_normalize (casefold, -1, G_NORMALIZE_NFD);
	g_free (casefold);

	casefold = g_utf8_casefold (s2, n2);
	normalized_s2 = g_utf8_normalize (casefold, -1, G_NORMALIZE_NFD);
	g_free (casefold);

	len_s1 = strlen (normalized_s1);
	len_s2 = strlen (normalized_s2);

	if (len_s1 < len_s2)
		goto finally_2;

	ret = (strncmp (normalized_s1, normalized_s2, len_s2) == 0);
	
finally_2:
	g_free (normalized_s1);
	g_free (normalized_s2);	

	return ret;
}

void
gtranslator_utils_activate_url (GtkAboutDialog *dialog,
				const gchar *url,
				gpointer data)
{
	gchar **open;
	gchar *program;
	GPtrArray *array;

	if (g_find_program_in_path ("xdg-open"))
	{
		program = g_strdup ("xdg-open");
	}
	else return;
	
	array = g_ptr_array_new ();
	g_ptr_array_add (array, program);
	g_ptr_array_add (array, g_strdup (url));
	
	open = (gchar **)g_ptr_array_free (array, FALSE);
					
	gdk_spawn_on_screen (gdk_screen_get_default (),
			     NULL,
			     open,
			     NULL,
			     G_SPAWN_SEARCH_PATH,
			     NULL,
			     NULL, NULL, NULL);
					
	g_strfreev (open);
}
