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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "formats.h"
#include "messages.h"
#include "nautilus-string.h"

/*
 * Checks whether the formats are right or wrong .-)
 */
gboolean gtranslator_formats_check(GtrMsg *message)
{
	g_return_val_if_fail(GTR_MSG(message)!=NULL, FALSE);
	g_return_val_if_fail(GTR_MSG(message)->msgid!=NULL, FALSE);

	return TRUE;
}
