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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "htmlizer.h"

#include <string.h>

#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>

/*
 * Returns a html'ified string of the 'original' textstring.
 */
gchar *gtranslator_string_htmlizer(gchar *textstring)
{
	GString *string;
	gint len=0, pif;

	/*
	 * A macro which easifies our work and saves much time.
	 */
	#define EndHtml(x) \
	string=g_string_append_c(string, textstring[x]); \
	string=g_string_append(string, "</font>");
	
	if(!textstring)
	{
		return "";
	}

	len=strlen(textstring);
	
	/*
	 * Create the new GString.
	 */ 
	string=g_string_new("<td>");

	/*
	 * Parse the single string characters.
	 */ 
	for(pif=0;pif < len; ++pif)
	{
		switch(textstring[pif])
		{
			/*
			 * Figures:
			 */ 
			 case '0':
			 case '1':
			 case '2':
			 case '3':
			 case '4':
			 case '5':
			 case '6':
			 case '7':
			 case '8':
			 case '9':
				 
				string=g_string_append(string,
					"<font color=\"orange\">");
				EndHtml(pif);
				break;
		
			/*
			 * Format paremeters & specifiers:
			 */ 
			 case '%':
				
				if(textstring[pif+1]=='l')
				{
					string=g_string_append(string,
						"<font color=\"red\">%");	
					EndHtml(pif+2);
					pif=pif+2;
				}
				else
				{
					string=g_string_append(string,
						"<font color=\"red\">%");
					EndHtml(pif+1);
					pif++;
				}
				break;
		
			/*
			 * Some special words which should be catched;
			 */
			 case 'G':

				if(textstring[pif+1]=='N' 
					&& textstring[pif+2]=='U')
				{
					string=g_string_append(string,
						"<font color=\"navy\">GNU</font>");
					pif=pif+2;
					break;
				}
				else
				{
					if(textstring[pif+1]=='N'
						&& textstring[pif+2]=='O'
						&& textstring[pif+3]=='M'
						&& textstring[pif+4]=='E')
					{
						string=g_string_append(string,
							"<font color=\"navy\">GNOME</font>");
						pif=pif+4;
						break;
					}
					else
					{
						string=g_string_append_c(string,
							textstring[pif]);
						break;
					}
				}
				
			/*
			 * Underscore; the usual hotkey specifier for GNOME.
			 */ 
			 case '_':
				
				string=g_string_append(string,
					"<font color=\"blue\">_");
				EndHtml(pif+1);
				pif++;
				break;
			
			/*
			 * Free space indicators.
			 */
			 case ' ':
			 case '·':
				
				string=g_string_append(string,
					"<font color=\"yellow\">");
				EndHtml(pif);	
				break;
		
			/*
			 * Punctuation characters.
			 */
			 case '.':
			 case ':':
			 case ',':
			 case ';':
			 case '!':
			 case '?':
				
				string=g_string_append(string,
					"<font color=\"grey\">");	
				EndHtml(pif);
				break;
			
			/*
			 * Text marker characters.
			 */
			 case '"':
			 case '\'':	
			 case '`':
				
				string=g_string_append(string,
					"<font color=\"red\">");
				EndHtml(pif);
				break;
			/*
			 * Newlines.
			 */
			 case '\n':

				string=g_string_append(string,
					"<br>");	
				
				EndHtml(pif);
				break;
				
				
			default:
				string=g_string_append_c(string,
					textstring[pif]);
				break;	
		}
	}

	string=g_string_append(string, "</td>");
	
	return string->str;
}

/*
 * Convert the given po file to a html document.
 */ 
void gtranslator_htmlizer(GtrPo *po, gchar *save_to)
{
	FILE *fstream;
	gint n;
	
	g_return_if_fail(po!=NULL);
	
	/*
	 * Ensure a filename.
	 */ 
	if(!save_to)
	{
		save_to=g_strdup_printf("%s.html",
			g_basename(po->filename));
	}

	fstream=fopen(save_to, "w");

	/*
	 * A last stream check.
	 */ 
	if(!fstream)
	{
		g_error(_("Couldn't save html output to %s!"), save_to);
	}

	/*
	 * Produce a nice HTML header.
	 */ 
	fprintf(fstream, "<html>\n<head>\n\t<title>%s -- %s</title>\n\t",
		po->header->prj_name, po->header->prj_version);
	
	fprintf(fstream, "<meta name=\"generator\" content=\"gtranslator\">\n");
	fprintf(fstream, 
		"\t<!-- gtranslator %s htmlizer output -->\n", VERSION);
	
	fprintf(fstream, "</head>\n<body bgcolor=\"white\" text=\"navy\">\n<table align=\"center\" border=\"0\"><tr><td>");
	
	/*
	 * Translatable output to a html file which is visible on the pages.
	 */ 
	fprintf(fstream, _("%s-%s contains %i messages."),
		po->header->prj_name,
		g_basename(po->filename),
		po->length);
	
	fprintf(fstream, "</td><td>&nbsp;</td></tr>\n<tr><td>\n");
	
	/*
	 * A translatable information string also displayed on the html pages.
	 *
	 * NOTE: Please do not forget to also "translate" the <b> tags as the
	 *  project name should be displayed bold on the output pages.
	 */ 
	fprintf(fstream, _("Project <b>%s</b> (last po file revision: %s)."),
		po->header->prj_name,
		po->header->po_date);
	
	fprintf(fstream, "</td><td>&nbsp;</td></tr>\n<tr><td>");

	/*
	 * Hm, surely not all po files do have got a translator field I guess.
	 */
	if(po->header->translator)
	{
		/*
		 * Print out informations about the last translator.
		 */
		fprintf(fstream,
			_("Last translator: %s"), po->header->translator);

		fprintf(fstream, " &lt;<a href=\"mailto:%s\">%s</a>&gt;",
			po->header->tr_email, po->header->tr_email);
	}

	fprintf(fstream, "</td><td>&nbsp;</td></tr>\n</table>\n<hr color=\"navy\" align=\"center\">\n");
	fprintf(fstream, "<table border=\"0\" align=\"center\">\n");
	
	/*
	 * Create a table row for every message pair and print them in plain
	 *  and nice HTML.
	 */  
	for(n=0; n < po->length; ++n)
	{
		fprintf(fstream,
			"<tr><td>%i</td><td>%s</td><td>%s</td></tr>\n",
			n+1,
			gtranslator_string_htmlizer(
			GTR_MSG(g_list_nth_data(po->messages, n))->msgid),
			gtranslator_string_htmlizer(
			GTR_MSG(g_list_nth_data(po->messages, n))->msgstr));
	}
	
	/*
	 * Finish the table and the html file.
	 */ 
	fprintf(fstream, "</table>\n<p align=\"center\">");
	
	/*
	 * This is printed at the end of the HTML page and should also include
	 *  the hyperlink in the translations.
	 *
	 * In the output it looks then like "HTML output of "tr.po" produced
	 *  by gtranslator version 0.39".
	 *
	 * Please don't forget to include the HTML tags in the msgid.
	 */
	fprintf(fstream, _("HTML output of \"%s\" produced by <a href=\"http://www.gtranslator.org\">gtranslator</a> version %s."), 
		po->filename, VERSION);
	fprintf(fstream, "</p></body>\n</html>\n");

	fclose(fstream);
}
