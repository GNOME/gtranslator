/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gtranslator.org>
 *			Gediminas Paulauskas <menesis@gtranslator.org>
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

#include "actions.h"
#include "dialogs.h"
#include "gui.h"
#include "header_stuff.h"
#include "languages.h"
#include "nautilus-string.h"
#include "parse.h"
#include "prefs.h"
#include "utils.h"

#include <locale.h>
#include <time.h>

#include <gtk/gtk.h>
#include <libgnomeui/gnome-uidefs.h>
#include <libgnomeui/gnome-propertybox.h>

static GtkWidget *e_header = NULL;

static GtkWidget *prj_page, *lang_page, *lang_vbox;

static GtkWidget *prj_name, *prj_version, *prj_comment, *take_my_options;
static GtkWidget *translator, *tr_email, *pot_date, *po_date;
static GtkWidget *language_combo, *charset_combo, *enc_combo, *lg_combo;

/*
 * These are defined below 
 */
static void gtranslator_header_edit_apply(GtkWidget * box, gint page_num, gpointer useless);
static void take_my_options_toggled(GtkWidget * widget, gpointer useless);
static void gtranslator_header_edit_changed(GtkWidget * widget, gpointer useless);
static void language_changed(GtkWidget * widget, gpointer useless);
static const gchar * language_in_english(const gchar *lang);
static void substitute(gchar **item, const gchar *bad, const gchar *good);
static void replace_substring(gchar **item, const gchar *bad, const gchar *good);

static void split_name_email(const gchar * str, gchar ** name, gchar ** email)
{
	regex_t *rx;
	regmatch_t m[3];
	
	if (!str) {
		*name = g_strdup("");
		*email = g_strdup("");
		return;
	}
	
	rx = gnome_regex_cache_compile(rxc,
		"(.+) <([a-zA-Z0-9._-]+@[a-zA-Z0-9._-]+)>", 
		REG_EXTENDED);
	if (!regexec(rx, str, 3, m, 0)) {
		if (m[1].rm_so != -1)
			*name = g_strndup(str+m[1].rm_so, 
					  m[1].rm_eo - m[1].rm_so);
		if (m[2].rm_so != -2)
			*email = g_strndup(str+m[2].rm_so, 
					  m[2].rm_eo - m[2].rm_so);
	}
}

GtrHeader * gtranslator_header_get(GtrMsg * msg)
{
	GtrHeader *ph;
	gchar **lines, **pair;
	gint i = 0;

	if(!msg->msgstr)
		return NULL;
	
	ph = g_new0(GtrHeader, 1);
	lines = g_strsplit(msg->msgstr, "\n", 0);
	while (lines[i] != NULL) {
		pair = g_strsplit(lines[i], ": ", 2);

#define if_key_is(str) if (!g_strcasecmp(pair[0],str))
		if_key_is("Project-Id-Version") {
			regex_t *rx;
			regmatch_t m[3];
			
			rx = gnome_regex_cache_compile(rxc,
				"(.+) +([[:alnum:]._-]+)$", 
				REG_EXTENDED);
			if (!regexec(rx, pair[1], 3, m, 0)) {
				if (m[1].rm_so != -1)
					ph->prj_name = 
					    g_strndup(pair[1]+m[1].rm_so,
						      m[1].rm_eo - m[1].rm_so);
				if (m[2].rm_so != -1)
					ph->prj_version =
					    g_strdup(pair[1]+m[2].rm_so);
			} else {
		  	  ph->prj_name = g_strdup(pair[1]);
			  ph->prj_version = g_strdup("");
			}
		}
		else
		if_key_is("POT-Creation-Date")
		    ph->pot_date = g_strdup(pair[1]);
		else
		if_key_is("PO-Revision-Date")
		    ph->po_date = g_strdup(pair[1]);
		else
		if_key_is("Last-Translator")
		    split_name_email(pair[1], &ph->translator, &ph->tr_email);
		else
		if_key_is("Language-Team")
		    split_name_email(pair[1], &ph->language, &ph->lg_email);
		else
		if_key_is("MIME-Version")
		    ph->mime_version = g_strdup(pair[1]);
		else
		if_key_is("Content-Type") {
			regex_t *rx;
			regmatch_t m[2];
			
			rx = gnome_regex_cache_compile(rxc,
				"text/plain; charset=(.+)$", REG_EXTENDED);
			if (!regexec(rx, pair[1], 2, m, 0)) {
				if (m[1].rm_so != -1)
					ph->charset =
					    g_strdup(pair[1]+m[1].rm_so);
			}
		}
		else
		if_key_is("Content-Transfer-Encoding")
		    ph->encoding = g_strdup(pair[1]);
		else
		g_print
		    ("New header entry found (please add it to header_stuff.c):\n%s\n",
		     pair[0]);

		g_strfreev(pair);
		i++;
	}
	g_strfreev(lines);
	ph->comment = g_strdup(msg->comment);
	if (ph->prj_name)
		return ph;
	else
		return NULL;
}

