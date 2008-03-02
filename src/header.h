/*
 * (C) 2007 	Pablo Sanxiao <psanxiao@gmail.com>
 *			
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __HEADER_H__
#define __HEADER_H__

#include <glib.h>
#include <glib-object.h>

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
typedef struct _GtranslatorHeaderPrivate	GtranslatorHeaderPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorHeader	GtranslatorHeader;

struct _GtranslatorHeader
{
	GObject parent_instance;
	/*< private > */
	GtranslatorHeaderPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorHeaderClass	GtranslatorHeaderClass;

struct _GtranslatorHeaderClass
{
	GObjectClass parent_class;
};

/*
 * Public methods
 */
GType		gtranslator_header_get_type		(void) G_GNUC_CONST;

GType		gtranslator_header_register_type	(GTypeModule * module);

GtranslatorHeader
		*gtranslator_header_new			(void);

gchar		*gtranslator_header_get_comment		(GtranslatorHeader *header);
void		 gtranslator_header_set_comment		(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_prj_id_version	(GtranslatorHeader *header);
void		 gtranslator_header_set_prj_id_version	(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_rmbt		(GtranslatorHeader *header);
void		 gtranslator_header_set_rmbt		(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_pot_date	(GtranslatorHeader *header);
void		 gtranslator_header_set_pot_date	(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_po_date		(GtranslatorHeader *header);
void		 gtranslator_header_set_po_date		(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_translator	(GtranslatorHeader *header);
void		 gtranslator_header_set_translator	(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_tr_email	(GtranslatorHeader *header);
void		 gtranslator_header_set_tr_email	(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_language	(GtranslatorHeader *header);
void		 gtranslator_header_set_language	(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_lg_email	(GtranslatorHeader *header);
void		 gtranslator_header_set_lg_email	(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_mime_version	(GtranslatorHeader *header);
void		 gtranslator_header_set_mime_version	(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_charset		(GtranslatorHeader *header);
void		 gtranslator_header_set_charset		(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_encoding	(GtranslatorHeader *header);
void		 gtranslator_header_set_encoding	(GtranslatorHeader *header, gchar *data);

gchar		*gtranslator_header_get_plural_forms	(GtranslatorHeader *header);
void		 gtranslator_header_set_plural_forms	(GtranslatorHeader *header, gchar *data);

#endif /* __HEADER_H__ */
