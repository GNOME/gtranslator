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

#ifndef SEMER_ENTRY_H
#define SEMER_ENTRY_H 1

#include "semer-props.h"

G_BEGIN_DECLS

/*
 * The general SemerEntry which encapsulates all the 3 formats.
 */
typedef struct
{
	gchar		*original;
	gchar		*original_language;

	gchar		*translation;
	gchar		*translation_language;

	gchar		*description;

	SemerProps 	*props;
} SemerEntry;

#define SEMER_ENTRY(x) ((SemerEntry *) x)
#define IS_SEMER_ENTRY(x) (sizeof(x) == sizeof(SemerEntry *))

/*
 * Creation/deletion.
 */
SemerEntry *semer_entry_new(void);
void semer_entry_free(SemerEntry *entry);

/*
 * Construct a SemerEntry out of the given specs.
 *
 * Note: "props" can be NULL.
 */
SemerEntry *semer_entry_new_with_specs(SemerProps *props,
	const gchar *original, 
	const gchar *original_language,
	const gchar *translation, 
	const gchar *translation_language,
	const gchar *description);

/*
 * Return a copy of the SemerProps of the SemerEntry.
 */
SemerProps *semer_entry_copy_props(SemerEntry *entry);

/*
 * Copy the SemerEntry.
 */
SemerEntry *semer_entry_copy(SemerEntry *entry);

/*
 * Get/Set the original/translation entry for the SemerEntry.
 */
gchar *semer_entry_get_original(SemerEntry *entry);
gchar *semer_entry_get_translation(SemerEntry *entry);

void semer_entry_set_original(SemerEntry *entry, const gchar *original);
void semer_entry_set_translation(SemerEntry *entry, const gchar *translation);

G_END_DECLS

#endif /* SEMER_ENTRY_H */
