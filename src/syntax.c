/*
 * (C) 2001		Fatih Demir <kabalak@gtranslator.org>
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

#include "syntax.h"
#include "preferences.h"

/*
 * Insert the syntax highlighted text into the given text widget.
 */ 
void gtranslator_syntax_insert_text(GtkWidget *textwidget, GtrMsg *msg)
{
	g_return_if_fail(textwidget!=NULL);
}

/*
 * Check if the given message does contain any format specifiers.
 */ 
gboolean gtranslator_syntax_get_format(GtrMsg *msg)
{
	g_return_val_if_fail(msg!=NULL, FALSE);

	/*
	 * Simply determine if there is any '%' character in the msgid
	 *  and if existent in the msgstr of the message.
	 */  
	if(strchr(GTR_MSG(msg)->msgid, '%'))
	{
		return TRUE;
	}
	if(GTR_MSG(msg)->msgstr &&
		strchr(GTR_MSG(msg)->msgstr, '%'))
	{
		return TRUE;
	}

	return FALSE;
}

/*
 * Return the requested GdkColor -- it is newly allocated and should
 *  possibly be freed somewhere.
 */
GdkColor *gtranslator_syntax_get_gdk_color(ColorName name)
{
	GdkColor *color;
	
	color=g_new0(GdkColor,1);

	switch(name)
	{
		case RED:
			color->red=(gushort) 65536;
			color->green=color->blue=(gushort) 0;
			break;

		case GREEN:
			color->green=(gushort) 65536;
			color->red=color->blue=(gushort) 0;
			break;
		
		case BLUE:
			color->blue=(gushort) 65536;
			color->red=color->green=(gushort) 0;
			break;
			
		case BLACK:
			color->red=color->green=color->blue=(gushort) 65536;
			break;
			
		case WHITE:
			color->red=color->green=color->blue=(gushort) 0;
			break;
		
		case YELLOW:
			color->red=color->green=(gushort) 65536;
			color->blue=(gushort) 0;
			break;
		
		case ORANGE:
			color->red=(gushort) 65536;
			color->green=(gushort) 43954;
			color->blue=(gushort) 0;
			break;
		
		case NAVY:
			color->red=(gushort) 9744;
			color->green=(gushort) 6773;
			color->blue=(gushort) 65536;
			break;
			
		case MAROON:
			color->red=color->blue=(gushort) 65536;
			color->green=(gushort) 0;
			break;
			
		case AQUA:
			color->red=(gushort) 0;
			color->green=color->blue=(gushort) 65536;
			break;
			
		default:
			/*
			 * Get the stored default values for the foreground
			 *  or the user specified ones.
			 */  
			gtranslator_config_init();
			
			color->red=gtranslator_config_get_int(
				"colors/fg_red");
			color->green=gtranslator_config_get_int(
				"colors/fg_green");
			color->blue=gtranslator_config_get_int(
				"colors/fg_blue");
			
			gtranslator_config_close();
			
			break;
	}

	
	color->pixel=(gulong) (
		(color->red)*65536 + (color->green)*255 + color->blue);

	gdk_color_alloc(gtk_widget_get_colormap(trans_box), color);

	return color;
}
