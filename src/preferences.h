/*
 * (C) 2000-2001 	Fatih Demir <kabalak@kabalak.net>
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

#ifndef GTR_PREFERENCES_H
#define GTR_PREFERENCES_H 1

/*
 * gtranslator's preferences encapsulation of gnome_config or GConf.
 */

#include <glib.h>
#include <gdk/gdk.h>

/*
 * gtranslator_config is only available after a initialization.
 */
void 		gtranslator_config_init(void);

/*
 * Boolean methods:
 */
void 		gtranslator_config_set_bool(gchar *path, gboolean value);
gboolean 	gtranslator_config_get_bool(gchar *path);

/*
 * Integer methods:
 */
void 		gtranslator_config_set_int(gchar *path, gint value);
gint 		gtranslator_config_get_int(gchar *path);

/*
 * String methods:
 */
void 		gtranslator_config_set_string(gchar *path, gchar *value);
gchar 		*gtranslator_config_get_string(gchar *path);

/*
 * Gfloat methods:
 */
void		gtranslator_config_set_float(gchar *path, gfloat value);
gfloat 		gtranslator_config_get_float(gchar *path);

/*
 * Utility functions to store/recall the last run date of gtranslator.
 */
gchar 		*gtranslator_config_get_last_run_date(void);
void 		gtranslator_config_set_last_run_date(void);

#endif
