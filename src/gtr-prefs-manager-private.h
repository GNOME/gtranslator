/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * gtr-prefs-manager-private.h
 * This file is part of gtranslator based in gedit
 *
 * Copyright (C) 2002  Paolo Maggi 
 *		 2007  Ignacio Casal Quinteiro
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


#ifndef __GTR_PREFS_MANAGER_PRIVATE_H__
#define __GTR_PREFS_MANAGER_PRIVATE_H__

#include <gconf/gconf-client.h>

typedef struct _GtrPrefsManager GtrPrefsManager;

struct _GtrPrefsManager
{
  GConfClient *gconf_client;
};

extern GtrPrefsManager *gtr_prefs_manager;

#endif /* __GTR_PREFS_MANAGER_PRIVATE_H__ */
