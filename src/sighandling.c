/*
 * (C) 2001 	Fatih Demir <kabalak@gmx.net>
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
	switch(signal)
	{
		/*
		 * "Normal" quit/interrupts which should be
		 *  catched.
		 */ 
		case SIGINT:
		case SIGQUIT:
		case SIGTERM:
		case SIGHUP:
			
			/*
			 * Only show up the rescue dialog of the file has been
			 *  changed at all.
			 */  
			if(po->file_changed)
			{
				ask_to_save_file();
			}

			exit(0);
			break;
		
		/*
		 * The "not nice" cases which are the reason for
		 *  our signal handler.
		 */  
		case SIGSEGV:
		case SIGILL:

			if(po->file_changed)
			{
				ask_to_save_file();
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
