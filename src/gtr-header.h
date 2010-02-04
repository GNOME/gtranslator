/*
 * (C) 2007 Pablo Sanxiao <psanxiao@gmail.com>
 *     2008 Igalia
 *
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or   
 * (at your option) any later version.
 *
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#ifndef __HEADER_H__
#define __HEADER_H__

#include <glib.h>
#include <glib-object.h>

#include "gtr-msg.h"

/*
 * Utility Macros
 */

#define GTR_TYPE_HEADER			(gtranslator_header_get_type ())
#define GTR_HEADER(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_HEADER, GtranslatorHeader))
#define GTR_HEADER_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_HEADER, GtranslatorHeaderClass))
#define GTR_IS_HEADER(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_HEADER))
#define GTR_IS_HEADER_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_HEADER))
#define GTR_HEADER_GET_CLASS(o)		(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_HEADER, GtranslatorHeaderClass))

/* Private structure type */
typedef struct _GtranslatorHeaderPrivate GtranslatorHeaderPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorHeader GtranslatorHeader;

struct _GtranslatorHeader
{
  GtranslatorMsg parent_instance;

  /*< private > */
  GtranslatorHeaderPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorHeaderClass GtranslatorHeaderClass;

struct _GtranslatorHeaderClass
{
  GtranslatorMsgClass parent_class;
};

/*
 * Public methods
 */
GType                 gtranslator_header_get_type            (void)G_GNUC_CONST;

GType                 gtranslator_header_register_type       (GTypeModule * module);

GtranslatorHeader    *gtranslator_header_new                 (po_message_iterator_t iter,
                                                              po_message_t          message);

const gchar          *gtranslator_header_get_comments        (GtranslatorHeader *header);
void                  gtranslator_header_set_comments        (GtranslatorHeader *header,
                                                              const gchar *comments);

gchar                *gtranslator_header_get_prj_id_version  (GtranslatorHeader *header);
void                  gtranslator_header_set_prj_id_version  (GtranslatorHeader *header,
                                                              const gchar *prj_id_version);

gchar                *gtranslator_header_get_rmbt            (GtranslatorHeader *header);
void                  gtranslator_header_set_rmbt            (GtranslatorHeader *header,
                                                              const gchar *rmbt);

gchar                *gtranslator_header_get_pot_date        (GtranslatorHeader *header);
void                  gtranslator_header_set_pot_date        (GtranslatorHeader *header,
                                                              const gchar *pot_date);

gchar                *gtranslator_header_get_po_date         (GtranslatorHeader *header);
void                  gtranslator_header_set_po_date         (GtranslatorHeader *header,
                                                              const gchar *po_date);

gchar                *gtranslator_header_get_translator      (GtranslatorHeader *header);
gchar                *gtranslator_header_get_tr_email        (GtranslatorHeader *header);
void                  gtranslator_header_set_translator      (GtranslatorHeader *header,
                                                              const gchar *name,
                                                              const gchar *email);

gchar                *gtranslator_header_get_language        (GtranslatorHeader *header);
gchar                *gtranslator_header_get_lg_email        (GtranslatorHeader *header);
void                  gtranslator_header_set_language        (GtranslatorHeader *header,
                                                              const gchar *language,
                                                              const gchar *email);

gchar                *gtranslator_header_get_mime_version    (GtranslatorHeader *header);
void                  gtranslator_header_set_mime_version    (GtranslatorHeader *header,
                                                              const gchar *mime_version);

gchar                *gtranslator_header_get_charset         (GtranslatorHeader *header);
void                  gtranslator_header_set_charset         (GtranslatorHeader *header,
                                                              const gchar *charset);

gchar                *gtranslator_header_get_encoding        (GtranslatorHeader *header);
void                  gtranslator_header_set_encoding        (GtranslatorHeader *header,
                                                              const gchar *encoding);

gchar                *gtranslator_header_get_plural_forms    (GtranslatorHeader *header);
void                  gtranslator_header_set_plural_forms    (GtranslatorHeader * header,
                                                              const gchar * plural_forms);
gint                  gtranslator_header_get_nplurals        (GtranslatorHeader * header);

#endif /* __HEADER_H__ */
