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

#ifndef __DB_ORIG_H__
#define __DB_ORIG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "gtr-db-base.h"

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_DB_ORIG		(gtr_db_orig_get_type ())
#define GTR_DB_ORIG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_DB_ORIG, GtrDbOrig))
#define GTR_DB_ORIG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_DB_ORIG, GtrDbOrigClass))
#define GTR_IS_DB_ORIG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_DB_ORIG))
#define GTR_IS_DB_ORIG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_DB_ORIG))
#define GTR_DB_ORIG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_DB_ORIG, GtrDbOrigClass))
/* Private structure type */
typedef struct _GtrDbOrigPrivate GtrDbOrigPrivate;

/*
 * Main object structure
 */
typedef struct _GtrDbOrig GtrDbOrig;

struct _GtrDbOrig
{
  GtrDbBase parent_instance;

  /*< private > */
  GtrDbOrigPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrDbOrigClass GtrDbOrigClass;

struct _GtrDbOrigClass
{
  GtrDbBaseClass parent_class;
};

/*
 * Public methods
 */
GType
gtr_db_orig_get_type (void)
  G_GNUC_CONST;

     GType gtr_db_orig_register_type (GTypeModule * module);

     GtrDbOrig *gtr_db_orig_new (void);

     gboolean gtr_db_orig_write (GtrDbOrig * orig,
					 const gchar * string,
					 db_recno_t value);

     db_recno_t gtr_db_orig_read (GtrDbOrig * orig,
					  const gchar * string);

G_END_DECLS
#endif /* __DB_ORIG_H__ */
