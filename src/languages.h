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
*
* NOTE : I had to use the N() macro as the table must be constant ...
*
**/
lang languages[] =
{
	{ N_("Arabic"),"ar","iso-8859-6","" },
	{ N_("Basque"),"eu","iso-8859-1","linux-eu@chanae.alphanet.ch" },
        { N_("Bulgarian"),"bg","windows-1251","bg@li.org" },
        { N_("Catalan"),"ca","iso-8859-1","" },
        { N_("Chinese/Simplified"),"zh_CN","GB2312","cn@linux-mandrake.com" },
        { N_("Chinese/Traditional"),"zh_TW","BIG5","zh@li.org" },
	{ N_("Croatian"),"hr","iso-8859-2","" },
	{ N_("Czech"),"cs","iso-8859-2","" },
        { N_("Danish"),"da","iso-8859-1","da@li.org" },
  	{ N_("English"),"en","ascii","en@li.org" },
	{ N_("English/Canada"),"en_CA","iso-8859-1","en@li.org" },
	{ N_("English/GB"),"en_GB","ascii","en@li.org" },
	{ N_("English/USA"),"en","ascii","en@li.org" },
	{ N_("Estonian"),"et","iso-8859-15","et@li.org" },
	{ N_("Farsi"),"fa","isiri-3342","" },
        { N_("Finnish"),"fi","iso-8859-1","fi@li.org" },
        { N_("French"),"fr","iso-8859-1","fr@li.org" },
        { N_("Galician"),"gl","iso-8859-1","gpul-traduccion@ceu.fi.udc.es"},
        { N_("German"),"de","iso-8859-1","de@li.org" },
	{ N_("Greek"),"el","iso-8859-7","nls@tux.hellug.gr" },
        { N_("Hungarian"),"hu","iso-8859-2","" },
        { N_("Indonesian"),"id","iso-8859-1","" }, 
        { N_("Italian"),"it","iso-8859-1","it@li.org" },
	{ N_("Irish"),"ga","","iso-8859-1" },
        { N_("Japanese"),"ja","euc-jp","translation@gnome.gr.jp" },
        { N_("Korean"),"ko","euc-kr","" },
	{ N_("Lithuanian"),"lt","iso-8859-13","komp_lt@konferencijos.lt" },
        { N_("Norwegian/Bookmal"),"no","iso-8859-1","no@li.org" },
	{ N_("Norwegian/Nynorsk"),"no@nynorsk","iso-8859-1","no@li.org" },
        { N_("Polish"),"pl","iso-8859-2","gnomepl@pandora.info.bielsko.pl" },
        { N_("Portuguese"),"pt","iso-8859-1","pt@li.org" },
        { N_("Portuguese/Brazil"),"pt_BR","iso-8859-1","" },
        { N_("Russian"),"ru","koi8-r","ru@li.org" },
	{ N_("Slovak"),"sk","iso-8859-2","" },
        { N_("Spanish"),"es","iso-8859-1","es@li.org" },
	{ N_("Spanish/Spain"),"es_ES","iso-8859-1","es@li.org" },
        { N_("Spanish/Mexico"),"es_MX","iso-8859-1","" },
        { N_("Swedish"),"sv","iso-8859-1","sv@li.org" },
        { N_("Turkish"),"tr","iso-8859-9","tr@li.org" },
        { N_("Ukrainian"),"uk","koi8-u","linux@linux.org.ua" },
	{ N_("Walloon"),"wa","iso-8859-1","linux-wa@chanae.alphanet.ch" },
	{ N_("Welsh"), "cy", "iso-8859-14", "" },
        /**
        * This structures-list has to be NULL-terminated
        **/
        { NULL,NULL,NULL,NULL }
};
