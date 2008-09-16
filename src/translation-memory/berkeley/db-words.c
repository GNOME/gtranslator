/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 *     Based in poedit transmem.cpp file:
 *     Copyright (C) 2001-2007 Vaclav Slavik
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

#include "db-words.h"
#include "db-keys.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <string.h>

#define GTR_DB_WORDS_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_DB_WORDS,     \
						 GtranslatorDbWordsPrivate))

G_DEFINE_TYPE(GtranslatorDbWords, gtranslator_db_words, GTR_TYPE_DB_BASE)


struct _GtranslatorDbWordsPrivate
{

};

static void
gtranslator_db_words_init (GtranslatorDbWords *db_words)
{
	//db_words->priv = GTR_DB_WORDS_GET_PRIVATE (db_words);
	
	gtranslator_db_base_create_dabatase (GTR_DB_BASE (db_words),
					     _("words.db"),
					     DB_HASH);
}

static void
gtranslator_db_words_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_db_words_parent_class)->finalize (object);
}

static void
gtranslator_db_words_class_init (GtranslatorDbWordsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	//g_type_class_add_private (klass, sizeof (GtranslatorDbWordsPrivate));

	object_class->finalize = gtranslator_db_words_finalize;
}

GtranslatorDbWords *
gtranslator_db_words_new ()
{
	GtranslatorDbWords *db_words;

	db_words = g_object_new (GTR_TYPE_DB_WORDS, NULL);
	
	return db_words;
}

gboolean
gtranslator_db_words_append (GtranslatorDbWords *db_words,
			     const gchar *word,
			     guint sentence_size,
			     db_recno_t value)
{
	// VS: there is a dirty trick: it is always true that 'value' is 
	//     greater than all values already present in the db, so we may
	//     append it to the end of list while still keeping the list sorted.
	//     This is important because it allows us to efficiently merge
	//     these lists when looking up inexact translations...
	
	DBT key, data;
	gsize len;
	gsize buf_len;
	GtranslatorDbKeys *keys;
	db_recno_t *value_buf = NULL;
	gint error = 0;
	
	/*
	 * (sentence_size)(len)(word)
	 */
	len = strlen (word);
	buf_len = sizeof (sentence_size) + sizeof (len) + len;
	
	//g_warning ("DEBUG: Append: %d|%d|%d|%s", sizeof (sentence_size), sizeof (len), len, word);
	
	//FIXME: use glib
	u_int8_t *p, buf[buf_len];
	
	p = &buf[0];
	memcpy (p, &sentence_size, sizeof (sentence_size));
	p += sizeof (sentence_size);
	memcpy (p, &len, sizeof (len));
	p += sizeof (len);
	memcpy (p, word, len);
	
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = buf;
	key.size = buf_len;
	
	keys = gtranslator_db_words_read (db_words, word, sentence_size);
	if (keys == NULL)
	{
		data.data = &value;
		data.size = sizeof(value);
	}
	else
	{
		gsize count = gtranslator_db_keys_get_count (keys);
		db_recno_t *list = gtranslator_db_keys_get_list (keys);
		
		value_buf = g_new (db_recno_t, count + 1);
		memcpy(value_buf, list, count * sizeof (db_recno_t));
		value_buf[count] = value;
		data.data = value_buf;
		data.size = (count + 1) * sizeof(db_recno_t);
		
		g_object_unref (keys);
	}

	error = gtranslator_db_base_put (GTR_DB_BASE (db_words),
					 &key,
					 &data,
					 0);
	g_free (value_buf);
	
	if (error != 0)
	{
		gtranslator_db_base_show_error (GTR_DB_BASE (db_words), error);
		return FALSE;
	} 

	return TRUE;
}

GtranslatorDbKeys *
gtranslator_db_words_read (GtranslatorDbWords *db_words,
			   const gchar *word,
			   guint sentence_size)
{
	DBT key, data;
	gsize len;
	gsize buf_len;
	GtranslatorDbKeys *keys;
	gint error = 0;
	
	/*
	 * (sentence_size)(len)(word)
	 */
	len = strlen (word);
	buf_len = sizeof (sentence_size) + sizeof (len) + len;
	//g_warning ("DEBUG: Read: %d|%d|%d|%s", sizeof (sentence_size), sizeof (len), len, word);
	/*
	 * Here we recreate the key and then with that key we get the list
	 * of keys
	 */
	//FIXME: use glib
	u_int8_t *p, buf[buf_len];
	
	p = &buf[0];
	memcpy (p, &sentence_size, sizeof (sentence_size));
	p += sizeof (sentence_size);
	memcpy (p, &len, sizeof (len));
	p += sizeof (len);
	memcpy (p, word, len);
	
	memset (&key, 0, sizeof (key));
	memset (&data, 0, sizeof (data));
	key.data = buf;
	key.size = buf_len;
	
	error = gtranslator_db_base_get (GTR_DB_BASE (db_words),
					 &key,
					 &data);
	if (error != 0)
	{
		gtranslator_db_base_show_error (GTR_DB_BASE (db_words), error);
		return NULL;
	}
	
	keys = gtranslator_db_keys_new (&data);
	
	return keys;
}
