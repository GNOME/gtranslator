/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
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
 * The homepage for semerkent is: http://semerkent.sourceforge.net
 */

#ifndef SEMER_PROPS_H
#define SEMER_PROPS_H 1

#include "semer-xml.h"

G_BEGIN_DECLS

/*
 * SemerProps are neat to handle and cleanse every file level data.
 */
typedef struct
{
	gchar	*id;
	gchar	*changedate;
	
	gchar	*type;
	gchar	*client;
	gchar	*domain;
	
	gchar	*sourcefile;

	gchar	*translator_name;
	gchar	*translator_email;
} SemerProps;

/*
 * Two usage macros for easier handling.
 */
#define IS_SEMER_PROPS(x) (sizeof(x) == sizeof(SemerProps *))
#define SEMER_PROPS(x) ((SemerProps *) x)

/*
 * Creation/deletion methods:
 */
SemerProps *semer_props_new(void);
void semer_props_free(SemerProps *props);

/*
 * "Advanced" props generation -- NULL arguments are "copied"
 *   over as NULL values.
 */
SemerProps *semer_props_new_with_specs(const gchar *id,
	const gchar *changedate,
	const gchar *type,
	const gchar *client,
	const gchar *domain,
	const gchar *sourcefile,
	const gchar *translator_name,
	const gchar *translator_email);

/*
 * Gets/sets the translator name and email -- the get function returns a
 *  usual formed string like "Fatih Demir <kabalak@gtranslator.org>".
 */
gchar *semer_props_get_translator(SemerProps *props);

void semer_props_set_translator(SemerProps *props,
	const gchar *translator_name,
	const gchar *translator_email);

/*
 * Copy the props.
 */
SemerProps *semer_props_copy(SemerProps *props);

G_END_DECLS

#endif /* SEMER_PROPS_H */
