/*
 * Copyright (C) 2012 Ignacio Casal Quinteiro <icq@gnome.org>
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
 */

#include "gtr-translation-memory-plugin.h"
#include "gtr-translation-memory-window-activatable.h"
#include "gtr-translation-memory-tab-activatable.h"

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
  gtr_window_activatable_register_peas_type (module);
  gtr_tab_activatable_register_peas_type (module);
}
