/*
 * gtr-status-combo-box.h
 * This file is part of gtr
 *
 * Copyright (C) 2008  Jesse van den Kieboom
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef __GTR_STATUS_COMBO_BOX_H__
#define __GTR_STATUS_COMBO_BOX_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTR_TYPE_STATUS_COMBO_BOX		(gtr_status_combo_box_get_type ())
#define GTR_STATUS_COMBO_BOX(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_STATUS_COMBO_BOX, GtrStatusComboBox))
#define GTR_STATUS_COMBO_BOX_CONST(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTR_TYPE_STATUS_COMBO_BOX, GtrStatusComboBox const))
#define GTR_STATUS_COMBO_BOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTR_TYPE_STATUS_COMBO_BOX, GtrStatusComboBoxClass))
#define GTR_IS_STATUS_COMBO_BOX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTR_TYPE_STATUS_COMBO_BOX))
#define GTR_IS_STATUS_COMBO_BOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_STATUS_COMBO_BOX))
#define GTR_STATUS_COMBO_BOX_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTR_TYPE_STATUS_COMBO_BOX, GtrStatusComboBoxClass))

typedef struct _GtrStatusComboBox		GtrStatusComboBox;
typedef struct _GtrStatusComboBoxClass	GtrStatusComboBoxClass;
typedef struct _GtrStatusComboBoxClassPrivate	GtrStatusComboBoxClassPrivate;

struct _GtrStatusComboBox
{
	GtkEventBox parent;
};

struct _GtrStatusComboBoxClass
{
	GtkEventBoxClass parent_class;

	GtrStatusComboBoxClassPrivate *priv;
	
	void (*changed) (GtrStatusComboBox *combo,
			 GtkMenuItem         *item);
};

GType gtr_status_combo_box_get_type 			(void) G_GNUC_CONST;
GtkWidget *gtr_status_combo_box_new			(const gchar 		*label);

const gchar *gtr_status_combo_box_get_label 		(GtrStatusComboBox 	*combo);
void gtr_status_combo_box_set_label 			(GtrStatusComboBox 	*combo,
							 const gchar         	*label);

void gtr_status_combo_box_add_item 			(GtrStatusComboBox 	*combo,
							 GtkMenuItem         	*item,
							 const gchar         	*text);
void gtr_status_combo_box_remove_item			(GtrStatusComboBox    *combo,
							 GtkMenuItem            *item);

GList *gtr_status_combo_box_get_items			(GtrStatusComboBox    *combo);
const gchar *gtr_status_combo_box_get_item_text 	(GtrStatusComboBox	*combo,
							 GtkMenuItem		*item);
void gtr_status_combo_box_set_item_text 		(GtrStatusComboBox	*combo,
							 GtkMenuItem		*item,
							 const gchar            *text);

void gtr_status_combo_box_set_item			(GtrStatusComboBox	*combo,
							 GtkMenuItem		*item);

GtkLabel *gtr_status_combo_box_get_item_label		(GtrStatusComboBox	*combo);

G_END_DECLS

#endif /* __GTR_STATUS_COMBO_BOX_H__ */

/* ex:set ts=8 noet: */
