/*
 * (C) 2001-2002 	Fatih Demir <kabalak@kabalak.net>
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

#ifndef GTR_SYNTAX_ELEMENTS_H
#define GTR_SYNTAX_ELEMENTS_H 1

/*
 * All highlighting words and/or patterns are declared here.
 */

#include <glib.h>

#ifdef NOT_PORTED
/*
 * The keywords which should be highlighted.
 */
static gchar *keywords[] = {
	"Authors",
	"Author",
	"BUGS",
	"C++",
	"C#",
	"Cent",
	"ChangeLog",
	"Contributors",
	"Contributor",
	"COPYING",
	"COPYING.LIB",
	"Copying",
	"Ct ",
	"Ct.",
	"CVS",
	"Dollar",
	"EMAIL",
	"EURO",
	"Euro ",
	"Euro.",
	"FAQ",
	"FDL",
	"FIXME",
	"FTP",
	"GDK",
	"Gdk",
	"Glib",
	"GLIB",
	"GNU",
	"Gnu",
	"GNOME",
	"Gnome",
	"GPL",
	"Gtk+",
	"GTK+",
	"Gtranslator",
	"gtranslator",
	"HEAD",
	"HELP",
	"HTML",
	"HTTP",
	"INSTALL",
	"JAVA",
	"Java",
	"JDK",
	"KDE",
	"Kde",
	"LICENSE",
	"License",
	"LGPL",
	"NEWS",
	"NULL",
	"OpenTag",
	"README",
	"THANKS",
	"TMX",
	"TODO",
	"UMTF",
	"URI",
	"URL",
	"Url",
	"URN",
	"XLIFF",
	"XML",
	"XMMS",
	"X11",
	"WWW",
	"WEB",
	"www",
	NULL
};

/*
 * The URL/URI prefixes which should be highlighted.
 */
static gchar *prefixes[] =  {
	"file:",
	"ftp:",
	"ghelp:",
	"gopher:",
	"gnome-help:",
	"gnome-search:",
	"https:",
	"http:",
	"icecast:",
	"imap:",
	"info:",
	"mailto:",
	"man:",
	"medusa:",
	"my-portal:",
	"news:",
	"nfs:",
	"nntp:",
	"pop:",
	"pop3:",
	"search:",
	"smtp:",
	"telnet:",
	"toc:",
	"uri:",
	"urn:",
	"vfolder:",
	"whois:",
	"wysiwyg:",
	NULL
};

#endif

#endif
