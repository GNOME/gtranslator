/*
 * (C) 2000-2003 	Fatih Demir <kabalak@gtranslator.org>
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
#include "comment.h"
#include "dialogs.h"
#include "gui.h"
#include "header_stuff.h"
#include "languages.h"
#include "nautilus-string.h"
#include "parse.h"
#include "prefs.h"
#include "translator.h"
#include "utils.h"
#include "utils_gui.h"

#include <locale.h>
#include <time.h>

#include <gtk/gtk.h>
#include <libgnomeui/gnome-uidefs.h>
#include <libgnomeui/gnome-propertybox.h>

static GtkWidget *e_header = NULL;

static GtkWidget *prj_page, *lang_page, *lang_vbox;

static GtkWidget *prj_name, *prj_version, *rmbt, *prj_comment, *take_my_options;
static GtkWidget *translator, *tr_email, *pot_date, *po_date;
static GtkWidget *language_combo, *charset_combo, *enc_combo, *lg_combo;

/*
 * These are defined below 
 */
static void gtranslator_header_edit_apply(GtkWidget * box, gint page_num, gpointer useless);
static void take_my_options_toggled(GtkWidget * widget, gpointer useless);
static void gtranslator_header_edit_changed(GtkWidget * widget, gpointer useless);
static void language_changed(GtkWidget * widget, gpointer useless);
static gchar *get_current_year(void);
static void substitute(gchar **item, const gchar *bad, const gchar *good);
static void replace_substring(gchar **item, const gchar *bad, const gchar *good);

static void split_name_email(const gchar * str, gchar ** name, gchar ** email)
{
	char *prefix_start;

	if (!str) {
		*name = g_strdup("");
		*email = g_strdup("");
		return;
	}
	
	prefix_start = strstr (str, " <");

	if (prefix_start == NULL) {
		*name = g_strdup(str);
	} else {

		*name = g_strndup (str, prefix_start - str);
		*email = g_strndup (prefix_start + 2, strlen (prefix_start) - 3);
	}
}

GtrHeader * gtranslator_header_get(GtrMsg * msg)
{
	GtrHeader *ph;
	gchar **lines, **pair, *pos;
	gint i = 0;

	g_return_val_if_fail(msg != NULL, NULL);
	g_return_val_if_fail(msg->msgstr != NULL, NULL);
	
	ph = g_new0(GtrHeader, 1);
	lines = g_strsplit(msg->msgstr, "\n", 0);
	for (i = 0; lines[i] != NULL; i++) {
		pair = g_strsplit(lines[i], ": ", 2);
		if(!pair[0] || !pair[1]) continue;
		pos = strstr(pair[1], "\\n");
		if(pos)
			*pos = '\0';
#define if_key_is(str) if (pair[0] && !g_strcasecmp(pair[0],str))

		if_key_is("Project-Id-Version") {
			gchar *space;
			space = strrchr (pair[1], ' ');
			if (space) {
				ph->prj_name = g_strndup(pair[1], space - pair[1]);
				ph->prj_version = g_strdup(space + 1);
			} else {
		  	  ph->prj_name = g_strdup(pair[1]);
			  ph->prj_version = g_strdup("");
			}
		}
		else
		if_key_is("Report-Msgid-Bugs-To")
		    ph->report_message_bugs_to = g_strdup(pair[1]);
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
			gchar *prefix;
			
			prefix = strstr (pair[1], "text/plain; charset=");
			if (prefix)
				ph->charset = g_strdup (prefix + 20);
			else
				ph->charset = g_strdup (pair[1]);
		}
		else
		if_key_is("Content-Transfer-Encoding")
		    ph->encoding = g_strdup(pair[1]);
		else
		if_key_is("X-Generator")
		    ph->generator = g_strdup(pair[1]);

		g_strfreev(pair);
	}
	
	g_strfreev(lines);

	if(msg->comment && GTR_COMMENT(msg->comment)->comment)
	{
		ph->comment=g_strdup(GTR_COMMENT(msg->comment)->comment);
	}
	else
	{
		ph->comment=g_strdup("# ");
	}
	
	if (ph->prj_name)
		return ph;
	else
		return NULL;
}