/*
 * Get the non-localized name for the language, if available
 */ 
const gchar * language_in_english(const gchar *lang)
{
	if(lang)
	{
		gint c;
		for(c=0;languages[c].name!=NULL;c++)
		{
			if(!strcmp(_(languages[c].name), lang))
			{
				return languages[c].name;
			}
		}

		/*
		 * Return the original language if no conversion could be made.
		 */
		return lang;
	}
	else
	{
		/*
		 * Return the language in the preferences it no language was
		 *  given/defined.
		 */
		return language;
	}
	
}		
/*
 * Creates new GtrMsg, with all data set to current state of header 
 */
GtrMsg * gtranslator_header_put(GtrHeader * h)
{
	gchar *group;
	GtrMsg *msg = g_new0(GtrMsg, 1);
	gchar *version;

	const gchar *lang=language_in_english(h->language);

	if (h->lg_email && h->lg_email[0] != '\0')
		group = g_strdup_printf("%s <%s>", lang, h->lg_email);
	else
		group = g_strdup(lang);

	if (h->prj_version && h->prj_version[0] != '\0')
		version = g_strdup_printf("%s %s", h->prj_name, h->prj_version);
	else
		version = g_strdup(h->prj_name);
	
	msg->msgstr = g_strdup_printf("\
Project-Id-Version: %s\n\
POT-Creation-Date: %s\n\
PO-Revision-Date: %s\n\
Last-Translator: %s <%s>\n\
Language-Team: %s\n\
MIME-Version: %s\n\
Content-Type: text/plain; charset=%s\n\
Content-Transfer-Encoding: %s\n",
		version,
		h->pot_date,
		h->po_date,
		h->translator, h->tr_email,
		group,
		h->mime_version,
		h->charset,
		h->encoding);

	g_free(group);
	g_free(version);

	/*
	 * Just copy the comment, and make sure it ends with endline
	 */
	if(h->comment[strlen(h->comment)-1] == '\n')
	{
		msg->comment = g_strdup(h->comment);
	}
	else
	{
		msg->comment = g_strconcat(h->comment, "\n", NULL);
	}
	
	return msg;
}

/*
 * Updates PO-Revision-Date field 
 */
void gtranslator_header_update(GtrHeader * h)
{
	time_t now;
	struct tm *now_here;
	char t[22];
	
	/*
	 * Update the po date 
	 */
	now = time(NULL);
	now_here = localtime(&now);
	strftime(t, 22, "%Y-%m-%d %H:%M%z", now_here);
	g_free(h->po_date);
	h->po_date = g_strdup(t);

	/*
	 * Convert the header comments back if necessary.
	 * FIXME: Why here??
	 */ 
	if(h->comment && h->comment[0]!='#')
	{
		h->comment=gtranslator_header_comment_convert_for_save(h->comment);
	}
}

void gtranslator_header_free(GtrHeader * h)
{
	if (h == NULL)
		return;
	g_free(h->comment);
	g_free(h->prj_name);
	g_free(h->prj_version);
	g_free(h->pot_date);
	g_free(h->po_date);
	g_free(h->translator);
	g_free(h->tr_email);
	g_free(h->language);
	g_free(h->lg_email);
	g_free(h->mime_version);
	g_free(h->charset);
	g_free(h->encoding);
	g_free(h);
}

