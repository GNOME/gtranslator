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

#ifndef GTR_HTMLIZER_H
#define GTR_HTMLIZER_H 1

#include "parse.h"

/*
 * Converts the given string to a raw html formatted string.
 */
gchar *gtranslator_string_htmlizer(gchar *textstring);

/*
 * Builds the html document out of the po file.
 */
void gtranslator_htmlizer(GtrPo *po, gchar *save_to);

#endif
