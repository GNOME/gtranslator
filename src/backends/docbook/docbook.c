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

/*
 * Docbook sgml file backend -- converts the paras -> po files and makes them
 *  possible to be translated.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../../nautilus-string.h"
#include "../../parse.h"

const gchar *gtranslator_backend_init(const gchar *filename)
{
	g_message("Write docbook -> po converting functions..");
	return filename;
}

void gtranslator_backend_close(void)
{
	g_message("Closing docbook backend...");
}
