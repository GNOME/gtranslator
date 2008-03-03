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

#include "header.h"

#include <glib.h>
#include <glib-object.h>

#define GTR_HEADER_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_HEADER,     \
					 GtranslatorHeaderPrivate))

G_DEFINE_TYPE(GtranslatorHeader, gtranslator_header, G_TYPE_OBJECT)

struct _GtranslatorHeaderPrivate
{
	gchar *comment;
	
	/*
	 * Project-ID-Version
	 */
	gchar *prj_id_version;

	/*
	 * Report msgid bugs to
	 */
	gchar *rmbt;
	
	/*
	 * POT-Creation-Date
	 */
	gchar *pot_date;
	
	/*
	 *  PO-Revision-Date
	 */
	gchar *po_date;
	
	/*
	 * Translator Name
	 */
	gchar *translator;
	
	/*
	 * Translator Email 
	 */
	gchar *tr_email;

	/*
	 * Previous Translator Name and Email
	 */
	gchar *prev_translator;

	/*
	 * Language Name
	 */
	gchar *language;

	/*
	 * Traslation group email
	 */
	gchar *lg_email;
	
	gchar *mime_version;

	/*
	 * Content-Type
	 */
	gchar *charset;

	/*
	 * Content-Transfer-Encoding
	 */
	gchar *encoding;

	gchar *plural_forms;
};

static void gtranslator_header_init (GtranslatorHeader *header)
{
	header->priv = GTR_HEADER_GET_PRIVATE (header);
}

static void gtranslator_header_finalize (GObject *object)
{
	GtranslatorHeader *header = GTR_HEADER(object);
	
	g_free(header->priv->comment);
	g_free(header->priv->prj_id_version);
	g_free(header->priv->rmbt);
	g_free(header->priv->pot_date);
	g_free(header->priv->po_date);
	g_free(header->priv->translator);
	g_free(header->priv->tr_email);
	g_free(header->priv->language);
	g_free(header->priv->lg_email);
	g_free(header->priv->mime_version);
	g_free(header->priv->charset);
	g_free(header->priv->encoding);
	g_free(header->priv->plural_forms);

	G_OBJECT_CLASS (gtranslator_header_parent_class)->finalize (object);
}

static void gtranslator_header_class_init (GtranslatorHeaderClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorHeaderPrivate));

	object_class->finalize = gtranslator_header_finalize;
}

/*
 * Public methods
 */

GtranslatorHeader *gtranslator_header_new(void)
{
	GtranslatorHeader *header;
	
	header = g_object_new(GTR_TYPE_HEADER, NULL);
	
	return header;
}

gchar *gtranslator_header_get_comment (GtranslatorHeader *header)
{
	return header->priv->comment;
}

void gtranslator_header_set_comment (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->comment)
		g_free(header->priv->comment);
	header->priv->comment = g_strdup(data);
}

gchar *gtranslator_header_get_prj_id_version (GtranslatorHeader *header)
{
	return header->priv->prj_id_version;
}

void gtranslator_header_set_prj_id_version (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->prj_id_version)
		g_free(header->priv->prj_id_version);
	header->priv->prj_id_version = g_strdup(data);
}

gchar *gtranslator_header_get_rmbt (GtranslatorHeader *header)
{
	return header->priv->rmbt;
}

void gtranslator_header_set_rmbt (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->rmbt)
		g_free(header->priv->rmbt);
	header->priv->rmbt = g_strdup(data);
}

gchar *gtranslator_header_get_pot_date (GtranslatorHeader *header)
{
	return header->priv->pot_date;
}

void gtranslator_header_set_pot_date (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->pot_date)
		g_free(header->priv->pot_date);
	header->priv->pot_date = g_strdup(data);
}

gchar *gtranslator_header_get_po_date (GtranslatorHeader *header)
{
	return header->priv->po_date;
}

void gtranslator_header_set_po_date (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->po_date)
		g_free(header->priv->po_date);
	header->priv->po_date = g_strdup(data);
}

gchar *gtranslator_header_get_translator (GtranslatorHeader *header)
{
	return header->priv->translator;
}

void gtranslator_header_set_translator (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->translator)
		g_free(header->priv->translator);
	header->priv->translator = g_strdup(data);
}

gchar *gtranslator_header_get_tr_email (GtranslatorHeader *header)
{
	return header->priv->tr_email;
}

void gtranslator_header_set_tr_email (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->tr_email)
		g_free(header->priv->tr_email);
	header->priv->tr_email = g_strdup(data);
}

gchar *gtranslator_header_get_prev_translator (GtranslatorHeader *header)
{
	return header->priv->prev_translator;
}

void gtranslator_header_set_prev_translator (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->prev_translator)
		g_free(header->priv->prev_translator);
	header->priv->prev_translator = g_strdup(data);
}

gchar *gtranslator_header_get_language (GtranslatorHeader *header)
{
	return header->priv->language;
}

void gtranslator_header_set_language (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->language)
		g_free(header->priv->language);
	header->priv->language = g_strdup(data);
}

gchar *gtranslator_header_get_lg_email (GtranslatorHeader *header)
{
	return header->priv->lg_email;
}

void gtranslator_header_set_lg_email (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->lg_email)
		g_free(header->priv->lg_email);
	header->priv->lg_email = g_strdup(data);
}

gchar *gtranslator_header_get_mime_version (GtranslatorHeader *header)
{
	return header->priv->mime_version;
}

void gtranslator_header_set_mime_version (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->mime_version)
		g_free(header->priv->mime_version);
	header->priv->mime_version = g_strdup(data);
}

gchar *gtranslator_header_get_charset (GtranslatorHeader *header)
{
	return header->priv->charset;
}

void gtranslator_header_set_charset (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->charset)
		g_free(header->priv->charset);
	header->priv->charset = g_strdup(data);
}

gchar *gtranslator_header_get_encoding (GtranslatorHeader *header)
{
	return header->priv->encoding;
}

void gtranslator_header_set_encoding (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->encoding)
		g_free(header->priv->encoding);
	header->priv->encoding = g_strdup(data);
}

gchar *gtranslator_header_get_plural_forms (GtranslatorHeader *header)
{
	return header->priv->plural_forms;
}

void gtranslator_header_set_plural_forms (GtranslatorHeader *header, gchar *data)
{
	if(header->priv->plural_forms)
		g_free(header->priv->plural_forms);
	header->priv->plural_forms;
}



