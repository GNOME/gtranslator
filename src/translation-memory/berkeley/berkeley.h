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

#ifndef __BERKELEY_H__
#define __BERKELEY_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_BERKELEY		(gtranslator_berkeley_get_type ())
#define GTR_BERKELEY(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_BERKELEY, GtranslatorBerkeley))
#define GTR_BERKELEY_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_BERKELEY, GtranslatorBerkeleyClass))
#define GTR_IS_BERKELEY(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_BERKELEY))
#define GTR_IS_BERKELEY_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_BERKELEY))
#define GTR_BERKELEY_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_BERKELEY, GtranslatorBerkeleyClass))

/* Private structure type */
typedef struct _GtranslatorBerkeleyPrivate	GtranslatorBerkeleyPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorBerkeley		GtranslatorBerkeley;

struct _GtranslatorBerkeley
{
	GObject parent_instance;
	
	/*< private > */
	GtranslatorBerkeleyPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorBerkeleyClass	GtranslatorBerkeleyClass;

struct _GtranslatorBerkeleyClass
{
	GObjectClass parent_class;
};

/*
 * Public methods
 */
GType		 gtranslator_berkeley_get_type	  (void) G_GNUC_CONST;

GType		 gtranslator_berkeley_register_type   (GTypeModule * module);

GtranslatorBerkeley   *gtranslator_berkeley_new	  (void);

G_END_DECLS

#endif /* __BERKELEY_H__ */
