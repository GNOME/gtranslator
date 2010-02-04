/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
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
 *   Pablo Sanxiao <psanxiao@gmail.com> 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-application.h"
#include "gtr-dirs.h"
#include "gtr-profile-dialog.h"
#include "gtr-preferences-dialog.h"
#include "gtr-profile.h"
#include "gtr-utils.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <libxml/tree.h>


#define GTR_PROFILE_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_PROFILE_DIALOG,     \
						 	GtrProfileDialogPrivate))

G_DEFINE_TYPE (GtrProfileDialog, gtranslator_profile_dialog,
	       GTK_TYPE_DIALOG)

struct _GtrProfileDialogPrivate
{
  GtkWidget *main_box;

  GtkWidget *profile_name_entry;

  GtkWidget *author_name_entry;
  GtkWidget *author_email_entry;

  GtkWidget *language_name_entry;
  GtkWidget *language_code_entry;
  GtkWidget *charset_entry;
  GtkWidget *encoding_entry;
  GtkWidget *language_email_entry;
  GtkWidget *plurals_forms_entry;

  GtrPreferencesDialog *prefs_dlg;

  GtrProfile *new_profile;
  GtrProfile *old_profile;
};

static void
gtranslator_profile_dialog_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtranslator_profile_dialog_parent_class)->finalize (object);
}

static void
gtranslator_profile_dialog_class_init (GtrProfileDialogClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrProfileDialogPrivate));

  object_class->finalize = gtranslator_profile_dialog_finalize;
}

/***************PROFILES****************/
static void
profile_name_entry_changed (GObject * gobject,
			    GParamSpec * arg1,
			    GtrProfileDialog * profile_dlg)
{
  const gchar *text;

  /*g_return_if_fail (GTK_ENTRY (gobject) == GTK_ENTRY (dlg->priv->profile_name_entry)); */

  text = gtk_entry_get_text (GTK_ENTRY (gobject));

  if (text)
    gtranslator_profile_set_name (profile_dlg->priv->new_profile,
				  (gchar *) text);
}

static void
author_name_entry_changed (GObject * gobject,
			   GParamSpec * arg1,
			   GtrProfileDialog * profile_dlg)
{
  const gchar *text;

  /*g_return_if_fail (GTK_ENTRY (gobject) == GTK_ENTRY (dlg->priv->author_name_entry)); */

  text = gtk_entry_get_text (GTK_ENTRY (gobject));

  if (text)
    gtranslator_profile_set_author_name (profile_dlg->priv->new_profile,
					 (gchar *) text);
}

static void
author_email_entry_changed (GObject * gobject,
			    GParamSpec * arg1,
			    GtrProfileDialog * profile_dlg)
{
  const gchar *text;

  /*g_return_if_fail (GTK_ENTRY (gobject) == GTK_ENTRY (dlg->priv->author_email_entry)); */

  text = gtk_entry_get_text (GTK_ENTRY (gobject));

  if (text)
    gtranslator_profile_set_author_email (profile_dlg->priv->new_profile,
					  (gchar *) text);
}

static void
language_name_entry_changed (GObject * gobject,
			     GParamSpec * arg1,
			     GtrProfileDialog * profile_dlg)
{
  const gchar *text;

  /*g_return_if_fail (GTK_ENTRY (gobject) == GTK_ENTRY (dlg->priv->profile_name_entry)); */

  text = gtk_entry_get_text (GTK_ENTRY (gobject));

  if (text)
    gtranslator_profile_set_language_name (profile_dlg->priv->new_profile,
					   (gchar *) text);
}

static void
language_code_entry_changed (GObject * gobject,
			     GParamSpec * arg1,
			     GtrProfileDialog * profile_dlg)
{
  const gchar *text;

  /*g_return_if_fail (GTK_ENTRY (gobject) == GTK_ENTRY (dlg->priv->profile_name_entry)); */

  text = gtk_entry_get_text (GTK_ENTRY (gobject));

  if (text)
    gtranslator_profile_set_language_code (profile_dlg->priv->new_profile,
					   (gchar *) text);
}

