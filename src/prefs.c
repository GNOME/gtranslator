/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *					Gediminas Paulauskas <menesis@kabalak.net>
 *                  Ross Golder <ross@golder.org>
 *                  Søren Wedel Nielsen <swn@herlevkollegiet.dk>
 *					Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
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

#include "dialogs.h"
#include "find.h"
#include "gui.h"
#include "languages.h"
#include "messages-table.h"
#include "nautilus-string.h"
#include "page.h"
#include "prefs.h"
#include "stylistics.h"
#include "translator.h"
#include "utils.h"
#include "utils_gui.h"

/*
 * The callbacks:
 */
static void gtranslator_preferences_dialog_changed(GtkWidget  * widget, gpointer useless);
static void gtranslator_preferences_dialog_close(GtkWidget  * widget, gint response_id,
			    gpointer useless);
static void toggle_sensitive(GtkWidget *widget, gpointer data);
#ifdef GTR_ABOUT_ME
static void toggle_insensitive(GtkWidget *widget, gpointer data);
#endif

typedef struct {
	GtkTreeIter file_node;
	GtkTreeIter editor_node;
	GtkTreeIter poheader_node;
	GtkTreeIter functionality_node;
	GtkTreeIter autotranslation_node;
} GtrControlTable;
static GtrControlTable *control_table;


typedef struct {
	GtkTreeIter general_item;
	GtkTreeIter autosave_item;
	GtkTreeIter recentfiles_item;
} GtrControlFileItems;
static GtrControlFileItems *ctrlfile_node;

typedef struct {
	GtkTreeIter text_item;
	GtkTreeIter content_item;	
} GtrControlEditorItems;
static GtrControlEditorItems *ctrleditor_node;

typedef struct {
	GtkTreeIter personal_item;
	GtkTreeIter language_item;
} GtrControlHeaderItems;
static GtrControlHeaderItems *ctrlheader_node;

typedef struct {
	GtkTreeIter general_item;
	GtkTreeIter messagestable_item;
} GtrControlFunctItems;
static GtrControlFunctItems *ctrlfunct_node;

typedef struct {
	GtkTreeIter general_item;
	GtkTreeIter fuzzy_item;
} GtrControlAutotranItems;
static GtrControlAutotranItems *ctrlautotran_node;

/*
 * Glade:
 */
static GladeXML *glade_prefs;
#define GLADE_PREF_PATH DATADIR "/gtranslator/preferences.glade"
/**Variables**/
#define GLADE_PREF_DIALOG "preferences_dialog"
#define GLADE_TREE_VIEW "treeview"
#define GLADE_NOTEBOOK "notebook"
//Page 0: General
#define GLADE_WARN_IF_FUZZY "warn_if_fuzzy"
#define GLADE_SWEEP_COMPILE_FILE "sweep_compile_file"
//Page 1: Autosave
#define GLADE_AUTOSAVE "autosave"
#define GLADE_AUTOSAVE_TIMEOUT "autosave_timeout"
#define GLADE_AUTOSAVE_TIMEOUT_LABEL "autosave_timeout_label"
#define GLADE_AUTOSAVE_WITH_SUFFIX "autosave_with_suffix"
#define GLADE_AUTOSAVE_SUFFIX "autosave_suffix"
#define GLADE_AUTOSAVE_SUFFIX_LABEL "autosave_suffix_label"
//Page 2
#define GLADE_MAX_HISTORY_ENTRIES "max_history_entries"
#define GLADE_CHECK_RECENT_FILES "check_recent_files"
//Page 3
#define GLADE_HIGHLIGHT "highlight"
#define GLADE_USE_DOT_CHAR "use_dot_char"
#define GLADE_OWN_FONTS "own_fonts"
#define GLADE_MSGID_FONT "msgid_font"
#define GLADE_MSGID_LABEL "msgid_label"
#define GLADE_MSGSTR_FONT "msgstr_font"
#define GLADE_MSGSTR_LABEL "msgstr_label"
#define GLADE_OWN_COLORS "own_colors"
/*#define GLADE_MSGID_COLOR "msgid_color"
#define GLADE_MSGID_COLOR_LABEL "msgid_color_label"*/
#define GLADE_TEXT_COLOR "text_color"
#define GLADE_TEXT_COLOR_LABEL "text_color_label"
#define GLADE_RB_1 "rb_1"
#define GLADE_RB_2 "rb_2"
//Page 4
#define GLADE_UNMARK_FUZZY "unmark_fuzzy"
#define GLADE_KEEP_OBSOLETE "keep_obsolete"
#define GLADE_INSTANT_SPELL_CHECKING "instant_spell_checking"
#define GLADE_AUTHORS_NAME "authors_name"
#define GLADE_AUTHORS_EMAIL "authors_email"
//Page 5
#define GLADE_AUTHORS_LANGUAGE "authors_language"
#define GLADE_LCODE "lcode"
#define GLADE_MIME_TYPE "mime_type"
#define GLADE_ENCODING "encoding"
#define GLADE_LG_EMAIL "lg_email"
#define GLADE_PLURAL "plural"
#define GLADE_PLURAL_NOTE "plural_note"
//Page 6
#define GLADE_USE_UPDATE_FUNCTION "use_update_function"
#define GLADE_RAMBO_FUNCTION "rambo_function"
#define GLADE_SHOW_COMMENT "show_comment"
#define GLADE_SAVE_GEOMETRY_TB "save_geometry_tb"
//Page 7
#define GLADE_SHOW_MESSAGES_TABLE "show_messages_table"
#define GLADE_COLLAPSE_ALL_ENTRIES "collapse_all_entries"
//Page 8
#define GLADE_USE_LEARN_BUFFER "use_learn_buffer"
#define GLADE_AUTO_LEARN "auto_learn"
//Page 9
#define GLADE_FUZZY_MATCHING "fuzzy_matching"



/*
 * The entries:
 */
static GtkWidget
	*authors_name, *authors_email, *authors_language,
	*mime_type, *encoding, *plural, *lcode, *lg_email,
#ifdef GTR_ABOUT_ME
	*about_me_box, *my_name, *my_email,
#endif	
	*autosave_suffix, *hotkey_chars, *plural_note;

/*
 * The toggle buttons/labels used in the preferences box:
 */
static GtkWidget
	*warn_if_fuzzy, *unmark_fuzzy, *save_geometry_tb,
	*use_dot_char, *use_update_function,	*check_recent_files, 
	*own_fonts, *own_colors, *instant_spell_checking, 
	*keep_obsolete, *autosave, *autosave_with_suffix,
	*sweep_compile_file, *use_learn_buffer, *autosave_timeout_label,
	*show_messages_table, *rambo_function, *autosave_suffix_label,
	*collapse_all_entries, *auto_learn, *fuzzy_matching,
#ifdef GTR_ABOUT_ME
	*use_about_me,
#endif
	*show_comment, *highlight;

/*
 * The autosave etc. timeout GtkSpinButtons:
 */
static GtkWidget
	*autosave_timeout, *max_history_entries, *min_match_percentage;

/*
 * Font/color specific widgets used in the preferences box.
 */
static GtkWidget *msgid_font, *msgstr_font, *msgid_label, *msgstr_label,
		 *text_color, *text_color_label;

/*
 * The preferences dialog widget itself, plus a notebook.
 */
static GtkWidget *prefs = NULL, *prefs_notebook = NULL;

/*
 * Flag to set when something changes
 */
static gboolean prefs_changed;

