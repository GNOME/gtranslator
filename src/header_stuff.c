/**
* Fatih Demir [ kabalak@gmx.net ]
* Gediminas Paulauskas <menesis@delfi.lt>
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Completely reorganized header-functions 
*
* -- source
**/

#include "dialogs.h"
#include "header_stuff.h"

// This is defined below
static void edit_header_clicked(GnomeDialog *dialog,gint button,gpointer useless);
static void take_my_options_toggled(GtkWidget *widget,gpointer useless);

gboolean get_header(GtrMsg * msg)
{
	gchar **lines,**pair,**proj;
	gint i=0;
	
	// Create the header structure
	if (ph!=NULL)
		g_warning("Header already created\n");
	ph=g_new(GtrHeader,1);
	lines=g_strsplit(msg->msgstr,"\n",0);
	while (lines[i]!=NULL)
	{
		pair=g_strsplit(lines[i],": ",2);

#define if_key_is(str) if (!g_strcasecmp(pair[0],str))
		if_key_is("Project-Id-Version") {
			proj=g_strsplit(pair[1]," ",2);
			ph->prj_name=g_strdup(proj[0]);
			ph->prj_version=g_strdup(proj[1]);
			g_strfreev(proj);
		}
		else if_key_is("POT-Creation-Date")
			ph->pot_date=g_strdup(pair[1]);
		else if_key_is("PO-Revision-Date")
			ph->po_date=g_strdup(pair[1]);
		else if_key_is("Last-Translator")
			ph->last_translator=g_strdup(pair[1]);
		else if_key_is("Language-Team")
			ph->language_team=g_strdup(pair[1]);
		else if_key_is("MIME-Version")
			ph->mime_version=g_strdup(pair[1]);
		else if_key_is("Content-Type")
			ph->mime_version=g_strdup(pair[1]);
		else if_key_is("Content-Transfer-Encoding")
			ph->encoding=g_strdup(pair[1]);
		else 
			g_print("New header entry found (please add it to header_stuff.c):\n%s\n",pair[1]);
		
		g_strfreev(pair);
		i++;
	}
	g_strfreev(lines);
	if (ph->prj_name)
		return TRUE;
	else return FALSE;
}

void free_header(GtrHeader *the_header)
{
	if (the_header==NULL) return;
	g_free(the_header->prj_name);
	g_free(the_header->prj_version);
	g_free(the_header->pot_date);
	g_free(the_header->po_date);
	g_free(the_header->last_translator);
	g_free(the_header->language_team);
	g_free(the_header->mime_version);
	g_free(the_header->mime_type);
	g_free(the_header->encoding);
	g_free(the_header);
}

GtkWidget *gtr_prj_name,*gtr_prj_version,*gtr_po_date,*gtr_pot_date,
	*gtr_last_translator,*gtr_language_team,
	*gtr_mime_version,*gtr_mime_type,*gtr_encoding;
GtkWidget *take_my_options;

static void take_my_options_toggled(GtkWidget *widget,gpointer useless)
{
	if_fill_header=!if_fill_header;
	gnome_config_set_bool("/gtranslator/Toggles/Fill header",if_fill_header);
	gnome_config_sync();
}

static void edit_header_clicked(GnomeDialog *dialog,gint button,gpointer useless)
{
	if (button==GNOME_OK)
	{
		g_print("TODO should apply the header now...\n");
	}
	gnome_dialog_close(dialog);
}

/**
* Creates the Header-edit dialog.
**/
void edit_header(GtkWidget *widget,gpointer useless)
{
	static GtkWidget *dialog=NULL;

        GtkWidget *prj_name_label,*prj_version_label,*e_table;

	raise_and_return_if_exists(dialog);
	/**
	* Do a simply check.
	**/
	if(!ph->prj_name)
	{
		gnome_app_error(GNOME_APP(app1),_("The header doesn't seem to be parsed rightly."));
		return ;
	}
	dialog=gnome_dialog_new(_("gtranslator -- edit header"),
		GNOME_STOCK_BUTTON_OK,
		GNOME_STOCK_BUTTON_CLOSE,
		NULL);

	prj_name_label=gtk_label_new(_("Project name:"));
	prj_version_label=gtk_label_new(_("Project version:"));
        take_my_options=gtk_check_button_new_with_label(_("Use my personal options to fill out the rest of the header."));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(take_my_options),
		if_fill_header);
	e_table=gtk_table_new(2,2,FALSE);
        /**
        * Add the GNOME-entry-boxes.
        **/
	gtr_prj_name=gnome_entry_new("PROJECT_NAME");
	gtr_prj_version=gnome_entry_new("PROJECT_VERSION");
	/**
	* Set the parsed values as standard.
	**/
	gtk_entry_set_text(
		GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(gtr_prj_name))),
		ph->prj_name);
	gtk_entry_set_text(
		GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(gtr_prj_version))),
		ph->prj_version);
	/**
	* Adds the main-widgets to the dialog.
	**/
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),
		e_table,TRUE,TRUE,0);
        gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),
		take_my_options,TRUE,TRUE,0);
	/**
	* Insert the widgets into the table.
	**/
	gtk_table_attach_defaults(GTK_TABLE(e_table),prj_name_label,0,1,0,1);
	gtk_table_attach_defaults(GTK_TABLE(e_table),gtr_prj_name,1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(e_table),prj_version_label,0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(e_table),gtr_prj_version,1,2,1,2);
	/**
	* Connect the signals
	**/
	gtk_signal_connect(GTK_OBJECT(dialog),"clicked",
        		GTK_SIGNAL_FUNC(edit_header_clicked),
			NULL);
	gtk_signal_connect(GTK_OBJECT(take_my_options), "toggled",
        		GTK_SIGNAL_FUNC(take_my_options_toggled),
			NULL);

	show_nice_dialog(&dialog,"gtranslator -- header");
}
