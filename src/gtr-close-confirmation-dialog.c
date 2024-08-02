/*
 * gtr-close-confirmation-dialog.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2004-2005  GNOME Foundation
 *               2008 Ignacio Casal Quinteiro
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n.h>
#include <adwaita.h>

#include "gtr-close-confirmation-dialog.h"
#include "gtr-application.h"
#include "gtr-utils.h"
#include "gtr-window.h"
#include "gtr-po.h"


/* Properties */
enum
{
  PROP_0,
  PROP_UNSAVED_DOCUMENTS,
  PROP_LOGOUT_MODE
};

/* Mode */
enum
{
  SINGLE_DOC_MODE,
  MULTIPLE_DOCS_MODE
};

/* Columns */
enum
{
  SAVE_COLUMN,
  NAME_COLUMN,
  DOC_COLUMN,                   /* a handy pointer to the document */
  N_COLUMNS
};

struct _GtrCloseConfirmationDialog
{
  AdwAlertDialog parent;
};

struct _GtrCloseConfirmationDialogClass
{
  AdwAlertDialogClass parent_class;
};

typedef struct
{
  gboolean logout_mode;
  GList *unsaved_documents;
  GList *selected_documents;

  gboolean disable_save_to_disk;
} GtrCloseConfirmationDialogPrivate;

#define GET_MODE(priv) (((priv->unsaved_documents != NULL) && \
                         (priv->unsaved_documents->next == NULL)) ? \
                          SINGLE_DOC_MODE : MULTIPLE_DOCS_MODE)

G_DEFINE_TYPE_WITH_PRIVATE (GtrCloseConfirmationDialog,
                            gtr_close_confirmation_dialog,
                            ADW_TYPE_ALERT_DIALOG)

static void set_unsaved_document (GtrCloseConfirmationDialog *dlg,
                                  const GList * list);

/*  Since we connect in the constructor we are sure this handler will be called
 *  before the user ones
 */
static void
response_cb (GtrCloseConfirmationDialog *dlg,
             char *response,
             gpointer data)
{
  GtrCloseConfirmationDialogPrivate *priv;
  priv = gtr_close_confirmation_dialog_get_instance_private (dlg);

  g_return_if_fail (GTR_IS_CLOSE_CONFIRMATION_DIALOG (dlg));

  if (priv->selected_documents != NULL)
    g_list_free (priv->selected_documents);

  if (g_strcmp0 (response, "yes") == 0)
    {
      priv->selected_documents = g_list_copy (priv->unsaved_documents);
    }
  else
    priv->selected_documents = NULL;
}

static void
set_logout_mode (GtrCloseConfirmationDialog * dlg, gboolean logout_mode)
{

  adw_alert_dialog_set_close_response (ADW_ALERT_DIALOG (dlg), "cancel");
  adw_alert_dialog_add_responses (
    ADW_ALERT_DIALOG (dlg),
    "cancel", _("_Cancel"),
    "no", _("Close _Without Saving"),
    "yes", _("_Save"),
    NULL
  );

  adw_alert_dialog_set_response_appearance (ADW_ALERT_DIALOG (dlg),
    "no", ADW_RESPONSE_DESTRUCTIVE);
  adw_alert_dialog_set_response_appearance (ADW_ALERT_DIALOG (dlg),
    "yes", ADW_RESPONSE_SUGGESTED);
  adw_alert_dialog_set_default_response (ADW_ALERT_DIALOG (dlg), "yes");
}

static void
gtr_close_confirmation_dialog_init (GtrCloseConfirmationDialog * dlg)
{
  g_signal_connect (dlg, "response", G_CALLBACK (response_cb), NULL);
  adw_alert_dialog_set_body_use_markup (ADW_ALERT_DIALOG (dlg), TRUE);
}

static void
gtr_close_confirmation_dialog_finalize (GObject * object)
{
  GtrCloseConfirmationDialogPrivate *priv;
  priv = gtr_close_confirmation_dialog_get_instance_private (GTR_CLOSE_CONFIRMATION_DIALOG (object));

  if (priv->unsaved_documents != NULL)
    g_list_free (priv->unsaved_documents);

  if (priv->selected_documents != NULL)
    g_list_free (priv->selected_documents);

  /* Call the parent's destructor */
  G_OBJECT_CLASS (gtr_close_confirmation_dialog_parent_class)->finalize
    (object);
}

