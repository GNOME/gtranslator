/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Ross Golder <rossg@kabalak.net>
 * 			Ignacio Casal <nacho.resa@gmail.com>
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
#include "message.h"
#include "nautilus-string.h"
#include "parse.h"
#include "prefs.h"
#include "translator.h"
#include "utils.h"
#include "utils_gui.h"

#include <locale.h>
#include <time.h>

#include <gtk/gtk.h>
#include <glade/glade.h>


	 
/*
 * Glade path
 */
#define GLADE_HEADER_PATH "header_dialog.glade"

/*
 * Glade variables
 */

/*
 * Header dialog
 */
#define GLADE_E_HEADER "e_header"

/*
 * Project page
 */
#define GLADE_PRJ_COMMENT "prj_comment"
#define GLADE_PRJ_NAME "prj_name"
#define GLADE_PRJ_VERSION "prj_version"
#define GLADE_POT_DATE "pot_date"
#define GLADE_PO_DATE "po_date"
#define GLADE_RMBT "rmbt"

/*
 * Translator and language page
 */
#define GLADE_TAKE_MY_OPTIONS "take_my_options"
#define GLADE_LANG_PAGE "lang_page"
#define GLADE_TRANSLATOR "translator"
#define GLADE_TR_EMAIL "tr_email"
#define GLADE_LANGUAGE_COMBO "language_combo"
#define GLADE_LG_COMBO "lg_combo"
#define GLADE_CHARSET_COMBO "charset_combo"
#define GLADE_ENC_COMBO "enc_combo"

static GtkWidget *e_header = NULL;
static GladeXML *glade_header;

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
static gchar *get_current_year(void);
static void substitute(gchar **item, const gchar *bad, const gchar *good);
static void replace_substring(gchar **item, const gchar *bad, const gchar *good);

/*
 * DUPLICATED CODE: This func is in prefs.c
 */
static GtkWidget*
gtranslator_header_combo_new(GList *list, 
								  const char *value,
								  gchar *name, 
								  GCallback callback,
								  gpointer user_data)
{
	GtkWidget *combo;
	GtkTreeIter iter;
	GtkListStore* store;
	
	enum {
		COLUMN_STRING,
		N_COLUMNS
	};
	
	combo = glade_xml_get_widget(glade_header, name);
									  
	store = gtk_list_store_new(1, G_TYPE_STRING);
		
	while(TRUE)
    {
		if (list->data != NULL || list->data != '\0'){
			gtk_list_store_append(store, &iter);
			gtk_list_store_set(store, &iter, COLUMN_STRING, list->data, -1);
		}
		if ((list = g_list_next(list)) == NULL) break;
    }
									  
	gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(store));
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo), 0);

	if (value)
		gtk_entry_set_text(GTK_ENTRY(GTK_BIN(combo)->child), value);
	
	g_signal_connect(G_OBJECT(GTK_ENTRY(GTK_BIN(combo)->child)), "changed",
			 G_CALLBACK(callback), user_data);
	return combo;
}

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

GtrHeader *gtranslator_header_get(const char *string)
{
#ifdef DONTFORGET
	GtrHeader *ph;
	gchar **lines, **pair, *pos;
	gint i = 0;
#endif

	g_return_val_if_fail(string != NULL, NULL);

	g_warning("Header: %s", string);
#ifdef DONTFORGET	
	ph = g_new0(GtrHeader, 1);
	lines = g_strsplit(message->msgstr, "\n", 0);
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

	if(message->comment)
	{
		ph->comment=g_strdup(message->comment);
	}
	else
	{
		ph->comment=g_strdup("# ");
	}

	if (ph->prj_name)
		return ph;

#endif
	return NULL;
}

#ifdef PROBABLY_NOT_NEEDED_ANY_MORE
/*
 * Creates new GtrMsg, with all data set to current state of header 
 */
