/*
 * (C) 2002 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTRANSLATOR_ZPMD_H
#define GTRANSLATOR_ZPMD_H 1

/*
 * We're defining the middle dot '·' here externally to avoid some massive i18n
 *  problems -- should also be more comprehensive if we're declaring it here
 *   externally.
 */

#include <glib.h>

/*
 * This is a middle dot (00B7 hexad.?) and represents the free space character
 *  if the gtranslator user wants to it so. It could be that the \xb7 character
 *   is already reserved in your language (locale) for something more important
 *    so that you will probably change the value of the translation of this
 *     string so, that the translated string doesn't conflict with your locale.
 */
#define GTRANSLATOR_MIDDLE_DOT (_("\xb7")[0])

/*
 * This is the fallback middle dot replacing char -- it shouldn't be needed to
 *  translate as it's pure ASCII or am I wrong here?
 */
#define GTRANSLATOR_MIDDLE_DOT_FALLBACK ('^')

#endif
