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

#include "semer-entry.h"
#include "semer-xliff.h"
#include "semer-xml.h"

G_BEGIN_DECLS

/*
 * Internal writing routines.
 */
static void semer_xliff_write_header(SemerFile *file, xmlDocPtr doc);

static void semer_xliff_write_entry(SemerEntry *entry, xmlDocPtr doc);

/*
 * Write the file "header" for the XLIFF file -- there's no header
 *  structure in XLIFF but we'd see the language node as the header .-)
 */
static void semer_xliff_write_header(SemerFile *file, xmlDocPtr doc)
{
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(doc!=NULL);

	/* 
	 * FIXME!
	 */
}

/*
 * Write a single SemerEntry to the xml document.
 */
static void semer_xliff_write_entry(SemerEntry *entry, xmlDocPtr doc)
{
	xmlNodePtr node;
	
	g_return_if_fail(entry!=NULL);
	g_return_if_fail(doc!=NULL);

	node=doc->xmlRootNode->xmlChildrenNode;

	/*
	 * FIXME!
	 */
}

/*
 * Parse the XLIFF file into a SemerFile.
 */
SemerFile *semer_xliff_parse_file(const gchar *filename)
{
	SemerFile *file=g_new0(SemerFile, 1);

	g_return_val_if_fail(filename!=NULL, NULL);

	/*
	 * FIXME!
	 */

	return file;
}

/*
 * Save the SemerFile in a XLIFF form.
 */
void semer_xliff_save_file(SemerFile *file)
{
	xmlDocPtr xliff;
	
	g_return_if_fail(SEMER_FILE(file)!=NULL);
	g_return_if_fail(SEMER_FILE(file)->filename!=NULL);

	/*
	 * Create the main document for the XLIFF informations.
	 */
	xliff=xmlNewDoc("1.0");
	semer_xml_set_root_node(file, xliff);
	semer_xliff_write_header(file, xliff);

	/*
	 * Write out all the nice & nifty SemerEntry's.
	 */
	semer_file_write_all_entries(file, xliff,
		(SemerEntryWriteFunction) semer_xliff_write_entry);

	xmlSaveFormatFile(file->filename, xliff, 2);
	xmlFreeDoc(xliff);
}

G_END_DECLS
