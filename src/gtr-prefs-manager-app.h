/*
 * gedit-prefs-manager-app.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2002-2005  Paolo Maggi 
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


#ifndef __GTR_PREFS_MANAGER_APP_H__
#define __GTR_PREFS_MANAGER_APP_H__

#include <glib.h>

#include "gtr-prefs-manager.h"

/** LIFE CYCLE MANAGEMENT FUNCTIONS **/

gboolean gtranslator_prefs_manager_app_init (void);

/* This function must be called before exiting gtranslator */
void gtranslator_prefs_manager_app_shutdown (void);


/* Window state */
gint gtranslator_prefs_manager_get_window_state (void);
void gtranslator_prefs_manager_set_window_state (gint ws);
gboolean gtranslator_prefs_manager_window_state_can_set (void);

/* Window size */
void gtranslator_prefs_manager_get_window_size (gint * width, gint * height);
void gtranslator_prefs_manager_get_default_window_size (gint * width,
							gint * height);
void gtranslator_prefs_manager_set_window_size (gint width, gint height);
gboolean gtranslator_prefs_manager_window_size_can_set (void);

/* Side panel */
gint gtranslator_prefs_manager_get_side_panel_size (void);
gint gtranslator_prefs_manager_get_default_side_panel_size (void);
void gtranslator_prefs_manager_set_side_panel_size (gint ps);
gboolean gtranslator_prefs_manager_side_panel_size_can_set (void);
gint gtranslator_prefs_manager_get_side_panel_active_page (void);
void gtranslator_prefs_manager_set_side_panel_active_page (gint id);
gboolean gtranslator_prefs_manager_side_panel_active_page_can_set (void);

/* Comment pane */
gint gtranslator_prefs_manager_get_comment_pane_pos (void);
gint gtranslator_prefs_manager_get_default_comment_pane_pos (void);
void gtranslator_prefs_manager_set_comment_pane_pos (gint new_pane_pos);


#endif /* __GTR_PREFS_MANAGER_APP_H__ */
