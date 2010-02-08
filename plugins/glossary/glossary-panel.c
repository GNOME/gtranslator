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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "glossary-panel.h"

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GTR_GLOSSARY_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_GLOSSARY_PANEL,     \
						 GtrGlossaryPanelPrivate))

G_DEFINE_TYPE (GtrGlossaryPanel, gtr_glossary_panel, GTK_TYPE_VBOX)
#define XML_FILE_NAME "glossary.xml"
/*
 * TreeItem structure
 */
     typedef struct _TreeItem TreeItem;
     struct _TreeItem
     {
       gchar *word;
       gchar *description;
       GList *children;
     };

     enum
     {
       WORD_COLUMN,
       DESCRIPTION_COLUMN,
       N_COLUMNS
     };

     struct _GtrGlossaryPanelPrivate
     {
       GtkWidget *tree;
       GtkWidget *search_entry;
       GtkWidget *add_button;
       xmlDocPtr *doc;
       GList *elements;
     };

     typedef struct person
     {
       xmlChar *name;
       xmlChar *email;
       xmlChar *company;
       xmlChar *organisation;
       xmlChar *smail;
       xmlChar *webPage;
       xmlChar *phone;
     } person, *personPtr;

/*
 * And the code needed to parse it
 */
     static personPtr parsePerson (xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur)
{
  personPtr ret = NULL;

  /*
   * allocate the struct
   */
  ret = (personPtr) g_malloc (sizeof (person));
  if (ret == NULL)
    {
      fprintf (stderr, "out of memory\n");
      return (NULL);
    }
  memset (ret, 0, sizeof (person));

  /* We don't care what the top level element name is */
  /* COMPAT xmlChildrenNode is a macro unifying libxml1 and libxml2 names */
  cur = cur->xmlChildrenNode;
  while (cur != NULL)
    {
      if ((!xmlStrcmp (cur->name, (const xmlChar *) "Person")) &&
          (cur->ns == ns))
        ret->name = xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
      if ((!xmlStrcmp (cur->name, (const xmlChar *) "Email")) &&
          (cur->ns == ns))
        ret->email = xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
      cur = cur->next;
    }

  return (ret);
}

static void
process_node (xmlNode * root_node)
{
  GtrGlossaryPanelPrivate *priv;

  xmlNodePtr cur_node;

  cur_node = cur_node->xmlChildrenNode;

}

static gboolean
gtr_glossary_panel_load_items (GtrGlossaryPanel * panel)
{
  GtrGlossaryPanelPrivate *priv = panel->priv;

  return FALSE;
}

static void
gtr_glossary_panel_draw (GtrGlossaryPanel * panel)
{
  GtrGlossaryPanelPrivate *priv = panel->priv;

  priv->tree = gtk_tree_view_new ();
}

static void
gtr_glossary_panel_init (GtrGlossaryPanel * panel)
{
  panel->priv = GTR_GLOSSARY_PANEL_GET_PRIVATE (panel);


}

static void
gtr_glossary_panel_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_glossary_panel_parent_class)->finalize (object);
}

static void
gtr_glossary_panel_class_init (GtrGlossaryPanelClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtrGlossaryPanelPrivate));

  object_class->finalize = gtr_glossary_panel_finalize;
}

GtkWidget *
gtr_glossary_panel_new (void)
{
  return GTK_WIDGET (g_object_new (GTR_TYPE_GLOSSARY_PANEL, NULL));
}