static void gtranslator_header_edit_apply(GtkWidget * box, gint page_num, gpointer useless)
{
	GtrHeader *ph = po->header;
	if (page_num != -1)
		return;
#define update(value,widget) g_free(value);\
	value = gtk_editable_get_chars(GTK_EDITABLE(widget),0,-1);
	update(ph->prj_name, prj_name);
	update(ph->prj_version, prj_version);
	update(ph->comment, prj_comment);

	/*
	 * Convert the header comment back for save and substitute the
	 *  PACKAGE and VERSION fields in the header.
	 */
	ph->comment=gtranslator_header_comment_convert_for_save(ph->comment);
	replace_substring(&ph->comment, "PACKAGE", ph->prj_name);
	replace_substring(&ph->comment, "VERSION", ph->prj_version);
	
	if (!wants.fill_header) {
		update(ph->translator, translator);
		update(ph->tr_email, tr_email);
		update(ph->language, GTK_COMBO(language_combo)->entry);
		update(ph->lg_email, GTK_COMBO(lg_combo)->entry);
		update(ph->charset, GTK_COMBO(charset_combo)->entry);
		update(ph->encoding, GTK_COMBO(enc_combo)->entry);
#undef update
	} else {
#define replace(what,with,entry) g_free(what); what = g_strdup(with);\
	gtk_entry_set_text(GTK_ENTRY(entry), with);
		replace(ph->translator, author, translator);
		replace(ph->tr_email, email, tr_email);
		replace(ph->language, language,
			GTK_COMBO(language_combo)->entry);
		replace(ph->lg_email, lg, GTK_COMBO(lg_combo)->entry);
		replace(ph->charset, mime, GTK_COMBO(charset_combo)->entry);
		replace(ph->encoding, enc, GTK_COMBO(enc_combo)->entry);
#undef replace
	}
	if (!po->file_changed) {
		po->file_changed = TRUE;
		gtranslator_actions_enable(ACT_SAVE, ACT_REVERT);
	}
}

/*
 * Creates the Header-edit dialog.
 */
void gtranslator_header_edit_dialog(GtkWidget * widget, gpointer useless)
{
	GtrHeader *ph = po->header;
	GtkWidget *label;

	gtranslator_raise_dialog(e_header);
	e_header = gnome_property_box_new();
	gtk_window_set_title(GTK_WINDOW(e_header),
			     _("gtranslator -- edit header"));

	gtranslator_utils_language_lists_create();

	prj_page = gtranslator_utils_append_page_to_preferences_dialog(e_header, 5, 2, _("Project"));
	label = gtk_label_new(_("Translator and Language"));
	lang_vbox = gtk_vbox_new(FALSE, GNOME_PAD);
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(e_header), lang_vbox,
				       label);
	
	/*
	 * Prepare the header comment for view and edit in the dialog. 
	 */
	ph->comment=gtranslator_header_comment_convert_for_view(ph->comment);
	
	prj_comment =
	    gtranslator_utils_attach_text_with_label(prj_page, 0, _("Comments:"), ph->comment,
	    			   gtranslator_header_edit_changed);
	
	gtk_widget_set_usize(prj_comment, 360, 90);
	
	prj_name =
	    gtranslator_utils_attach_entry_with_label(prj_page, 1, _("Project name:"),
	    			    ph->prj_name, gtranslator_header_edit_changed);
	prj_version =
	    gtranslator_utils_attach_entry_with_label(prj_page, 2, _("Project version:"),
	    			    ph->prj_version, gtranslator_header_edit_changed);
	pot_date =
	    gtranslator_utils_attach_entry_with_label(prj_page, 3, _("Pot file creation date:"),
				    ph->pot_date, gtranslator_header_edit_changed);
	gtk_widget_set_sensitive(pot_date, FALSE);
	po_date =
	    gtranslator_utils_attach_entry_with_label(prj_page, 4, _("Po file revision date:"),
				    ph->po_date, gtranslator_header_edit_changed);
	gtk_widget_set_sensitive(po_date, FALSE);

	/*
	 * Toggles whether personal options or entries are used to fill header
	 */
	take_my_options = 
	    gtk_check_button_new_with_label(
		_("Use my options to fill the following entries."));
	gtk_box_pack_start(GTK_BOX(lang_vbox), take_my_options, TRUE, TRUE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(take_my_options), 
				     wants.fill_header);
	gtk_signal_connect(GTK_OBJECT(take_my_options), "toggled",
			   GTK_SIGNAL_FUNC(take_my_options_toggled), NULL);
	/*
	 * Language and translator options
	 */
	lang_page = gtk_table_new(7, 2, FALSE);
	gtk_widget_set_sensitive(lang_page, !wants.fill_header);
	gtk_box_pack_start(GTK_BOX(lang_vbox), lang_page, TRUE, TRUE, 0);
	translator =
	    gtranslator_utils_attach_entry_with_label(lang_page, 1, _("Translator's name:"),
				    ph->translator, gtranslator_header_edit_changed);
	tr_email =
	    gtranslator_utils_attach_entry_with_label(lang_page, 2, _("Translator's e-mail:"),
				    ph->tr_email, gtranslator_header_edit_changed);
	language_combo =
	    gtranslator_utils_attach_combo_with_label(lang_page, 3, _("Language:"),
				    languages_list, ph->language,
				    language_changed, NULL);
	lg_combo =
	    gtranslator_utils_attach_combo_with_label(lang_page, 4,
				    _("Language group's e-mail:"),
				    group_emails_list, ph->lg_email,
				    gtranslator_header_edit_changed, NULL);
	charset_combo =
	    gtranslator_utils_attach_combo_with_label(lang_page, 5, _("Charset:"),
				    encodings_list, ph->charset,
				    gtranslator_header_edit_changed, NULL);
	enc_combo =
	    gtranslator_utils_attach_combo_with_label(lang_page, 6, _("Encoding:"),
				    bits_list, ph->encoding,
				    gtranslator_header_edit_changed, NULL);
	/*
	 * Connect the signals
	 */
	gtk_signal_connect(GTK_OBJECT(e_header), "apply",
			   GTK_SIGNAL_FUNC(gtranslator_header_edit_apply), NULL);
	gtk_signal_connect(GTK_OBJECT(e_header), "close",
			   GTK_SIGNAL_FUNC(gtranslator_utils_language_lists_free), NULL);

	gtranslator_dialog_show(&e_header, "gtranslator -- header");

	/*
	 * Allow resizing of the dialog in growth direction but not in
	 *  shrink direction.
	 */  
	gtk_window_set_policy(GTK_WINDOW(e_header), 0, 1, 1);
}

