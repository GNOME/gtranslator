/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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
 *
 */

#ifndef __VIEW_H__
#define __VIEW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_VIEW		(gtranslator_view_get_type ())
#define GTR_VIEW(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_VIEW, GtranslatorView))
#define GTR_VIEW_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_VIEW, GtranslatorViewClass))
#define GTR_IS_VIEW(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_VIEW))
#define GTR_IS_VIEW_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_VIEW))
#define GTR_VIEW_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_VIEW, GtranslatorViewClass))

typedef enum
{
        GTR_SEARCH_DONT_SET_FLAGS     = 1 << 0, 
        GTR_SEARCH_ENTIRE_WORD        = 1 << 1,
        GTR_SEARCH_CASE_SENSITIVE     = 1 << 2

} GtranslatorSearchFlags;

/* Private structure type */
typedef struct _GtranslatorViewPrivate	GtranslatorViewPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorView		GtranslatorView;

struct _GtranslatorView
{
	GtkSourceView parent_instance;
	
	/*< private > */
	GtranslatorViewPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorViewClass	GtranslatorViewClass;

struct _GtranslatorViewClass
{
	GtkSourceViewClass parent_class;
};


/*
 * Public methods
 */
GType		 gtranslator_view_get_type               (void) G_GNUC_CONST;

GType		 gtranslator_view_register_type          (GTypeModule * module);

GtkWidget	*gtranslator_view_new                    (void);

gboolean         gtranslator_view_get_selected_text      (GtranslatorView *view,
							  gchar         **selected_text,
							  gint           *len);

void             gtranslator_view_enable_spellcheck      (GtranslatorView *view,
							  gboolean enable);

void             gtranslator_view_enable_visible_whitespace
                                                         (GtranslatorView *view,
							  gboolean enable);

void             gtranslator_view_cut_clipboard          (GtranslatorView *view);

void             gtranslator_view_copy_clipboard         (GtranslatorView *view);

void             gtranslator_view_paste_clipboard        (GtranslatorView *view);

void             gtranslator_view_set_font               (GtranslatorView   *view, 
							  gboolean     def, 
							  const gchar *font_name);

void             gtranslator_view_set_search_text        (GtranslatorView *view,
							  const gchar   *text,
							  guint          flags);

gchar           *gtranslator_view_get_search_text        (GtranslatorView *view,
							  guint         *flags);

void             gtranslator_view_reload_scheme_color    (GtranslatorView *view);


/* Search macros */
#define GTR_SEARCH_IS_DONT_SET_FLAGS(sflags) ((sflags & GTR_SEARCH_DONT_SET_FLAGS) != 0)
#define GTR_SEARCH_SET_DONT_SET_FLAGS(sflags,state) ((state == TRUE) ? \
(sflags |= GTR_SEARCH_DONT_SET_FLAGS) : (sflags &= ~GTR_SEARCH_DONT_SET_FLAGS))

#define GTR_SEARCH_IS_ENTIRE_WORD(sflags) ((sflags & GTR_SEARCH_ENTIRE_WORD) != 0)
#define GTR_SEARCH_SET_ENTIRE_WORD(sflags,state) ((state == TRUE) ? \
(sflags |= GTR_SEARCH_ENTIRE_WORD) : (sflags &= ~GTR_SEARCH_ENTIRE_WORD))

#define GTR_SEARCH_IS_CASE_SENSITIVE(sflags) ((sflags &  GTR_SEARCH_CASE_SENSITIVE) != 0)
#define GTR_SEARCH_SET_CASE_SENSITIVE(sflags,state) ((state == TRUE) ? \
(sflags |= GTR_SEARCH_CASE_SENSITIVE) : (sflags &= ~GTR_SEARCH_CASE_SENSITIVE))

G_END_DECLS

#endif /* __VIEW_H__ */
