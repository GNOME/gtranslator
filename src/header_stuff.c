/*
 * (C) 2000-2003 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Ross Golder <rossg@kabalak.net>
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

static GtkWidget *e_header = NULL;
static GtkWidget *e_notebook = NULL;

static GtkWidget *prj_page, *lang_page;
static GtkWidget *lang_vbox;

static GtkWidget *prj_name, *prj_version, *rmbt, *prj_comment, *take_my_options;
static GtkWidget *translator, *tr_email, *pot_date, *po_date;
static GtkWidget *language_combo, *charset_combo, *enc_combo, *lg_combo;

static gboolean header_changed;

/*
 * These are defined below 
 */
static void gtranslator_header_edit_close(GtkWidget * widget, gint response, gpointer useless);
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
	gchar *old_translator,
	      *old_tr_email;
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
#define if_key_is(str) if (pair[0] && !strcmp(pair[0],str))

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
		{
			if(pair[1]!=NULL && pair[1]!='\0' && strchr(pair[1], '.'))
			{
				ph->report_message_bugs_to = g_strdup(pair[1]);
			}
			else
			{
				ph->report_message_bugs_to = g_strdup("");
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
			gchar *prefix;
			
			prefix = strstr (pair[1], "charset=");
			if (prefix)
				ph->charset = g_strdup (prefix + 8);
			else
				ph->charset = g_strdup ("UTF-8");
		}
		else
		if_key_is("Content-Transfer-Encoding")
		    ph->encoding = g_strdup(pair[1]);
		else
		if_key_is("Plural-Forms")
		    ph->plural_forms = g_strdup(pair[1]); 
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

	/*
	 * If toggle button "take_my_options" is marked then the details
	 * about translator must be read from the preferences to update
	 * the Last Translator when users save the file.
	 */

	if (GtrPreferences.fill_header) 
	{

		old_translator = ph->translator;		
		ph->translator = gtranslator_translator->name;

		old_tr_email = ph->tr_email;		
		ph->tr_email = gtranslator_translator->email;
		
		GTR_FREE(ph->language);
		ph->language = gtranslator_translator->language->name;
		GTR_FREE(ph->lg_email);
		ph->lg_email = gtranslator_translator->language->group_email;
		GTR_FREE(ph->charset);
		ph->charset = gtranslator_translator->language->encoding;
		GTR_FREE(ph->encoding);
		ph->encoding = gtranslator_translator->language->bits;

		if(old_translator && old_tr_email && ph->translator &&
		nautilus_strcasecmp(ph->translator, old_translator) &&
		nautilus_strcasecmp(ph->comment, old_translator))
        	{
			gchar   *prev_header_comment;
			gchar   *year;

			prev_header_comment = ph->comment;
                
			year=get_current_year();

			ph->comment=g_strdup_printf("%s# %s <%s>, %s.\n",
				prev_header_comment,
				old_translator, old_tr_email, year);
		
			GTR_FREE(year);
			GTR_FREE(prev_header_comment);
        	}
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

	if(!h->report_message_bugs_to)
	{
		h->report_message_bugs_to=g_strdup("");
	}

	if(h->plural_forms)
	{
		msg->msgstr = g_strdup_printf("\n"\
"Project-Id-Version: %s\\n\n"\
"Report-Msgid-Bugs-To: %s\\n\n"\
"POT-Creation-Date: %s\\n\n"\
"PO-Revision-Date: %s\\n\n"\
"Last-Translator: %s <%s>\\n\n"\
"Language-Team: %s\\n\n"\
"MIME-Version: %s\\n\n"\
"Content-Type: text/plain; charset=%s\\n\n"\
"Content-Transfer-Encoding: %s\\n\n"\
"Plural-Forms: %s",
		version,
		h->report_message_bugs_to,
		h->pot_date,
		h->po_date,
		h->translator, h->tr_email,
		group,
		h->mime_version,
		h->charset,
		h->encoding,
		h->plural_forms);
	}
	else
	{
		msg->msgstr = g_strdup_printf("\n"\
"Project-Id-Version: %s\\n\n"\
"Report-Msgid-Bugs-To: %s\\n\n"\
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
	}

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
	GTR_FREE(h->plural_forms);

	if(h->generator)
	{
		GTR_FREE(h->generator);
	}
	
	GTR_FREE(h);
}

static void gtranslator_header_edit_close(GtkWidget * widget, gint response, gpointer useless)
{
	GtrHeader 	*ph = po->header;
	gchar		*prev_translator,
			*prev_translator_email;
	GtkTextBuffer   *buff;
	GtkTextIter     start, end;

	prev_translator=prev_translator_email=NULL;

	/*
	 * Free the languages list
	 */
	gtranslator_utils_language_lists_free(widget, useless);

	if(!header_changed) {
		gtk_widget_destroy(GTK_WIDGET(e_header));
		return;
	}

	buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(prj_comment));
	gtk_text_buffer_get_bounds(buff, &start, &end);
	GTR_FREE(ph->comment);
	ph->comment = gtk_text_buffer_get_text(buff, &start, &end, FALSE);

#define update(value,widget) GTR_FREE(value);\
	value = gtk_editable_get_chars(GTK_EDITABLE(widget),0,-1);

	update(ph->prj_name, prj_name);
	update(ph->prj_version, prj_version);

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
	
	/*
	 * Mark file as having unsaved changes
	 */
	po->file_changed = TRUE;
	gtranslator_actions_enable(ACT_SAVE, ACT_REVERT);
	
	gtk_widget_destroy(GTK_WIDGET(e_header));
	return;
}

/*
 * Creates the Header-edit dialog.
 */
void gtranslator_header_edit_dialog(GtkWidget * widget, gpointer useless)
{
	GtrHeader *ph = po->header;
	GtkWidget *foo_me_i_ve_been_wracked;
	GtkTextBuffer *buffer;

	header_changed = FALSE;

	if(e_header != NULL) {
		gtk_window_present(GTK_WINDOW(e_header));
		return;
	}

	/*
	 * Prepare the header comment for view and edit in the dialog. 
	 */
	ph->comment=gtranslator_header_comment_convert_for_view(ph->comment);

	/*
	 * Prepare the languages list pulldown information
	 */
	gtranslator_utils_language_lists_create();

	/*
	 * The main dialog
	 */
	e_header = gtk_dialog_new_with_buttons(
		_("gtranslator -- edit header"),
		GTK_WINDOW(gtranslator_application),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
		NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(e_header), GTK_RESPONSE_CLOSE);

	/*
	 * The notebook containing the pages
	 */
	e_notebook = gtk_notebook_new();
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(e_header)->vbox),
                      e_notebook);

	/*
	 * Project page
	 */
	prj_page = gtk_table_new(6, 2, FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(prj_page), 2);
	gtk_table_set_row_spacings(GTK_TABLE(prj_page), 2);

	prj_comment = gtk_text_view_new();
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(prj_comment));
	gtk_text_buffer_set_text(buffer, ph->comment, -1);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), gtk_label_new(_("Comments:")), 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), prj_comment, 1, 2, 0, 1);
	gtk_widget_set_size_request(prj_comment, 360, 90);
	g_signal_connect(buffer, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	prj_name = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(prj_name), ph->prj_name);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), gtk_label_new(_("Project name:")), 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), prj_name, 1, 2, 1, 2);
	g_signal_connect(prj_name, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	prj_version = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(prj_version), ph->prj_version);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), gtk_label_new(_("Project version:")), 0, 1, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), prj_version, 1, 2, 2, 3);
	g_signal_connect(prj_version, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	pot_date = gtk_entry_new();
	gtk_widget_set_sensitive(pot_date, FALSE);
	gtk_entry_set_text(GTK_ENTRY(pot_date), ph->pot_date);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), gtk_label_new(_("Pot file creation date:")), 0, 1, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), pot_date, 1, 2, 3, 4);

	po_date = gtk_entry_new();
	gtk_widget_set_sensitive(po_date, FALSE);
	gtk_entry_set_text(GTK_ENTRY(po_date), ph->po_date);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), gtk_label_new(_("Po file revision date:")), 0, 1, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), po_date, 1, 2, 4, 5);

	rmbt = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(rmbt), ph->report_message_bugs_to);

	if(ph->report_message_bugs_to && strchr(ph->report_message_bugs_to, '.'))
	{
		gtk_widget_set_sensitive(rmbt, TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(rmbt, FALSE);
	}
	gtk_table_attach_defaults(GTK_TABLE(prj_page), gtk_label_new(_("Report message string bugs to:")), 0, 1, 5, 6);
	gtk_table_attach_defaults(GTK_TABLE(prj_page), rmbt, 1, 2, 5, 6);

	if(ph->generator)
	{
		/*
		 * Resize table, and add 'generator' row
		 */
		gtk_table_resize(GTK_TABLE(prj_page), 7, 2);

		foo_me_i_ve_been_wracked = gtk_entry_new();
		gtk_widget_set_sensitive(foo_me_i_ve_been_wracked, FALSE);
		gtk_entry_set_text(GTK_ENTRY(foo_me_i_ve_been_wracked), ph->generator);
		gtk_table_attach_defaults(GTK_TABLE(prj_page), gtk_label_new(_("Generator:")), 0, 1, 6, 7);
		gtk_table_attach_defaults(GTK_TABLE(prj_page), foo_me_i_ve_been_wracked, 1, 2, 6, 7);
	}

	gtk_notebook_append_page(GTK_NOTEBOOK(e_notebook), prj_page, gtk_label_new(_("Project")));

	/*
	 * Translator and language page
	 */
	take_my_options = gtk_check_button_new_with_label(
		_("Use my options to complete the following entries:"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(take_my_options), 
				     GtrPreferences.fill_header);
	g_signal_connect(G_OBJECT(take_my_options), "toggled",
			 G_CALLBACK(take_my_options_toggled), NULL);

	lang_page = gtk_table_new(6, 2, FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(lang_page), 2);
	gtk_table_set_row_spacings(GTK_TABLE(lang_page), 2);
	gtk_widget_set_sensitive(lang_page, !GtrPreferences.fill_header);

	translator = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(translator), ph->translator);
	gtk_table_attach_defaults(GTK_TABLE(lang_page), gtk_label_new(_("Translator's name:")), 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(lang_page), translator, 1, 2, 0, 1);
	g_signal_connect(translator, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	tr_email = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(tr_email), ph->tr_email);
	gtk_table_attach_defaults(GTK_TABLE(lang_page), gtk_label_new(_("Translator's e-mail:")), 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(lang_page), tr_email, 1, 2, 1, 2);
	g_signal_connect(tr_email, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);
	
	language_combo = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(language_combo), languages_list);
	gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (language_combo)->entry), _(ph->language));
	gtk_table_attach_defaults(GTK_TABLE(lang_page), gtk_label_new(_("Language:")), 0, 1, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(lang_page), language_combo, 1, 2, 2, 3);
	g_signal_connect(GTK_COMBO(language_combo)->entry, "changed", G_CALLBACK(language_changed), NULL);

	lg_combo = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(lg_combo), group_emails_list);
	gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (lg_combo)->entry), _(ph->lg_email));
	gtk_table_attach_defaults(GTK_TABLE(lang_page), gtk_label_new(_("Language group's email:")), 0, 1, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(lang_page), lg_combo, 1, 2, 3, 4);
	g_signal_connect(GTK_COMBO(lg_combo)->entry, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	charset_combo = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(charset_combo), encodings_list);
	gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (charset_combo)->entry), _(ph->charset));
	gtk_table_attach_defaults(GTK_TABLE(lang_page), gtk_label_new(_("Charset:")), 0, 1, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(lang_page), charset_combo, 1, 2, 4, 5);
	g_signal_connect(GTK_COMBO(charset_combo)->entry, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	enc_combo = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(enc_combo), bits_list);
	gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (enc_combo)->entry), _(ph->encoding));
	gtk_table_attach_defaults(GTK_TABLE(lang_page), gtk_label_new(_("Encoding:")), 0, 1, 5, 6);
	gtk_table_attach_defaults(GTK_TABLE(lang_page), enc_combo, 1, 2, 5, 6);
	g_signal_connect(GTK_COMBO(enc_combo)->entry, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	lang_vbox = gtk_vbox_new(FALSE, GNOME_PAD);
	gtk_box_pack_start(GTK_BOX(lang_vbox), take_my_options, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(lang_vbox), lang_page, TRUE, TRUE, 0);

	gtk_notebook_append_page(GTK_NOTEBOOK(e_notebook), lang_vbox, gtk_label_new(_("Translator and Language")));

	/*
	 * Disable any charset changes directly from the header by making the
	 *  charset combo non-editable.
	 */
	gtk_widget_set_sensitive(GTK_WIDGET(charset_combo), FALSE);

	/*
	 * Connect the signals
	 */
	g_signal_connect(G_OBJECT(e_header), "response",
			 G_CALLBACK(gtranslator_header_edit_close), NULL);

	gtranslator_dialog_show(&e_header, "gtranslator -- header");
}

static void language_changed(GtkWidget * widget, gpointer useless)
{
	guint c = 0;
	G_CONST_RETURN gchar *current = gtk_entry_get_text(GTK_ENTRY
					    (GTK_COMBO(language_combo)->entry));
	while (languages[c].name != NULL) {
		if (!strcmp(current, _(languages[c].name))) {
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
	header_changed = TRUE;
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
	h->plural_forms=g_strdup("");
	
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