static void language_changed(GtkWidget * widget, gpointer useless)
{
	guint c = 0;
	gchar *current = gtk_entry_get_text(GTK_ENTRY
					    (GTK_COMBO(language_combo)->entry));
	while (languages[c].name != NULL) {
		if (!g_strcasecmp(current, _(languages[c].name))) {
#define set_text(widget,field) \
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(widget)->entry),\
			   languages[c].field)
			set_text(charset_combo, enc);
			set_text(enc_combo, bits);
			set_text(lg_combo, group);
#undef set_text
			break;
		}
		c++;
	}
	gtranslator_header_edit_changed(widget, useless);
}

static void gtranslator_header_edit_changed(GtkWidget * widget, gpointer useless)
{
	gnome_property_box_changed(GNOME_PROPERTY_BOX(e_header));
}

static void take_my_options_toggled(GtkWidget * widget, gpointer useless)
{
	wants.fill_header =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(take_my_options));
	gtranslator_config_init();
	gtranslator_config_set_bool("toggles/fill_header",
			      wants.fill_header);
	gtranslator_config_close();
	
	gtk_widget_set_sensitive(lang_page, !wants.fill_header);
	gtranslator_header_edit_changed(widget, useless);
}

/*
 * Rip off all '#'s and return the right comment string.
 */ 
gchar *gtranslator_header_comment_convert_for_view(gchar *comment)
{
	GString *mystring=g_string_new("");
	gchar **stringarray;
	gint count=0;

	/*
	 * Avoid NULL stuff in comments -- there should always
	 *  be a comment in a po file but you never know..
	 */
	g_return_val_if_fail(comment!=NULL, "");
	
	/*
	 * Split up the comment and let's rip off the '#'s from the
	 *  comments.
	 */  
	stringarray=g_strsplit(comment, "#", 0);

	while(stringarray[count]!=NULL)
	{
		/*
		 * Remove all malformed header comment lines which are like
		 *  "#: smtp" etc.
		 */
		if(strncmp(stringarray[count], ":", 1))
		{
			/*
			 * If there were spaced before the next block (like in
			 *  "# Hello", strip it out.
			 */ 
			mystring=g_string_append(mystring,
				g_strchug(stringarray[count]));
		}
		
		count++;
	}

	g_strfreev(stringarray);
	
	return mystring->str;
}

/*
 * Convert the shows comment "back" to the old form.
 */
