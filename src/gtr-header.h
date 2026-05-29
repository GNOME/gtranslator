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

#pragma once

#include <glib-object.h>
#include <glib.h>

#include "gtr-msg.h"
#include "gtr-profile.h"

G_BEGIN_DECLS

/* Utility Macros */

#define GTR_TYPE_HEADER			(gtr_header_get_type ())

G_DECLARE_FINAL_TYPE (GtrHeader, gtr_header, GTR, HEADER, GtrMsg);

struct _GtrHeaderClass
{
  GtrMsgClass parent_class;
};

/* Public methods */
GtrHeader      *gtr_header_new                       (po_message_iterator_t iter,
                                                      po_message_t          message);

const char     *gtr_header_get_comments              (GtrHeader  *header);
void            gtr_header_set_comments              (GtrHeader  *header,
                                                      const char *comments);

char           *gtr_header_get_prj_id_version        (GtrHeader  *header);
void            gtr_header_set_prj_id_version        (GtrHeader  *header,
                                                      const char *prj_id_version);

char           *gtr_header_get_rmbt                  (GtrHeader  *header);
void            gtr_header_set_rmbt                  (GtrHeader  *header,
                                                      const char *rmbt);

char           *gtr_header_get_pot_date              (GtrHeader  *header);
void            gtr_header_set_pot_date              (GtrHeader  *header,
                                                      const char *pot_date);

char           *gtr_header_get_po_date               (GtrHeader  *header);
void            gtr_header_set_po_date               (GtrHeader  *header,
                                                      const char *po_date);

char           *gtr_header_get_translator            (GtrHeader  *header);
char           *gtr_header_get_tr_email              (GtrHeader  *header);
void            gtr_header_set_translator            (GtrHeader  *header,
                                                      const char *name,
                                                      const char *email);

char           *gtr_header_get_language              (GtrHeader  *header);
char           *gtr_header_get_lg_email              (GtrHeader  *header);
void            gtr_header_set_language              (GtrHeader  *header,
                                                      const char *language,
                                                      const char *lang_code,
                                                      const char *email);

char           *gtr_header_get_mime_version          (GtrHeader  *header);
void            gtr_header_set_mime_version          (GtrHeader  *header,
                                                      const char *mime_version);

char           *gtr_header_get_charset               (GtrHeader  *header);
void            gtr_header_set_charset               (GtrHeader  *header,
                                                      const char *charset);

char           *gtr_header_get_encoding              (GtrHeader  *header);
void            gtr_header_set_encoding              (GtrHeader  *header,
                                                      const char *encoding);

char           *gtr_header_get_plural_forms          (GtrHeader  *header);
void            gtr_header_set_plural_forms          (GtrHeader  *header,
                                                      const char *plural_forms);
int             gtr_header_get_nplurals              (GtrHeader  *header);

void            gtr_header_update_header             (GtrHeader  *header);

void            gtr_header_set_profile               (GtrHeader  *header,
                                                      GtrProfile *profile);
GtrProfile     *gtr_header_get_profile               (GtrHeader  *header);

char           *gtr_header_get_language_code         (GtrHeader  *header);

char           *gtr_header_get_dl_lang               (GtrHeader  *header);
char           *gtr_header_get_dl_module             (GtrHeader  *header);
char           *gtr_header_get_dl_branch             (GtrHeader  *header);
char           *gtr_header_get_dl_vcs_web            (GtrHeader  *header);
char           *gtr_header_get_dl_domain             (GtrHeader  *header);
char           *gtr_header_get_dl_state              (GtrHeader  *header);

void            gtr_header_set_dl_info               (GtrHeader  *header,
                                                      const char *lang,
                                                      const char *module_name,
                                                      const char *branch,
                                                      const char *domain,
                                                      const char *vcs_web);

void            gtr_header_set_field                 (GtrHeader  *header,
                                                      const char *field,
                                                      const char *data);

G_END_DECLS
