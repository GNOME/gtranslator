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
 * A helper function for the number view -- extracts the figures from
 *  the given parts and displays them -- or not.
 */
void show_up_figures(GtkWidget *output_widget, const gchar *string);

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
	gtk_text_set_editable(GTK_TEXT(trans_box), FALSE);

	/*
	 * Show the nude figures!
	 */
	show_up_figures(text1, msg->msgid);
	show_up_figures(trans_box, msg->msgstr);
}

/*
 * Show c format elements in both parts of a message in the text boxes.
 */
void show_c_format()
{
	GtrMsg *msg=GTR_MSG(po->current->data);
	gboolean activate=TRUE;

	g_return_if_fail(msg!=NULL);
	
	current_view=GTR_C_FORMAT_VIEW;

	if(!strstr(msg->msgid, "%"))
	{
		activate=FALSE;
		
		gtk_editable_delete_text(GTK_EDITABLE(text1), 0, -1);

		gtk_text_insert(GTK_TEXT(text1), NULL, NULL, NULL,
			_("No C format symbols present for this message"), -1);
	}
	
	if(msg->msgstr && !strstr(msg->msgstr, "%"))
	{
		activate=FALSE;
		
		gtk_editable_delete_text(GTK_EDITABLE(trans_box), 0, -1);

		gtk_text_insert(GTK_TEXT(trans_box), NULL, NULL, NULL,
			_("No C format symbols present for this message"), -1);
	}

	if(activate)
	{
		#define delta(x) (pos[x].rm_eo-pos[x].rm_so)
		GString *format=g_string_new("");
		gint z=1;
		
		regex_t *rX;
		regmatch_t pos[3];
	
		rX=gnome_regex_cache_compile(rxc, "\%[a-zA-Z0-9.-+]", 
			REG_EXTENDED|REG_NEWLINE);
		
		/*
		 * Disable editing of the format view data.
		 */
		gtk_text_set_editable(GTK_TEXT(trans_box), FALSE);

		clean_text_boxes();

		if(msg->msgid && !regexec(rX, msg->msgid, 3, pos, 0))
		{
			while(pos[z].rm_so!=-1)
			{
				if(format->len > 0)
				{
					if(wants.dot_char)
					{
						format=g_string_append(format,
							_("·"));
					}
					else
					{
						format=g_string_append(format, 
							" ");
					}
				}
				
				format=g_string_append(format,
					g_strndup(msg->msgid+pos[z].rm_so, delta(z)));
			}
		}

		if(format->len > 0)
		{
			gtranslator_syntax_insert_text(text1, format->str);
		}
		else
		{
			gtk_text_insert(GTK_TEXT(text1), NULL, NULL, NULL,
				_("Couldn't extract C format symbols!"), -1);
		}

		format=g_string_truncate(format, 0);
		z=1;

		if(msg->msgstr && !regexec(rX, msg->msgstr, 3, pos, 0))
		{
			while(pos[z].rm_so!=-1)
			{
				if(format->len > 0)
				{
					if(wants.dot_char)
					{
						format=g_string_append(format,
							_("·"));
					}
					else
					{
						format=g_string_append(format, 
							" ");
					}
				}

				format=g_string_append(format,
					g_strndup(msg->msgstr+pos[z].rm_so, delta(z)));
			}
		}

		if(format->len > 0)
		{
			gtranslator_syntax_insert_text(trans_box, format->str);
		}
		else
		{
			gtk_text_insert(GTK_TEXT(trans_box), NULL, NULL, NULL,
				_("Couldn't extract C format symbols!"), -1);
		}

		g_string_free(format, FALSE);
	}
	
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

	if(!string)
	{
		return;
	}
	else
	{
		gint c;

		for(c=0; c < (strlen(string)-1); ++c)
		{
			if(isdigit(string[c]))
			{
				figures=g_string_append_c(figures, string[c]);
			}
			else if(((c-1) > 0) && (isdigit(string[c-1])))
			{
				if(wants.dot_char)
				{
					figures=g_string_append(figures, 
						_("·"));
				}
				else
				{
					figures=g_string_append(figures,
						" ");
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