/*
 * Creates new GtrMsg, with all data set to current state of header 
 */
GtrMsg * gtranslator_header_put(GtrHeader * h)
{
	gchar *group;
	GtrMsg *msg = g_new0(GtrMsg, 1);
	gchar *version;

	const gchar *lang=gtranslator_utils_get_english_language_name(h->language);

	if (h->lg_email && h->lg_email[0] != '\0')
		group = g_strdup_printf("%s <%s>", lang, h->lg_email);
	else
		group = g_strdup(lang);

	if (h->prj_version && h->prj_version[0] != '\0')
		version = g_strdup_printf("%s %s", h->prj_name, h->prj_version);
	else
		version = g_strdup(h->prj_name);
	
	msg->msgstr = g_strdup_printf("\n"\
"Project-Id-Version: %s\\n\n"\
"POT-Creation-Date: %s\\n\n"\
"PO-Revision-Date: %s\\n\n"\
"Last-Translator: %s <%s>\\n\n"\
"Language-Team: %s\\n\n"\
"MIME-Version: %s\\n\n"\
"Content-Type: text/plain; charset=%s\\n\n"\
"Content-Transfer-Encoding: %s",
		version,
		h->report_message_bugs_to,
		h->pot_date,
		h->po_date,
		h->translator, h->tr_email,
		group,
		h->mime_version,
		h->charset,
		h->encoding);

	GTR_FREE(group);
	GTR_FREE(version);

	/*
	 * Just copy the comment, and make sure it ends with endline.
	 */
	if(h->comment[strlen(h->comment)-1] == '\n')
	{
		msg->comment = gtranslator_comment_new(h->comment);
	}
	else
	{
		gchar *comchar=g_strdup_printf("%s\n", h->comment);
		msg->comment = gtranslator_comment_new(comchar);
		GTR_FREE(comchar);
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
	GTR_FREE(h->po_date);
	h->po_date = g_strdup(t);

	/*
	 * Convert the header comments back if necessary.
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
	GTR_FREE(h->comment);
	GTR_FREE(h->prj_name);
	GTR_FREE(h->prj_version);
	GTR_FREE(h->report_message_bugs_to);
	GTR_FREE(h->pot_date);
	GTR_FREE(h->po_date);
	GTR_FREE(h->translator);
	GTR_FREE(h->tr_email);
	GTR_FREE(h->language);
	GTR_FREE(h->lg_email);
	GTR_FREE(h->mime_version);
	GTR_FREE(h->charset);
	GTR_FREE(h->encoding);

	if(h->generator)
	{
		GTR_FREE(h->generator);
	}
	
	GTR_FREE(h);
}

static void gtranslator_header_edit_apply(GtkWidget * box, gint page_num, gpointer useless)
{
	GtrHeader 	*ph = po->header;
	gchar		*prev_translator,
			*prev_translator_email;
	GtkTextBuffer   *buff;
	GtkTextIter     start, end;

	prev_translator=prev_translator_email=NULL;

	if (page_num != -1)
		return;

#define update(value,widget) GTR_FREE(value);\
	value = gtk_editable_get_chars(GTK_EDITABLE(widget),0,-1);
	update(ph->prj_name, prj_name);
	update(ph->prj_version, prj_version);

	buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(prj_comment));
	gtk_text_buffer_get_start_iter(buff, &start);
	gtk_text_buffer_get_start_iter(buff, &end);
	GTR_FREE(ph->comment);
	ph->comment = gtk_text_buffer_get_text(buff, &start, &end, FALSE);

	if (!GtrPreferences.fill_header) {
		
		if(ph->translator)
		{
			prev_translator=g_strdup(ph->translator);
			GTR_FREE(ph->translator);
		}
		
		ph->translator = gtk_editable_get_chars(GTK_EDITABLE(translator),0,-1);
		
		if(ph->tr_email)
		{
			prev_translator_email=g_strdup(ph->tr_email);
		}
		
		update(ph->tr_email, tr_email);
		update(ph->language, GTK_COMBO(language_combo)->entry);
		update(ph->lg_email, GTK_COMBO(lg_combo)->entry);
		update(ph->charset, GTK_COMBO(charset_combo)->entry);
		update(ph->encoding, GTK_COMBO(enc_combo)->entry);
#undef update
	} else {
#define replace(what,with,entry) g_free(what); what = g_strdup(with);\
	gtk_entry_set_text(GTK_ENTRY(entry), with);
	
		if(ph->translator)
		{
			prev_translator=g_strdup(ph->translator);
		}
		
		replace(ph->translator, gtranslator_translator->name, translator);

		if(ph->tr_email)
		{
			prev_translator_email=g_strdup(ph->tr_email);
		}

		replace(ph->tr_email, gtranslator_translator->email, tr_email);
		replace(ph->language, gtranslator_translator->language->name,
			GTK_COMBO(language_combo)->entry);
		replace(ph->lg_email, gtranslator_translator->language->group_email,
			GTK_COMBO(lg_combo)->entry);
		replace(ph->charset, gtranslator_translator->language->encoding, 
			GTK_COMBO(charset_combo)->entry);
		replace(ph->encoding, gtranslator_translator->language->bits,
			GTK_COMBO(enc_combo)->entry);
#undef replace
	}
	
	/*
	 * Check if the  previous translator data could be get and if the
	 *  last and previous translator were different persons: but only,
	 *   if the previously last translator isn't listed there yet.
	 */
	if(prev_translator && prev_translator_email && ph->translator &&
		nautilus_strcasecmp(ph->translator, prev_translator) &&
		nautilus_strcasecmp(ph->comment, prev_translator))
	{
		gchar	*prev_header_comment;
		gchar	*year;

		/*
		 * Rescue the old header comment and free it's variable.
		 */
		prev_header_comment=g_strdup(ph->comment);
		GTR_FREE(ph->comment);

		/*
		 * Eh, what would we do without it ,-)
		 */
		year=get_current_year();

		/*
		 * Now create the "new" header comment from the previously
		 *  backup'd comment and the previous last translator data.
		 */
		ph->comment=g_strdup_printf("%s%s <%s>, %s.\n",
			prev_header_comment,
			prev_translator, prev_translator_email, year);

		GTR_FREE(year);
		GTR_FREE(prev_header_comment);
	}
	
	/*
	 * Convert the header comment back for save and substitute the
	 *  PACKAGE and VERSION fields in the header.
	 */
	ph->comment=gtranslator_header_comment_convert_for_save(ph->comment);
	replace_substring(&ph->comment, "PACKAGE", ph->prj_name);
	replace_substring(&ph->comment, "VERSION", ph->prj_version);
	
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
	GtkWidget *foo_me_i_ve_been_wracked;

	if(e_header != NULL) {
		gtk_window_present(GTK_WINDOW(e_header));
		return;
	}

	e_header = gnome_property_box_new();
	gtk_window_set_title(GTK_WINDOW(e_header),
			     _("gtranslator -- edit header"));

	gtranslator_utils_language_lists_create();

	if(ph->generator)
	{
		prj_page=gtranslator_utils_append_page_to_preferences_dialog(
			e_header, 7, 2, _("Project"));
	}
	else
	{
		prj_page=gtranslator_utils_append_page_to_preferences_dialog(
			e_header, 6, 2, _("Project"));
	}
	
	label = gtk_label_new(_("Translator and Language"));
	lang_vbox = gtk_vbox_new(FALSE, GNOME_PAD);
	gnome_property_box_append_page(GNOME_PROPERTY_BOX(e_header), lang_vbox,
				       label);
	
	/*
	 * Prepare the header comment for view and edit in the dialog. 
	 */
	ph->comment=gtranslator_header_comment_convert_for_view(ph->comment);

	prj_comment=gtranslator_utils_attach_text_with_label(prj_page, 0, _("Comments:"),
							     ph->comment, G_CALLBACK(gtranslator_header_edit_changed));
	
	gtk_widget_set_usize(prj_comment, 360, 90);
	
	prj_name =
	    gtranslator_utils_attach_entry_with_label(prj_page, 1, _("Project name:"),
	    			    ph->prj_name, G_CALLBACK(gtranslator_header_edit_changed));
	prj_version =
	    gtranslator_utils_attach_entry_with_label(prj_page, 2, _("Project version:"),
	    			    ph->prj_version, G_CALLBACK(gtranslator_header_edit_changed));
	pot_date =
	    gtranslator_utils_attach_entry_with_label(prj_page, 3, _("Pot file creation date:"),
				    ph->pot_date, G_CALLBACK(gtranslator_header_edit_changed));
	gtk_widget_set_sensitive(pot_date, FALSE);
	
	po_date =
	    gtranslator_utils_attach_entry_with_label(prj_page, 4, _("Po file revision date:"),
				    ph->po_date, G_CALLBACK(gtranslator_header_edit_changed));
	gtk_widget_set_sensitive(po_date, FALSE);

	if(ph->generator)
	{
		foo_me_i_ve_been_wracked =
	    		gtranslator_utils_attach_entry_with_label(prj_page, 5, _("Generator:"),
	    			ph->generator, G_CALLBACK(gtranslator_header_edit_changed));
		gtk_widget_set_sensitive(foo_me_i_ve_been_wracked, FALSE);

		rmbt = gtranslator_utils_attach_entry_with_label(prj_page, 6, _("Report message string bugs to:"),
			ph->report_message_bugs_to, NULL);
	}

	rmbt = gtranslator_utils_attach_entry_with_label(prj_page, 5, _("Report message string bugs to:"),
		ph->report_message_bugs_to, NULL);

	gtk_widget_set_sensitive(rmbt, FALSE);

	/*
	 * Toggles whether personal options or entries are used to fill header
	 */
	take_my_options = 
	    gtk_check_button_new_with_label(
		_("Use my options to complete the following entries:"));
	gtk_box_pack_start(GTK_BOX(lang_vbox), take_my_options, TRUE, TRUE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(take_my_options), 
				     GtrPreferences.fill_header);
	g_signal_connect(G_OBJECT(take_my_options), "toggled",
			 G_CALLBACK(take_my_options_toggled), NULL);
	/*
	 * Language and translator options
	 */
	lang_page = gtk_table_new(7, 2, FALSE);
	gtk_widget_set_sensitive(lang_page, !GtrPreferences.fill_header);
	gtk_box_pack_start(GTK_BOX(lang_vbox), lang_page, TRUE, TRUE, 0);

	translator=gtranslator_utils_attach_entry_with_label(lang_page, 1, 
							     _("Translator's name:"), ph->translator, G_CALLBACK(gtranslator_header_edit_changed));

	tr_email =
	    gtranslator_utils_attach_entry_with_label(lang_page, 2, _("Translator's e-mail:"),
				    ph->tr_email, G_CALLBACK(gtranslator_header_edit_changed));
	language_combo =
	    gtranslator_utils_attach_combo_with_label(lang_page, 3, _("Language:"),
				    languages_list, _(ph->language),
				    FALSE,
				    G_CALLBACK(language_changed), NULL);
	lg_combo =
	    gtranslator_utils_attach_combo_with_label(lang_page, 4,
				    _("Language group's e-mail:"),
				    group_emails_list, ph->lg_email,
				    TRUE,
				    G_CALLBACK(gtranslator_header_edit_changed), NULL);
	charset_combo =
	    gtranslator_utils_attach_combo_with_label(lang_page, 5, _("Charset:"),
				    encodings_list, ph->charset,
				    FALSE,
				    G_CALLBACK(gtranslator_header_edit_changed), NULL);

	/*
	 * Disable any charset changes directly from the header by making the
	 *  charset combo non-editable.
	 */
	gtk_widget_set_sensitive(GTK_WIDGET(charset_combo), FALSE);

	enc_combo =
	    gtranslator_utils_attach_combo_with_label(lang_page, 6, _("Encoding:"),
				    bits_list, ph->encoding,
				    FALSE,
				    G_CALLBACK(gtranslator_header_edit_changed), NULL);
	/*
	 * Connect the signals
	 */
	g_signal_connect(G_OBJECT(e_header), "apply",
			 G_CALLBACK(gtranslator_header_edit_apply), NULL);
	g_signal_connect(G_OBJECT(e_header), "close",
			 G_CALLBACK(gtranslator_utils_language_lists_free), NULL);

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
	G_CONST_RETURN gchar *current = gtk_entry_get_text(GTK_ENTRY
					    (GTK_COMBO(language_combo)->entry));
	while (languages[c].name != NULL) {
		if (!g_strcasecmp(current, _(languages[c].name))) {
#define set_text(widget,field) \
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(widget)->entry),\
			   languages[c].field)
			set_text(charset_combo, encoding);
			set_text(enc_combo, bits);
			set_text(lg_combo, group_email);
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
	GtrPreferences.fill_header =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(take_my_options));
	gtranslator_config_set_bool("toggles/fill_header",
			      GtrPreferences.fill_header);
	
	gtk_widget_set_sensitive(lang_page, !GtrPreferences.fill_header);
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
	g_return_if_fail(good!=NULL);

	/* If string still has standard value or nothing */
	if((*item==NULL) ||
	   (!strcmp(*item, bad)))
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
	GTR_FREE(old);
}

