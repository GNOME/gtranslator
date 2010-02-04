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

#ifndef __DB_BASE_H__
#define __DB_BASE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include DB_HEADER

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_DB_BASE		(gtranslator_db_base_get_type ())
#define GTR_DB_BASE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_DB_BASE, GtrDbBase))
#define GTR_DB_BASE_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_DB_BASE, GtrDbBaseClass))
#define GTR_IS_DB_BASE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_DB_BASE))
#define GTR_IS_DB_BASE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_DB_BASE))
#define GTR_DB_BASE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_DB_BASE, GtrDbBaseClass))
/* Private structure type */
typedef struct _GtrDbBasePrivate GtrDbBasePrivate;

/*
 * Main object structure
 */
typedef struct _GtrDbBase GtrDbBase;

struct _GtrDbBase
{
  GObject parent_instance;

  /*< private > */
  GtrDbBasePrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrDbBaseClass GtrDbBaseClass;

struct _GtrDbBaseClass
{
  GObjectClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_db_base_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_db_base_register_type (GTypeModule * module);

     void gtranslator_db_base_create_dabatase (GtrDbBase * base,
					       const gchar * filename,
					       DBTYPE type);

     void gtranslator_db_base_show_error (GtrDbBase * base,
					  gint error);

     gint gtranslator_db_base_put (GtrDbBase * base,
				   DBT * key, DBT * data, u_int32_t flags);

     gint gtranslator_db_base_get (GtrDbBase * base,
				   DBT * key, DBT * data);

G_END_DECLS
#endif /* __DB_BASE_H__ */
