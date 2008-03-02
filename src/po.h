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
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __PO_H__
#define __PO_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

#include "header.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_PO		(gtranslator_po_get_type ())
#define GTR_PO(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_PO, GtranslatorPo))
#define GTR_PO_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_PO, GtranslatorPoClass))
#define GTR_IS_PO(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_PO))
#define GTR_IS_PO_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_PO))
#define GTR_PO_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_PO, GtranslatorPoClass))

/* Private structure type */
typedef struct _GtranslatorPoPrivate	GtranslatorPoPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorPo		GtranslatorPo;

struct _GtranslatorPo
{
	GObject parent_instance;
	
	/*< private > */
	GtranslatorPoPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorPoClass	GtranslatorPoClass;

struct _GtranslatorPoClass
{
	GObjectClass parent_class;
};

#define GTR_PO_ERROR gtranslator_po_error_quark()

enum
{
	GTR_PO_ERROR_GETTEXT,
	GTR_PO_ERROR_FILENAME,
	GTR_PO_ERROR_OTHER,
};

/*
 * Public methods
 */
GQuark           gtranslator_po_error_quark            (void);

GType		 gtranslator_po_get_type	       (void) G_GNUC_CONST;

GType		 gtranslator_po_register_type	       (GTypeModule * module);

GtranslatorPo   *gtranslator_po_new                    (void);

void             gtranslator_po_parse                  (GtranslatorPo *po,
							const gchar *filename,
							GError **error);

gchar           *gtranslator_po_get_filename           (GtranslatorPo *po);

void             gtranslator_po_set_filename           (GtranslatorPo *po,
							gchar *data);

gboolean         gtranslator_po_get_write_perms        (GtranslatorPo *po);

GList           *gtranslator_po_get_messages           (GtranslatorPo *po);

void		gtranslator_po_set_messages		(GtranslatorPo *po,
							 GList *messages);

GList           *gtranslator_po_get_current_message    (GtranslatorPo *po);

GList           *gtranslator_po_get_domains            (GtranslatorPo *po);

po_file_t        gtranslator_po_get_po_file            (GtranslatorPo *po);

GList           *gtranslator_po_get_next_fuzzy         (GtranslatorPo *po);

GList           *gtranslator_po_get_prev_fuzzy         (GtranslatorPo *po);

GList           *gtranslator_po_get_next_untrans       (GtranslatorPo *po);

GList           *gtranslator_po_get_prev_untrans       (GtranslatorPo *po);

GtranslatorHeader	*gtranslator_po_get_header	(GtranslatorPo *po);

G_END_DECLS

#endif /* __PO_H__ */