const char *gtranslator_header_put(GtrHeader *header)
{
	gchar *group;
	GtrMsg *msg = g_new0(GtrMsg, 1);
	gchar *version;
	gchar *msgstr;

	const gchar *lang=gtranslator_utils_get_english_language_name(header->language);

	if (header->lg_email && header->lg_email[0] != '\0')
		group = g_strdup_printf("%s <%s>", lang, header->lg_email);
	else
		group = g_strdup(lang);

	if (header->prj_version && header->prj_version[0] != '\0')
		version = g_strdup_printf("%s %s", header->prj_name, header->prj_version);
	else
		version = g_strdup(header->prj_name);

	if(!header->report_message_bugs_to)
	{
		header->report_message_bugs_to=g_strdup("");
	}

	if(header->plural_forms)
	{
		msgstr = g_strdup_printf("\n"\
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
		header->report_message_bugs_to,
		header->pot_date,
		header->po_date,
		header->translator, header->tr_email,
		group,
		header->mime_version,
		header->charset,
		header->encoding,
		header->plural_forms);
	}
	else
	{
		msgstr = g_strdup_printf("\n"\
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
		header->report_message_bugs_to,
		header->pot_date,
		header->po_date,
		header->translator, header->tr_email,
		group,
		header->mime_version,
		header->charset,
		header->encoding);
	}

	g_free(group);
	g_free(version);

	/*
	 * Just copy the comment, and make sure it ends with endline.
	 */
	if(header->comment[strlen(header->comment)-1] == '\n')
	{
		msg->comment = gtranslator_comment_new(header->comment);
	}
	else
	{
		gchar *comchar=g_strdup_printf("%s\n", header->comment);
		msg->comment = gtranslator_comment_new(comchar);
		g_free(comchar);
	}
	
	return (const char *)msgstr;
}
#endif

/*
 * Updates PO-Revision-Date field 
 */
void gtranslator_header_update(GtrHeader *header)
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
	g_free(header->po_date);
	header->po_date = g_strdup(t);

	/*
	 * Convert the header comments back if necessary.
	 */ 
	if(header->comment && header->comment[0]!='#')
	{
		header->comment=gtranslator_header_comment_convert_for_save(header->comment);
	}
}

void gtranslator_header_free(GtrHeader *header)
{
	g_return_if_fail(header!=NULL);

	g_free(header->comment);
	g_free(header->prj_name);
	g_free(header->prj_version);
	g_free(header->report_msgid_bugs_to);
	g_free(header->pot_date);
	g_free(header->po_date);
	g_free(header->translator);
	g_free(header->tr_email);
	g_free(header->language);
	g_free(header->lg_email);
	g_free(header->mime_version);
	g_free(header->charset);
	g_free(header->encoding);
	g_free(header->plural_forms);

	if(header->generator)
	{
		g_free(header->generator);
	}
	
	g_free(header);
}

static void gtranslator_header_edit_close(GtkWidget * widget, gint response, gpointer useless)
{
	gchar *prev_translator, *prev_translator_email;

	prev_translator=prev_translator_email=NULL;

	/*
	 * Free the languages list
	 */
	gtranslator_utils_language_lists_free(widget, useless);

	if(!header_changed || response == GTK_BUTTONS_CANCEL) {
		gtk_widget_destroy(GTK_WIDGET(e_header));
		return;
	}

	/*
	 * TODO: Extract values from editted header fields back into
	 * the message header. Waiting on a gettext function to handle this.
	 */

	/*
	 * Mark file as having unsaved changes
	 */
	current_page->po->file_changed = TRUE;
	gtranslator_actions_enable(ACT_SAVE, ACT_REVERT);
	
	gtk_widget_destroy(GTK_WIDGET(e_header));
	return;
}

/*
 * Creates the Header-edit dialog.
 */
