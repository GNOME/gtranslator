/*
 * (C) 2001-2002 	Fatih Demir <kabalak@gtranslator.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef GTR_LEARN_H
#define GTR_LEARN_H 1

#include "parse.h"

/*
 * Learn made translations and store them into a personal file.
 */

/*
 * Initialize the learn process ,-)
 */
void gtranslator_learn_init(void);
gboolean gtranslator_learn_initialized(void);

/*
 * Learns the given GtrPo file completely -- every translated message
 *  is learned.
 */
void gtranslator_learn_po_file(GtrPo *po_file);

/*
 * Export the learn buffer into a standard gettext po file.
 */
void gtranslator_learn_export_to_po_file(const gchar *po_file);

/*
 * Learn the given strings.
 */
void gtranslator_learn_string(const gchar *id_string, const gchar *str_string);

/*
 * Return whether the given string was already "learned".
 */
gboolean gtranslator_learn_learned(const gchar *string);

/*
 * Returns a gchar if there are any matches; returns 1st matching gchar or
 *  NULL.
 */
gchar *gtranslator_learn_get_learned_string(const gchar *search_string);

/*
 * Translates the missing msgstr if the msgid string had been already learned.
 */
void gtranslator_learn_translate(gpointer gtr_msg_gpointer);

/*
 * Autotranslate the opened po file -- the gboolean arg defines some plays
 *  for the then visible GUI.
 */
void gtranslator_learn_autotranslate(gboolean visual_interface);

/*
 * Shutdown our learn process.
 */
void gtranslator_learn_shutdown(void);

#endif