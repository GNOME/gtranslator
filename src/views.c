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

#include "views.h"
#include "gui.h"
#include "parse.h"
#include "prefs.h"
#include "syntax.h"

#include <ctype.h>
#include <string.h>

#include <gtk/gtk.h>

/*
 * Stored the value of the previous view -- per default at GTR_MESSAGE_VIEW.
 */
static GtrView previous_view=GTR_MESSAGE_VIEW;
static GtrView current_view=GTR_MESSAGE_VIEW;

/*
 * The views functions are declared here.
 */
void show_comment(void);
void show_c_format(void);
void show_number(void);
void show_hotkey(void);

/*
 * Helper functions for the different views -- extract the content from the
 *  the given parts and display them as desired.
 */
void show_up_figures(GtkWidget *output_widget, const gchar *string);
void show_up_formats(GtkWidget *output_widget, const gchar *string);
void show_up_hotkeys(GtkWidget *output_widget, const gchar *string);

/*
 * Generally used functions for instance.
 */
void insert_space(GString **string);
void setup_text(GtkWidget *widget, const gchar *string, const gchar *errstring);

/*
 * Set up the given view for the current message.
 */
gboolean gtranslator_views_set(GtrView view)
{
	static gint index=0;
	
	g_return_val_if_fail(view >= 0 && view < GTR_LAST_VIEW, FALSE);

	/* Need nothing to change */
	if(view==current_view)
		return TRUE;

	/*
	 * Rescue the current position in the message view.
	 */
	if(current_view==GTR_MESSAGE_VIEW)
	{
		index=gtk_editable_get_position(GTK_EDITABLE(trans_box));
		/* Put text from textbox into msg */
		gtranslator_message_update();
	}

	previous_view=current_view;

	nothing_changes=TRUE;
	
	switch(view)
	{
		case GTR_C_FORMAT_VIEW:
			show_c_format();
			break;
				
		case GTR_COMMENT_VIEW:
			show_comment();
			break;

		case GTR_NUMBER_VIEW:
			show_number();
			break;

		case GTR_HOTKEY_VIEW:
			show_hotkey();
			break;
			
		case GTR_MESSAGE_VIEW:
		default:
			gtranslator_message_show(po->current);

			if(index >= 0 && index <= gtk_text_get_length(GTK_TEXT(trans_box)))
			{
				gtk_editable_set_position(GTK_EDITABLE(trans_box), index);
			}
			
			current_view=GTR_MESSAGE_VIEW;
			break;
	}

	if(view==GTR_MESSAGE_VIEW)
		gtk_text_set_editable(GTK_TEXT(trans_box), TRUE);
	else
		gtk_text_set_editable(GTK_TEXT(trans_box), FALSE);

	nothing_changes=FALSE;
	
	return TRUE;
}

/*
 * Show the messages comment in the upper text widget.
 */
void show_comment()
{
	gchar *comment=GTR_MSG(po->current->data)->comment;

	current_view=GTR_COMMENT_VIEW;
	
	gtk_editable_delete_text(GTK_EDITABLE(text_box), 0, -1);

	setup_text(text_box, comment, _("No comments available for this message."));
}

/*
 * Show any numbers found in the message.
 */
void show_number()
{
	GtrMsg  *msg=GTR_MSG(po->current->data);

	g_return_if_fail(msg!=NULL);

	/*
	 * Set up the view informations and clean up our text boxes.
	 */
	current_view=GTR_NUMBER_VIEW;
	gtranslator_text_boxes_clean();

	/*
	 * Show the nude figures!
	 */
	show_up_figures(text_box, msg->msgid);
	show_up_figures(trans_box, msg->msgstr);
}

/*
 * Show c format elements in both parts of a message in the text boxes.
 */
void show_c_format()
{
	GtrMsg *msg=GTR_MSG(po->current->data);

	g_return_if_fail(msg!=NULL);
	
	current_view=GTR_C_FORMAT_VIEW;
	gtranslator_text_boxes_clean();

	/*
	 * Use the new helper functions for the real core task.
	 */
	show_up_formats(text_box, msg->msgid);
	show_up_formats(trans_box, msg->msgstr);
}

/*
 * Show the hotkeys of the current message.
 */
