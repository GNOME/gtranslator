/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *               2008  Igalia 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtr-profile.h"
#include "gtr-window.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_APPLICATION		(gtranslator_application_get_type ())
#define GTR_APPLICATION(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_APPLICATION, GtrApplication))
#define GTR_APPLICATION_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_APPLICATION, GtrApplicationClass))
#define GTR_IS_APPLICATION(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_APPLICATION))
#define GTR_IS_APPLICATION_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_APPLICATION))
#define GTR_APPLICATION_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_APPLIAPPLICATION, GtrApplicationClass))
#define GTR_APP			        (gtranslator_application_get_default())
/* Private structure type */
typedef struct _GtrApplicationPrivate GtrApplicationPrivate;

/*
 * Main object structure
 */
typedef struct _GtrApplication GtrApplication;

struct _GtrApplication
{
  GObject base_instance;

  /*< private > */
  GtrApplicationPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrApplicationClass GtrApplicationClass;

struct _GtrApplicationClass
{
  GObjectClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_application_get_type (void)
  G_GNUC_CONST;
     GtrApplication *gtranslator_application_get_default (void);

     void gtranslator_application_shutdown (GtrApplication * app);

     GList *gtranslator_application_get_views (GtrApplication * app,
					       gboolean original,
					       gboolean translated);

     GtrWindow
       *gtranslator_application_open_window (GtrApplication * app);

     GtrWindow
       *gtranslator_application_get_active_window (GtrApplication *
						   app);

     const GList *gtranslator_application_get_windows (GtrApplication
						       * app);

     GtrProfile
       *gtranslator_application_get_active_profile (GtrApplication *
						    app);

     void gtranslator_application_set_active_profile (GtrApplication *
						      app,
						      GtrProfile *
						      profile);

     GList *gtranslator_application_get_profiles (GtrApplication *
						  app);

     void gtranslator_application_set_profiles (GtrApplication * app,
						GList * profiles);

     void gtranslator_application_register_icon (GtrApplication * app,
						 const gchar * icon,
						 const gchar * stock_id);

     GObject
       *gtranslator_application_get_translation_memory (GtrApplication
							* app);

/* Non exported funcs */

     GObject
       *_gtranslator_application_get_toolbars_model (GtrApplication *
						     application);

     void _gtranslator_application_save_toolbars_model (GtrApplication
							* application);

     const gchar
       *_gtranslator_application_get_last_dir (GtrApplication * app);

     void _gtranslator_application_set_last_dir (GtrApplication * app,
						 const gchar * last_dir);

G_END_DECLS
#endif /* __APPLICATION_H__ */
