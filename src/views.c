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

/*
 * Helper functions for the different views -- extract the content from the
 *  the given parts and display them as desired.
 */
void show_up_figures(GtkWidget *output_widget, const gchar *string);
void show_up_formats(GtkWidget *output_widget, const gchar *string);

/*
 * Set up the given view for the current message.
 */
gboolean gtranslator_views_set(GtrView view)
{
	gint index;
	
	if(view < 0 || view > GTR_LAST_VIEW)
	{
		return FALSE;
	}

	index=gtk_editable_get_position(GTK_EDITABLE(trans_box));

	if(view!=current_view)
	{
		previous_view=current_view;
	}

	
	/*
	 * First sync the text boxes with the po file data:
	 */
	gtranslator_views_prepare_for_navigation();

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
			
		case GTR_MESSAGE_VIEW:
		default:
			gtk_text_set_editable(GTK_TEXT(trans_box), TRUE);
			display_msg(po->current);
				break;
	}

	gtk_editable_set_position(GTK_EDITABLE(trans_box), index);

	return TRUE;
}

/*
 * Show the messages comment in the upper text widget.
 */
void show_comment()
{
	gchar *comment=GTR_MSG(po->current->data)->comment;

	current_view=GTR_COMMENT_VIEW;
	
	gtk_editable_delete_text(GTK_EDITABLE(text1), 0, -1);
	
	if(comment)
	{
		gtk_text_insert(GTK_TEXT(text1), NULL, NULL, NULL, comment, -1);
	}
	else
	{
		gtk_text_insert(GTK_TEXT(text1), NULL, NULL, NULL,
			_("No comment available for this message."), -1);
	}
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
	clean_text_boxes();

	/*
	 * Make the translation box non-editable.
	 */
	if(previous_view==GTR_MESSAGE_VIEW)
	{
		gtk_text_set_editable(GTK_TEXT(trans_box), FALSE);
	}

	/*
	 * Show the nude figures!
	 */
	show_up_figures(text1, msg->msgid);
	show_up_figures(trans_box, msg->msgstr);

	/*
	 * Disable saving as the figures view does also make changes
	 *  which shouldn't be saved.
	 */
	disable_actions(ACT_SAVE);
}

/*
 * Show c format elements in both parts of a message in the text boxes.
 */
void show_c_format()
{
	GtrMsg *msg=GTR_MSG(po->current->data);

	g_return_if_fail(msg!=NULL);
	
	current_view=GTR_C_FORMAT_VIEW;
	clean_text_boxes();

	/*
	 * Disable editing of pure view data.
	 */
	if(previous_view==GTR_MESSAGE_VIEW)
	{
		gtk_text_set_editable(GTK_TEXT(trans_box), FALSE);
	}

	/*
	 * Use the new helper functions for the real core task.
	 */
	show_up_formats(text1, msg->msgid);
	show_up_formats(trans_box, msg->msgstr);

	/*
	 * Disable the current save action as the C format function applies 
	 *  changes which shouldn't be saved.
	 */
	disable_actions(ACT_SAVE);
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
		gint c;

		for(c=0; c < (strlen(string) - 1); ++c)
		{
			/*
			 * Aha, a number has been found!
			 */
			if(isdigit(string[c]))
			{
				figures=g_string_append_c(figures, string[c]);
			}
			else if(((c-1) >= 0) && (isdigit(string[c-1])))
			{
				/*
				 * If the last char was a number but the current one
				 *  is not a number we'd insert a free space (or the
				 *   special char) for better readability.
				 */
				if(wants.dot_char)
				{
					figures=g_string_append(figures, _("·"));
				}
				else
				{
					figures=g_string_append(figures, " ");
				}
			}
		}
	}

	if(figures->len > 0)
	{
		gtranslator_syntax_insert_text(output_widget, figures->str);
	}
	else
	{
		gtk_text_insert(GTK_TEXT(output_widget), NULL, NULL, NULL,
			_("No numbers found!"), -1);
	}

	g_string_free(figures, FALSE);
}

/*
 * Extract the C format calls from the string and display them.
 */
void show_up_formats(GtkWidget *output_widget, const gchar *string)
{
	GString *formats=g_string_new("");
	gint z;

	g_return_if_fail(output_widget!=NULL);

	if(!string)
	{
		return;
	}

	for(z=0; z < (strlen(string) - 1); ++z)
	{
		if(string[z]=='%' && string[z+1])
		{
			formats=g_string_append_c(formats, string[z]);

			/*
			 * Recognize and get the formats.
			 */
			if(string[z+1]=='l' && string[z+2])
			{
				formats=g_string_append_c(formats, 'l');
				formats=g_string_append_c(formats, string[z+2]);
			}
			else
			{
				formats=g_string_append_c(formats, string[z+1]);
			}

			/*
			 * Insert the special/dot char after a format block.
			 */
			if(wants.dot_char)
			{
				formats=g_string_append(formats, _("·"));
			}
			else
			{
				formats=g_string_append(formats, " ");
			}
		}
	}

	if(formats->len > 0)
	{
		gtranslator_syntax_insert_text(output_widget, formats->str);
	}
	else
	{
		gtk_text_insert(GTK_TEXT(output_widget), NULL, NULL, NULL,
			_("No C formats present!"), -1);
	}
	
	g_string_free(formats, FALSE);
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

/*
 * Prepare the message for navigation.
 */
void gtranslator_views_prepare_for_navigation()
{
	if(current_view!=GTR_MESSAGE_VIEW)
	{
		/*
		 * Get the message view before navigating to anywhere and make
		 *  the translation box editable again.
		 */
		display_msg(po->current);
		gtk_text_set_editable(GTK_TEXT(trans_box), TRUE);
	}
	
	gtranslator_update_msg();
}
