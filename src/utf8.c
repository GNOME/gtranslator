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

#include "convert.h"
#include "syntax.h"
#include "utf8.h"
#include "utils.h"

#include <locale.h>

#include <libgnome/gnome-i18n.h>

#include <gtk/gtkeditable.h>
#include <gtk/gtkentry.h>
#include <gtk/gtktext.h>
#include <gtk/gtkwidget.h>

/*
 * Return whether we do have got a UTF-8 file or not.
 */
gboolean gtranslator_utf8_po_file_is_utf8()
{
	g_return_val_if_fail(po->header->charset!=NULL, FALSE);

	if(!g_strcasecmp(po->header->charset, "utf-8") ||
		!g_strcasecmp(po->header->charset, "utf8"))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * Convert the given GtrMsg to UTF-8.
 */
void gtranslator_utf8_convert_message_to_utf8(GtrMsg *msg)
{
	gchar *msgstr;

	g_return_if_fail(msg!=NULL);

	msgstr=(msg)->msgstr;
	(msg)->msgstr=gtranslator_convert_string_to_utf8(msgstr,
		po->header->charset);
	
	g_free(msgstr);
}

/*
 * Convert the given GtrMsg from UTF-8 to a "normal" string.
 */
void gtranslator_utf8_convert_message_from_utf8(GtrMsg *msg)
{
	gchar *msgstr;

	g_return_if_fail(msg!=NULL);

	msgstr=(msg)->msgstr;
	(msg)->msgstr=gtranslator_convert_string_from_utf8(msgstr,
		po->header->charset);
	
	g_free(msgstr);
}

/*
 * Convert all messages completely to UTF-8.
 */
void gtranslator_utf8_convert_po_to_utf8(void)
{
	g_list_foreach(po->messages, 
		(GFunc) gtranslator_utf8_convert_message_to_utf8, NULL);
		
	/*
	 * Set the po file charset to UTF-8 as we did convert it now.
	 */
	g_free(po->header->charset); 
	po->header->charset=g_strdup("utf-8");
}

/*
 * Convert all messages completely from UTF-8.
 */
void gtranslator_utf8_convert_po_from_utf8(void)
{
	gchar *charset;
	charset=gtranslator_utils_get_locale_charset();

	g_list_foreach(po->messages,
		(GFunc) gtranslator_utf8_convert_message_from_utf8, NULL);

	/*
	 * Assign the converted charset value.
	 */
	if(charset)
	{
		g_free(po->header->charset);
		po->header->charset=g_strdup(charset);
		g_free(charset);
	}
}

/*
 * Return the plain string from the given UTF-8 string.
 */
gchar *gtranslator_utf8_get_plain_string(gchar **string)
{
	gchar *plain_string;

	g_return_val_if_fail(*string!=NULL, NULL);

	plain_string=gtranslator_convert_string_to_utf8(*string,
		po->header->charset);

	return plain_string;
}

/*
 * Return a pure UTF-8 string back.
 */
gchar *gtranslator_utf8_get_utf8_string(gchar **string)
{
	gchar 	*utf8_string;
	
	g_return_val_if_fail(*string!=NULL, NULL);

	utf8_string=gtranslator_convert_string_to_utf8(*string,
		po->header->charset);
	
	return utf8_string;
}

/*
 * Return the plain msgstr (uses the function above).
 */
gchar *gtranslator_utf8_get_plain_msgstr(GtrMsg **message)
{
	g_return_val_if_fail(GTR_MSG(*message)->msgstr!=NULL, NULL);

	return (gtranslator_utf8_get_plain_string(&GTR_MSG(*message)->msgstr));
}

/*
 * Convenience functions to get/set the "plain" content of a GtkEntry
 *  in UTF-8 form.
 */
gchar *gtranslator_utf8_get_gtk_entry_as_utf8_string(GtkWidget *entry)
{
	gchar *entry_content;
	
	g_return_val_if_fail(GTK_WIDGET(entry)!=NULL, NULL);

	entry_content=gtk_entry_get_text(GTK_ENTRY(entry));
	g_return_val_if_fail(entry_content!=NULL, NULL);

	return (gtranslator_convert_string_to_utf8(entry_content, 
		po->header->charset));
}

void gtranslator_utf8_set_gtk_entry_from_utf8_string(GtkWidget *entry,
	const gchar *utf8_string)
{
	gchar *plain_string;
	
	g_return_if_fail(GTK_WIDGET(entry)!=NULL);
	g_return_if_fail(utf8_string!=NULL);

	plain_string=gtranslator_convert_string_from_utf8(utf8_string,
		po->header->charset);
	g_return_if_fail(plain_string!=NULL);

	gtk_entry_set_text(GTK_ENTRY(entry), plain_string);
}

/*
 * Convenience function to get/set the normal content of a GtkText in UTF-8
 *  form.
 */
gchar *gtranslator_utf8_get_gtk_text_as_utf8_string(GtkWidget *text)
{
	gchar *text_content;
	
	g_return_val_if_fail(GTK_WIDGET(text)!=NULL, NULL);

	text_content=gtk_editable_get_chars(GTK_EDITABLE(text), 0, -1);
	g_return_val_if_fail(text_content!=NULL, NULL);

	return (gtranslator_convert_string_to_utf8(text_content,
		po->header->charset));
}

void gtranslator_utf8_set_gtk_text_from_utf8_string(GtkWidget *text,
	const gchar *utf8_string)
{
	gchar *plain_string;

	g_return_if_fail(GTK_WIDGET(text)!=NULL);
	g_return_if_fail(utf8_string!=NULL);

	plain_string=gtranslator_convert_string_from_utf8(utf8_string,
		po->header->charset);
	g_return_if_fail(plain_string!=NULL);
	
	/*
	 * We'll use our syntax highlighted insertion method also here .-)
	 */
	gtranslator_syntax_insert_text(text, plain_string);
}
