/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*	future development in direction to better IQ-Test rankings!
*
**/

#ifndef PREFERENCES_H
#define PREFERENCES_H 1

#include <libgtranslator/parse-db.h>
#include <time.h>

/**
* Include the GConf headers if they're present.
**/
#ifdef GCONF_IS_PRESENT
#include <gconf/gconf-client.h>
#include <gconf/gconf-error.h>
#endif

/**
* This is an encapsulation for the configuration-stuff used both in
*  gtranslatord and gtranslator.
**/

/**
* Some define wrappers for the gfloat/gdouble stuff.
**/
#define gtranslator_config_set_double(path, value) \
	gtranslator_config_set_float(path, (gfloat) value)
#define gtranslator_config_get_double(path) \
	(gdouble) gtranslator_config_get_float(path)

/**
* libgtranslator's preferences routines have to be prepended by
*  an initialization and a finalization.
**/
void gtranslator_config_init(void);
void gtranslator_config_close(void);

/**
* These routines are good for setting/getting boolean values.
**/
void gtranslator_config_set_bool(gchar *path, gboolean value);
gboolean gtranslator_config_get_bool(gchar *path);

/**
* These routines are used for setting/getting integer values.
**/
void gtranslator_config_set_int(gchar *path, gint value);
gint gtranslator_config_get_int(gchar *path);

/**
* And these routines are used for setting/getting strings.
**/
void gtranslator_config_set_string(gchar *path, gchar *value);
gchar *gtranslator_config_get_string(gchar *path);

/**
* Now the ones for the gfloat values.
**/
void gtranslator_config_set_float(gchar *path, gfloat value);
gfloat gtranslator_config_get_float(gchar *path);

/**
* These functions store/recall the last run date of gtranslator.
**/
gchar *gtranslator_config_get_last_run_date(void);
void gtranslator_config_set_last_run_date(void);

#endif
