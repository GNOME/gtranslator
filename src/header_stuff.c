/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* Completely reorganized header-functions 
*
* -- source
**/

#include "header_stuff.h"

/**
* A structure which will be hopefully used now...
**/
gtr_header *ph;

/**
* This variable is used to recognize the end of the header.
**/
gboolean header_finish;

/**
* A temp-char.
**/
gchar *inp;

/**
* A simple define; .. ok I'm lazy but it avoids many typos ..
**/
#define kabalak_str(x) inp=NULL; inp=strstr(hline, ": ");\
strcpy(ph->x,g_strdup(strtok(g_strchug(strstr(inp, " ")),"\\\"")));\
g_print("--  %s\n",ph->x);

void apply_header(gtr_header *the_header)
{
	if((the_header->prj_name==NULL))
	{
		g_warning(_("Error while parsing the header!"));
	}
}

/**
* Creates and allocates the header.
**/
gtr_header *new_header()
{
	gtr_header *th=g_new(gtr_header,1);
	/**
	* Also allocate the parts of it.
	**/
	th->prj_name=g_new(gchar,1);
	th->prj_version=g_new(gchar,1);
	th->pot_date=g_new(gchar,1);
	th->po_date=g_new(gchar,1);
	th->last_translator=g_new(gchar,1);
	th->language_team=g_new(gchar,1);
	th->mime_version=g_new(gchar,1);
	th->mime_type=g_new(gchar,1);
	th->encoding=g_new(gchar,1);
		return th;
}

void get_header(gchar *hline)
{
	(gtr_header *)ph=new_header();
	header_finish=FALSE;
	if(!g_strncasecmp(hline,"\"Pro",4))
	{
		gchar *temp=g_new(gchar,1);
		kabalak_str(prj_name);
		temp=ph->prj_name;
		ph->prj_name=index(temp, ' ');
		ph->prj_version=rindex(temp, ' ');
		if(temp)
		{
			g_free(temp);
		}
	}
	if(!g_strncasecmp(hline,"\"POT-",5))
	{
		kabalak_str(pot_date);
	}
	if(!g_strncasecmp(hline,"\"PO-",4))
	{
		kabalak_str(po_date);
	}
	if(!g_strncasecmp(hline,"\"Las",4))
	{
		kabalak_str(last_translator);
	}
	if(!g_strncasecmp(hline,"\"Lang",5))
	{
		kabalak_str(language_team);
	}
	if(!g_strncasecmp(hline,"\"MIME",5))
	{
		kabalak_str(mime_version);
	}
	if(!g_strncasecmp(hline,"\"Content-Ty",11))
	{
		kabalak_str(mime_type);
	}
	if(!g_strncasecmp(hline,"\"Content-Tr",11))
	{
		kabalak_str(encoding);
		header_finish=TRUE;
	}
	if(header_finish==TRUE)
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Header has been parsed."));
	}
}

/**
* Creates the Header-edit dialog.
**/
void edit_header_create(gtr_header *head)
{
	/**
	* Some local widgets.
	**/
	GtkWidget *prj_name_label,*prj_version_label,*e_table;
        GtkWidget *take_my_options;
	/**
	* Do a simply check.
	**/
	if(!head->prj_name)
	{
		gnome_app_error(GNOME_APP(app1),_("The header doesn't seem to be parsed rightly."));
	}
	prj_name_label=gtk_label_new(_("Project name:"));
	prj_version_label=gtk_label_new(_("Project version:"));
        take_my_options=gtk_check_button_new_with_label(_("Use my personal options to fill out the rest of the header."));
	/**
	* Create the widgets.
	**/
	gtr_edit_header_dlg=gtk_dialog_new();
	e_table=gtk_table_new(2,2,FALSE);
        /**
        * Set the window-specs.
        **/
	gtk_window_set_title(GTK_WINDOW(gtr_edit_header_dlg),_("gtranslator -- edit header"));
	gtk_window_set_wmclass(GTK_WINDOW(gtr_edit_header_dlg),"gtranslator","gtranslator");
        /**
        * Add the buttons.
        **/
	gtr_edit_header_dlg_cancel=gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	gtr_edit_header_dlg_apply=gnome_stock_button(GNOME_STOCK_BUTTON_APPLY);
        /**
        * Add the GNOME-entry-boxes.
        **/
	gtr_prj_name=gnome_entry_new("PROJECT_NAME");
	gtr_prj_version=gnome_entry_new("PROJECT_VERSION");
	/**
	* Set the parsed values as standard.
	**/
	gtk_entry_set_text(GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(gtr_prj_name))),head->prj_name);
	gtk_entry_set_text(GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(gtr_prj_version))),head->prj_version);
	/**
	* Adds the main-widgets to the dialog.
	**/
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gtr_edit_header_dlg)->vbox),e_table);
        gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gtr_edit_header_dlg)->vbox),take_my_options);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gtr_edit_header_dlg)->action_area),gtr_edit_header_dlg_apply);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gtr_edit_header_dlg)->action_area),gtr_edit_header_dlg_cancel);
	/**
	* Insert the widgets into the table.
	**/
	gtk_table_attach_defaults(GTK_TABLE(e_table),prj_name_label,0,1,0,1);
	gtk_table_attach_defaults(GTK_TABLE(e_table),gtr_prj_name,1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(e_table),prj_version_label,0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(e_table),gtr_prj_version,1,2,1,2);
	/**
	* Show the inner-widgets
	**/
	gtk_widget_show_all(e_table);
        gtk_widget_show(take_my_options);
	gtk_widget_show(gtr_edit_header_dlg_apply);
	gtk_widget_show(gtr_edit_header_dlg_cancel);
	/**
	* Connect the signals
	**/
	gtk_signal_connect(GTK_OBJECT(gtr_edit_header_dlg_cancel),"clicked",
		GTK_SIGNAL_FUNC(edit_header_hide),NULL);
}

/**
* Simply calls the edit_header_create()-function and shows the box.
**/
void edit_header_show()
{
	/**
	* Give the exisint function as an argument.
	**/
	edit_header_create((gtr_header *)ph);
	gtk_widget_show(gtr_edit_header_dlg);
}

/**
* Hides the dialog.
**/
void edit_header_hide(GtkWidget *widget,gpointer useless)
{
	gtk_widget_hide(gtr_edit_header_dlg);	
}

/**
* This function is called from the other widgets.
**/
void edit_header(GtkWidget *widget,gpointer useless)
{
	edit_header_show();
}