void gtranslator_header_edit_dialog(GtkWidget * widget, gpointer useless)
{
	GtkTextBuffer *buffer;
	gchar *space;
	const char *headerstr;
	char *project_id_ver;
	char *project_name, *project_ver;
	char *pot_creation_date, *po_revision_date;
	char *report_msgid_bugs_to;
	char *last_translator, *last_translator_name, *last_translator_email;
	char *language_team, *language_name, *language_email;
	char *content_type, *content_charset;
	char *content_transfer_encoding;

	header_changed = FALSE;

	if(e_header != NULL) {
		gtk_window_present(GTK_WINDOW(e_header));
		return;
	}

	/*
	 * Grab the header string
	 */
	if(!(headerstr = po_file_domain_header(current_page->po->gettext_po_file, NULL)))
	{
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			_("No header for this file/domain"));
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}
	
	/*
	 * Unpack header into values for dialog
	 */
	project_id_ver = po_header_field(headerstr, "Project-Id-Version");
	space = strrchr(project_id_ver, ' ');
	if (space) {
		project_name = g_strndup(project_id_ver, space - project_id_ver);
		project_ver = g_strdup(space + 1);
	} else {
		project_name = g_strdup(project_id_ver);
		project_ver = g_strdup("");
	}
	pot_creation_date = po_header_field(headerstr, "POT-Creation-Date");
	po_revision_date = po_header_field(headerstr, "PO-Revision-Date");
	report_msgid_bugs_to = po_header_field(headerstr, "Report-Msgid-Bugs-To");
	last_translator = po_header_field(headerstr, "Last-Translator");
	language_team = po_header_field(headerstr, "Language-Team");
	content_type = po_header_field(headerstr, "Content-Type");
	content_transfer_encoding = po_header_field(headerstr, "Content-Transfer-Encoding");

	/*
	 * Break up translator name/email
	 */
	split_name_email(last_translator, &last_translator_name, &last_translator_email);
	
	/*
	 * Prepare the languages list pulldown information
	 */
	gtranslator_utils_language_lists_create();

	/*
	 * The main dialog
	 */
	glade_header = glade_xml_new(GLADE_HEADER_PATH, NULL, NULL);
	e_header = glade_xml_get_widget(glade_header, GLADE_E_HEADER);
	//gtk_dialog_set_default_response(GTK_DIALOG(e_header), GTK_RESPONSE_CLOSE);
	
	/*
	 * Project page
	 */

	/*Project comment*/
	prj_comment = glade_xml_get_widget(glade_header, GLADE_PRJ_COMMENT);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(prj_comment));
	//gtk_text_buffer_set_text(buffer, ph->comment, -1);
	gtk_widget_set_size_request(prj_comment, 360, 90);
	g_signal_connect(buffer, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	/*Project name*/
	prj_name = glade_xml_get_widget(glade_header, GLADE_PRJ_NAME);
	gtk_entry_set_text(GTK_ENTRY(prj_name), project_name);
	g_signal_connect(prj_name, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	/*Project version*/
	prj_version = glade_xml_get_widget(glade_header, GLADE_PRJ_VERSION);
	gtk_entry_set_text(GTK_ENTRY(prj_version), project_ver);
	g_signal_connect(prj_version, "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	/*Pot date*/
	pot_date = glade_xml_get_widget(glade_header, GLADE_POT_DATE);
	gtk_widget_set_sensitive(pot_date, FALSE);
	gtk_entry_set_text(GTK_ENTRY(pot_date), pot_creation_date);

	/*Po date*/
	po_date = glade_xml_get_widget(glade_header, GLADE_PO_DATE);
	gtk_widget_set_sensitive(po_date, FALSE);
	gtk_entry_set_text(GTK_ENTRY(po_date), po_revision_date);

	/*Report message string bugs to entry*/
	rmbt = glade_xml_get_widget(glade_header, GLADE_RMBT);
	gtk_entry_set_text(GTK_ENTRY(rmbt), report_msgid_bugs_to);
	gtk_widget_set_sensitive(rmbt, TRUE);


/*#ifdef DONTFORGET
	if(ph->generator)
	{*/
		/*
		 * Resize table, and add 'generator' row
		 */
/*		gtk_table_resize(GTK_TABLE(prj_page), 7, 2);

		foo_me_i_ve_been_wracked = gtk_entry_new();
		gtk_widget_set_sensitive(foo_me_i_ve_been_wracked, FALSE);
		gtk_entry_set_text(GTK_ENTRY(foo_me_i_ve_been_wracked), ph->generator);
		gtk_table_attach_defaults(GTK_TABLE(prj_page), gtk_label_new(_("Generator:")), 0, 1, 6, 7);
		gtk_table_attach_defaults(GTK_TABLE(prj_page), foo_me_i_ve_been_wracked, 1, 2, 6, 7);
	}
#endif*/

	/*
	 * Translator and language page
	 */

	/*Checkbox to take my options*/
	take_my_options = glade_xml_get_widget(glade_header, GLADE_TAKE_MY_OPTIONS);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(take_my_options), 
				     GtrPreferences.fill_header);
	g_signal_connect(G_OBJECT(take_my_options), "toggled",
			 G_CALLBACK(take_my_options_toggled), NULL);

	/*Table*/
	lang_page = glade_xml_get_widget(glade_header, GLADE_LANG_PAGE);
	gtk_widget_set_sensitive(lang_page, !GtrPreferences.fill_header);

	/*translator entry*/
	translator = glade_xml_get_widget(glade_header, GLADE_TRANSLATOR);
	gtk_entry_set_text(GTK_ENTRY(translator), last_translator_name);
	g_signal_connect(GTK_ENTRY(translator), "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);

	/*translator e-mail entry*/
	tr_email = glade_xml_get_widget(glade_header, GLADE_TR_EMAIL);
	gtk_entry_set_text(GTK_ENTRY(tr_email), last_translator_email);
	g_signal_connect(GTK_ENTRY(tr_email), "changed", G_CALLBACK(gtranslator_header_edit_changed), NULL);
	
	/* Here i have a big problem
	 * If take_my_options is set to true and code below is uncomment
	 * the program crashes
	 * but is set to false, program works fine
	 * Any idea?
	 */
	/*language combo*/
/*	language_combo = gtranslator_header_combo_new(languages_list,
												  language_name,
												  GLADE_LANGUAGE_COMBO, 
												  G_CALLBACK(gtranslator_header_edit_changed),
												  NULL);*/
	
	/*language group's email combo*/
/*	lg_combo = gtranslator_header_combo_new(group_emails_list,
											language_email,
											GLADE_LG_COMBO,
											G_CALLBACK(gtranslator_header_edit_changed),
											NULL);*/
	/*charset combo*/
/*	charset_combo = gtranslator_header_combo_new(encodings_list,
												 content_charset,
												 GLADE_CHARSET_COMBO,
												 G_CALLBACK(gtranslator_header_edit_changed),
												 NULL);*/
	/*Encoding combo*/
/*	enc_combo = gtranslator_header_combo_new(bits_list,
											 content_transfer_encoding,
											 GLADE_ENC_COMBO,
											 G_CALLBACK(gtranslator_header_edit_changed),
											 NULL);*/
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
	g_free(old);
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
		g_free(year);

		/*
		 * Fill header->po_date with current time 
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
		g_free(title);
	}

	return have_changed;
}

GtrHeader *gtranslator_header_create_from_prefs(void)
{
	GtrHeader *header = g_new0(GtrHeader, 1);
	gchar *year;

	header->prj_name=g_strdup("PACKAGE");
	header->prj_version=g_strdup("VERSION");
	header->report_msgid_bugs_to=g_strdup("");
	header->plural_forms=g_strdup("");
	
	/* 
	 * Fill header->po_date with current time 
	 */
	gtranslator_header_update(header);
	
	header->pot_date=g_strdup(header->po_date);
	header->translator=g_strdup(gtranslator_translator->name);
	header->tr_email=g_strdup(gtranslator_translator->email);
	header->language=g_strdup(gtranslator_translator->language->name);
	header->lg_email=g_strdup(gtranslator_translator->language->group_email);
	header->mime_version=g_strdup("1.0");
	header->charset=g_strdup(gtranslator_translator->language->encoding);
	header->encoding=g_strdup(gtranslator_translator->language->bits);

	year=get_current_year();

	header->comment=g_strdup_printf(
"# %s translation of PACKAGE.\n"
"# Copyright (C) %s Free Software Foundation, Inc.\n"
"# %s <%s>, %s.\n"
"#\n",
		header->language,
		year,
		header->translator,
		header->tr_email,
		year);

	g_free(year);
	return header;
}