static void
gtr_close_confirmation_dialog_set_property (GObject * object,
                                            guint prop_id,
                                            const GValue * value,
                                            GParamSpec * pspec)
{
  GtrCloseConfirmationDialog *dlg;

  dlg = GTR_CLOSE_CONFIRMATION_DIALOG (object);

  switch (prop_id)
    {
    case PROP_UNSAVED_DOCUMENTS:
      set_unsaved_document (dlg, g_value_get_pointer (value));
      break;

    case PROP_LOGOUT_MODE:
      set_logout_mode (dlg, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtr_close_confirmation_dialog_get_property (GObject * object,
                                            guint prop_id,
                                            GValue * value,
                                            GParamSpec * pspec)
{
  GtrCloseConfirmationDialogPrivate *priv;
  priv = gtr_close_confirmation_dialog_get_instance_private (GTR_CLOSE_CONFIRMATION_DIALOG (object));

  switch (prop_id)
    {
    case PROP_UNSAVED_DOCUMENTS:
      g_value_set_pointer (value, priv->unsaved_documents);
      break;

    case PROP_LOGOUT_MODE:
      g_value_set_boolean (value, priv->logout_mode);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
  gtr_close_confirmation_dialog_class_init
  (GtrCloseConfirmationDialogClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gtr_close_confirmation_dialog_set_property;
  gobject_class->get_property = gtr_close_confirmation_dialog_get_property;
  gobject_class->finalize = gtr_close_confirmation_dialog_finalize;

  g_object_class_install_property (gobject_class,
                                   PROP_UNSAVED_DOCUMENTS,
                                   g_param_spec_pointer ("unsaved_documents",
                                                         "Unsaved Documents",
                                                         "List of Unsaved Documents",
                                                         (G_PARAM_READWRITE |
                                                          G_PARAM_CONSTRUCT_ONLY)));

  g_object_class_install_property (gobject_class,
                                   PROP_LOGOUT_MODE,
                                   g_param_spec_boolean ("logout_mode",
                                                         "Logout Mode",
                                                         "Whether the dialog is in logout mode",
                                                         FALSE,
                                                         (G_PARAM_READWRITE |
                                                          G_PARAM_CONSTRUCT_ONLY)));
}

GList *gtr_close_confirmation_dialog_get_selected_documents
  (GtrCloseConfirmationDialog * dlg)
{
  GtrCloseConfirmationDialogPrivate *priv;
  priv = gtr_close_confirmation_dialog_get_instance_private (dlg);
  g_return_val_if_fail (GTR_IS_CLOSE_CONFIRMATION_DIALOG (dlg), NULL);

  return g_list_copy (priv->selected_documents);
}

GtkWidget *
gtr_close_confirmation_dialog_new (GList * unsaved_documents,
                                   gboolean logout_mode)
{
  GtkWidget *dlg;
  g_return_val_if_fail (unsaved_documents != NULL, NULL);

  dlg = GTK_WIDGET (g_object_new (GTR_TYPE_CLOSE_CONFIRMATION_DIALOG,
                                  "unsaved_documents", unsaved_documents,
                                  "logout_mode", logout_mode, NULL));
  g_return_val_if_fail (dlg != NULL, NULL);
  return dlg;
}

GtkWidget *
gtr_close_confirmation_dialog_new_single (GtrPo * doc, gboolean logout_mode)
{
  GtkWidget *dlg;
  GList *unsaved_documents;
  g_return_val_if_fail (doc != NULL, NULL);

  unsaved_documents = g_list_prepend (NULL, doc);

  dlg = gtr_close_confirmation_dialog_new (unsaved_documents, logout_mode);

  g_list_free (unsaved_documents);

  return dlg;
}

static void
build_single_doc_dialog (GtrCloseConfirmationDialog * dlg)
{
  GtrPo *doc;
  GtrCloseConfirmationDialogPrivate *priv;

  g_autoptr (GFile) location = NULL;
  g_autofree char *doc_name = NULL;

  priv = gtr_close_confirmation_dialog_get_instance_private (dlg);

  g_return_if_fail (priv->unsaved_documents->data != NULL);
  doc = GTR_PO (priv->unsaved_documents->data);
  location = gtr_po_get_location (doc);
  doc_name = g_file_get_basename (location);

  adw_alert_dialog_format_body (ADW_ALERT_DIALOG (dlg),
    _("Save the changes to document “%s” before closing?"), doc_name);
}

static void
build_multiple_docs_dialog (GtrCloseConfirmationDialog * dlg)
{
  GtrCloseConfirmationDialogPrivate *priv;
  g_autofree char *str = NULL;

  priv = gtr_close_confirmation_dialog_get_instance_private (dlg);

  if (priv->disable_save_to_disk)
    str =
      g_strdup_printf (ngettext
                       ("Changes to %d document will be permanently lost.",
                        "Changes to %d documents will be permanently lost.",
                        g_list_length (priv->unsaved_documents)),
                       g_list_length (priv->unsaved_documents));
  else
    str =
      g_strdup_printf (ngettext
                       ("There is %d document with unsaved changes. "
                        "Save changes before closing?",
                        "There are %d documents with unsaved changes. "
                        "Save changes before closing?",
                        g_list_length (priv->unsaved_documents)),
                       g_list_length (priv->unsaved_documents));

  adw_alert_dialog_set_body (ADW_ALERT_DIALOG (dlg), str);
}

static void
set_unsaved_document (GtrCloseConfirmationDialog * dlg, const GList * list)
{
  GtrCloseConfirmationDialogPrivate *priv;
  priv = gtr_close_confirmation_dialog_get_instance_private (dlg);

  g_return_if_fail (list != NULL);

  g_return_if_fail (priv->unsaved_documents == NULL);

  priv->unsaved_documents = g_list_copy ((GList *) list);

  adw_alert_dialog_set_heading (ADW_ALERT_DIALOG (dlg), _("Unsaved Changes"));
  if (GET_MODE (priv) == SINGLE_DOC_MODE)
    {
      build_single_doc_dialog (dlg);
    }
  else
    {
      build_multiple_docs_dialog (dlg);
    }
}

const GList *gtr_close_confirmation_dialog_get_unsaved_documents
  (GtrCloseConfirmationDialog * dlg)
{
  GtrCloseConfirmationDialogPrivate *priv;
  priv = gtr_close_confirmation_dialog_get_instance_private (dlg);
  g_return_val_if_fail (GTR_IS_CLOSE_CONFIRMATION_DIALOG (dlg), NULL);

  return priv->unsaved_documents;
}
