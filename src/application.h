/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "profile.h"
#include "window.h"
#include "dialogs/preferences-dialog.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_APPLICATION		(gtranslator_application_get_type ())
#define GTR_APPLICATION(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_APPLICATION, GtranslatorApplication))
#define GTR_APPLICATION_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_APPLICATION, GtranslatorApplicationClass))
#define GTR_IS_APPLICATION(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_APPLICATION))
#define GTR_IS_APPLICATION_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_APPLICATION))
#define GTR_APPLICATION_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_APPLIAPPLICATION, GtranslatorApplicationClass))

#define GTR_APP			        (gtranslator_application_get_default())

/* Private structure type */
typedef struct _GtranslatorApplicationPrivate	GtranslatorApplicationPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorApplication		GtranslatorApplication;

struct _GtranslatorApplication
{
	GObject base_instance;
	
	/*< private > */
	GtranslatorApplicationPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorApplicationClass	GtranslatorApplicationClass;

struct _GtranslatorApplicationClass
{
	GObjectClass parent_class;
};

/*
 * Public methods
 */
GType		         gtranslator_application_get_type	   (void) G_GNUC_CONST;
GtranslatorApplication	*gtranslator_application_get_default	           (void);

void              gtranslator_application_shutdown            (GtranslatorApplication *app);

GList            *gtranslator_application_get_views           (GtranslatorApplication *app,
							       gboolean original,
							       gboolean translated);

GtranslatorWindow *gtranslator_application_open_window (GtranslatorApplication *app);

GtranslatorWindow *gtranslator_application_get_active_window  (GtranslatorApplication * app);

const GList       *gtranslator_application_get_windows (GtranslatorApplication *app);

GtranslatorProfile *gtranslator_application_get_active_profile (GtranslatorApplication *app);

void              gtranslator_application_set_active_profile (GtranslatorApplication *app,
							      GtranslatorProfile *profile);

GList             *gtranslator_application_get_profiles (GtranslatorApplication *app);

void              gtranslator_application_set_profiles (GtranslatorApplication *app,
							GList *profiles);

GtranslatorPreferencesDialog *
gtranslator_application_get_preferences_dialog (GtranslatorApplication *app);

void
gtranslator_application_set_preferences_dialog (GtranslatorApplication *app,
						GtranslatorPreferencesDialog *dlg);

void              gtranslator_application_register_icon (GtranslatorApplication *app,
							 const gchar *icon,
				       			 const gchar *stock_id);

/* Non exported funcs */

GObject          *_gtranslator_application_get_toolbars_model  (GtranslatorApplication   *application);

void              _gtranslator_application_save_toolbars_model (GtranslatorApplication   *application);

const gchar *     _gtranslator_application_get_last_dir        (GtranslatorApplication *app);

void              _gtranslator_application_set_last_dir        (GtranslatorApplication *app,
							        const gchar *last_dir);

G_END_DECLS

#endif /* __APPLICATION_H__ */