/*
 * Fill up the header entries which are also set up in the prefs.
 * return TRUE if any changes have been done.
 */
gboolean gtranslator_header_fill_up(GtrHeader *header)
{
	g_return_val_if_fail(header!=NULL, FALSE);

	have_changed=FALSE;
	substitute(&header->translator, "FULL NAME", gtranslator_translator->name);
	substitute(&header->tr_email, "EMAIL@ADDRESS", gtranslator_translator->email);
	
	substitute(&header->language, "LANGUAGE", gtranslator_translator->language->name);
	substitute(&header->lg_email, "LL@li.org", gtranslator_translator->language->group_email);
	substitute(&header->charset, "CHARSET", gtranslator_translator->language->encoding);
	substitute(&header->encoding, "ENCODING", gtranslator_translator->language->bits);

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
		GTR_FREE(year);

		/*
		 * Fill h->po_date with current time 
		 */
		gtranslator_header_update(header);

		/*
		 * Should be a good description line .-)
		 */
		if(gtranslator_utils_get_english_language_name(header->language))
		{
			title=g_strdup_printf("%s translation of %s.",
				gtranslator_utils_get_english_language_name(header->language),
				header->prj_name);
		}
		else
		{
			title=g_strdup_printf("Translation of %s.",
				header->prj_name);	
		}
		
		replace_substring(&header->comment,
				"SOME DESCRIPTIVE TITLE.", title);
		GTR_FREE(title);
	}

	return have_changed;
}

GtrHeader *gtranslator_header_create_from_prefs(void)
{
	GtrHeader *h=g_new0(GtrHeader, 1);
	gchar *year;

	h->prj_name=g_strdup("PACKAGE");
	h->prj_version=g_strdup("VERSION");
	h->report_message_bugs_to=g_strdup("");
	
	/* 
	 * Fill h->po_date with current time 
	 */
	gtranslator_header_update(h);
	
	h->pot_date=g_strdup(h->po_date);
	h->translator=g_strdup(gtranslator_translator->name);
	h->tr_email=g_strdup(gtranslator_translator->email);
	h->language=g_strdup(gtranslator_translator->language->name);
	h->lg_email=g_strdup(gtranslator_translator->language->group_email);
	h->mime_version=g_strdup("1.0");
	h->charset=g_strdup(gtranslator_translator->language->encoding);
	h->encoding=g_strdup(gtranslator_translator->language->bits);

	year=get_current_year();

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

	GTR_FREE(year);
	return h;
}