void show_hotkey()
{
	GtrMsg *msg=GTR_MSG(po->current->data);

	g_return_if_fail(msg!=NULL);

	/*
	 * Set the data for our internal use.
	 */
	current_view=GTR_HOTKEY_VIEW;
	gtranslator_text_boxes_clean();

	/*
	 * Handle both msgid and msgstr for the hotkeys.
	 */
	show_up_hotkeys(text_box, msg->msgid);
	show_up_hotkeys(trans_box, msg->msgstr);
}

/*
 * Strip off any numbers, show them up or print the corresponding error
 *  message into the specified text box.
 */
void show_up_figures(GtkWidget *output_widget, const gchar *string)
{
	GString *figures=g_string_new("");
	
	g_return_if_fail(output_widget!=NULL);

	/*
	 * Do not operate on empty strings.
	 */
	if(!string)
	{
		return;
	}
	else
	{
		gint c, rc=0;

		for(c=0; c < (strlen(string) - 1); ++c)
		{
			/*
			 * Aha, a number has been found!
			 */
			if(isdigit(string[c]))
			{
				if(rc > 1)
				{
					insert_space(&figures);
				}
				
				figures=g_string_append_c(figures, string[c]);

				rc++;
			}
		}
	}

	setup_text(output_widget, figures->str, _("No numbers found."));

	g_string_free(figures, FALSE);
}

/*
 * Extract the C format calls from the string and display them.
 */
void show_up_formats(GtkWidget *output_widget, const gchar *string)
{
	GString *formats=g_string_new("");
	gint z, rc=0;

	g_return_if_fail(output_widget!=NULL);

	if(!string)
	{
		return;
	}

	for(z=0; z < (strlen(string) - 1); ++z)
	{
		if(string[z]=='%')
		{
			formats=g_string_append_c(formats, string[z]);
			z++;
			
			/*
			 * Recognize and get the formats.
			 */
			if(wants.dot_char)
			{
				if(string[z]!=_(" ")[0] && !ispunct(string[z]) && 
					!iscntrl(string[z]))
				{
					formats=g_string_append_c(formats, string[z]);
				}
			}
			else
			{
				if(string[z]!=_("·")[0] && !ispunct(string[z]) &&
					!iscntrl(string[z]))
				{
					formats=g_string_append_c(formats, string[z]);
				}
			}
			
			if(rc > 1)
			{
				insert_space(&formats);
			}
			
			rc++;
		}
	}

	setup_text(output_widget, formats->str, _("No C formats present."));

	g_string_free(formats, FALSE);
}

/*
 * Helper function for the hotkeys view.
 */
void show_up_hotkeys(GtkWidget *output_widget, const gchar *string)
{
	GString *hotkeys=g_string_new("");
	gint z, rc=0;

	g_return_if_fail(output_widget!=NULL);

	if(!string)
	{
		return;
	}

	for(z=0; z < (strlen(string) - 1); ++z)
	{
		/*
		 * Hotkeys are out of _ + character, so recognize it here.
		 */
		if(string[z]=='_' && string[z+1] && isalpha(string[z+1]))
		{
			if(rc > 1)
			{
				insert_space(&hotkeys);
			}
			
			hotkeys=g_string_append_c(hotkeys, string[z]);
			hotkeys=g_string_append_c(hotkeys, string[z+1]);

			rc++;
		}
		
	}

	setup_text(output_widget, hotkeys->str, _("No hotkeys defined in message."));
	
	g_string_free(hotkeys, FALSE);
}

/*
 * Inserts a space or a dot/special char.
 */
void insert_space(GString **string)
{
	g_return_if_fail(*string!=NULL);

	if(wants.dot_char)
	{
		*string=g_string_append(*string, _("·"));
	}
	else
	{
		*string=g_string_append(*string, " ");
	}
}

/*
 * Setup the text for the box or print out the to-use error string.
 */
void setup_text(GtkWidget *widget, const gchar *string, const gchar *errstring)
{
	g_return_if_fail(widget!=NULL);
	g_return_if_fail(errstring!=NULL);
	
	if(string && (strlen(string) >= 1))
	{
		gtranslator_syntax_insert_text(widget, string);
	}
	else
	{
		gtk_text_insert(GTK_TEXT(widget), NULL, NULL, NULL,
			errstring, -1);
	}
}

/*
 * Return the name of the current/previously used view.
 */
GtrView gtranslator_views_get_current()
{
	return current_view;
}

GtrView gtranslator_views_get_previous()
{
	return previous_view;
}

