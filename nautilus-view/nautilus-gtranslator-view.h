/*
 * (C) 2001 	Fatih Demir <kabalak@gmx.net>
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
 *
 */

#ifndef NAUTILUS_GTRANSLATOR_VIEW_H
#define NAUTILUS_GTRANSLATOR_VIEW_H

#include <gtk/gtklabel.h>
#include <libnautilus/nautilus-view.h>

#include <libgnome/gnome-i18n.h>

#define NAUTILUS_TYPE_GTRANSLATOR_VIEW	     (nautilus_gtranslator_view_get_type ())
#define NAUTILUS_GTRANSLATOR_VIEW(obj)	     (GTK_CHECK_CAST ((obj), NAUTILUS_TYPE_GTRANSLATOR_VIEW, NautilusGtranslatorView))
#define NAUTILUS_GTRANSLATOR_VIEW_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), NAUTILUS_TYPE_GTRANSLATOR_VIEW, NautilusGtranslatorViewClass))
#define NAUTILUS_IS_GTRANSLATOR_VIEW(obj)	     (GTK_CHECK_TYPE ((obj), NAUTILUS_TYPE_GTRANSLATOR_VIEW))
#define NAUTILUS_IS_GTRANSLATOR_VIEW_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), NAUTILUS_TYPE_GTRANSLATOR_VIEW))

typedef struct NautilusGtranslatorViewDetails NautilusGtranslatorViewDetails;

typedef struct {
	NautilusView parent;
	NautilusGtranslatorViewDetails *details;
} NautilusGtranslatorView;

typedef struct {
	NautilusViewClass parent;
} NautilusGtranslatorViewClass;

GtkType       nautilus_gtranslator_view_get_type          (void);

#endif