static void
charset_entry_changed (GObject * gobject,
		       GParamSpec * arg1,
		       GtrProfileDialog * profile_dlg)
{
  const gchar *text;

  /*g_return_if_fail (GTK_ENTRY (gobject) == GTK_ENTRY (dlg->priv->profile_name_entry)); */

  text = gtk_entry_get_text (GTK_ENTRY (gobject));

  if (text)
    gtranslator_profile_set_charset (profile_dlg->priv->new_profile,
				     (gchar *) text);
}

static void
encoding_entry_changed (GObject * gobject,
			GParamSpec * arg1,
			GtrProfileDialog * profile_dlg)
{
  const gchar *text;

  /*g_return_if_fail (GTK_ENTRY (gobject) == GTK_ENTRY (dlg->priv->profile_name_entry)); */

  text = gtk_entry_get_text (GTK_ENTRY (gobject));

  if (text)
    gtranslator_profile_set_encoding (profile_dlg->priv->new_profile,
				      (gchar *) text);
}

static void
group_email_entry_changed (GObject * gobject,
			   GParamSpec * arg1,
			   GtrProfileDialog * profile_dlg)
{
  const gchar *text;

  /*g_return_if_fail (GTK_ENTRY (gobject) == GTK_ENTRY (dlg->priv->profile_name_entry)); */

  text = gtk_entry_get_text (GTK_ENTRY (gobject));

  if (text)
    gtranslator_profile_set_group_email (profile_dlg->priv->new_profile,
					 (gchar *) text);
}

static void
plurals_entry_changed (GObject * gobject,
		       GParamSpec * arg1,
		       GtrProfileDialog * profile_dlg)
{
  const gchar *text;

  /*g_return_if_fail (GTK_ENTRY (gobject) == GTK_ENTRY (dlg->priv->profile_name_entry)); */

  text = gtk_entry_get_text (GTK_ENTRY (gobject));

  if (text)
    gtranslator_profile_set_plurals (profile_dlg->priv->new_profile,
				     (gchar *) text);
}

static void
save_new_profile (GtkWidget * widget, GtrProfileDialog * profile_dlg)
{
  GList *profiles_list = NULL, *l = NULL;
  GtrPreferencesDialog *dlg;
  gboolean profile_exists;
  GtkWidget *treeview;
  GtkTreeModel *model;
  profile_exists = FALSE;

  profiles_list = gtranslator_application_get_profiles (GTR_APP);

  for (l = profiles_list; l; l = l->next)
    {
      GtrProfile *profile_in_list;
      profile_in_list = (GtrProfile *) l->data;
      if (!strcmp (gtranslator_profile_get_name (profile_in_list),
		   gtranslator_profile_get_name (profile_dlg->priv->
						 new_profile)))
	profile_exists = TRUE;
    }

  if (profile_exists)
    {
      GtkWidget *dialog;
      dialog =
	gtk_message_dialog_new (GTK_WINDOW
				(gtk_widget_get_ancestor
				 (widget, GTK_TYPE_DIALOG)), GTK_DIALOG_MODAL,
				GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, NULL);

      gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog),
				     _
				     ("<span weight=\"bold\" size=\"large\">The profile name already exists</span>"));

      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
    }
  else
    {
      /*
       * Add new profile to profiles list in memory
       */
      if (profiles_list == NULL)
	{
	  GList *initial_list = NULL;
	  initial_list =
	    g_list_append (profiles_list, profile_dlg->priv->new_profile);
	  gtranslator_application_set_active_profile (GTR_APP,
						      profile_dlg->priv->
						      new_profile);
	  gtranslator_application_set_profiles (GTR_APP, initial_list);
	}
      else
	{
	  profiles_list =
	    g_list_append (profiles_list, profile_dlg->priv->new_profile);
	}
    }

  /*
   *Reload the preferences dialog
   */
  dlg = profile_dlg->priv->prefs_dlg;

  treeview = gtranslator_preferences_dialog_get_treeview (dlg);

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
  g_return_if_fail (GTK_IS_LIST_STORE (model));

  gtranslator_preferences_fill_profile_treeview (dlg, model);

  gtk_widget_destroy (gtk_widget_get_ancestor (widget, GTK_TYPE_DIALOG));
}