//Maybe this is useful with glade to change the colors
/*GtkWidget* 
gtranslator_preferences_category_new_pack_start(GtkWidget *page,
												const gchar *caption,
												gchar *image_file) 
{
	GtkWidget *vbox;
	GdkPixbuf *image;
	GdkColor bg;
	GtkStyle *style;
	GtkWidget *event_box, *label, *hbox, *content_box;
	gchar *labeltext;

	vbox = gtk_vbox_new (FALSE, 5);

	labeltext = g_strconcat("<span weight=\"bold\" size=\"large\" foreground=\"white\">",caption,"</span>",NULL);
	label = gtk_label_new (labeltext);
	g_free(labeltext);
	gtk_label_set_use_markup(GTK_LABEL (label), TRUE);
	gtk_label_set_justify(GTK_LABEL (label), GTK_JUSTIFY_CENTER);
	gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);  
	
	event_box = gtk_event_box_new();
	style = gtk_widget_get_style(gtk_button_new());
	if (gdk_color_parse("#7f7f7f",&bg))
		gtk_widget_modify_bg(event_box, GTK_STATE_NORMAL, &bg);
	
	image = gtranslator_pixbuf_from_file(
				g_strconcat(DATADIR,"/pixmaps/gtranslator/",image_file,NULL));
	hbox = gtk_hbox_new(FALSE, 20);
	gtk_container_add(GTK_CONTAINER(event_box),hbox);
	gtk_box_pack_start(GTK_BOX (hbox), gtk_image_new_from_pixbuf(image),FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (hbox), label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX (vbox), event_box, FALSE, FALSE, 0);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

	content_box = gtk_vbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (hbox), content_box, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (page), vbox, TRUE, TRUE, 0);

	return content_box;
}*/



/*
 * Set sensitive of a widget and/or a label
 */
static void
gtranslator_preferences_pack_set_up_with_label(GtkWidget *widget, 
					       GtkWidget *label,
					       GtkWidget *depend,
					       gboolean expand)
{
	if (depend != NULL) {
		if (label != NULL){
			gtk_widget_set_sensitive(GTK_WIDGET(label), 
					 gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(depend)));
			g_signal_connect(G_OBJECT(depend), "toggled",
				 G_CALLBACK(toggle_sensitive), label); 
		}
		if (widget != NULL) {
			gtk_widget_set_sensitive(GTK_WIDGET(widget), 
					 gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(depend)));
			g_signal_connect(G_OBJECT(depend), "toggled",
				 G_CALLBACK(toggle_sensitive), widget);
		}
	}
}


//This could be useful to use it in header_stuff.c
static GtkWidget*
gtranslator_preferences_combo_new(GList *list, 
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
	
	combo = glade_xml_get_widget(glade_prefs, name);
									  
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

/*
 * Set up hotkey chars
 */
static void
gtranslator_preferences_hotkey_char_widget_new()
{
	GtkWidget *rb_1, *rb_2;
	
	rb_1 = glade_xml_get_widget(glade_prefs, GLADE_RB_1);
	rb_2 = glade_xml_get_widget(glade_prefs, GLADE_RB_2);

	if(GtrPreferences.hotkey_char=='_')
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_1), TRUE);
	}
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_2), TRUE);
	}
	
	g_signal_connect(G_OBJECT(rb_1), "toggled", G_CALLBACK(gtranslator_preferences_dialog_changed), GINT_TO_POINTER(10));
	g_signal_connect(G_OBJECT(rb_2), "toggled", G_CALLBACK(gtranslator_preferences_dialog_changed), GINT_TO_POINTER(11));
}

/*
 * Set up entries
 */
static GtkWidget*
gtranslator_preferences_entry_new(const char *value,
				  gchar *name, 
				  GCallback callback)
{
	GtkWidget *entry;

	entry = glade_xml_get_widget(glade_prefs, name);
	if (value)
		gtk_entry_set_text(GTK_ENTRY(entry), value);
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(callback), NULL);
	return entry;
}


/*
 * Set up and return toggle buttons
 */
static GtkWidget *
gtranslator_preferences_toggle_new(gchar *name,
				   gboolean value,
				   GCallback callback)
{
	GtkWidget *toggle;
	toggle = glade_xml_get_widget(glade_prefs, name);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), value);
	if(callback)
	{
		g_signal_connect(G_OBJECT(toggle), "toggled",
				 G_CALLBACK(callback), NULL);
	}
  
	return toggle;
}

/*
 * Set up font button
 */
static GtkWidget *
gtranslator_preferences_font_picker_new(const gchar *name,
					const gchar *fontspec,
					GCallback callback)
{
	GtkWidget *font_selector = NULL;

	font_selector = glade_xml_get_widget(glade_prefs, name);
	if(fontspec)
		gtk_font_button_set_font_name(GTK_FONT_BUTTON(font_selector), fontspec);
	g_signal_connect(G_OBJECT(font_selector), "font-set",
			 G_CALLBACK(callback), NULL);
	return font_selector;
}

static GtkWidget *
gtranslator_preferences_color_button_new(const gchar *name,
					 GCallback callback)
{
	GtkWidget *color_button;
		
	color_button = glade_xml_get_widget(glade_prefs, name);
	g_signal_connect(G_OBJECT(color_button), "color-set",
			 G_CALLBACK(callback), NULL);
	return color_button;
}

enum
{
	CATEGORY_COLUMN,
	PAGENUM_COLUMN,
	N_COLUMNS
};

GtkTreePath* prev_path = NULL;

void
gtranslator_control_table_node_expanded(GtkTreeView *treeview,
					GtkTreeIter *arg1,
					GtkTreePath *new_path,
					gpointer selection)
{	
	if (prev_path != NULL) {
		if(gtk_tree_path_get_depth(prev_path) > 1)
			gtk_tree_path_up(prev_path);
	}
	gtk_tree_path_down(new_path);
	gtk_tree_selection_select_path(selection,new_path);
}

static void 
gtranslator_control_table_selection_changed(GtkTreeSelection *selection,
					    gpointer data)
{
	GtkTreePath *new_path, *new_path_parent = gtk_tree_path_new();
	GtkTreeIter iter;
	GtkTreeModel *model;
	gint page;
	GtkTreeView *view;
  
	view = gtk_tree_selection_get_tree_view(selection);
  
	if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
	{
    		gtk_tree_model_get(model, &iter, PAGENUM_COLUMN, &page, -1);
	
		/*compare old & new selected path*/
		new_path = gtk_tree_model_get_path(model, &iter);
    		new_path_parent = gtk_tree_path_copy(new_path);
	
		if(prev_path != NULL)
		{		
			if(gtk_tree_path_get_depth(prev_path) > 1)
				gtk_tree_path_up(prev_path);
			if(gtk_tree_path_get_depth(new_path) > 1)
				gtk_tree_path_up(new_path_parent);
			
		/*collapse old node*/
			if(gtk_tree_path_compare(prev_path,new_path_parent) !=0)
				gtk_tree_view_collapse_row(view,prev_path);
		}
		/*expand new node*/
		gtk_tree_view_expand_to_path(view,new_path);
	
		/*show new page*/
		if(page !=-1)
		{  
	  		gtk_notebook_set_current_page(GTK_NOTEBOOK(prefs_notebook),page);
		} else { 
	  		gtk_tree_path_down(new_path);
	  		gtk_tree_selection_select_path(selection,new_path);	  
		}
		prev_path = gtk_tree_path_copy(new_path);
		gtk_tree_path_free(new_path);
		gtk_tree_path_free(new_path_parent);
	}

}


