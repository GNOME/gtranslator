/*
 * (C) 2001-2003 	Fatih Demir <kabalak@kabalak.net>
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

#ifndef SEMERKENT_H
#define SEMERKENT_H 1

#include <glib.h>

G_BEGIN_DECLS

#include "semer-csv.h"
#include "semer-entry.h"
#include "semer-file.h"
#include "semer-header.h"
#include "semer-opentag.h"
#include "semer-props.h"
#include "semer-query.h"
#include "semer-tmx.h"
#include "semer-umtf.h"
#include "semer-utils.h"
#include "semer-xliff.h"
#include "semer-xml.h"

/*
 * The global initialize function which does some loading stuff.
 */
void semerkent_init(void); 

/*
 * ... and his antagonist; cleans up space etc.
 */
void semerkent_shutdown(void);

/*
 * Get the current status of Semerkent -- TRUE on already done init.
 */
gboolean semerkent_is_initialized(void);

G_END_DECLS

#endif /* SEMERKENT_H */
