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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "formats.h"
#include "runtime-config.h"
#include "utils.h"

#include <stdlib.h>

#include <gtk/gtkmain.h>

/*
 * Checks whether the formats are right or wrong .-)
 */
gboolean gtranslator_formats_check(GtrMsg *message)
{
	gint	 result;
	gchar 	*cmd_str;

	g_return_val_if_fail(GTR_MSG(message)!=NULL, FALSE);
	g_return_val_if_fail(GTR_MSG(message)->msgid!=NULL, FALSE);

	/*
	 * A quite backward method to check the messages for correctness -- but
	 *  this should work as we do "simply" use msgfmt.
	 */
	cmd_str=g_strdup_printf("echo '# Temporary gtranslator formats compiling file.\n"
		"msgid \"\"\n"
		"msgstr \"\"\n"
		"\"Project-Id-Version: TemporaryCompileFile\\n\"\n"
		"\"POT-Creation-Date: 2001-12-27 19:48+0200\\n\"\n"
		"\"PO-Revision-Date: 2001-12-27 19:48+0300\\n\"\n"
		"\"Last-Translator: gtranslator <gtranslator-devel@lists.sourceforge.net>\\n\"\n"
		"\"Language-Team: gtranslator <gtranslator-devel@lists.sourceforge.net>\\n\"\n"
		"\"MIME-Version: 1.0\\n\"\n"
		"\"Content-Type: text/plain; charset=%s\\n\"\n"
		"\"Content-Transfer-Encoding: 8bit\\n\"\n"
		"\n"
		"msgid \"%s\"\n"
		"msgstr \"%s\"' > '%s' | msgfmt -c '%s' -o /dev/null 2>/dev/null",
		((GTR_HEADER(po->header)->charset) ? GTR_HEADER(po->header)->charset : po->locale_charset),
		message->msgid, ((message->msgstr) ? message->msgstr : ""),
		gtranslator_runtime_config->check_filename,
		gtranslator_runtime_config->check_filename);

	result=system(cmd_str);

	/*
	 * Iterate for the main loop -- this _should_ avoid some unwated
	 *  GIMP GUI "blur" effects.
	 */
	while(gtk_events_pending())
	{
		gtk_main_iteration();
	}

	GTR_FREE(cmd_str);

	if(!result)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