GtkWidget *gtranslator_preferences_dialog_control_new()
{
	GtkWidget *control;
	GtkTreeViewColumn *column;
	GtkTreeStore *store;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	
	store = gtk_tree_store_new (
		N_COLUMNS,
		G_TYPE_STRING,
		G_TYPE_INT);
	
	//control = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
	control = glade_xml_get_widget(glade_prefs, GLADE_TREE_VIEW);
	gtk_tree_view_set_model(GTK_TREE_VIEW(control), GTK_TREE_MODEL(store));
	//gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (control), TRUE);
	
	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes(_(" Categories: "), renderer,
	"text", CATEGORY_COLUMN, NULL);
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (control), column);
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (control));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT(selection), "changed", 
		    G_CALLBACK(gtranslator_control_table_selection_changed), 
		    NULL);
	g_signal_connect (G_OBJECT(control), "row-expanded", 
		    G_CALLBACK(gtranslator_control_table_node_expanded), 
		    selection);
	//gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(control),FALSE);
	return control;
}

void gtranslator_preferences_plural_string_request(gchar *locale){
	gchar *plural_str = NULL, *msg = NULL;
	
	msg = _("Note: It was not possible to determine GNU header for plural forms. \
Your GNU gettext tools may be too old or they do not contain \
a suggested value for your language.");

	plural_str = gtranslator_get_plural_form_string(locale);

	if (g_string_equal(g_string_new(""), g_string_new(gtk_entry_get_text(GTK_ENTRY(plural)))))
	{
		if (plural_str != NULL)
				msg = _("Note: Your settings do not contain a plural forms string. These data retrieved from GNU gettext tools.");		  
	} else {
		if (!g_string_equal(g_string_new(plural_str), g_string_new(gtk_entry_get_text(GTK_ENTRY(plural)))))
		{	
			if (plural_str != NULL) {
				msg = _("Note: Your settings contain an invalid plural forms string. These data retrieved from GNU gettext tools.");
			} else {
				gtk_label_set_text(GTK_LABEL(plural_note),"");
				return;
			}
		} else {
				msg = "";
		}
	}
	if (plural_str == NULL) plural_str = "";
	gtk_entry_set_text(GTK_ENTRY(plural),plural_str);
	gtk_label_set_text(GTK_LABEL(plural_note), g_strconcat("<i>", msg,"</i>",NULL));
	gtk_label_set_use_markup(GTK_LABEL(plural_note), TRUE);
	gtk_label_set_line_wrap (GTK_LABEL(plural_note), TRUE);
	
}

#ifdef GTR_ABOUT_ME
void gtranslator_preferences_no_aboutme(gchar *message)
{
	GtkWidget *about_me_note;
	about_me_note = gtk_label_new(message);
	gtk_label_set_line_wrap (GTK_LABEL(about_me_note), TRUE);
	gtk_label_set_use_markup(GTK_LABEL(about_me_note), TRUE);
	gtk_box_pack_start (GTK_BOX(about_me_box), 
					about_me_note,
					FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(use_about_me),FALSE);
	gtk_widget_set_sensitive(use_about_me,FALSE);
}
#endif

