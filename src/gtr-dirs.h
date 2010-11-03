/*
 * gtr-dirs.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2008 Ignacio Casal Quinteiro
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
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA. 
 */


#ifndef __GTR_DIRS_H__
#define __GTR_DIRS_H__

#include <glib.h>

G_BEGIN_DECLS
/* This function must be called before starting gtranslator */
void gtr_dirs_init (void);
/* This function must be called before exiting gtranslator */
void gtr_dirs_shutdown (void);


const gchar *gtr_dirs_get_user_config_dir (void);

const gchar *gtr_dirs_get_user_cache_dir (void);

const gchar *gtr_dirs_get_user_plugins_dir (void);

const gchar *gtr_dirs_get_gtr_data_dir (void);

const gchar *gtr_dirs_get_gtr_locale_dir (void);

const gchar *gtr_dirs_get_gtr_lib_dir (void);

const gchar *gtr_dirs_get_gtr_plugins_dir (void);

const gchar *gtr_dirs_get_gtr_plugins_data_dir (void);

const gchar *gtr_dirs_get_gtr_pixmaps_dir (void);

gchar *gtr_dirs_get_ui_file (const gchar * file);

G_END_DECLS
#endif /* __GTR_DIRS_H__ */
/* ex:ts=8:noet: */
