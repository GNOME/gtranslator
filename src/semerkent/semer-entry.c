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

#include "semer-entry.h"
#include "semer-utils.h"

G_BEGIN_DECLS

/*
 * Create/delete the SemerEntry.
 */
SemerEntry *semer_entry_new(void)
{
	SemerEntry *entry=g_new0(SemerEntry, 1);

	entry->props=semer_props_new();
	entry->original=NULL;
	entry->original_language=NULL;
	entry->translation=NULL;
	entry->translation_language=NULL;
	entry->description=NULL;

	return entry;
}

/*
 * Create a new SemerEntry out of the given argument specs.
 */
SemerEntry *semer_entry_new_with_specs(SemerProps *props,
	const gchar *original, const gchar *original_language,
	const gchar *translation, const gchar *translation_language,
	const gchar *description)
{
	SemerEntry *entry=g_new0(SemerEntry, 1);

	/*
	 * Original, translation and translation language had to be supplied.
	 */
	g_return_val_if_fail(original!=NULL, NULL);
	g_return_val_if_fail(translation!=NULL, NULL);
	g_return_val_if_fail(translation_language!=NULL, NULL);

	/*
	 * Copy the SemerProps if possible.
	 */
	if(!props)
	{
		entry->props=semer_props_new();
	}
	else
	{
		entry->props=semer_props_copy(SEMER_PROPS(props));
	}

	/*
	 * Set the original language for the entry if given; normally
	 *  it's possible to set the original language flag via the "srclang"
	 *   property of the SemerHeader.
	 */
	semer_utils_strset(&SEMER_ENTRY(entry)->original_language, 
		original_language, NULL); 
	
	semer_utils_strset(&SEMER_ENTRY(entry)->description, 
		description, NULL);

	/*
	 * Copy the strings from the arguments into the SemerEntry.
	 */
	entry->original=g_strdup(original);
	entry->translation=g_strdup(translation);
	entry->translation_language=g_strdup(translation_language);

	return entry;
}

/*
 * Return a copy of the SemerProps of the SemerEntry.
 */
SemerProps *semer_entry_copy_props(SemerEntry *entry)
{
	g_return_val_if_fail(SEMER_ENTRY(entry)!=NULL, NULL);

	return (semer_props_copy(SEMER_ENTRY(entry)->props));
}

/*
 * Copy function for SemerEntry's.
 */
SemerEntry *semer_entry_copy(SemerEntry *entry)
{
	SemerEntry *copyentry=semer_entry_new();
	
	g_return_val_if_fail(SEMER_ENTRY(entry)!=NULL, NULL);

	copyentry->original=semer_utils_strcopy(SEMER_ENTRY(entry)->original);
	copyentry->original_language=semer_utils_strcopy(
		SEMER_ENTRY(entry)->original_language);
	
	copyentry->translation=semer_utils_strcopy(
		SEMER_ENTRY(entry)->translation);
	copyentry->translation_language=semer_utils_strcopy(
		SEMER_ENTRY(entry)->translation_language);

	copyentry->description=semer_utils_strcopy(
		SEMER_ENTRY(entry)->description);

	copyentry->props=semer_props_copy(SEMER_ENTRY(entry)->props);

	return copyentry;
}

void semer_entry_free(SemerEntry *entry)
{
	g_return_if_fail(SEMER_ENTRY(entry)!=NULL);

	semer_utils_free(SEMER_ENTRY(entry)->original);
	semer_utils_free(SEMER_ENTRY(entry)->original_language);
	semer_utils_free(SEMER_ENTRY(entry)->translation);
	semer_utils_free(SEMER_ENTRY(entry)->translation_language);
	semer_utils_free(SEMER_ENTRY(entry)->description);
	
	semer_props_free(SEMER_ENTRY(entry)->props);
	
	semer_utils_free(SEMER_ENTRY(entry));
}

/*
 * Return the original/translation strings of a SemerEntry:
 */
gchar *semer_entry_get_original(SemerEntry *entry)
{
	g_return_val_if_fail(entry!=NULL, NULL);

	if(SEMER_ENTRY(entry)->original)
	{
		return g_strdup(SEMER_ENTRY(entry)->original);
	}
	else
	{
		return NULL;
	}
}

gchar *semer_entry_get_translation(SemerEntry *entry)
{
	g_return_val_if_fail(entry!=NULL, NULL);

	if(SEMER_ENTRY(entry)->translation)
	{
		return g_strdup(SEMER_ENTRY(entry)->translation);
	}
	else
	{
		return NULL;
	}
}

/*
 * Set up the original/translation parts of a SemerEntry -- safely .-)
 */
void semer_entry_set_original(SemerEntry *entry, const gchar *original)
{
	g_return_if_fail(SEMER_ENTRY(entry)!=NULL);
	g_return_if_fail(original!=NULL);

	semer_utils_strset(&SEMER_ENTRY(entry)->original, original, NULL);
}

void semer_entry_set_translation(SemerEntry *entry, const gchar *translation)
{
	g_return_if_fail(SEMER_ENTRY(entry)!=NULL);
	g_return_if_fail(translation!=NULL);

	semer_utils_strset(&SEMER_ENTRY(entry)->translation, translation, NULL);
}

G_END_DECLS