void gtranslator_preferences_dialog_create(GtkWidget *widget, gpointer useless)
{
 	GtkObject *adjustment;

	GtkWidget *page, *category_box, *hbox, *control, *dialog_hbox, *label;

	prefs_changed = FALSE;

 	if(prefs != NULL) {
		gtk_window_present(GTK_WINDOW(prefs));
		return;
 	}
	
	/*
	 * Initialize glade
	 */
	glade_prefs = glade_xml_new(GLADE_PREF_PATH, NULL, NULL);
 
 	/*
 	 * Create the preferences box... 
 	 */
 	prefs = glade_xml_get_widget(glade_prefs, GLADE_PREF_DIALOG);

	
	/*
	 * Control widget
	 */
	control = gtranslator_preferences_dialog_control_new();

	/*
	 * The notebook
	 */
	prefs_notebook = glade_xml_get_widget(glade_prefs, GLADE_NOTEBOOK);
 
	GtkTreeStore *model;
	model = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(control)));
	control_table=g_new0(GtrControlTable, 1);

	/*
	 * Files category
	 */
	gtk_tree_store_append (model, &control_table->file_node, NULL);
	gtk_tree_store_set (model, &control_table->file_node, 
		      CATEGORY_COLUMN, _("Files"), 
			  PAGENUM_COLUMN, -1,
		      -1);
			  
	ctrlfile_node=g_new0(GtrControlFileItems, 1);
	
	/* General item */
	gchar *general_str = _("General");
	gtk_tree_store_append(model, &ctrlfile_node->general_item, &control_table->file_node);
	gtk_tree_store_set(model, &ctrlfile_node->general_item,
			 CATEGORY_COLUMN, general_str, 
			 PAGENUM_COLUMN, 0,
			 -1);
	//Ver isto
	/*page = gtranslator_preferences_page_new_append(prefs_notebook, general_str); 
	category_box = gtranslator_preferences_category_new_pack_start(page, general_str,"files.png");
	*/
	
	
	//warn_if_fuzzy toggle button
	warn_if_fuzzy = gtranslator_preferences_toggle_new(GLADE_WARN_IF_FUZZY,
							   GtrPreferences.warn_if_fuzzy,
							   G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	//sweep_compile_file toggle button
	sweep_compile_file = gtranslator_preferences_toggle_new(GLADE_SWEEP_COMPILE_FILE,
								GtrPreferences.sweep_compile_file,
								G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	
	
	/* Autosave item */
	gchar *autosave_str = _("Autosave");
	gtk_tree_store_append(model, &ctrlfile_node->autosave_item, &control_table->file_node);
	gtk_tree_store_set(model, &ctrlfile_node->autosave_item,
			 CATEGORY_COLUMN, autosave_str, 
			  PAGENUM_COLUMN, 1,
			 -1);
	/*page = gtranslator_preferences_page_new_append(prefs_notebook, autosave_str); 
	category_box = gtranslator_preferences_category_new_pack_start(page, autosave_str,"auto.png");*/

	//Autosave check button
	autosave = gtranslator_preferences_toggle_new(GLADE_AUTOSAVE,
						      GtrPreferences.autosave,
						      G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	
	//Autosave_timeout spin button
	adjustment = gtk_adjustment_new (GtrPreferences.autosave_timeout, 1.0, 30.0, 1.0, 1.0, 1.0);
	autosave_timeout = glade_xml_get_widget(glade_prefs, GLADE_AUTOSAVE_TIMEOUT);
	gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(autosave_timeout), GTK_ADJUSTMENT(adjustment));
 	gtk_widget_set_sensitive(GTK_WIDGET(autosave_timeout), GtrPreferences.autosave);
	g_signal_connect(G_OBJECT(autosave_timeout), "changed",
 			 G_CALLBACK(gtranslator_preferences_dialog_changed), NULL);
	autosave_timeout_label = glade_xml_get_widget(glade_prefs, GLADE_AUTOSAVE_TIMEOUT_LABEL);
	gtranslator_preferences_pack_set_up_with_label(autosave_timeout,
						       autosave_timeout_label,
						       autosave, FALSE);
    
	//autosave_with_suffix check button
	autosave_with_suffix = gtranslator_preferences_toggle_new(GLADE_AUTOSAVE_WITH_SUFFIX,
								  GtrPreferences.autosave_with_suffix,
								  G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	
	//Autosave_suffix entry
	autosave_suffix = gtranslator_preferences_entry_new(GtrPreferences.autosave_suffix,
							    GLADE_AUTOSAVE_SUFFIX, 
							    G_CALLBACK(gtranslator_preferences_dialog_changed));
	autosave_suffix_label = glade_xml_get_widget(glade_prefs, GLADE_AUTOSAVE_SUFFIX_LABEL);
	gtranslator_preferences_pack_set_up_with_label(autosave_suffix, 
						       autosave_suffix_label, 
						       autosave_with_suffix,
						       FALSE);
	
	
	/* Recent item */
	gchar *recent_str = _("Recent files");
	gtk_tree_store_append(model, &ctrlfile_node->recentfiles_item, &control_table->file_node);
	gtk_tree_store_set(model, &ctrlfile_node->recentfiles_item,
			 CATEGORY_COLUMN, recent_str, 
			  PAGENUM_COLUMN, 2,
			 -1);
	
	/*
	page = gtranslator_preferences_page_new_append(prefs_notebook, recent_str);
	category_box =  gtranslator_preferences_category_new_pack_start(page, recent_str,"recent.png");
	*/
	
	//max_history_entries spin button
	adjustment = gtk_adjustment_new (GtrPreferences.max_history_entries, 3, 15, 1, 10, 10);
	max_history_entries = glade_xml_get_widget(glade_prefs, GLADE_MAX_HISTORY_ENTRIES);
	gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(max_history_entries), GTK_ADJUSTMENT(adjustment));
	g_signal_connect(G_OBJECT(max_history_entries), "changed",
 			 G_CALLBACK(gtranslator_preferences_dialog_changed), NULL);
	
	
	//check_recent_files check button	
	check_recent_files = gtranslator_preferences_toggle_new(GLADE_CHECK_RECENT_FILES,
								GtrPreferences.check_recent_file,
								G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	 
	/*
	 * Editor category
	 */
	ctrleditor_node=g_new0(GtrControlEditorItems, 1);		 
	gtk_tree_store_append (model, &control_table->editor_node, NULL);
	gtk_tree_store_set (model, &control_table->editor_node, 
		      CATEGORY_COLUMN, _("Editor"), 
			  PAGENUM_COLUMN, -1,
		      -1);
			  
	/* Text item */
	gchar *display_str = _("Text display");
	
	GtkSizeGroup *font_label_size_group, *font_control_size_group;
	font_label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	font_control_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	
	gtk_tree_store_append(model, &ctrleditor_node->text_item, &control_table->editor_node);			  
	gtk_tree_store_set(model, &ctrleditor_node->text_item,
			 CATEGORY_COLUMN, display_str, 
			  PAGENUM_COLUMN, 3,
			 -1);
	//page = gtranslator_preferences_page_new_append(prefs_notebook, display_str);
	
	//category_box = gtranslator_preferences_category_new_pack_start(page, display_str,"text.png");
	
	//highlight toggle button
    	highlight = gtranslator_preferences_toggle_new(GLADE_HIGHLIGHT,
						       GtrPreferences.highlight,
						       G_CALLBACK(gtranslator_preferences_dialog_changed));

	//use_dot_char toggle button
    	use_dot_char = gtranslator_preferences_toggle_new(GLADE_USE_DOT_CHAR,
							  GtrPreferences.dot_char,
							  G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	
	//Set up hotkey chars
	gtranslator_preferences_hotkey_char_widget_new();
	
	//own_fonts check button	
	own_fonts = gtranslator_preferences_toggle_new(GLADE_OWN_FONTS,
						       GtrPreferences.use_own_fonts,
						       G_CALLBACK(gtranslator_preferences_dialog_changed));	
	
	
	//Set up msgid_font button maybe is not neccessary return the widget
	msgid_font = gtranslator_preferences_font_picker_new(GLADE_MSGID_FONT, 
							   GtrPreferences.msgid_font,
							   G_CALLBACK(gtranslator_preferences_dialog_changed));	
	msgid_label = glade_xml_get_widget(glade_prefs, GLADE_MSGID_LABEL);
	gtranslator_preferences_pack_set_up_with_label(msgid_font, msgid_label, own_fonts, TRUE);
	
	
	//Set up msgstr_font button
	msgstr_font = gtranslator_preferences_font_picker_new(GLADE_MSGSTR_FONT, 
							      GtrPreferences.msgstr_font,
							      G_CALLBACK(gtranslator_preferences_dialog_changed));
	msgstr_label = glade_xml_get_widget(glade_prefs, GLADE_MSGSTR_LABEL);
	gtranslator_preferences_pack_set_up_with_label(msgstr_font, msgstr_label, own_fonts, TRUE);

	//own_colors check button
	own_colors = gtranslator_preferences_toggle_new(GLADE_OWN_COLORS,
							GtrPreferences.use_own_colors,
							G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	//Set up text_color button
	text_color = gtranslator_preferences_color_button_new(GLADE_TEXT_COLOR,
							      G_CALLBACK(gtranslator_preferences_dialog_changed));
	text_color_label = glade_xml_get_widget(glade_prefs, GLADE_TEXT_COLOR_LABEL);
	gtranslator_preferences_pack_set_up_with_label(text_color, text_color_label, own_colors, TRUE);
	gtranslator_color_values_get(GTK_COLOR_BUTTON(text_color), COLOR_FG);
	
	/* Contents item */
	gchar *content_str = _("Contents");
	
	GtkSizeGroup *spell_label_size_group, *spell_control_size_group;
	spell_label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	spell_control_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	
	gtk_tree_store_append(model, &ctrleditor_node->content_item, &control_table->editor_node);			  
	gtk_tree_store_set(model, &ctrleditor_node->content_item,
			 CATEGORY_COLUMN, content_str, 
			  PAGENUM_COLUMN, 4,
			 -1);	
	/*page = gtranslator_preferences_page_new_append(prefs_notebook, content_str);
	category_box = gtranslator_preferences_category_new_pack_start(page, content_str,"content.png");*/
	
	
	//unmark_fuzzy toggle button
	unmark_fuzzy = gtranslator_preferences_toggle_new(GLADE_UNMARK_FUZZY,
							  GtrPreferences.unmark_fuzzy,
							  G_CALLBACK(gtranslator_preferences_dialog_changed));
	
		
	//keep_obsolete toggle button
    	keep_obsolete = gtranslator_preferences_toggle_new(GLADE_KEEP_OBSOLETE,
							   GtrPreferences.keep_obsolete,
							   G_CALLBACK(gtranslator_preferences_dialog_changed));
	
		
	//instant_spell_checking toggle button
	instant_spell_checking = gtranslator_preferences_toggle_new(GLADE_INSTANT_SPELL_CHECKING,
								    GtrPreferences.instant_spell_check,
								    G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	                                                                                
	/*
	 * Po header category
	 */
	ctrlheader_node=g_new0(GtrControlHeaderItems, 1);		 
	gtk_tree_store_append (model, &control_table->poheader_node, NULL);
	gtk_tree_store_set (model, &control_table->poheader_node, 
		      CATEGORY_COLUMN, _("PO header"), 
			  PAGENUM_COLUMN, -1,
		      -1);
			  
	/* personal info item */
	GtkSizeGroup *personal_label_size_group, *personal_control_size_group;
	personal_label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	personal_control_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	
	gchar *personal_str = _("Personal Information");
	gtk_tree_store_append(model, &ctrlheader_node->personal_item, &control_table->poheader_node);			  
	gtk_tree_store_set(model, &ctrlheader_node->personal_item,
			 CATEGORY_COLUMN, personal_str, 
			  PAGENUM_COLUMN, 5,
			 -1);
	/*page = gtranslator_preferences_page_new_append(prefs_notebook, _("PO header")); 
	
	category_box = gtranslator_preferences_category_new_pack_start(page, personal_str,"about_me.png");*/
	
	GtkWidget *personal_info_box;
	personal_info_box = gtk_vbox_new(FALSE,0);
	
	gchar	*name = NULL;
	gchar	*email = NULL;
	
//#ifdef GTR_ABOUT_ME
	/*use_about_me = gtranslator_preferences_toggle_new(_("Use self contact info from Evolution Data Server"),
								    GtrPreferences.use_about_me,
								    G_CALLBACK(gtranslator_preferences_dialog_changed));
			
	gtk_box_pack_start (GTK_BOX(category_box), use_about_me, FALSE, FALSE, 0);
	
	about_me_box = gtk_vbox_new(FALSE,0);
	
	if (gtranslator_config_about_me(&name, &email) && !(name == NULL || name == '\0')) {
		if (email == NULL) {
			gtransaltor_preferences_no_aboutme(_("<i>Unable to get email from Evolution Data Server.</i>"));
		} else {
			my_name = gtk_label_new(name);
			gtk_label_set_justify(GTK_LABEL(my_name), GTK_JUSTIFY_LEFT);
			gtranslator_preferences_pack_start_with_label(about_me_box, my_name, personal_label_size_group, 
						    NULL, _("My name:"),FALSE);
			my_email = gtk_label_new(email);
			gtk_label_set_justify(GTK_LABEL(my_email), GTK_JUSTIFY_LEFT);
			gtranslator_preferences_pack_start_with_label(about_me_box, my_email,
							personal_label_size_group, 
						    NULL, _("My Email:"),FALSE);
		}
		
		GtkWidget *another_info;
		another_info = gtk_label_new(_("<b>Author's info</b>"));
		gtk_label_set_use_markup(GTK_LABEL(another_info), TRUE);
		gtk_box_pack_start (GTK_BOX(personal_info_box), another_info, FALSE, FALSE, 5);
	} else {
		gtranslator_preferences_no_aboutme(_("<i>Unable to get personal info from Evolution Data Server.</i>"));
	}
	gtk_box_pack_start (GTK_BOX(category_box), about_me_box, FALSE, FALSE, 0);
	gtk_widget_set_sensitive(about_me_box, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(use_about_me)));	
	g_signal_connect(G_OBJECT(use_about_me), "toggled",
					 G_CALLBACK(toggle_sensitive), about_me_box);	
#endif*/
	name = gtranslator_translator->name;
	email = gtranslator_translator->email;
/*#ifdef GTR_ABOUT_ME	
	name = gtranslator_config_get_string("translator/name");
	email = gtranslator_config_get_string("translator/email");
#endif	*/
	
	//authors_name entry
	authors_name = gtranslator_preferences_entry_new(name, 
							 GLADE_AUTHORS_NAME, 
							 G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	//authors_email entry
	authors_email = gtranslator_preferences_entry_new(email,
							  GLADE_AUTHORS_EMAIL,
							  G_CALLBACK(gtranslator_preferences_dialog_changed));
	
/*#ifdef GTR_ABOUT_ME
	gtk_widget_set_sensitive(personal_info_box, 
					!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(use_about_me)));
	g_signal_connect(G_OBJECT(use_about_me), "toggled",
					G_CALLBACK(toggle_insensitive), personal_info_box);
#endif*/

	/* Language item */
	gchar *language_str = _("Language settings");
	
	GtkSizeGroup *language_label_size_group, *language_control_size_group;
	language_label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	language_control_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	
	gtk_tree_store_append(model, &ctrlheader_node->language_item, &control_table->poheader_node);			  
	gtk_tree_store_set(model, &ctrlheader_node->language_item,
			 CATEGORY_COLUMN, language_str, 
			  PAGENUM_COLUMN, 6,
			 -1);
	/*page = gtranslator_preferences_page_new_append(prefs_notebook, language_str); 			 
	category_box = gtranslator_preferences_category_new_pack_start(page, language_str,"language.png");*/
	gtranslator_utils_language_lists_create();
	
	//authors_language combobox
	authors_language = gtranslator_preferences_combo_new(languages_list,
							     gtranslator_translator->language->name,
							     GLADE_AUTHORS_LANGUAGE,
							     G_CALLBACK(gtranslator_preferences_dialog_changed),
							     GINT_TO_POINTER(1));
	
	//lcode combobox
	lcode = gtranslator_preferences_combo_new(lcodes_list,
						  gtranslator_translator->language->locale,
						  GLADE_LCODE, 
						  G_CALLBACK(gtranslator_preferences_dialog_changed),
						  GINT_TO_POINTER(2));
	
	//mime_type combobox
	mime_type = gtranslator_preferences_combo_new(encodings_list,
						      gtranslator_translator->language->encoding,
						      GLADE_MIME_TYPE, 
						      G_CALLBACK(gtranslator_preferences_dialog_changed),
						      NULL);
	
	//encoding combobox
	encoding = gtranslator_preferences_combo_new(bits_list,
						     gtranslator_translator->language->bits,
						     GLADE_ENCODING, 
						     G_CALLBACK(gtranslator_preferences_dialog_changed),
						     NULL);

	//lg_email combobox
	lg_email = gtranslator_preferences_combo_new(group_emails_list, 
						     gtranslator_translator->language->group_email,
						     GLADE_LG_EMAIL, 
						     G_CALLBACK(gtranslator_preferences_dialog_changed),
						     NULL);

	//plural entry
	plural = gtranslator_preferences_entry_new(gtranslator_translator->language->plural,
						   GLADE_PLURAL,
						   G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	
	//label showing information about plural forms
	plural_note = glade_xml_get_widget(glade_prefs, GLADE_PLURAL_NOTE);
	gtranslator_preferences_plural_string_request(gtranslator_translator->language->locale);
	
	/*
	 * Functionality category
	 */
	ctrlfunct_node=g_new0(GtrControlFunctItems, 1);	
	gtk_tree_store_append (model, &control_table->functionality_node, NULL);
	gtk_tree_store_set (model, &control_table->functionality_node, 
		      CATEGORY_COLUMN, _("Functionality"), 
			  PAGENUM_COLUMN, -1,
		      -1);
	/* General item */
	GtkSizeGroup *label_size_group, *control_size_group;
	label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	control_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	gtk_tree_store_append(model, &ctrlfunct_node->general_item, &control_table->functionality_node);			  
	gtk_tree_store_set(model, &ctrlfunct_node->general_item,
			 CATEGORY_COLUMN, general_str, 
			  PAGENUM_COLUMN, 7,
			 -1);
	/*page = gtranslator_preferences_page_new_append(prefs_notebook, general_str);
	label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	category_box = gtranslator_preferences_category_new_pack_start(page, general_str,"func.png");*/
	
	//use_update_function toggle button
	use_update_function = gtranslator_preferences_toggle_new(GLADE_USE_UPDATE_FUNCTION,
															 GtrPreferences.update_function,
															 G_CALLBACK(gtranslator_preferences_dialog_changed));
		
	//rambo_function toggle button
	rambo_function = gtranslator_preferences_toggle_new(GLADE_RAMBO_FUNCTION,
														GtrPreferences.rambo_function,
														G_CALLBACK(gtranslator_preferences_dialog_changed));
		
	//show_comment toggle button
	show_comment = gtranslator_preferences_toggle_new(GLADE_SHOW_COMMENT,
													  GtrPreferences.show_comment,
													  G_CALLBACK(gtranslator_preferences_dialog_changed));
		
	//save_geometry_tb toggle button
	save_geometry_tb = gtranslator_preferences_toggle_new(GLADE_SAVE_GEOMETRY_TB,
														  GtrPreferences.save_geometry,
														  G_CALLBACK(gtranslator_preferences_dialog_changed));
		
	/* Messages table item */
	gchar *messagestable_str = _("Messages table");
	gtk_tree_store_append(model, &ctrlfunct_node->messagestable_item, &control_table->functionality_node);			  
	gtk_tree_store_set(model, &ctrlfunct_node->messagestable_item,
			 CATEGORY_COLUMN, messagestable_str, 
			  PAGENUM_COLUMN, 8,
			 -1);
	/*page = gtranslator_preferences_page_new_append(prefs_notebook, messagestable_str);
	category_box = gtranslator_preferences_category_new_pack_start(page, messagestable_str,"table.png");*/
	
	
	//show_messages_table toggle button
	show_messages_table = gtranslator_preferences_toggle_new(GLADE_SHOW_MESSAGES_TABLE,
															 GtrPreferences.show_messages_table,
															 G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	//collapse_all_entries toggle button
	collapse_all_entries = gtranslator_preferences_toggle_new(GLADE_COLLAPSE_ALL_ENTRIES,
															  GtrPreferences.collapse_all,
															  G_CALLBACK(gtranslator_preferences_dialog_changed));
	

	/*
	 * Autotranslation category
	 */
	ctrlautotran_node=g_new0(GtrControlAutotranItems, 1);			 
	gtk_tree_store_append (model, &control_table->autotranslation_node, NULL);
	gtk_tree_store_set (model, &control_table->autotranslation_node, 
		      CATEGORY_COLUMN, _("Autotranslation"), 
			  PAGENUM_COLUMN, -1,
		      -1);
				  
	/* General item */
	gtk_tree_store_append(model, &ctrlautotran_node->general_item, &control_table->autotranslation_node);			  
	gtk_tree_store_set(model, &ctrlautotran_node->general_item,
			 CATEGORY_COLUMN, general_str, 
			  PAGENUM_COLUMN, 9,
			 -1);
	/*page = gtranslator_preferences_page_new_append(prefs_notebook, general_str);
	category_box = gtranslator_preferences_category_new_pack_start(page, general_str,"autotrans.png");*/
															 
	//use_learn_buffer toggle button
	use_learn_buffer = gtranslator_preferences_toggle_new(GLADE_USE_LEARN_BUFFER,
														  GtrPreferences.use_learn_buffer,
														  G_CALLBACK(gtranslator_preferences_dialog_changed));
	
	//auto_learn toggle button
	auto_learn = gtranslator_preferences_toggle_new(GLADE_AUTO_LEARN,
													GtrPreferences.auto_learn,
													G_CALLBACK(gtranslator_preferences_dialog_changed));
		
	/* Fuzzy item */
	gchar *fuzzy_str = _("Fuzzy matching");
	gtk_tree_store_append(model, &ctrlautotran_node->fuzzy_item, &control_table->autotranslation_node);			  
	gtk_tree_store_set(model, &ctrlautotran_node->fuzzy_item,
			 CATEGORY_COLUMN, fuzzy_str, 
			  PAGENUM_COLUMN, 10,
			 -1);
	/*page = gtranslator_preferences_page_new_append(prefs_notebook, fuzzy_str);
	category_box = gtranslator_preferences_category_new_pack_start(page, fuzzy_str, "fuzzy.png");*/
	
	//fuzzy_matching toggle button
	fuzzy_matching = gtranslator_preferences_toggle_new(GLADE_FUZZY_MATCHING,
							    GtrPreferences.fuzzy_matching,
							    G_CALLBACK(gtranslator_preferences_dialog_changed));

	
	/*
	 * Connect the signals to the preferences box.
	 */
	/*g_signal_connect(G_OBJECT(prefs), "response",
			 G_CALLBACK(gtranslator_preferences_dialog_close), NULL);*/
	glade_xml_signal_connect_data(glade_prefs, 
								  "on_preferences_dialog_response",
								  G_CALLBACK(gtranslator_preferences_dialog_close),
								  prefs);

	gtranslator_dialog_show(&prefs, "gtranslator -- prefs");
}
 

/*
 * The actions to take when the user presses "Apply".
 */
static void gtranslator_preferences_dialog_close(GtkWidget * widget, gint response_id, gpointer useless)
{
	gchar	*translator_str=NULL;
	gchar	*translator_email_str=NULL;
	
	/*
	 * Free the languages list
	 */
	gtranslator_utils_language_lists_free(widget, useless);

	/*
	 * If nothing changed, just return
	 */
	if(response_id != GTK_RESPONSE_OK || !prefs_changed) {
		gtk_widget_destroy(GTK_WIDGET(prefs));
		return;
	}

#define update(value,widget) g_free(value); \
	value=gtk_editable_get_chars(GTK_EDITABLE(widget),0,-1);
#ifdef GTR_ABOUT_ME	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(use_about_me)))
	{
		g_free(translator_str);
		translator_str = g_strdup(gtk_label_get_text(GTK_LABEL(my_name)));
	} else {
#endif
	update(translator_str, authors_name);
#ifdef GTR_ABOUT_ME
	}
#endif	

	/*
	 * Check if the user did forget to enter his/her name into the prefs
	 *  dialog.
	 */
	if(!translator_str || *translator_str=='\0')
	{
		GtkWidget *dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			_("Please enter your name!"));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		return;
	}
	
#ifdef GTR_ABOUT_ME	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(use_about_me)))
	{
		g_free(translator_email_str);
		translator_email_str = g_strdup(gtk_label_get_text(GTK_LABEL(my_email)));
	} else {
#endif
	update(translator_email_str, authors_email);
#ifdef GTR_ABOUT_ME
	}
#endif
	/*
	 * Also check if the user did forget to enter his/her EMail address
	 *  into the prefs dialog.
	 */
	if(!translator_email_str || *translator_email_str=='\0')
	{
		GtkWidget *dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			_("Please enter your EMail address!"));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		return;
	}
	else
	{
		/*
		 * Also check an eventually given EMail address for brevity.
		 */
		if(!strchr(translator_email_str, '@') || 
			!strchr(translator_email_str, '.') || 
			(strlen(translator_email_str) <= 6))
		{
			GtkWidget *dialog = gtk_message_dialog_new(
				GTK_WINDOW(gtranslator_application),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_OK,
				_("Please enter a valid EMail address!"));
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			
			return;
		}
	}

	/*
	 * Now set the translator informations for the "gtranslator_translator".
	 */
	gtranslator_translator_set_translator(gtranslator_translator,
		translator_str, translator_email_str);

	g_free(translator_str);
	g_free(translator_email_str);
#define entry(x) 	GTK_ENTRY(GTK_BIN(x)->child)
	update(gtranslator_translator->language->name, entry(authors_language));
	update(gtranslator_translator->language->locale, entry(lcode));
	update(gtranslator_translator->language->group_email, entry(lg_email));
	update(gtranslator_translator->language->encoding, entry(mime_type));
	update(gtranslator_translator->language->bits, entry(encoding));
	update(gtranslator_translator->language->plural, GTK_ENTRY(plural));
	update(GtrPreferences.autosave_suffix, autosave_suffix);
#undef entry
#undef update
#define if_active(widget) \
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
	GtrPreferences.save_geometry = if_active(save_geometry_tb);
	GtrPreferences.unmark_fuzzy = if_active(unmark_fuzzy);
	GtrPreferences.warn_if_fuzzy = if_active(warn_if_fuzzy);
	GtrPreferences.highlight = if_active(highlight);
	GtrPreferences.dot_char = if_active(use_dot_char);
	GtrPreferences.update_function = if_active(use_update_function);
	GtrPreferences.rambo_function = if_active(rambo_function);
	GtrPreferences.sweep_compile_file = if_active(sweep_compile_file);
	GtrPreferences.show_comment = if_active(show_comment);
	GtrPreferences.show_messages_table = if_active(show_messages_table);
	GtrPreferences.collapse_all = if_active(collapse_all_entries);
	GtrPreferences.check_recent_file = if_active(check_recent_files);
	GtrPreferences.instant_spell_check = if_active(instant_spell_checking);
	GtrPreferences.use_own_fonts = if_active(own_fonts);
	GtrPreferences.use_own_colors = if_active(own_colors);
	GtrPreferences.use_learn_buffer = if_active(use_learn_buffer);
	GtrPreferences.fuzzy_matching = if_active(fuzzy_matching);
	GtrPreferences.auto_learn = if_active(auto_learn);
	GtrPreferences.keep_obsolete = if_active(keep_obsolete);
	GtrPreferences.autosave = if_active(autosave);
	GtrPreferences.autosave_with_suffix = if_active(autosave_with_suffix);
#ifdef GTR_ABOUT_ME
	GtrPreferences.use_about_me = if_active(use_about_me);
#endif
#undef if_active

	GtrPreferences.autosave_timeout = 
		gtk_spin_button_get_value(GTK_SPIN_BUTTON(
			autosave_timeout));
		
	GtrPreferences.max_history_entries =
		gtk_spin_button_get_value(GTK_SPIN_BUTTON(
			max_history_entries));
	
	GtrPreferences.min_match_percentage =
		gtk_spin_button_get_value(GTK_SPIN_BUTTON(
			min_match_percentage));

	gtranslator_config_set_string("informations/autosave_suffix", 
		GtrPreferences.autosave_suffix);
	
	gtranslator_config_set_float("informations/autosave_timeout", 
		GtrPreferences.autosave_timeout);

	gtranslator_config_set_float("informations/max_history_entries", 
		GtrPreferences.max_history_entries);
	
	gtranslator_config_set_float("informations/min_match_percentage",
		GtrPreferences.min_match_percentage);
	
	g_free(GtrPreferences.msgid_font);
	g_free(GtrPreferences.msgstr_font);
	
	//Fonts
	GtrPreferences.msgid_font=g_strdup(
		gtk_font_button_get_font_name(
			GTK_FONT_BUTTON(msgid_font)));
	
	GtrPreferences.msgstr_font=g_strdup(
		gtk_font_button_get_font_name(
			GTK_FONT_BUTTON(msgstr_font)));
	
	gtranslator_config_set_string("interface/original_font",
		GtrPreferences.msgid_font);
	gtranslator_config_set_string("interface/translation_font",
		GtrPreferences.msgstr_font);
	
	//Colors
	gtranslator_color_values_set(GTK_COLOR_BUTTON(text_color), COLOR_FG);
	

	/*
	 * Assign custom fonts and colors to GtkTextView
	 * TODO: Missing plural handling
	 */
	if(current_page != NULL)
	{
		gtranslator_set_style(current_page->text_msgid, 0);
		gtranslator_set_style(current_page->trans_msgstr, 1);
	}
	
	/*
	 * Assign our attended hotkey character from the prefs dialog.
	 */
	gtranslator_config_set_int("editor/hotkey_char", GtrPreferences.hotkey_char);
	
	 
	gtranslator_config_set_bool("toggles/save_geometry", GtrPreferences.save_geometry);
	gtranslator_config_set_bool("toggles/warn_if_fuzzy", GtrPreferences.warn_if_fuzzy);
	gtranslator_config_set_bool("toggles/set_non_fuzzy_if_changed", 
			      GtrPreferences.unmark_fuzzy);
	gtranslator_config_set_bool("toggles/use_dot_char",
			      GtrPreferences.dot_char);
	gtranslator_config_set_bool("toggles/use_update_function",
			      GtrPreferences.update_function);
	gtranslator_config_set_bool("toggles/sweep_compile_file",
			      GtrPreferences.sweep_compile_file);
	gtranslator_config_set_bool("toggles/show_comment",
			      GtrPreferences.show_comment);
	gtranslator_config_set_bool("toggles/rambo_function",
			      GtrPreferences.rambo_function);
	gtranslator_config_set_bool("toggles/check_recent_files",
			      GtrPreferences.check_recent_file);
	gtranslator_config_set_bool("toggles/highlight",
			      GtrPreferences.highlight);
	gtranslator_config_set_bool("toggles/instant_spell_check",
			      GtrPreferences.instant_spell_check);
	gtranslator_config_set_bool("toggles/use_own_fonts",
			      GtrPreferences.use_own_fonts);
	gtranslator_config_set_bool("toggles/use_own_colors",
				    GtrPreferences.use_own_colors);
	gtranslator_config_set_bool("toggles/use_learn_buffer",
			      GtrPreferences.use_learn_buffer);
	gtranslator_config_set_bool("toggles/fuzzy_matching",
			      GtrPreferences.fuzzy_matching);
	gtranslator_config_set_bool("toggles/auto_learn",
			      GtrPreferences.auto_learn);
	gtranslator_config_set_bool("toggles/show_messages_table",
			      GtrPreferences.show_messages_table);
	gtranslator_config_set_bool("toggles/collapse_all",
			      GtrPreferences.collapse_all);
	gtranslator_config_set_bool("toggles/keep_obsolete",
			      GtrPreferences.keep_obsolete);
	gtranslator_config_set_bool("toggles/autosave",
			      GtrPreferences.autosave);
	gtranslator_config_set_bool("toggles/autosave_with_suffix",
			      GtrPreferences.autosave_with_suffix);
#ifdef GTR_ABOUT_ME	
	gtranslator_config_set_bool("toggles/use_about_me",
				  GtrPreferences.use_about_me);
#endif

	/*
	 * Save our translator data.
	 */
	gtranslator_translator_save(gtranslator_translator);

	/*
	 * Hide the comment viewing (area) via our new util function.
	 */
	if(!GtrPreferences.show_comment)
	{
		gtranslator_comment_hide();
	}

	gtk_widget_destroy(GTK_WIDGET(prefs));
	return;
}

static void gtranslator_preferences_dialog_changed(GtkWidget  * widget, gpointer flag)
{
	const gchar *locale;
	gint c = 0;
	G_CONST_RETURN gchar *current;

	prefs_changed = TRUE;

#define set_text(widget,field) \
	gtk_entry_set_text(GTK_ENTRY(GTK_BIN(widget)->child),\
			   languages[c].field)
	switch (GPOINTER_TO_INT(flag)) {
	case 1:
		current =
		    gtk_entry_get_text(GTK_ENTRY(GTK_BIN(authors_language)->child));
		while (languages[c].name != NULL) {
			if (!nautilus_strcasecmp(current, _(languages[c].name))) {
				set_text(lcode, locale);
				set_text(mime_type, encoding);
				set_text(encoding, bits);
				set_text(lg_email, group_email);
				locale = gtk_entry_get_text(GTK_ENTRY(GTK_BIN(lcode)->child));
				gtranslator_preferences_plural_string_request((char *)locale);
				break;
			}
			c++;
		}
		break;
	case 2:
		current =
		    gtk_entry_get_text(GTK_ENTRY(GTK_BIN(lcode)->child));
		while (languages[c].name != NULL) {
			if (!nautilus_strcmp(current, languages[c].locale)) {
				set_text(mime_type, encoding);
				set_text(encoding, bits);
				set_text(lg_email, group_email);
				locale = gtk_entry_get_text(GTK_ENTRY(GTK_BIN(lcode)->child));
				gtranslator_preferences_plural_string_request((char *)locale);
				break;
			}
			c++;
		}
		break;
	case 10:
		/*
		 * This is the next group where we're handling the hotkey char changes.
		 */
		if(GTK_TOGGLE_BUTTON(widget)->active)
		{
			GtrPreferences.hotkey_char='_';
		}
		break;
	case 11:
		if(GTK_TOGGLE_BUTTON(widget)->active)
		{
			GtrPreferences.hotkey_char='&';
		}
		break;
	default:
		break;
	}
#undef set_text
}

void toggle_sensitive(GtkWidget *widget, gpointer data)
{
	gboolean active;
	active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	gtk_widget_set_sensitive(GTK_WIDGET(data), active);
	prefs_changed = TRUE;
}

#ifdef GTR_ABOUT_ME
void toggle_insensitive(GtkWidget *widget, gpointer data)
{
	gboolean active;
	active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	gtk_widget_set_sensitive(GTK_WIDGET(data), !active);
	prefs_changed = TRUE;
}
#endif

void gtranslator_preferences_read(void)
{
	/*
	 * Initialize the preferences with default values if this is our first
	 *  startup of gtranslator.
	 */  
	/*GtrNotebookPage *page;
	page = g_list_nth_data(gtranslator_notebook_pages, 0);*/
	
	gtranslator_preferences_init_default_values();

	GtrPreferences.msgid_font = 
		gtranslator_config_get_string("interface/original_font");
	GtrPreferences.msgstr_font = 
		gtranslator_config_get_string("interface/translation_font");
	
	GtrPreferences.hotkey_char = gtranslator_config_get_int("editor/hotkey_char");
#ifdef GTR_ABOUT_ME	
	GtrPreferences.use_about_me = 
		gtranslator_config_get_bool("toggles/use_about_me");
#endif	
	/*
	 * Beware us of stupid, non-existing hotkey characters please! If so,
	 *  then assign the standard GNOME '_' hotkey mnemonicial for it.
	 */
	if(!GtrPreferences.hotkey_char || GtrPreferences.hotkey_char<=1)
	{
		GtrPreferences.hotkey_char='_';
	}

	GtrPreferences.autosave =
		gtranslator_config_get_bool("toggles/autosave");
	GtrPreferences.autosave_timeout = 
		gtranslator_config_get_float("informations/autosave_timeout");
	GtrPreferences.autosave_with_suffix =
		gtranslator_config_get_bool("toggles/autosave_with_suffix");
	GtrPreferences.autosave_suffix =
		gtranslator_config_get_string("informations/autosave_suffix");
	GtrPreferences.max_history_entries =
		gtranslator_config_get_float("informations/max_history_entries");
	GtrPreferences.min_match_percentage =
		gtranslator_config_get_float("informations/min_match_percentage");

	/*
	 * Check some prefs values for sanity and set sane values if no really
	 *  good prefs values have been detected.
	 */
	if(GtrPreferences.min_match_percentage < 25.0)
	{
		GtrPreferences.min_match_percentage=25.0;
	}

	if(GtrPreferences.autosave_timeout < 1.0)
	{
		GtrPreferences.autosave_timeout=1.0;
	}

	GtrPreferences.instant_spell_check = 
		gtranslator_config_get_bool("toggles/instant_spell_check");
	GtrPreferences.save_geometry =
		gtranslator_config_get_bool("toggles/save_geometry");
	GtrPreferences.unmark_fuzzy =
		gtranslator_config_get_bool("toggles/set_non_fuzzy_if_changed"); 
	GtrPreferences.warn_if_fuzzy =
		gtranslator_config_get_bool("toggles/warn_if_fuzzy");
	GtrPreferences.update_function =
		gtranslator_config_get_bool("toggles/use_update_function");
	GtrPreferences.sweep_compile_file =
		gtranslator_config_get_bool("toggles/sweep_compile_file");
	GtrPreferences.dot_char = 
		gtranslator_config_get_bool("toggles/use_dot_char");
	GtrPreferences.check_recent_file = 
		gtranslator_config_get_bool("toggles/check_recent_files");
	GtrPreferences.use_own_fonts =
		gtranslator_config_get_bool("toggles/use_own_fonts");
	GtrPreferences.use_own_colors =
		gtranslator_config_get_bool("toggles/use_own_colors");
	GtrPreferences.keep_obsolete =
		gtranslator_config_get_bool("toggles/keep_obsolete");
	GtrPreferences.rambo_function =
		gtranslator_config_get_bool("toggles/rambo_function");

	GtrPreferences.use_learn_buffer = gtranslator_config_get_bool(
		"toggles/use_learn_buffer");
	GtrPreferences.fuzzy_matching = gtranslator_config_get_bool(
		"toggles/fuzzy_matching");
	GtrPreferences.auto_learn = gtranslator_config_get_bool(
		"toggles/auto_learn");
	GtrPreferences.highlight = gtranslator_config_get_bool(
		"toggles/highlight");

	GtrPreferences.match_case = gtranslator_config_get_bool(
		"find/case_sensitive");
	GtrPreferences.ignore_hotkeys = gtranslator_config_get_bool(
		"find/ignore_hotkeys");
	
	GtrPreferences.fi_comments = gtranslator_config_get_bool("find/find_in_comments");
	GtrPreferences.fi_english = gtranslator_config_get_bool("find/find_in_english");
	GtrPreferences.fi_translation = gtranslator_config_get_bool("find/find_in_translation");
	
	GtrPreferences.fi_comments = gtranslator_config_get_bool("replace/replace_in_comments");
	GtrPreferences.fi_english = gtranslator_config_get_bool("replace/replace_in_english");
	GtrPreferences.fi_translation = gtranslator_config_get_bool("replace/replace_in_translation");

	gtranslator_update_regex_flags();

	GtrPreferences.fill_header = gtranslator_config_get_bool(
		"toggles/fill_header");
	GtrPreferences.show_comment = gtranslator_config_get_bool(
		"toggles/show_comment");
	GtrPreferences.show_messages_table = gtranslator_config_get_bool(
		"toggles/show_messages_table");
	GtrPreferences.collapse_all = gtranslator_config_get_bool(
		"toggles/collapse_all");
	
}

/*
 * Free the resting pieces of the configuration.
 */
void gtranslator_preferences_free()
{
	g_free(GtrPreferences.autosave_suffix);
	g_free(GtrPreferences.msgid_font);
	g_free(GtrPreferences.msgstr_font);
}