static void
save_modified_profile (GtkWidget * widget,
		       GtrProfileDialog * profile_dlg)
{
  GtkWidget *treeview;
  GtkTreeModel *model;

  g_return_if_fail (GTR_IS_PREFERENCES_DIALOG (profile_dlg->priv->prefs_dlg));

  treeview =
    gtranslator_preferences_dialog_get_treeview (profile_dlg->priv->
						 prefs_dlg);

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
  g_return_if_fail (GTK_IS_LIST_STORE (model));

  gtranslator_preferences_fill_profile_treeview (profile_dlg->priv->prefs_dlg,
						 model);
  gtk_widget_destroy (gtk_widget_get_ancestor (widget, GTK_TYPE_DIALOG));
}

static void
destroy_without_changes (GtkWidget * widget, GtrProfileDialog * dlg)
{
  gtranslator_profile_set_name (dlg->priv->new_profile,
				gtranslator_profile_get_name (dlg->priv->
							      old_profile));
  gtranslator_profile_set_author_name (dlg->priv->new_profile,
				       gtranslator_profile_get_author_name
				       (dlg->priv->old_profile));
  gtranslator_profile_set_author_email (dlg->priv->new_profile,
					gtranslator_profile_get_author_email
					(dlg->priv->old_profile));
  gtranslator_profile_set_language_name (dlg->priv->new_profile,
					 gtranslator_profile_get_language_name
					 (dlg->priv->old_profile));
  gtranslator_profile_set_language_code (dlg->priv->new_profile,
					 gtranslator_profile_get_language_code
					 (dlg->priv->old_profile));
  gtranslator_profile_set_charset (dlg->priv->new_profile,
				   gtranslator_profile_get_charset (dlg->
								    priv->
								    old_profile));
  gtranslator_profile_set_encoding (dlg->priv->new_profile,
				    gtranslator_profile_get_encoding (dlg->
								      priv->
								      old_profile));
  gtranslator_profile_set_group_email (dlg->priv->new_profile,
				       gtranslator_profile_get_group_email
				       (dlg->priv->old_profile));
  gtranslator_profile_set_plurals (dlg->priv->new_profile,
				   gtranslator_profile_get_plurals (dlg->
								    priv->
								    old_profile));

  gtk_widget_destroy (gtk_widget_get_ancestor (widget, GTK_TYPE_DIALOG));
}

static void
gtranslator_profile_dialog_init (GtrProfileDialog * dlg)
{
  gboolean ret;
  GtkWidget *error_widget;
  gchar *path;
  gchar *root_objects[] = {
    "profiles_dialog",
    NULL
  };

  dlg->priv = GTR_PROFILE_DIALOG_GET_PRIVATE (dlg);

  gtk_window_set_title (GTK_WINDOW (dlg), _("Gtr Profile"));
  gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
  gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);

  gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
  gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->vbox), 2);

  path = gtranslator_dirs_get_ui_file ("gtr-profile-dialog.ui");
  ret = gtranslator_utils_get_ui_objects (path,
					  root_objects,
					  &error_widget,
					  "profiles_dialog",
					  &dlg->priv->main_box,
					  "profile_entry",
					  &dlg->priv->profile_name_entry,
					  "name_entry",
					  &dlg->priv->author_name_entry,
					  "email_entry",
					  &dlg->priv->author_email_entry,
					  "language_entry",
					  &dlg->priv->language_name_entry,
					  "langcode_entry",
					  &dlg->priv->language_code_entry,
					  "charset_entry",
					  &dlg->priv->charset_entry,
					  "encoding_entry",
					  &dlg->priv->encoding_entry,
					  "team_email_entry",
					  &dlg->priv->language_email_entry,
					  "plurals_entry",
					  &dlg->priv->plurals_forms_entry,
					  NULL);
  g_free (path);

  if (!ret)
    {
      gtk_widget_show (error_widget);
      gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			  error_widget, TRUE, TRUE, 0);

      return;
    }

  gtk_widget_set_tooltip_text (dlg->priv->plurals_forms_entry,
			       _("Example: nplurals=2; plural=(n != 1);"));

  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
		      dlg->priv->main_box, FALSE, FALSE, 0);
  gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);

}

