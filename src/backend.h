/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTR_BACKEND_H
#define GTR_BACKEND_H 1

#include "parse.h"

#include <gmodule.h>

/*
 * Backends integration file with all the converting surface.
 */

/*
 * Informations about the xml file.
 */
typedef struct
{
	gchar 		*modulename;
	const gchar 	*name;
	const gchar 	*description;
	GList	 	*extensions;
	GList		*filenames;

	gboolean	compilable;
	const gchar	*compile_command;
} GtrBackendInformations;

/*
 * The main structure which is also used to access the module functions
 *  directly.
 */
typedef struct 
{
	GtrBackendInformations	*info;
	GFunc			open_file;
	GFunc			save_file;
	GFunc			save_file_as;
} GtrBackend;

#define GTR_BACKEND(x) ((GtrBackend *) x)

#define GTR_BACKEND_INFORMATIONS(x) \
	((GtrBackendInformations *) x)

/*
 * The generally used backends list.
 */
GList *backends;

/*
 * Opening and registering of backends from the given dir -- returns FALSE on
 *  failure.
 */
gboolean gtranslator_backend_open_all_backends(const gchar *directory);

/*
 * Registers a single module with the given xml-file descriptor.
 */
void gtranslator_backend_add(const gchar *filename); 

/*
 * Remove the given backend module.
 */
gboolean gtranslator_backend_remove(GtrBackend **backend);

/*
 * Remove all registered modules and clean up our "namespace".
 */
gboolean gtranslator_backend_remove_all_backends(void);

/*
 * Can we open this file via any registered backend? Returns, yes, rightly 
 *  guessed: TRUE on success/opening of the file.
 */
gboolean gtranslator_backend_open(gchar *filename); 

#endif
