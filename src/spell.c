/**
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Spell checking component
*
* -- the source 
**/

#include "spell.h"
#include "find.h"
#include "messages.h"
#include "dialogs.h"
#include "gui.h"

static GtkWidget * spellcheck_create(void)
{
	static GtkWidget *spell = NULL;
	static GtkWidget *dialog = NULL;
	if (dialog) {
		gdk_window_show(dialog->window);
		gdk_window_raise(dialog->window);
	} else {
		dialog = gnome_dialog_new(_("Check spelling"), 
					  GNOME_STOCK_BUTTON_CLOSE, NULL);
		gnome_dialog_set_close(GNOME_DIALOG(dialog), TRUE);
		spell = gnome_spell_new();
		gtk_container_add(GTK_CONTAINER(GNOME_DIALOG(dialog)->vbox),
				  spell);
		show_nice_dialog(&dialog, "gtranslator -- spell");
	}
	return spell;
}

static gboolean spellcheck_msg(GList * msg, gpointer * spell)
{
	if (GTR_MSG(msg->data)->msgstr == NULL) return FALSE;
	if (gnome_spell_check(GNOME_SPELL(spell), 
		GTR_MSG(msg->data)->msgstr) == 0) {
		g_print("Translation is OK!\n");
		return FALSE;
	} else {
		g_print("There are typos :(\n");
		return TRUE;
	}
}

void check_spelling(GtkWidget * widget, gpointer useless)
{
	GtkWidget *spell = spellcheck_create();
	update_msg();
	if (for_each_msg(po->current, (FEFunc)spellcheck_msg, spell) == FALSE)
		gnome_app_message(GNOME_APP(app1), 
				  _("Spell checking complete!"));
}

