/*
 * (C) 2007 Pablo Sanxiao <psanxiao@gmail.com>
 *          Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 *   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 */

#include "header.h"
#include "prefs-manager.h"

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

	/*
	 * Plural forms and number of plurals
	 */
	gchar *plural_forms;
	gint nplurals;

        /*
         * Check if header has changed
         */
        gboolean header_changed;
};

/*
 * Set nplurals variable
 */
static void
parse_nplurals (GtranslatorHeader *header)
{
	gchar *pointer;
	
	g_return_if_fail (header->priv->plural_forms != NULL);
	
	pointer = header->priv->plural_forms;
	
	while (*pointer != '=')
		pointer++;
	pointer++;
	
	//if there are any space between '=' and nplural number pointer++
	while(*pointer == ' ')
		pointer++;
	
	header->priv->nplurals = g_ascii_digit_value(*pointer);
}

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

const gchar *gtranslator_header_get_comment (GtranslatorHeader *header)
{
	return header->priv->comment;
}

void gtranslator_header_set_comment (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->comment)
		g_free(header->priv->comment);
	header->priv->comment = g_strdup(data);
}

const gchar *gtranslator_header_get_prj_id_version (GtranslatorHeader *header)
{
	return header->priv->prj_id_version;
}

void gtranslator_header_set_prj_id_version (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->prj_id_version)
		g_free(header->priv->prj_id_version);
	header->priv->prj_id_version = g_strdup(data);
}

const gchar *gtranslator_header_get_rmbt (GtranslatorHeader *header)
{
	return header->priv->rmbt;
}

void gtranslator_header_set_rmbt (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->rmbt)
		g_free(header->priv->rmbt);
	header->priv->rmbt = g_strdup(data);
}

const gchar *gtranslator_header_get_pot_date (GtranslatorHeader *header)
{
	return header->priv->pot_date;
}

void gtranslator_header_set_pot_date (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->pot_date)
		g_free(header->priv->pot_date);
	header->priv->pot_date = g_strdup(data);
}

const gchar *gtranslator_header_get_po_date (GtranslatorHeader *header)
{
	return header->priv->po_date;
}

void gtranslator_header_set_po_date (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->po_date)
		g_free(header->priv->po_date);
	header->priv->po_date = g_strdup(data);
}

const gchar *gtranslator_header_get_translator (GtranslatorHeader *header)
{
	return header->priv->translator;
}

void gtranslator_header_set_translator (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->translator)
		g_free(header->priv->translator);
	header->priv->translator = g_strdup(data);
}

const gchar *gtranslator_header_get_tr_email (GtranslatorHeader *header)
{
	return header->priv->tr_email;
}

void gtranslator_header_set_tr_email (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->tr_email)
		g_free(header->priv->tr_email);
	header->priv->tr_email = g_strdup(data);
}

const gchar *gtranslator_header_get_prev_translator (GtranslatorHeader *header)
{
	return header->priv->prev_translator;
}

void gtranslator_header_set_prev_translator (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->prev_translator)
		g_free(header->priv->prev_translator);
	header->priv->prev_translator = g_strdup(data);
}

const gchar *gtranslator_header_get_language (GtranslatorHeader *header)
{
	return header->priv->language;
}

void gtranslator_header_set_language (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->language)
		g_free(header->priv->language);
	header->priv->language = g_strdup(data);
}

const gchar *gtranslator_header_get_lg_email (GtranslatorHeader *header)
{
	return header->priv->lg_email;
}

void gtranslator_header_set_lg_email (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->lg_email)
		g_free(header->priv->lg_email);
	header->priv->lg_email = g_strdup(data);
}

const gchar *gtranslator_header_get_mime_version (GtranslatorHeader *header)
{
	return header->priv->mime_version;
}

void gtranslator_header_set_mime_version (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->mime_version)
		g_free(header->priv->mime_version);
	header->priv->mime_version = g_strdup(data);
}

const gchar *gtranslator_header_get_charset (GtranslatorHeader *header)
{
	return header->priv->charset;
}

void gtranslator_header_set_charset (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->charset)
		g_free(header->priv->charset);
	header->priv->charset = g_strdup(data);
}

const gchar *gtranslator_header_get_encoding (GtranslatorHeader *header)
{
	return header->priv->encoding;
}

void gtranslator_header_set_encoding (GtranslatorHeader *header, const gchar *data)
{
	if(header->priv->encoding)
		g_free(header->priv->encoding);
	header->priv->encoding = g_strdup(data);
}

/**
 * gtranslator_header_get_plural_forms:
 * @header: a #GtranslatorHeader.
 *
 * Return value: the plural form of the po file.
 */
const gchar *
gtranslator_header_get_plural_forms (GtranslatorHeader *header)
{
	g_return_val_if_fail (GTR_IS_HEADER (header), NULL);
	
	return (const gchar *)header->priv->plural_forms;
}

/**
 * gtranslator_header_set_plural_forms:
 * @header: a #GtranslatorHeader
 * @plural_forms: the plural forms string.
 *
 * Sets the plural form string in the @header and it sets the number of plurals.
 */
void
gtranslator_header_set_plural_forms (GtranslatorHeader *header,
				     const gchar *plural_forms)
{
	g_return_if_fail (GTR_IS_HEADER (header));
	
	if (header->priv->plural_forms)
		g_free (header->priv->plural_forms);
	
	if (!plural_forms)
	{
		header->priv->plural_forms = NULL;
		return;
	}
	
	header->priv->plural_forms = g_strdup (plural_forms);
	
	/*Now we parse the plural forms to know the number of plurals*/
	parse_nplurals (header);
}

/**
 * gtranslator_header_get_header_changed:
 * @header: a #GtranslatorHeader.
 *
 * Return value: TRUE if the header has changed, FALSE if not.
 */

gboolean
gtranslator_header_get_header_changed (GtranslatorHeader *header)
{
  g_return_val_if_fail (GTR_IS_HEADER (header), FALSE);

  return header->priv->header_changed;
}

/**
 * gtranslator_header_set_header_changed:
 * @header: a #GtranslatorHeader
 * @gboolean: Indicate if the header has changed or not.
 *
 * Sets the state of the header, TRUE if has changed, FALSE if not.
 */

void
gtranslator_header_set_header_changed (GtranslatorHeader *header, gboolean change)
{
  header->priv->header_changed = change;
}

/**
 * gtranslator_header_get_plural:
 * @header: a #GtranslatorHeader
 *
 * Return value: The number of plurals of the po file, if there is not a plural
 * form in the po file it returns the predefined by user number of plurals 
 * or 0 if there is not a plural form string stored.
 */
gint 
gtranslator_header_get_nplurals (GtranslatorHeader *header)
{
	const gchar *plural_form;
	
	g_return_val_if_fail (GTR_IS_HEADER (header), -1);
	
	/*
	 * If the priv->plural_forms exists that means that there is a plural
	 * form in our po file, If not we have to use the predefined plural form
	 * by the user.
	 */
	if (header->priv->plural_forms)
		return header->priv->nplurals;
	else if (plural_form = (const gchar *)gtranslator_profile_get_plurals ())
	{
		gtranslator_header_set_plural_forms (header, plural_form);
		/*
		 * FIXME: To don't produce a gettext error maybe we have to set the
		 * plural form in the gettext header field too.
		 */
		return header->priv->nplurals;
	}
	else return 1;
}