void
gtranslator_show_profile_dialog (GtrPreferencesDialog * dialog,
				 GtrProfile * profile, gint action)
{
  static GtrProfileDialog *dlg = NULL;
  GtkButton *ok_button, *cancel_button;
  GtrProfile *old_profile;

  old_profile = gtranslator_profile_new ();

  cancel_button = GTK_BUTTON (gtk_button_new_from_stock (GTK_STOCK_CANCEL));
  gtk_widget_show (GTK_WIDGET (cancel_button));
  ok_button = GTK_BUTTON (gtk_button_new_from_stock (GTK_STOCK_OK));
  gtk_widget_show (GTK_WIDGET (ok_button));

  g_return_if_fail (GTR_IS_PREFERENCES_DIALOG (dialog));

  if (dlg == NULL)
    {
      dlg = g_object_new (GTR_TYPE_PROFILE_DIALOG, NULL);
      g_signal_connect (dlg,
			"destroy", G_CALLBACK (gtk_widget_destroyed), &dlg);
      gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dlg)->action_area),
			 GTK_WIDGET (cancel_button));

      gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dlg)->action_area),
			 GTK_WIDGET (ok_button));

      dlg->priv->prefs_dlg = dialog;

      gtk_widget_show (GTK_WIDGET (dlg));
    }

  if (GTK_WINDOW (dialog) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
    {
      gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (dialog));
    }

  if (gtranslator_profile_get_name (profile) != NULL)
    {
      gtk_entry_set_text (GTK_ENTRY
			  (GTR_PROFILE_DIALOG (dlg)->priv->
			   profile_name_entry),
			  gtranslator_profile_get_name (profile));
      gtranslator_profile_set_name (old_profile,
				    gtranslator_profile_get_name (profile));
    }

  if (gtranslator_profile_get_author_name (profile) != NULL)
    {
      gtk_entry_set_text (GTK_ENTRY
			  (GTR_PROFILE_DIALOG (dlg)->priv->author_name_entry),
			  gtranslator_profile_get_author_name (profile));
      gtranslator_profile_set_author_name (old_profile,
					   gtranslator_profile_get_author_name
					   (profile));
    }

  if (gtranslator_profile_get_author_email (profile) != NULL)
    {
      gtk_entry_set_text (GTK_ENTRY
			  (GTR_PROFILE_DIALOG (dlg)->priv->
			   author_email_entry),
			  gtranslator_profile_get_author_email (profile));
      gtranslator_profile_set_author_email (old_profile,
					    gtranslator_profile_get_author_email
					    (profile));
    }

  if (gtranslator_profile_get_language_name (profile) != NULL)
    {
      gtk_entry_set_text (GTK_ENTRY
			  (GTR_PROFILE_DIALOG (dlg)->priv->
			   language_name_entry),
			  gtranslator_profile_get_language_name (profile));
      gtranslator_profile_set_language_name (old_profile,
					     gtranslator_profile_get_language_name
					     (profile));
    }

  if (gtranslator_profile_get_language_code (profile) != NULL)
    {
      gtk_entry_set_text (GTK_ENTRY
			  (GTR_PROFILE_DIALOG (dlg)->priv->
			   language_code_entry),
			  gtranslator_profile_get_language_code (profile));
      gtranslator_profile_set_language_code (old_profile,
					     gtranslator_profile_get_language_code
					     (profile));
    }

  if (gtranslator_profile_get_charset (profile) != NULL)
    {
      gtk_entry_set_text (GTK_ENTRY
			  (GTR_PROFILE_DIALOG (dlg)->priv->charset_entry),
			  gtranslator_profile_get_charset (profile));
      gtranslator_profile_set_charset (old_profile,
				       gtranslator_profile_get_charset
				       (profile));
    }

  if (gtranslator_profile_get_encoding (profile) != NULL)
    {
      gtk_entry_set_text (GTK_ENTRY
			  (GTR_PROFILE_DIALOG (dlg)->priv->encoding_entry),
			  gtranslator_profile_get_encoding (profile));
      gtranslator_profile_set_encoding (old_profile,
					gtranslator_profile_get_encoding
					(profile));
    }

  if (gtranslator_profile_get_group_email (profile) != NULL)
    {
      gtk_entry_set_text (GTK_ENTRY
			  (GTR_PROFILE_DIALOG (dlg)->priv->
			   language_email_entry),
			  gtranslator_profile_get_group_email (profile));
      gtranslator_profile_set_group_email (old_profile,
					   gtranslator_profile_get_group_email
					   (profile));
    }

  if (gtranslator_profile_get_plurals (profile) != NULL)
    {
      gtk_entry_set_text (GTK_ENTRY
			  (GTR_PROFILE_DIALOG (dlg)->priv->
			   plurals_forms_entry),
			  gtranslator_profile_get_plurals (profile));
      gtranslator_profile_set_plurals (old_profile,
				       gtranslator_profile_get_plurals
				       (profile));
    }

  dlg->priv->old_profile = old_profile;
  dlg->priv->new_profile = profile;

  /* Connect entry signals */
  g_signal_connect (GTR_PROFILE_DIALOG (dlg)->priv->profile_name_entry,
		    "notify::text", G_CALLBACK (profile_name_entry_changed),
		    dlg);
  g_signal_connect (GTR_PROFILE_DIALOG (dlg)->priv->author_name_entry,
		    "notify::text", G_CALLBACK (author_name_entry_changed),
		    dlg);
  g_signal_connect (GTR_PROFILE_DIALOG (dlg)->priv->author_email_entry,
		    "notify::text", G_CALLBACK (author_email_entry_changed),
		    dlg);
  g_signal_connect (GTR_PROFILE_DIALOG (dlg)->priv->language_name_entry,
		    "notify::text", G_CALLBACK (language_name_entry_changed),
		    dlg);
  g_signal_connect (GTR_PROFILE_DIALOG (dlg)->priv->language_code_entry,
		    "notify::text", G_CALLBACK (language_code_entry_changed),
		    dlg);
  g_signal_connect (GTR_PROFILE_DIALOG (dlg)->priv->charset_entry,
		    "notify::text", G_CALLBACK (charset_entry_changed), dlg);
  g_signal_connect (GTR_PROFILE_DIALOG (dlg)->priv->encoding_entry,
		    "notify::text", G_CALLBACK (encoding_entry_changed), dlg);
  g_signal_connect (GTR_PROFILE_DIALOG (dlg)->priv->language_email_entry,
		    "notify::text", G_CALLBACK (group_email_entry_changed),
		    dlg);
  g_signal_connect (GTR_PROFILE_DIALOG (dlg)->priv->plurals_forms_entry,
		    "notify::text", G_CALLBACK (plurals_entry_changed), dlg);

  /*
   * Connect signals to dialog buttons.
   */

  g_signal_connect (cancel_button,
		    "clicked", G_CALLBACK (destroy_without_changes), dlg);

  if (action == NEW_PROFILE)
    {
      g_signal_connect (ok_button,
			"clicked", G_CALLBACK (save_new_profile), dlg);
    }
  else
    {
      if (action == EDIT_PROFILE)
	{
	  g_signal_connect (ok_button,
			    "clicked",
			    G_CALLBACK (save_modified_profile), dlg);
	}
    }

}
