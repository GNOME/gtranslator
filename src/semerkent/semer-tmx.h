/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
 *
 * semerkent is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Library General Public License as published by the
 *   Free Software Foundation; either version 2 of the License, or (at your
 *    option) any later version.
 *    
 * semerkent is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *    License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * semerkent is being developed around gtranslator, so that any wishes or own
 *  requests should be mailed to me, as I will naturally keep semerkent very
 *   close to gtranslator. This shouldn't avoid any use of semerkent from other
 *    applications/packages.
 *
 */

#ifndef SEMER_TMX_H
#define SEMER_TMX_H 1

#include "semer-file.h"
#include "semer-header.h"
#include "semer-xml.h"

G_BEGIN_DECLS

/*
 * Parse the given TMX file into a SemerFile tree.
 */
SemerFile *semer_tmx_parse_file(const gchar *filename);

/*
 * Save the given SemerFile as a TMX file.
 */
void semer_tmx_save_file(SemerFile *file);

G_END_DECLS

#endif /* SEMER_TMX_H */
