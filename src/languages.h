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
const lang languages[] =
{
	/**
	* Does someone know a package where almost every
	*  language has got a translation ( eog/Gtk+ ? ) .. 
	**/
	{ _("Arabic"),"ar","","" },
        { _("Bulgarian"),"bg","iso8859-2","bg@li.org" },
        { _("Catalan"),"ca","iso-8859-1","" },
        { _("Chinese/Simplified"),"zh_CN","GB2312","" },
        { _("Chinese/Traditional"),"zh_TW","BIG5","" },
	{ _("Czech"),"cs","iso-8859-2","" },
        { _("Danish"),"da","iso-8859-1","da@li.org" },
  	{ _("English"),"en","ascii","en@li.org" },
	{ _("English/Canada"),"en","iso-8859-1","en@li.org" },
	{ _("English/UK"),"en","ascii","en@li.org" },
	{ _("English/USA"),"en","ascii","en@li.org" },
	{ _("Estonian"),"et","","et@li.org" },
	{ _("Farsi"),"fa","","" },
        { _("Finnish"),"fi","iso-8859-1","fi@li.org" },
        { _("French"),"fr","iso-8859-1","fr@li.org" },
        { _("Galician"),"ga","","ga@li.org"},
        { _("German"),"de","iso-8859-1","de@li.org" },
	{ _("Greek"),"el","iso-8859-7","nls@tux.hellug.gr" },
        { _("Hungarian"),"hr","","" },
        { _("Indonesian"),"","","" }, 
        { _("Italian"),"it","iso-8859-1","it@li.org" },
        { _("Japanese"),"ja","euc-jp","translation@gnome.gr.jp" },
        { _("Korean"),"ko","","" },
        { _("Norwegian"),"no","iso-8859-1","no@li.org" },
        { _("Polish"),"pl","iso-8859-2","" },
        { _("Portuguese"),"pt","iso-8859-1","pt@li.org" },
        { _("Portuguese/Brazil"),"pt_BR","iso-8859-1","" },
        { _("Russian"),"ru","koi-8r","ru@li.org" },
	{ _("Slovak"),"sk","iso-8859-2","" },
        { _("Spanish"),"es","iso-8859-1","es@li.org" },
        { _("Spanish/Mexico"),"es_MX","iso-8859-1","" },
        { _("Swedish"),"sv","iso-8859-1","sv@li.org" },
        { _("Turkish"),"tr","iso-8859-9","tr@li.org" },
        { _("Ukrainian"),"uk","","" },
        /**
        * This structures-list has to be NULL-terminated
        **/
        { NULL,NULL,NULL,NULL }
};
