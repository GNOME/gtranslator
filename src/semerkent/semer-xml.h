/*
 * (C) 2001-2003 	Fatih Demir <kabalak@gtranslator.org>
 *
 * semerkent is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Library General Public License as published by the
 *   Free Software Foundation; either version 2 of the License, or (at your
 *    option) any later version.
 *    
 * semerkent is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *    License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * semerkent is being developed around gtranslator, so that any wishes or own
 *  requests should be mailed to me, as I will naturally keep semerkent very
 *   close to gtranslator. This shouldn't avoid any use of semerkent from other
 *    applications/packages.
 *
 */

#ifndef SEMER_XML_H
#define SEMER_XML_H 1

#include <glib.h>

G_BEGIN_DECLS

#include <libxml/parser.h>
#include <libxml/tree.h>

/*
 * Utility functions to handle libxml more easily.
 */
gchar *semer_xml_get_prop(xmlNodePtr node, gchar *prop_name);
gchar *semer_xml_get_content(xmlNodePtr node);

/*
 * Write the main root node.
 */
void semer_xml_set_root_node(gpointer file, xmlDocPtr doc);

/*
 * These 2 functions are taken from eel and changed slightly:
 */
xmlNodePtr semer_xml_get_by_name(xmlNodePtr parent, const gchar *name);
void semer_xml_remove_node(xmlNodePtr node);

/*
 * Compare the document's main root node's name with the given "doctype".
 */
gboolean semer_xml_doctype_cmp(xmlDocPtr doc, gchar *doctypename);

G_END_DECLS
	
#endif /* SEMER_XML_H */
