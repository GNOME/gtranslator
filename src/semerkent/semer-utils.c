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

#include "semer-utils.h"

#include <time.h>

G_BEGIN_DECLS

/*
 * Creates and returns a freshly allocated TMX date-string.
 */
gchar *semer_utils_tmx_date_new()
{
	struct tm 	*timebox;
	time_t		now;

	gchar		*date=g_malloc(17);

	now=time(NULL);
	timebox=localtime(&now);

	strftime(date, 17, "%Y%m%dT%H%M%SZ", timebox);
	return date;
}

/*
 * Plain, good-old po files' date string.
 */
gchar *semer_utils_po_date_new()
{
	struct tm 	*timebox;
	time_t		now;

	gchar		*date=g_malloc(22);

	now=time(NULL);
	timebox=localtime(&now);

	strftime(date, 22, "%Y-%m-%d %H:%M%z", timebox);
	return date;
}

/*
 * Get an UMTF alike formatted date string.
 */
gchar *semer_utils_umtf_date_new()
{
	struct tm	*timebox;
	time_t		now;

	gchar		*date=g_malloc(20);

	now=time(NULL);
	timebox=localtime(&now);

	strftime(date, 20, "%Y-%m-%d %H:%M:%S", timebox);
	return date;
}

/*
 * Test the given filename for existence and regularity.
 */
gboolean semer_utils_test_file(const gchar *filename)
{
	g_return_val_if_fail(filename!=NULL, FALSE);

	if(g_file_test(filename, G_FILE_TEST_IS_REGULAR))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * Check the given directory.
 */
gboolean semer_utils_test_directory(const gchar *directoryname)
{
	g_return_val_if_fail(directoryname!=NULL, FALSE);

	if(g_file_test(directoryname, G_FILE_TEST_IS_DIR))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * Free our data safely.
 */
void semer_utils_free(gpointer freedata)
{
	if(freedata)
	{
		g_free(freedata);
	}
}

/*
 * Copy the data for the string.
 */
gchar *semer_utils_strcopy(const gchar *string)
{
	if(!string)
	{
		return NULL;
	}
	else
	{
		return (g_strdup(string));
	}
}

/*
 * Set the string argument safely -- fallback should be generally useful.
 */
void semer_utils_strset(gchar **string, const gchar *set_string, 
	const gchar *fallback_string)
{
	/*
	 * Free any already existing content.
	 */
	if(*string)
	{
		semer_utils_free(*string);
	}

	/*
	 * Set the string contents to the given string, it's fallback
	 *  string content or to NULL.
	 */
	if(set_string)
	{
		*string=g_strdup(set_string);
	}
	else
	{
		if(fallback_string)
		{
			*string=g_strdup(fallback_string);
		}
		else
		{
			*string=NULL;
		}
	}
}

G_END_DECLS
