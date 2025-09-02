/*
 * Copyright (C) 2007  Pablo Sanxiao <psanxiao@gmail.com>
 *               2008  Igalia
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
 * Authors:
 *   Pablo Sanxiao <psanxiao@gmail.com>
 *   Ignacio Casal Quinteiro <icq@gnome.org>
 */

#ifndef __HEADER_H__
#define __HEADER_H__

#include <glib.h>
#include <glib-object.h>

#include "gtr-msg.h"
#include "gtr-profile.h"

G_BEGIN_DECLS

/* Utility Macros */

#define GTR_TYPE_HEADER			(gtr_header_get_type ())

G_DECLARE_FINAL_TYPE (GtrHeader, gtr_header, GTR, HEADER, GtrMsg)

struct _GtrHeaderClass
{
  GtrMsgClass parent_class;
};

/* Public methods */
GtrHeader       *gtr_header_new                      (po_message_iterator_t iter,
                                                      po_message_t message);

const gchar     *gtr_header_get_comments             (GtrHeader *header);
void             gtr_header_set_comments             (GtrHeader   *header,
                                                      const gchar *comments);

gchar           *gtr_header_get_prj_id_version       (GtrHeader *header);
void             gtr_header_set_prj_id_version       (GtrHeader   *header,
                                                      const gchar *prj_id_version);

gchar           *gtr_header_get_rmbt                 (GtrHeader *header);
void             gtr_header_set_rmbt                 (GtrHeader   *header,
                                                      const gchar *rmbt);

gchar           *gtr_header_get_pot_date             (GtrHeader *header);
void             gtr_header_set_pot_date             (GtrHeader   *header,
                                                      const gchar *pot_date);

gchar           *gtr_header_get_po_date              (GtrHeader *header);
void             gtr_header_set_po_date              (GtrHeader   *header,
                                                      const gchar *po_date);

gchar           *gtr_header_get_translator           (GtrHeader *header);
gchar           *gtr_header_get_tr_email             (GtrHeader *header);
void             gtr_header_set_translator           (GtrHeader   *header,
                                                      const gchar *name,
                                                      const gchar *email);

gchar           *gtr_header_get_language             (GtrHeader *header);
gchar           *gtr_header_get_lg_email             (GtrHeader *header);
void             gtr_header_set_language             (GtrHeader   *header,
                                                      const gchar *language,
                                                      const gchar *lang_code,
                                                      const gchar *email);

gchar           *gtr_header_get_mime_version         (GtrHeader *header);
void             gtr_header_set_mime_version         (GtrHeader   *header,
                                                      const gchar *mime_version);

gchar           *gtr_header_get_charset              (GtrHeader *header);
void             gtr_header_set_charset              (GtrHeader   *header,
                                                      const gchar *charset);

gchar           *gtr_header_get_encoding             (GtrHeader *header);
void             gtr_header_set_encoding             (GtrHeader   *header,
                                                      const gchar *encoding);

gchar           *gtr_header_get_plural_forms         (GtrHeader *header);
void             gtr_header_set_plural_forms         (GtrHeader   *header,
                                                      const gchar *plural_forms);
gint             gtr_header_get_nplurals             (GtrHeader *header);

void             gtr_header_update_header            (GtrHeader *header);

void             gtr_header_set_profile              (GtrHeader  *header,
                                                      GtrProfile *profile);
GtrProfile      *gtr_header_get_profile              (GtrHeader *header);

gchar           *gtr_header_get_language_code        (GtrHeader * header);

gchar           *gtr_header_get_dl_lang              (GtrHeader * header);
gchar           *gtr_header_get_dl_module            (GtrHeader * header);
gchar           *gtr_header_get_dl_branch            (GtrHeader * header);
char            *gtr_header_get_dl_vcs_web           (GtrHeader  *header);
gchar           *gtr_header_get_dl_domain            (GtrHeader * header);
gchar           *gtr_header_get_dl_state             (GtrHeader * header);

void            gtr_header_set_dl_info               (GtrHeader * header,
                                                      const gchar * lang,
                                                      const gchar * module_name,
                                                      const gchar * branch,
                                                      const gchar * domain,
                                                      const char  * vcs_web);

void             gtr_header_set_field                (GtrHeader   *header,
                                                      const gchar *field,
                                                      const gchar *data);

G_END_DECLS

#endif /* __HEADER_H__ */
