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
* A simple define; .. ok I'm lazy but it avoids many typos ..
**/
#define kabalak_str(x) inp=strstr(hline, ": ");\
ph.x=g_strdup(strtok(g_strchug(strstr(inp, " ")),"\\\""));

void apply_header(gtr_header the_header)
{
	if((the_header.prj_name==NULL)||(strlen(the_header.prj_version)<=0))
	{
		g_warning(_("Error while parsing the header!"));
	}
}

void get_header(gchar *hline)
{
	gboolean header_finish=FALSE;
	gchar *inp;
	if(!g_strncasecmp(hline,"\"Pro",4))
	{
		gchar *temp=g_new(gchar,1);
		kabalak_str(prj_name);
		temp=ph.prj_name;
		ph.prj_name=index(temp, ' ');
		ph.prj_version=rindex(temp, ' ');
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
void edit_header_create()
{
	/**
	* Some local widgets.
	**/
	GtkWidget *prj_name_label,*prj_version_label;
	/**
	* Do a simply check.
	**/
	if(ph.prj_name==NULL)
	{
		gnome_app_error(GNOME_APP(app1),_("The header doesn't seem to be parsed rightly."));
	}
	prj_name_label=gtk_label_new(_("Project name:"));
	prj_version_label=gtk_label_new(_("Project version:"));
	/**
	* Create the widgets.
	**/
	gtr_edit_header_dlg=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(gtr_edit_header_dlg),_("gtranslator -- edit header"));
	gtk_window_set_wmclass(GTK_WINDOW(gtr_edit_header_dlg),"gtranslator","gtranslator");
	gtr_edit_header_dlg_cancel=gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	gtr_edit_header_dlg_apply=gnome_stock_button(GNOME_STOCK_BUTTON_APPLY);
	/**
	* Adds the widgets to the dialog.
	**/
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gtr_edit_header_dlg)->vbox),prj_name_label);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gtr_edit_header_dlg)->vbox),prj_version_label);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gtr_edit_header_dlg)->action_area),gtr_edit_header_dlg_apply);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(gtr_edit_header_dlg)->action_area),gtr_edit_header_dlg_cancel);
	/**
	* Show the inner-widgets
	**/
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
	edit_header_create();
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
