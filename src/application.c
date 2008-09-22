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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "application.h"
#include "utils.h"
#include "window.h"
#include "egg-toolbars-model.h"
#include "dialogs/preferences-dialog.h"
#include "./translation-memory/translation-memory.h"
#include "./translation-memory/berkeley/berkeley.h"
  


#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_APPLICATION_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_APPLICATION,     \
					 GtranslatorApplicationPrivate))

G_DEFINE_TYPE(GtranslatorApplication, gtranslator_application, G_TYPE_OBJECT)

struct _GtranslatorApplicationPrivate
{
	GList *windows;
	GtranslatorWindow *active_window;

        GList *profiles;
        GtranslatorProfile *active_profile;
	
        GtranslatorPreferencesDialog *preferences_dialog;

	gchar *toolbars_file;
	EggToolbarsModel *toolbars_model;
	
	GtkIconFactory *icon_factory;

	gchar *last_dir;
	
	GtranslatorTranslationMemory *tm;
};

static gchar *
get_accel_file (void)
{
	gchar *config;

	config = gtranslator_utils_get_user_config_dir ();

	if (config != NULL)
	{
		return g_build_filename (config,
					 "gtranslator-accels",
					 NULL);
		g_free (config);
	}

	return NULL;
}

static void
load_accels (void)
{
	gchar *filename;

	filename = get_accel_file ();
	if (filename != NULL)
	{
		gtk_accel_map_load (filename);
		g_free (filename);
	}
}

static void
save_accels (void)
{
	gchar *filename;

	filename = get_accel_file ();
	if (filename != NULL)
	{
		gtk_accel_map_save (filename);
		g_free (filename);
	}
}

static gboolean
on_window_delete_event_cb(GtranslatorWindow *window,
			  GdkEvent *event,
			  GtranslatorApplication *app)
{
	gtranslator_file_quit(NULL, window);
	return TRUE;
}

static void
on_window_destroy_cb(GtranslatorWindow *window,
		     GtranslatorApplication *app)
{
	save_accels ();
	//if(app->priv->active_window == NULL)
		g_object_unref(app);
}

static void
gtranslator_application_init (GtranslatorApplication *application)
{
	gchar *gtranslator_folder;
	gchar *filename;

	GtranslatorApplicationPrivate * priv;
	
	application->priv = GTR_APPLICATION_GET_PRIVATE (application);
	priv = application->priv;
	
	priv->windows = NULL;
	priv->last_dir = NULL;
	
	/*
	 * Creating config folder
	 */
	gtranslator_folder = gtranslator_utils_get_user_config_dir ();

	if (!g_file_test (gtranslator_folder, G_FILE_TEST_IS_DIR))
	{
		GFile *file;
		GError *error = NULL;
		
		file = g_file_new_for_path (gtranslator_folder);
		
		if (g_file_test (gtranslator_folder, G_FILE_TEST_IS_REGULAR))
		{
			if (!g_file_delete (file, NULL, &error))
			{
				g_warning ("There was an error deleting the "
					   "old gtranslator file: %s", error->message);
				g_error_free (error);
				g_object_unref (file);
				g_free (gtranslator_folder);
				gtranslator_application_shutdown (application);
			}
		}
		
		if (!g_file_make_directory (file, NULL, &error))
		{
			g_warning ("There was an error making the gtranslator config directory: %s",
				   error->message);
					   
			g_error_free (error);
			g_object_unref (file);
			g_free (gtranslator_folder);
			gtranslator_application_shutdown (application);
		}
		
		g_object_unref (file);
	}

	priv->toolbars_model = egg_toolbars_model_new ();
				     
	priv->toolbars_file = g_build_filename (gtranslator_folder,
						"gtr-toolbar.xml",
						NULL);

	filename = g_build_filename (DATADIR,
				     "gtr-toolbar.xml",
				     NULL);

	g_free (gtranslator_folder);
	
	egg_toolbars_model_load_names (priv->toolbars_model,
				       filename);

	if (!egg_toolbars_model_load_toolbars (priv->toolbars_model,
					       priv->toolbars_file)) 
	{
		egg_toolbars_model_load_toolbars (priv->toolbars_model,
						  filename);
	}
	
	g_free (filename);

	egg_toolbars_model_set_flags (priv->toolbars_model, 0,
				      EGG_TB_MODEL_NOT_REMOVABLE);	
	
	load_accels ();
	
	/* Create Icon factory */
	application->priv->icon_factory = gtk_icon_factory_new ();
	gtk_icon_factory_add_default (application->priv->icon_factory);
	
	/* Creating translation memory */
	application->priv->tm = GTR_TRANSLATION_MEMORY (gtranslator_berkeley_new ());
	gtranslator_translation_memory_set_max_omits (application->priv->tm,
 						      gtranslator_prefs_manager_get_missing_words ());
 	gtranslator_translation_memory_set_max_delta (application->priv->tm,
						      gtranslator_prefs_manager_get_sentence_length ());
}


