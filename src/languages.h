/**
* Fatih Demir [ kabalak@gmx.net ]
*       
* (C) 2000 Published under GNU GPL V 2.0+
*       
* Language structure and different
*  things related to this ..
*       
* -- the languages defining header
**/

#include <glib.h>
#include <libgnome/gnome-i18n.h>

/**
* A typedef for the language structure
**/
typedef struct _lang lang;

/**
* The language structure 
**/
struct _lang
{
	/**
	* The different informations for a language :
	**/
	/* 1->The name of the language */
	/* I hope that's clear ...     */
	gchar *name;
	/* 2->The language-code */
	/* For example `tr'     */
	gchar *lcode;
	/* 3->The encoding preferably used for this language */
	/*     For example `iso-8859-9` for Turkish          */
	gchar *enc;
	/* 4->The group's( at LI/somewhere else ) EMail for the language */
	/*     For example `tr@li.org' for the Turkish group             */  
	gchar *group;
};

/**
* Nothing changed here, besides that we've switched to the structure
*  instead of the gchar*-array ..
**/
lang languages[] =
{
	{ _("Arabic"),"ar","iso-8859-6","" },
	{ _("Basque"),"eu","iso-8859-1","linux-eu@chanae.alphanet.ch" },
        { _("Bulgarian"),"bg","windows-1251","bg@li.org" },
        { _("Catalan"),"ca","iso-8859-1","" },
        { _("Chinese/Simplified"),"zh_CN","GB2312","cn@linux-mandrake.com" },
        { _("Chinese/Traditional"),"zh_TW","BIG5","zh@li.org" },
	{ _("Croatian"),"hr","iso-8859-2","" },
	{ _("Czech"),"cs","iso-8859-2","" },
        { _("Danish"),"da","iso-8859-1","da@li.org" },
  	{ _("English"),"en","ascii","en@li.org" },
	{ _("English/Canada"),"en","iso-8859-1","en@li.org" },
	{ _("English/UK"),"en","ascii","en@li.org" },
	{ _("English/USA"),"en","ascii","en@li.org" },
	{ _("Estonian"),"et","iso-8859-15","et@li.org" },
	{ _("Farsi"),"fa","isiri-3342","" },
        { _("Finnish"),"fi","iso-8859-1","fi@li.org" },
        { _("French"),"fr","iso-8859-1","fr@li.org" },
        { _("Galician"),"gl","iso-8859-1","gpul-traduccion@ceu.fi.udc.es"},
        { _("German"),"de","iso-8859-1","de@li.org" },
	{ _("Greek"),"el","iso-8859-7","nls@tux.hellug.gr" },
        { _("Hungarian"),"hu","iso-8859-2","" },
        { _("Indonesian"),"id","iso-8859-1","" }, 
        { _("Italian"),"it","iso-8859-1","it@li.org" },
	{ _("Irish"),"ga","","iso-8859-1" },
        { _("Japanese"),"ja","euc-jp","translation@gnome.gr.jp" },
        { _("Korean"),"ko","euc-kr","" },
        { _("Norwegian/Bookmal"),"no","iso-8859-1","no@li.org" },
	{ _("Norwegian/Nynorsk"),"no@nynorsk","iso-8859-1","no@li.org" }
        { _("Polish"),"pl","iso-8859-2","gnomepl@pandora.info.bielsko.pl" },
        { _("Portuguese"),"pt","iso-8859-1","pt@li.org" },
        { _("Portuguese/Brazil"),"pt_BR","iso-8859-1","" },
        { _("Russian"),"ru","koi8-r","ru@li.org" },
	{ _("Slovak"),"sk","iso-8859-2","" },
        { _("Spanish"),"es","iso-8859-1","es@li.org" },
	{ _("Spanish/Spain"),"es_ES","iso-8859-1","es@li.org" },
        { _("Spanish/Mexico"),"es_MX","iso-8859-1","" },
        { _("Swedish"),"sv","iso-8859-1","sv@li.org" },
        { _("Turkish"),"tr","iso-8859-9","tr@li.org" },
        { _("Ukrainian"),"uk","koi8-u","linux@linux.org.ua" },
	{ _("Walloon"),"wa","iso-8859-1","linux-wa@chanae.alphanet.ch" },
        /**
        * This structures-list has to be NULL-terminated
        **/
        { NULL,NULL,NULL,NULL }
};
