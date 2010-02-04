/*
 * Copyright (C) 2008  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#ifndef __DB_WORDS_H__
#define __DB_WORDS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "gtr-db-base.h"
#include "gtr-db-keys.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_DB_WORDS		(gtranslator_db_words_get_type ())
#define GTR_DB_WORDS(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_DB_WORDS, GtranslatorDbWords))
#define GTR_DB_WORDS_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_DB_WORDS, GtranslatorDbWordsClass))
#define GTR_IS_DB_WORDS(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_DB_WORDS))
#define GTR_IS_DB_WORDS_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_DB_WORDS))
#define GTR_DB_WORDS_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_DB_WORDS, GtranslatorDbWordsClass))
/* Private structure type */
typedef struct _GtranslatorDbWordsPrivate GtranslatorDbWordsPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorDbWords GtranslatorDbWords;

struct _GtranslatorDbWords
{
  GtranslatorDbBase parent_instance;

  /*< private > */
  GtranslatorDbWordsPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorDbWordsClass GtranslatorDbWordsClass;

struct _GtranslatorDbWordsClass
{
  GtranslatorDbBaseClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_db_words_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_db_words_register_type (GTypeModule * module);

     GtranslatorDbWords *gtranslator_db_words_new (void);

     gboolean gtranslator_db_words_append (GtranslatorDbWords * db_words,
					   const gchar * word,
					   guint sentence_size,
					   db_recno_t value);

     GtranslatorDbKeys *gtranslator_db_words_read (GtranslatorDbWords *
						   db_words,
						   const gchar * word,
						   guint sentence_size);

G_END_DECLS
#endif /* __DB_WORDS_H__ */
