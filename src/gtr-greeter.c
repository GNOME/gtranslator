/*
 * Copyright (C) 2022  Daniel Garcia <danigm@gnome.org>
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n.h>

#include "gtr-greeter.h"
#include "gtr-window.h"
#include "gtr-profile.h"
#include "gtr-profile-manager.h"
#include "gtr-languages-fetcher.h"

typedef struct
{
  GtkWidget *greeter_stack;

  GtkWidget *profile_name;
  GtkWidget *name;
  GtkWidget *email;
  GtkWidget *team_email;
  GtkWidget *languages_fetcher;

  GtkWidget *next_button;
  GtkWidget *back_button;

  GtkWidget *conf_profile;
  GtkWidget *conf_name;
  GtkWidget *conf_email;
  GtkWidget *conf_team;
  GtkWidget *conf_lang;
  GtkWidget *conf_lang_code;
  GtkWidget *conf_charset;
  GtkWidget *conf_encoding;
  GtkWidget *conf_plural_forms;

  GtrWindow *main_window;
} GtrGreeterPrivate;

struct _GtrGreeter
{
  AdwNavigationPage parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtrGreeter, gtr_greeter, ADW_TYPE_NAVIGATION_PAGE)

static void
add_profile (GtrGreeter *self)
{
  GtrProfile *profile;
  GtrProfileManager *prof_manager;
  GtrGreeterPrivate *priv = gtr_greeter_get_instance_private (GTR_GREETER (self));
  GtrLanguagesFetcher *lf = GTR_LANGUAGES_FETCHER (priv->languages_fetcher);

  profile = gtr_profile_new ();
  prof_manager = gtr_profile_manager_get_default ();

  gtr_profile_set_name (profile, gtk_editable_get_text (GTK_EDITABLE (priv->profile_name)));
  gtr_profile_set_author_name (profile, gtk_editable_get_text (GTK_EDITABLE (priv->name)));
  gtr_profile_set_author_email (profile, gtk_editable_get_text (GTK_EDITABLE (priv->email)));
  gtr_profile_set_group_email (profile, gtk_editable_get_text (GTK_EDITABLE (priv->team_email)));

  gtr_profile_set_language_name (profile, gtr_languages_fetcher_get_language_name (lf));
  gtr_profile_set_language_code (profile, gtr_languages_fetcher_get_language_code (lf));
  gtr_profile_set_charset (profile, gtr_languages_fetcher_get_charset (lf));
  gtr_profile_set_encoding (profile, gtr_languages_fetcher_get_encoding (lf));
  gtr_profile_set_plural_forms (profile, gtr_languages_fetcher_get_plural_form (lf));

  /* Add profile to profile manager and save it */
  gtr_profile_manager_add_profile (prof_manager, profile);
  g_object_unref (prof_manager);
}

static void
update_confirmation_label (GtrGreeter *self)
{
  g_autofree char *string = NULL;
  GtrGreeterPrivate *priv = gtr_greeter_get_instance_private (GTR_GREETER (self));
  GtrLanguagesFetcher *lf = GTR_LANGUAGES_FETCHER (priv->languages_fetcher);

  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->conf_profile),
                               gtk_editable_get_text (GTK_EDITABLE (priv->profile_name)));
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->conf_name),
                               gtk_editable_get_text (GTK_EDITABLE (priv->name)));
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->conf_email),
                               gtk_editable_get_text (GTK_EDITABLE (priv->email)));
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->conf_team),
                               gtk_editable_get_text (GTK_EDITABLE (priv->team_email)));

  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->conf_lang),
                               gtr_languages_fetcher_get_language_name (lf));
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->conf_lang_code),
                               gtr_languages_fetcher_get_language_code (lf));
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->conf_charset),
                               gtr_languages_fetcher_get_charset (lf));
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->conf_encoding),
                               gtr_languages_fetcher_get_encoding (lf));
  adw_action_row_set_subtitle (ADW_ACTION_ROW (priv->conf_plural_forms),
                               gtr_languages_fetcher_get_plural_form (lf));
}

static void
go_next_cb (GtkButton *btn, GtrGreeter *self)
{
  const char *child_name = NULL;
  GtrGreeterPrivate *priv = gtr_greeter_get_instance_private (GTR_GREETER (self));

  gtk_widget_set_visible (priv->back_button, TRUE);
  child_name = adw_view_stack_get_visible_child_name (ADW_VIEW_STACK (priv->greeter_stack));

  if (g_strcmp0 (child_name, "welcome") == 0)
    adw_view_stack_set_visible_child_name (ADW_VIEW_STACK (priv->greeter_stack), "profile");
  else if (g_strcmp0 (child_name, "profile") == 0)
    {
      adw_view_stack_set_visible_child_name (ADW_VIEW_STACK (priv->greeter_stack), "confirmation");
      update_confirmation_label (self);
    }
  else
    {
      add_profile (self);
      gtr_window_show_projects (priv->main_window);
    }
}

static void
go_back_cb (GtkButton   *btn, GtrGreeter *self)
{
  const char *child_name = NULL;
  GtrGreeterPrivate *priv = gtr_greeter_get_instance_private (GTR_GREETER (self));

  child_name = adw_view_stack_get_visible_child_name (ADW_VIEW_STACK (priv->greeter_stack));
  if (g_strcmp0 (child_name, "confirmation") == 0)
    adw_view_stack_set_visible_child_name (ADW_VIEW_STACK (priv->greeter_stack), "profile");
  else if (g_strcmp0 (child_name, "profile") == 0)
    {
      adw_view_stack_set_visible_child_name (ADW_VIEW_STACK (priv->greeter_stack), "welcome");
      gtk_widget_set_visible (priv->back_button, FALSE);
    }
}

static void
gtr_greeter_dispose (GObject *object)
{
  G_OBJECT_CLASS (gtr_greeter_parent_class)->dispose (object);
}

static void
gtr_greeter_class_init (GtrGreeterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gtr_greeter_dispose;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/translator/gtr-greeter.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, greeter_stack);

  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, back_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, next_button);

  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, profile_name);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, name);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, email);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, team_email);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, languages_fetcher);

  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, conf_profile);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, conf_name);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, conf_email);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, conf_team);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, conf_lang);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, conf_lang_code);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, conf_charset);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, conf_encoding);
  gtk_widget_class_bind_template_child_private (widget_class, GtrGreeter, conf_plural_forms);
}

static void
gtr_greeter_init (GtrGreeter *self)
{
  GtrGreeterPrivate *priv = gtr_greeter_get_instance_private (self);

  g_type_ensure (GTR_TYPE_LANGUAGES_FETCHER);

  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect (priv->next_button,
                    "clicked",
                    G_CALLBACK (go_next_cb),
                    self);

  g_signal_connect (priv->back_button,
                    "clicked",
                    G_CALLBACK (go_back_cb),
                    self);
}

GtrGreeter*
gtr_greeter_new (GtrWindow *window)
{
  GtrGreeter *self = g_object_new (GTR_TYPE_GREETER, NULL);
  GtrGreeterPrivate *priv = gtr_greeter_get_instance_private (self);

  priv->main_window = window;
  return self;
}