gchar *gtranslator_header_comment_convert_for_save(gchar *comment)
{
	GString *mystring=g_string_new("");
	gchar **stringarray;
	gint count=0;

	/*
	 * Give back a securement free space comment for
	 *  reasonibility.
	 */
	g_return_val_if_fail(comment!=NULL, "");

	stringarray=g_strsplit(comment, "\n", 0);
	
	/*
	 * Split and re-combine the comment and # characters.
	 */ 
	while(stringarray[count]!=NULL)
	{
		mystring=g_string_append(mystring, "# ");
		mystring=g_string_append(mystring, stringarray[count]);
		mystring=g_string_append(mystring, "\n");

		count++;
	}

	/*
	 * Add a single empty line per default to improve readability.
	 */ 
	mystring=g_string_append(mystring, "# \n");	
	
	g_strfreev(stringarray);
	
	return mystring->str;
}

static gchar * get_current_year(void)
{
	time_t now;
	struct tm *timebox;
	gchar *year=g_malloc(5);
	
	/*
	 * Substitute YEAR with current year
	 */
	now=time(NULL);
	timebox=localtime(&now);

	strftime(year, 5, "%Y", timebox);
	return year;
}

static gboolean have_changed=FALSE;

void substitute(gchar **item, const gchar *bad, const gchar *good)
{
	/*
	 * Check the values before acting on them.
	 */
	g_return_if_fail(*item!=NULL);
	g_return_if_fail(good!=NULL);
	
	/* If string still has standard value */
	if(!strcmp(*item, bad))
	{
		/* Replace it with copy of good one */
		g_free(*item);
		*item=g_strdup(good);
		have_changed=TRUE;
	}
}

/* This replaces std with good in item */
void replace_substring(gchar **item, const gchar *bad, const gchar *good)
{
	gchar *old=*item;
	*item=nautilus_str_replace_substring(old, bad, good);
	g_free(old);
}

/*
 * Fill up the header entries which are also set up in the prefs.
 * return TRUE if any changes have been done.
 */
gboolean gtranslator_header_fill_up(GtrHeader *header)
{
	if(header==NULL)
		return FALSE;

	have_changed=FALSE;
	substitute(&header->translator, "FULL NAME", author);
	substitute(&header->tr_email, "EMAIL@ADDRESS", email);
	
	substitute(&header->language, "LANGUAGE", language);
	substitute(&header->lg_email, "LL@li.org", lg);
	
	substitute(&header->charset, "CHARSET", mime);
	substitute(&header->encoding, "ENCODING", enc);

	/*
	 * If there's any header comment we should also substitute the 
	 *  values there with useful stuff.
	 */
	if(header->comment)
	{
		gchar *title, *year;
		
		/*
		 * Translator data should be in sync with the header I guess .-)
		 */
		replace_substring(&header->comment,
				"FIRST AUTHOR", header->translator);
		replace_substring(&header->comment,
				"EMAIL@ADDRESS", header->tr_email);

		year = get_current_year();
		replace_substring(&header->comment, "YEAR", year);
		g_free(year);

		/* Fill h->po_date with current time */
		gtranslator_header_update(header);

		/*
		 * Should be a good description line .-)
		 */
		if(language_in_english(header->language))
		{
			title=g_strdup_printf("%s translation of %s.",
				language_in_english(header->language),
				header->prj_name);
		}
		else
		{
			title=g_strdup_printf("Translation of %s.",
				header->prj_name);	
		}
		
		replace_substring(&header->comment,
				"SOME DESCRIPTIVE TITLE.", title);
		g_free(title);
	}

	return have_changed;
}

GtrHeader * gtranslator_header_create_from_prefs(void)
{
	GtrHeader *h=g_new0(GtrHeader, 1);
	gchar *year;

	h->prj_name=g_strdup("PACKAGE");
	h->prj_version=g_strdup("VERSION");
	/* Fill h->po_date with current time */
	gtranslator_header_update(h);
	h->pot_date=g_strdup(h->po_date);
	h->translator=g_strdup(author);
	h->tr_email=g_strdup(email);
	h->language=g_strdup(language);
	h->lg_email=g_strdup(lg);
	h->mime_version=g_strdup("1.0");
	h->charset=g_strdup(lc);
	h->encoding=g_strdup(enc);

	year=get_current_year();

	/* Let fill_up_header replace YEAR later  */
	h->comment=g_strdup_printf(
"# %s translation of PACKAGE.\n"
"# Copyright (C) %s Free Software Foundation, Inc.\n"
"# %s <%s>, %s.\n"
"#\n",
		h->language,
		year,
		h->translator,
		h->tr_email,
		year);

	g_free(year);
	
	return h;
}

