/*
 * Copyright (C) 2018  Teja Cetinski <teja@cetinski.eu>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <gtk/gtk.h>
#include <adwaita.h>

G_BEGIN_DECLS

#define GTR_TYPE_DL_TEAMS (gtr_dl_teams_get_type())
G_DECLARE_FINAL_TYPE (GtrDlTeams, gtr_dl_teams, GTR, DL_TEAMS,
                      AdwNavigationPage)

GtrDlTeams* gtr_dl_teams_new         (GtrWindow *window);
void        gtr_dl_teams_load_json   (GtrDlTeams *self);

// Domains GObject
#define GTR_TYPE_DL_TEAMS_DOMAIN (gtr_dl_teams_domain_get_type ())
G_DECLARE_FINAL_TYPE (GtrDlTeamsDomain, gtr_dl_teams_domain, GTR, DL_TEAMS_DOMAIN, GObject)

GtrDlTeamsDomain* gtr_dl_teams_domain_new (const char *name, const char *description);
const char* gtr_dl_teams_domain_name (GtrDlTeamsDomain *domain);
const char* gtr_dl_teams_domain_description (GtrDlTeamsDomain *domain);

G_END_DECLS
