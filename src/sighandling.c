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

#include "sighandling.h"

#include "parse.h"
#include "dialogs.h"
#include <libgnome/gnome-i18n.h>

#include <signal.h>

/*
 * Get and enclose the signals that could put much translation work within
 *  gtranslatorinto the trash.
 */ 
void gtranslator_signal_handler(int signal)
{
	static gint signalscount=0;

	if(signalscount > 0)
	{
		return;
	}	
	
	switch(signal)
	{
		/*
		 * Catch all signals in one function.
		 */ 
		case SIGINT:
		case SIGQUIT:
		case SIGTERM:
		case SIGSEGV:
		case SIGILL:

			if(po->file_changed)
			{
				gchar answer;

				g_print(_("gtranslator is about being closed.\n\
Would you like to save `%s'?\n[y/n] "),
					po->filename);

				scanf("%c", &answer);

				/*
				 * Translators: Please use here the same
				 *  characters as in the question where
				 *   you translated [y/n]!
				 */  
				if(answer==_("y")[0] || answer==_("Y")[0])
				{
					g_print(_("Saving...\n"));

					save_current_file(NULL, NULL);

					g_print(_("Saved `%s'.\n"),
						po->filename);
				}
				else
				{
					g_print(_("Not saving `%s'!\n"),
						po->filename);
				}
			}
			
			exit(1);
			break;
			
		default:

			/*
			 * What to say else? This signals doesn't seem
			 *  interesting for us but should still get a
			 *   warning message.
			 */   
			g_warning(_("Uncatched signal %i!"), signal);

			exit(1);
			break;
	}
}