static void
gtranslator_application_finalize (GObject *object)
{
	GtranslatorApplication *app = GTR_APPLICATION (object);
	
	if (app->priv->icon_factory)
		g_object_unref (app->priv->icon_factory);

	g_free (app->priv->last_dir);
	
	if (app->priv->tm)
		g_object_unref (app->priv->tm);

	G_OBJECT_CLASS (gtranslator_application_parent_class)->finalize (object);
}

static void
gtranslator_application_class_init (GtranslatorApplicationClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorApplicationPrivate));
	
	object_class->finalize = gtranslator_application_finalize;
}

static void
app_weak_notify (gpointer data,
                 GObject *where_the_app_was)
{
        gtk_main_quit ();
}

/**
 * gtranslator_application_get_default:
 * 
 * Returns the default instance of the application.
 * 
 * Returns: the default instance of the application.
 */
GtranslatorApplication *
gtranslator_application_get_default (void)
{
	static GtranslatorApplication *instance = NULL;
	
	if(!instance)
	{
		instance = GTR_APPLICATION(g_object_new (GTR_TYPE_APPLICATION, NULL));
		
		g_object_weak_ref (G_OBJECT (instance),
				   app_weak_notify,
				   NULL);
	}
	return instance;
}

/**
 * gtranslator_application_open_window:
 * @app: a #GtranslatorApplication
 *
 * Creates a new #GtranslatorWindow and shows it.
 * 
 * Returns: the #GtranslatorWindow to be opened
 */
GtranslatorWindow *
gtranslator_application_open_window (GtranslatorApplication *app)
{
	GtranslatorWindow *window;
	GdkWindowState state;
	gint w,h;

	app->priv->active_window = window = GTR_WINDOW(g_object_new(GTR_TYPE_WINDOW, NULL));
	
	state = gtranslator_prefs_manager_get_window_state();
	
	if((state & GDK_WINDOW_STATE_MAXIMIZED) != 0)
	{
		gtranslator_prefs_manager_get_default_window_size(&w, &h);
		gtk_window_set_default_size (GTK_WINDOW (window), w, h);
                gtk_window_maximize (GTK_WINDOW (window));
	}
	else
	{
		gtranslator_prefs_manager_get_window_size(&w, &h);
		gtk_window_set_default_size (GTK_WINDOW (window), w, h);
                gtk_window_unmaximize (GTK_WINDOW (window));
	}
	
	g_signal_connect(window, "delete-event",
			 G_CALLBACK(on_window_delete_event_cb), GTR_APP);
	
	g_signal_connect(window, "destroy",
			 G_CALLBACK(on_window_destroy_cb), GTR_APP);

	gtk_widget_show(GTK_WIDGET(window));
	
	return window;
}
				     
/**
 * _gtranslator_application_get_toolbars_model:
 * @application: a #GtranslatorApplication
 * 
 * Returns the toolbar model.
 * 
 * Retuns: the toolbar model.
 */
GObject *
_gtranslator_application_get_toolbars_model (GtranslatorApplication *application)
{
	return G_OBJECT (application->priv->toolbars_model);
}

/**
 * _gtranslator_application_save_toolbars_model:
 * @application: a #GtranslatorApplication
 * 
 * Saves the toolbar model.
 */
void
_gtranslator_application_save_toolbars_model (GtranslatorApplication *application)
{
        egg_toolbars_model_save_toolbars (application->priv->toolbars_model,
			 	          application->priv->toolbars_file, "1.0");
}

/**
 * gtranslator_application_shutdown:
 * @app: a #GtranslatorApplication
 * 
 * Shutdowns the application.
 */
void
gtranslator_application_shutdown(GtranslatorApplication *app)
{
	if(app->priv->toolbars_model)
	{
		g_object_unref(app->priv->toolbars_model);
		g_free(app->priv->toolbars_file);
		app->priv->toolbars_model = NULL;
		app->priv->toolbars_file = NULL;
	}
	
	g_object_unref(app);
}

/**
 * gtranslator_application_get_views:
 * @app: the #GtranslationApplication
 * @original: TRUE if you want original TextViews.
 * @translated: TRUE if you want translated TextViews.
 *
 * Returns all the views currently present in #GtranslationApplication.
 *
 * Return value: a newly allocated list of #GtranslationApplication objects
 */
