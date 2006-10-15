<?xml version="1.0" encoding="UTF-8" ?>

<xsl:stylesheet version="1.1"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="text" encoding="ISO-8859-1" omit-xml-declaration="yes"/>

  <xsl:template match="/">
<![CDATA[/*
 * (C) 2001-2003 	Fatih Demir <kabalak@kabalak.net>
 * 			GNOME I18N List <gnome-i18n@gnome.org>
 * 			Guntupalli Karunakar <karunakar@freedomink.org>
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

#include "languages.h"

#include <libgnome/gnome-i18n.h>

/*
 * This is the main languages list used in gtranslator -- the languages values
 *  are used in many places in the sources so that this table should be quite
 *   right.
 *
 * Warning/Hint: This file is generated using 'languages.xsl':
 *
 * $ xsltproc languages.xsl /usr/share/xml/iso-codes/iso_639.xml > languages.c
 */
GtrLanguage languages[] = {
]]>
<xsl:for-each select="//iso_639_entry[@iso_639_1_code]">
	{
	 N_("<xsl:value-of select="@name"/>"), "<xsl:value-of select="@iso_639_1_code"/>",
	"", "", ""
	},
</xsl:for-each>
	/*
         * This structures-list has to be NULL-terminated
         */
	{
	 NULL, NULL
	 NULL, NULL, NULL,
	}
};
</xsl:template>

</xsl:stylesheet>
