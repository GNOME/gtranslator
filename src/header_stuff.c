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

#include "header_stuff.h"
#include "dialogs.h"
#include "gui.h"
#include "prefs.h"
#include "parse.h"
#include "languages.h"

#include <locale.h>
#include <time.h>
#include <string.h>

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
static void edit_header_apply(GtkWidget * box, gint page_num, gpointer useless);
static void take_my_options_toggled(GtkWidget * widget, gpointer useless);
static void edit_header_changed(GtkWidget * widget, gpointer useless);
static void language_changed(GtkWidget * widget, gpointer useless);

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

GtrHeader * get_header(GtrMsg * msg)
{
	GtrHeader *ph;
	gchar **lines, **pair;
	gint i = 0;

	ph = g_new0(GtrHeader, 1);
	ph->comment = g_strdup(msg->comment);
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
	if (ph->prj_name)
		return ph;
	else
		return NULL;
}

/*
 * Creates new GtrMsg, with all data set to current state of header 
 */
GtrMsg * put_header(GtrHeader * h)
{
	gchar *group;
	GtrMsg *msg = g_new0(GtrMsg, 1);
	gint c;
	gchar *lang=h->language;
	gchar *version;

	/*
	 * Get the non-localized name for the language, if available
	 */ 
	for(c=0;languages[c].group!=NULL;c++)
	{
		if(!strcmp(_(languages[c].name), h->language))
		{
			lang=languages[c].name;
			break;
		}
	}
		
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
void update_header(GtrHeader * h)
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
	 */ 
	if(h->comment && h->comment[0]!='#')
	{
		h->comment=prepare_comment_for_save(h->comment);
	}
}

void free_header(GtrHeader * h)
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

static void edit_header_apply(GtkWidget * box, gint page_num, gpointer useless)
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
	 * Convert the header comment back for save.
	 */ 
	ph->comment=prepare_comment_for_save(ph->comment);
	
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
		enable_actions(ACT_SAVE, ACT_REVERT);
	}
}

/*
 * Creates the Header-edit dialog.
 */
void edit_header(GtkWidget * widget, gpointer useless)
{
	GtrHeader *ph = po->header;
	GtkWidget *label;

	raise_and_return_if_exists(e_header);
	e_header = gnome_property_box_new();
	gtk_window_set_title(GTK_WINDOW(e_header),
			     _("gtranslator -- edit header"));

	create_lists();

	prj_page = append_page_table(e_header, 5, 2, _("Project"));
	label = gtk_label_new(_("Translator and Language"));
	lang_vbox = gtk_vbox_new(FALSE, GNOME_PAD);
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(e_header), lang_vbox,
				       label);
	
	/*
	 * Prepare the header comment for view and edit in the dialog. 
	 */
	ph->comment=prepare_comment_for_view(ph->comment);
	
	prj_comment =
	    attach_text_with_label(prj_page, 0, _("Comments :"), ph->comment,
	    			   edit_header_changed);
	
	gtk_widget_set_usize(prj_comment, 360, 90);
	
	prj_name =
	    attach_entry_with_label(prj_page, 1, _("Project name:"),
	    			    ph->prj_name, edit_header_changed);
	prj_version =
	    attach_entry_with_label(prj_page, 2, _("Project version:"),
	    			    ph->prj_version, edit_header_changed);
	pot_date =
	    attach_entry_with_label(prj_page, 3, _("pot file creation date :"),
				    ph->pot_date, edit_header_changed);
	gtk_widget_set_sensitive(pot_date, FALSE);
	po_date =
	    attach_entry_with_label(prj_page, 4, _("po file revision date :"),
				    ph->po_date, edit_header_changed);
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
	    attach_entry_with_label(lang_page, 1, _("Translator's name :"),
				    ph->translator, edit_header_changed);
	tr_email =
	    attach_entry_with_label(lang_page, 2, _("Translator's e-mail :"),
				    ph->tr_email, edit_header_changed);
	language_combo =
	    attach_combo_with_label(lang_page, 3, _("Language :"),
				    languages_list, ph->language,
				    language_changed, NULL);
	lg_combo =
	    attach_combo_with_label(lang_page, 4,
				    _("Language group's e-mail :"),
				    group_emails_list, ph->lg_email,
				    edit_header_changed, NULL);
	charset_combo =
	    attach_combo_with_label(lang_page, 5, _("Charset :"),
				    encodings_list, ph->charset,
				    edit_header_changed, NULL);
	enc_combo =
	    attach_combo_with_label(lang_page, 6, _("Encoding :"),
				    bits_list, ph->encoding,
				    edit_header_changed, NULL);
	/*
	 * Connect the signals
	 */
	gtk_signal_connect(GTK_OBJECT(e_header), "apply",
			   GTK_SIGNAL_FUNC(edit_header_apply), NULL);
	gtk_signal_connect(GTK_OBJECT(e_header), "close",
			   GTK_SIGNAL_FUNC(destroy_lists), NULL);

	show_nice_dialog(&e_header, "gtranslator -- header");

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
	edit_header_changed(widget, useless);
}

static void edit_header_changed(GtkWidget * widget, gpointer useless)
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
	edit_header_changed(widget, useless);
}

/*
 * Rip off all '#'s and return the right comment string.
 */ 
gchar *prepare_comment_for_view(gchar *comment)
{
	GString *mystring=g_string_new("");
	gchar **stringarray;
	gint count=0;

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
gchar *prepare_comment_for_save(gchar *comment)
{
	GString *mystring=g_string_new("");
	gchar **stringarray;
	gint count=0;

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
