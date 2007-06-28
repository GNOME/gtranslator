/* Gtranslator - Message Area 
 *
 * Copyright (C) 2007 The Free Software Foundation
 *
 * 	Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * Based on gedit code (gedit/gedit-message-area.h) by: 
 * 	- Paolo Maggi <paolo@gnome.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GTRANSLATOR_MESSAGE_AREA_H__
#define __GTRANSLATOR_MESSAGE_AREA_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GtranslatorMessageArea GtranslatorMessageArea;
typedef struct _GtranslatorMessageAreaClass GtranslatorMessageAreaClass;
typedef struct _GtranslatorMessageAreaPrivate GtranslatorMessageAreaPrivate;

#define GTRANSLATOR_TYPE_MESSAGE_AREA            (gtranslator_message_area_get_type())
#define GTRANSLATOR_MESSAGE_AREA(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GTRANSLATOR_TYPE_MESSAGE_AREA, GtranslatorMessageArea))
#define GTRANSLATOR_MESSAGE_AREA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GTRANSLATOR_TYPE_MESSAGE_AREA, GtranslatorMessageAreaClass))
#define GTRANSLATOR_IS_MESSAGE_AREA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTRANSLATOR_TYPE_MESSAGE_AREA))
#define GTRANSLATOR_IS_MESSAGE_AREA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTRANSLATOR_TYPE_MESSAGE_AREA))
#define GTRANSLATOR_MESSAGE_AREA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GTRANSLATOR_TYPE_MESSAGE_AREA, GtranslatorMessageAreaClass))

struct _GtranslatorMessageArea {
	GtkHBox parent;

	GtranslatorMessageAreaPrivate *priv;
};

struct _GtranslatorMessageAreaClass {
	GtkHBoxClass parent_class;

	void (* response) (GtranslatorMessageArea *message_area, 
			   gint            response_id);

	void (* close)    (GtranslatorMessageArea *message_area);
};

GType 		 gtranslator_message_area_get_type 		(void) G_GNUC_CONST;

GtkWidget	*gtranslator_message_area_new      		(void);

GtkWidget	*gtranslator_message_area_new_with_buttons	(const gchar      *first_button_text,
                                        		 ...);

void		 gtranslator_message_area_set_contents	        (GtranslatorMessageArea *message_area,
                                             		 GtkWidget        *contents);
                              		 
void		 gtranslator_message_area_add_action_widget	(GtranslatorMessageArea   *message_area,
                                         		 GtkWidget        *child,
                                         		 gint              response_id);
                                         		 
GtkWidget	*gtranslator_message_area_add_button        	(GtranslatorMessageArea *message_area,
                                         		 const gchar      *button_text,
                                         		 gint              response_id);
             		 
GtkWidget	*gtranslator_message_area_add_stock_button_with_text 
							(GtranslatorMessageArea   *message_area, 
				    			 const gchar      *text, 
				    			 const gchar      *stock_id, 
				    			 gint              response_id);

void       	 gtranslator_message_area_add_buttons 	        (GtranslatorMessageArea *message_area,
                                         		 const gchar      *first_button_text,
                                         		 ...);

void		 gtranslator_message_area_set_response_sensitive 
							(GtranslatorMessageArea   *message_area,
                                        		 gint              response_id,
                                        		 gboolean          setting);
void 		 gtranslator_message_area_set_default_response 
							(GtranslatorMessageArea *message_area,
                                        		 gint              response_id);

void		 gtranslator_message_area_response           	(GtranslatorMessageArea *message_area,
                                    			 gint              response_id);

G_END_DECLS

#endif  /* __GTRANSLATOR_MESSAGE_AREA_H__ */
