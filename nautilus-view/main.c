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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nautilus-gtranslator-view.h"
#include "gnome-regex.h"
#include "parse.h"
#include <libnautilus/nautilus-view-standard-main.h>

#define FACTORY_IID     \
"OAFIID:Nautilus_Gtranslator_View_Factory:a924c35f-4f67-453b-b862-8ccc2d920322"
#define VIEW_IID        \
"OAFIID:Nautilus_Gtranslator_View:569f901f-e30c-46cf-b17e-28a6087701ba"

#define EXECUTABLE_NAME "nautilus-gtranslator-view"

#define GET_TYPE_FUNCTION nautilus_gtranslator_view_get_type

int
main (int argc, char *argv[])
{
	rxc=gnome_regex_cache_new_with_size(20);
	
	bindtextdomain(PACKAGE, GNOMELOCALEDIR);
	textdomain(PACKAGE);
	
	return nautilus_view_standard_main (EXECUTABLE_NAME, VERSION,
					    argc, argv,
					    FACTORY_IID, VIEW_IID,
					    nautilus_view_create_from_get_type_function,
					    GET_TYPE_FUNCTION);
}