GList *
gtranslator_application_get_views (GtranslatorApplication *app,
				   gboolean original,
				   gboolean translated)
{
	GList *res = NULL;

	g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

	res = g_list_concat (res,
			     gtranslator_window_get_all_views (GTR_WINDOW (app->priv->active_window),
							       original,
							       translated));
	
	return res;
}

/**
 * gtranslator_application_get_active_window:
 * @app: a #GtranslatorApplication
 * 
 * Return value: the active #GtranslatorWindow
 **/
GtranslatorWindow *
gtranslator_application_get_active_window(GtranslatorApplication * app)
{
	return GTR_WINDOW(app->priv->active_window);
}

/**
 * gtranslator_application_get_windows:
 * @app: a #GtranslatorApplication
 * 
 * Return value: a list of all opened windows.
 **/
const GList *
gtranslator_application_get_windows (GtranslatorApplication *app)
{
	g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

	if(!app->priv->windows)
		app->priv->windows = g_list_prepend(app->priv->windows, app->priv->active_window);
		
	return app->priv->windows;
}

/**
 * gtranslator_application_get_active_profile:
 * @app: a #GtranslatorApplication
 * 
 * Return value: the active #GtranslatorProfile
 **/
GtranslatorProfile *
gtranslator_application_get_active_profile (GtranslatorApplication *app)
{
	return app->priv->active_profile;
}

/**
 * gtranslator_application_set_profiles:
 * @app: a #GtranslatorApplication
 * @profiles: a #GList
 *
 **/
void
gtranslator_application_set_active_profile (GtranslatorApplication *app,
					    GtranslatorProfile *profile) {
  app->priv->active_profile = profile;
}

/**
 * gtranslator_application_get_profiles:
 * @app: a #GtranslatorApplication
 * 
 * Return value: a list of all profiles.
 **/
GList *
gtranslator_application_get_profiles (GtranslatorApplication *app)
{
	g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

	return app->priv->profiles;
}

/**
 * gtranslator_application_set_profiles:
 * @app: a #GtranslatorApplication
 * @profiles: a #GList
 *
 **/
void
gtranslator_application_set_profiles (GtranslatorApplication *app, 
				      GList *profiles) {
   app->priv->profiles = profiles;   
}

GtranslatorPreferencesDialog *
gtranslator_application_get_preferences_dialog (GtranslatorApplication *app)
{
  return app->priv->preferences_dialog;
}

void
gtranslator_application_set_preferences_dialog (GtranslatorApplication *app,
						GtranslatorPreferencesDialog *dlg)
{
  app->priv->preferences_dialog = dlg;
}


/**
 * gtranslator_application_register_icon:
 * @app: a #GtranslatorApplication
 * @icon: the name of the icon
 * @stock_id: the stock id for the new icon
 * 
 * Registers a new @icon with the @stock_id.
 */
void
gtranslator_application_register_icon (GtranslatorApplication *app,
				       const gchar *icon,
				       const gchar *stock_id)
{
	GtkIconSet *icon_set;
	GtkIconSource *	icon_source = gtk_icon_source_new ();
	gchar *path;
	
	path = g_build_filename (PIXMAPSDIR,
				 icon,
				 NULL);
	
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file (path, NULL);
	if (pixbuf)
	{
		icon_set = gtk_icon_set_new_from_pixbuf (pixbuf);
		gtk_icon_factory_add (app->priv->icon_factory, stock_id, icon_set);
		g_object_unref (pixbuf);
	}
	
	g_free (path);
	gtk_icon_source_free (icon_source);
}

/**
 * gtranslator_application_get_last_dir:
 * @app: a #GtranslatorApplication
 *
 * Return value: the last dir where a file was opened in the GtkFileChooser
 */
const gchar *
_gtranslator_application_get_last_dir (GtranslatorApplication *app)
{
	g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);

	return app->priv->last_dir;
}

/**
 * gtranslator_application_set_last_dir:
 * @app: a #GtranslatorApplication
 * @last_dir: the path of the last directory where a file was opened in the
 * GtkFileChooser.
 */
void
_gtranslator_application_set_last_dir (GtranslatorApplication *app,
				       const gchar *last_dir)
{
	g_return_if_fail (GTR_IS_APPLICATION (app));

	app->priv->last_dir = g_strdup (last_dir);
}

GObject *
gtranslator_application_get_translation_memory (GtranslatorApplication *app)
{
	g_return_val_if_fail (GTR_IS_APPLICATION (app), NULL);
	
	return G_OBJECT (app->priv->tm);
}
