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
#include <gtksourceview/gtksource.h>

G_BEGIN_DECLS
/*
 * Type checking and casting macros
 */
#define GTR_TYPE_VIEW		(gtr_view_get_type ())

G_DECLARE_FINAL_TYPE (GtrView, gtr_view, GTR, VIEW, GtkSourceView)

typedef enum
{
  GTR_SEARCH_DONT_SET_FLAGS = 1 << 0,
  GTR_SEARCH_ENTIRE_WORD = 1 << 1,
  GTR_SEARCH_CASE_SENSITIVE = 1 << 2
} GtrSearchFlags;

/*
 * Main object structure
 */
struct _GtrView
{
  GtkSourceView parent_instance;
};

/*
 * Class definition
 */
struct _GtrViewClass
{
  GtkSourceViewClass parent_class;
};


/*
 * Public methods
 */
     GtkWidget *gtr_view_new (void);

     gboolean gtr_view_get_selected_text (GtrView * view,
                                          gchar ** selected_text, gint * len);

     void gtr_view_enable_visible_whitespace
       (GtrView * view, gboolean enable);

     void gtr_view_cut_clipboard (GtrView * view);

     void gtr_view_copy_clipboard (GtrView * view);

     void gtr_view_paste_clipboard (GtrView * view);

     void gtr_view_set_search_text (GtrView * view,
                                    const gchar * text, guint flags);

     gchar *gtr_view_get_search_text (GtrView * view, guint * flags);

     gboolean gtr_view_get_can_search_again (GtrView * view);

     gboolean gtr_view_search_forward (GtrView * view,
                                       const GtkTextIter * start,
                                       const GtkTextIter * end,
                                       GtkTextIter * match_start,
                                       GtkTextIter * match_end);

     gboolean gtr_view_search_backward (GtrView * view,
                                        const GtkTextIter * start,
                                        const GtkTextIter * end,
                                        GtkTextIter * match_start,
                                        GtkTextIter * match_end);

     gint gtr_view_replace_all (GtrView * view,
                                const gchar * find,
                                const gchar * replace, guint flags);

     void gtr_view_reload_scheme_color (GtrView * view);

     void gtr_view_set_font (GtrView *view, char *font);
     void gtr_view_set_lang (GtrView *view, const char *lang);


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
