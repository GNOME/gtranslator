/*
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
 */
GtrLanguage languages[] = {
	{
	 N_("Afrikaans"), "af", "iso-8859-1",
	 "", "8bit"
	},
	{
	 N_("Albanian"), "sq", "UTF-8",
	 "gnome-albanian-perkthyesit@lists.sourceforge.net", "8bit"
	},
        {
	 N_("Arabic"), "ar", "iso-8859-6",
	 "", "8bit"
	},
	{
	 N_("Azerbaijani Turkish"), "az", "UTF-8",
	 "linuxaz@azerimal.net", "8bit"
	},
	{
	 N_("Basque"), "eu", "iso-8859-1",
	 "linux-eu@chanae.alphanet.ch", "8bit"
	},
	{
	 N_("Belorussian"), "be", "windows-1251",
	 "", "8bit"
	},
        {
         N_("Bengali"), "bn", "UTF-8",
         "gnome-translation@bengalinux.org", "8bit"
        },		
	{
	 N_("Britton"), "br", "iso-8859-1",
	 "", "8bit"
	},
	{
	 N_("Bulgarian"), "bg", "windows-1251",
	 "bg@li.org", "8bit"
	},
	{
	 N_("Catalan"), "ca", "iso-8859-1",
	 "ca@dodds.net", "8bit"
	},
	{
	 N_("Chinese/Simplified"), "zh_CN", "GB2312",
	 "i18n-translation@lists.linux.net.cn", "8bit"
	},
	{
	 N_("Chinese/Traditional"), "zh_TW", "Big5",
	 "zh-l10n@linux.org.tw", "8bit"
	},
	{
	 N_("Croatian"), "hr", "iso-8859-2",
	 "lokalizacija@linux.hr", "8bit"
	},
	{
	 N_("Czech"), "cs", "iso-8859-2",
	 "cs@li.org", "8bit"
	},
	{
	 N_("Danish"), "da", "iso-8859-1",
	 "dansk@klid.dk", "8bit"
	},
	{
	 N_("Dutch"), "nl", "iso-8859-15",
	 "nl@li.org", "8bit"
	},
	{
	 N_("English"), "en", "iso-8859-1",
	 "en@li.org", "8bit"
	},
	{
	 N_("English/Canada"), "en_CA", "iso-8859-1",
	 "en@li.org", "8bit"
	},
	{
	 N_("English/GB"), "en_GB", "iso-8859-1",
	 "en@li.org", "8bit"
	},
	{
	 N_("English/USA"), "en", "iso-8859-1",
	 "en@li.org", "8bit"
	},
	{ 
	 N_("Esperanto"), "eo", "iso-8859-3",
	 "eo@li.org", "8bit"
	},
	{
	 N_("Estonian"), "et", "iso-8859-15",
	 "et@li.org", "8bit"
	},
	{
	 N_("Persian"), "fa", "UTF-8",
	 "farsi@lists.sharif.edu", "8bit"
	},
	{
	 N_("Finnish"), "fi", "iso-8859-15",
	 "fi@li.org", "8bit"
	},
	{
	 N_("French"), "fr", "iso-8859-1",
	 "gnomefr@traduc.org", "8bit"
	},
	{
	 N_("Galician"), "gl", "iso-8859-1",
	 "trasno@ceu.fi.udc.es", "8bit"
	},
	{
	 N_("German"), "de", "iso-8859-1",
	 "gnome-de@gnome.org", "8bit"
	},
	{
	 N_("Greek"), "el", "iso-8859-7",
	 "nls@tux.hellug.gr", "8bit"
	},
	{
	 N_("Gujarati"), "gu", "UTF-8",
	 "indlinux-gujarati@lists.sourceforge.net", "8bit"
	},
	{
	 N_("Hebrew"), "he", "UTF-8",
	 "", "8bit"
	},
	{
	 N_("Hindi"), "hi", "UTF-8",
	 "indlinux-hindi-gnome@lists.sourceforge.net", "8bit"
	},
	{
	 N_("Hungarian"), "hu", "iso-8859-2",
	 "hu@gnome.hu", "8bit"
	},
	{
	 N_("Icelandic"), "is", "iso-8859-1",
	 "gnome@techattack.nu", "8bit"
	},
	{
	 N_("Indonesian"), "id", "iso-8859-1",
	 "", "8bit"
	},
	{
	 N_("Italian"), "it", "iso-8859-1",
	 "it@li.org", "8bit"
	},
	{
	 N_("Irish"), "ga", "iso-8859-1",
	 "", "8bit"
	},
	{
	 N_("Japanese"), "ja", "euc-jp",
	 "gnome-translation@gnome.gr.jp", "8bit"
	},
	{
	 N_("Kannada"), "kn", "UTF-8",
	 "kannada@sharma-home.net", "8bit"
	},
	{
	 N_("Korean"), "ko", "euc-kr",
	 "ko@li.org", "8bit"
	},
	{
	 N_("Latvian"), "lv", "iso-8859-13",
	 "", "8bit"
	},
	{
	 N_("Lithuanian"), "lt", "iso-8859-13",
	 "komp_lt@konferencijos.lt", "8bit"
	},
	{
	 N_("Malay/Bahasa Melayu"), "ms", "iso-8859-1",
	 "gabai-penyumbang@lists.sourceforge.net", "8bit"
	},
	{
	 N_("Maltese"), "mt", "iso-8859-3",
	 "", "8bit"
	},
	{
	 N_("Marathi"), "mr", "UTF-8",
	 "indlinux-marathi@lists.sourceforge.net", "8bit"
	},
	{
	 N_("Norwegian/Bokmaal"), "no", "iso-8859-1",
	 "i18n-nb@lister.ping.uio.no", "8bit"
	},
	{
	 N_("Norwegian/Nynorsk"), "nn", "iso-8859-1",
	 "i18n-nn@lister.ping.uio.no", "8bit"
	},
	{
	 N_("Punjabi"), "pa", "UTF-8",
	 "indlinux-punjabi@lists.sourceforge.net", "8bit"
	},
	{
	 N_("Polish"), "pl", "iso-8859-2",
	 "gnomepl@pandora.info.bielsko.pl", "8bit"
	},
	{
	 N_("Portuguese"), "pt", "iso-8859-1",
	 "pt@li.org", "8bit"
	},
	{
	 N_("Portuguese/Brazil"), "pt_BR", "iso-8859-1",
	 "", "8bit"
	},
	{
	 N_("Romanian"), "ro", "iso-8859-2",
	 "gnomero-list@lists.sourceforge.net", "8bit"
	},
	{
	 N_("Russian"), "ru", "koi8-r",
	 "ru@li.org", "8bit"
	},
	{
	 N_("Serbian/Cyrillic"), "sp", "iso-8859-5",
	 "", "8bit"
	},
	{
	 N_("Serbian/Latin"), "sr", "iso-8859-2",
	 "", "8bit"
	},
	{
	 N_("Slovak"), "sk", "iso-8859-2",
	 "sk-i18n@linux.sk", "8bit"
	},
	{
	 N_("Slovenian"), "sl", "iso-8859-2",
	 "", "8bit"
	},
	{
	 N_("Spanish"), "es", "iso-8859-1",
	 "es@li.org", "8bit"
	},
	{
	 N_("Spanish/Spain"), "es_ES", "iso-8859-1",
	 "es@li.org", "8bit"
	},
	{
	 N_("Spanish/Mexico"), "es_MX", "iso-8859-1",
	 "", "8bit"
	},
	{
	 N_("Swedish"), "sv", "iso-8859-1",
	 "sv@li.org", "8bit"
	},
	{
	 N_("Tamil"), "ta", "tscii",
	 "", "8bit"
	},
	{
	 N_("Telugu"), "te", "UTF-8",
	 "indlinux-telugu@lists.sourceforge.net", "8bit"
	},
	{
	 N_("Thai"), "th", "tis-620",
	 "", "8bit"
	},
	{
	 N_("Turkish"), "tr", "ISO-8859-9",
	 "gnome-turk@gnome.org", "8bit"
	},
	{
	 N_("Ukrainian"), "uk", "koi8-u",
	 "linux@linux.org.ua", "8bit"
	},
	{
	 N_("Uzbekian"), "uz", "iso-8859-1",
	 "", "8bit"
	},
	{
	 N_("Vietnamese"), "vi", "tcvn-5712",
	 "", "8bit"
	},
	{
	 N_("Walloon"), "wa", "iso-8859-1",
	 "linux-wa@chanae.alphanet.ch", "8bit"
	},
	{
	 N_("Welsh"), "cy", "iso-8859-14",
	 "", "8bit"
	},
	/*
         * This structures-list has to be NULL-terminated
         */
	{
	 NULL, NULL, NULL,
	 NULL, NULL
	}
};
