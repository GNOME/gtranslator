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

#include <gtk/gtk.h>
#include <string.h>

/*
 * Stored the value of the previous view -- per default at GTR_MESSAGE_VIEW.
 */
static GtrView previous_view=GTR_MESSAGE_VIEW;

/*
 * The views functions are declared here.
 */
void show_comment(void);
void show_c_format(void);

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

	if(previous_view==GTR_C_FORMAT_VIEW && view!=GTR_C_FORMAT_VIEW)
	{
		display_msg(po->current);
	}
	
	/*
	 * First sync the text boxes with the po file data:
	 */
	gtranslator_update_msg();

	switch(view)
	{
		case GTR_C_FORMAT_VIEW:
			show_c_format();
				break;
				
		case GTR_COMMENT_VIEW:
			show_comment();
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

	previous_view=GTR_COMMENT_VIEW;
	
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
 * Show c format elements in both parts of a message in the text boxes.
 */
void show_c_format()
{
	GtrMsg *msg=GTR_MSG(po->current->data);
	gboolean activate=TRUE;

	g_return_if_fail(msg!=NULL);
	
	previous_view=GTR_C_FORMAT_VIEW;

	if(msg->msgid && !strstr(msg->msgid, "%"))
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
		#define append_char1(x) \
			msgidformat=g_string_append_c(msgidformat, x)
		#define append_char2(x) \
			msgstrformat=g_string_append_c(msgstrformat,x)
			
		GString *msgidformat=g_string_new("");
		GString *msgstrformat=g_string_new("");
		gint z=0;
	
		/*
		 * Disable editing of the format view data.
		 */
		gtk_text_set_editable(GTK_TEXT(trans_box), FALSE);

		clean_text_boxes();
		
		while(msg->msgid[z])
		{
			if(msg->msgid[z]=='%' && msg->msgid[z++])
			{
				/*
				 * The special char or a white space.
				 */
				if(wants.dot_char)
				{
					append_char1(_("·")[0]);
				}
				else
				{
					append_char1(' ');
				}

				if(msg->msgid[z++]=='l' && msg->msgid[z+2])
				{
					append_char1(msg->msgid[z]);
					append_char1(msg->msgid[z++]);
					append_char1(msg->msgid[z+2]);

					z=z+2;
				}
				else
				{
					append_char1(msg->msgid[z]);
					append_char1(msg->msgid[z++]);
					
					z++;
				}
			}
			else
			{
				z++;
			}
		}

		if(msgidformat->len > 0)
		{
			gtranslator_syntax_insert_text(text1, 
				msgidformat->str);
		}
		else
		{
			gtk_text_insert(GTK_TEXT(text1), NULL, NULL, NULL,
				_("Couldn't extract C formats!"), -1);
		}
	
		z=0;
		
		while(msg->msgstr[z])
		{
			if(msg->msgstr[z]=='%' && msg->msgstr[z++])
			{

				/*
				 * The special char or a white space.
				 */
				if(wants.dot_char)
				{
					append_char2(_("·")[0]);
				}
				else
				{
					append_char2(' ');
				}
				
				if(msg->msgid[z++]=='l' && msg->msgid[z+2])
				{
					append_char2(msg->msgstr[z]);
					append_char2(msg->msgstr[z++]);
					append_char2(msg->msgstr[z+2]);

					z=z+2;
				}
				else
				{
					append_char2(msg->msgstr[z]);
					append_char2(msg->msgstr[z++]);
					
					z++;
				}
			}
			else
			{
				z++;
			}
		}

		if(msgstrformat->len > 0)
		{
			gtranslator_syntax_insert_text(trans_box, 
				msgstrformat->str);
		}
		else
		{
			gtk_text_insert(GTK_TEXT(trans_box), NULL, NULL, NULL,
				_("Couldn't extract C formats!"), -1);
		}

		g_string_free(msgidformat, FALSE);
		g_string_free(msgstrformat, FALSE);
	}
	
	/*
	 * Disable the current save action as the C format function applies 
	 *  chanegs which shouldn't be saved.
	 */
	disable_actions(ACT_SAVE);
}
