/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * It's too anormally written / don't look for many sense in it ...
 *
 * Some routines need to be added here in the next days ...
 **/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "callbacks.h"
#include "interface.h"
#include "parse.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>

/**
 * The variables for all that ....
 **/
gchar cmd[256];
gchar fname[768];
static GnomeHelpMenuEntry help_me = { "gtranslator", "index.html" };
int warn_me,show_me_errors,startup_at_last;

/**
 * FIXME FIXME FIXME :
 * why do I have to do this ( or I get NULL-casts to GtkLabel )
 **/
#define gnome_appbar_set_status(x,y) /*gnome_appbar_set_status(x,y)*/

void 
on_search_button_pressed		(GtkButton 	*button,
					gpointer user_data)
{
	
}

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget* open_my_file;
	open_my_file=create_open_file();
	gtk_widget_show(open_my_file);
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),"");
}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(file_changed==TRUE)
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Saving file ..."));
		
	}
}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget* save_as_;
	save_as_=create_save_file_as();
	gtk_widget_show(save_as_);
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),"");
}


void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_exit(0);
}


void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer	user_data)
{
	
}


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
}


void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
}


void
on_clear1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer        user_data)
{

}


void
on_properties1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget* opt;
	opt=create_propertybox1();
	gtk_widget_show(opt);
}


void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget* opt_it;
	opt_it=create_propertybox1();
	gtk_widget_show(opt_it);
}

void
on_propertybox1_help			(GtkMenuItem	*menuitem,
					gpointer	user_data)
{
	GtkWidget* help_propbox;
	help_propbox=create_help_for_the_propbox1();
	gtk_widget_show(help_propbox);
}

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget* about_me;
	about_me=create_about1();
	gtk_widget_show(about_me);
}

void
on_online_help1_activate		(GtkMenuItem	*menuitem,
					gpointer	user_data)
{
	
	gnome_help_pbox_goto(NULL,NULL,&help_me);	
}

void
on_open_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget* open_it;
	open_it=create_open_file();
	gtk_widget_show(open_it);
}


void
on_save_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data)
{
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Saving file ..."));
}


void
on_save_as_button_pressed              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget* save_it;
	save_it=create_save_file_as();
	gtk_widget_show(save_it);
}


void
on_first_button_pressed                (GtkButton       *button,
                                        gpointer         user_data)
{
	if(file_opened==TRUE)
	{
		/* ... */
	}
	else
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Open a .po-file first !"));
	}
}


void
on_back_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data)
{
	if(at_the_first==TRUE)
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("You're already at the first translatable string !"));
	}
	else
	{
		if(file_opened==TRUE)
		{
			/* ... */
		}
		else
		{
			gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Open a .po-file first !"));
		}
	}
}


void
on_next_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data)
{
	at_the_first=FALSE;
	if(file_opened==TRUE)
	{
			/* .. */
	}
	else
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Open a .po-file first !"));
	}
}


void
on_last_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data)
{
	at_the_first=FALSE;
	if(file_opened==TRUE)
	{
		/* ... */
	}
}


void
on_exit_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_exit(0);
}


void
on_compile_button_pressed              (GtkButton       *button,
                                        gpointer         user_data)
{
	if(file_opened==TRUE)
	{
		GtkWidget* comp_;
		comp_=create_compiling_po();
		gtk_widget_show(comp_);
	}
	else
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("No .po-file is opened at the moment !"));	
	}
}


void
on_refresh1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_first1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(file_opened==TRUE)
	{
		/* ... */
	}
	else
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Open a .po-file first !"));
	}
}


void
on_back1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(at_the_first==TRUE)
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("You are at the first translatable string !"));
	}
	else
	{
		if(file_opened==TRUE)
		{
				/* .. */
		}
		else
		{
			gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Open a .po-file first !"));
		}
	}
}


void
on_next1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	at_the_first=FALSE;
	if(file_opened==TRUE)
	{
		/* .. */
	}
	else
	{
		gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Open a .po-file first !"));
	}
}


void
on_last1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(file_opened==TRUE)
        {
		/* .. */
        }
}


void
on_trans_box_changed                   (GtkWidget	*w,
                                        gpointer	user_data)
{
	file_changed=TRUE;	
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("File has been changed !"));
}


void
on_trans_box_copy_clipboard            (GtkWidget	*w,
                                        gpointer 	user_data)
{
}


void
on_trans_box_cut_clipboard             (GtkWidget	*w,
                                        gpointer	user_data)
{
}


void
on_trans_box_paste_clipboard           (GtkWidget	*w,
                                        gpointer	user_data)
{
}


void
on_trans_box_activate                  (GtkWidget	*w,
                                        gpointer	user_data)
{
	gnome_appbar_set_status(GNOME_APPBAR(appbar1),_("Happy translation !"));
}


void
on_propertybox1_apply                  (GnomePropertyBox *gnomepropertybox,
                                        gint             arg1,
                                        gpointer         user_data)
{
	gnome_config_push_prefix ("/gtranslator/");
	gnome_config_sync();
	gnome_config_pop_prefix ();
}


void
on_save_last_file_toggled              (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas)
{
	int savelast;
	gnome_property_box_changed(esas);
	gnome_config_push_prefix ("/gtranslator/");
	savelast=gnome_config_get_int("savelast");
	if(savelast==0)
	{
		gnome_config_set_int("savelast",1);	
	}
	else
	{
		gnome_config_set_int("savelast",0);
	}
	gnome_config_sync();
	gnome_config_pop_prefix();
}


void
on_warn_for_errors_toggled             (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas)
{
	gboolean werrors;
	gnome_property_box_changed(esas);
	gnome_config_push_prefix ("/gtranslator/");
	werrors=gnome_config_get_bool("warn_for_errors");
	if(werrors==FALSE)
	{
		gnome_config_set_bool("warn_for_errors",TRUE);
	}
	else
	{
		gnome_config_set_bool("warn_for_errors",FALSE);
	}
	gnome_config_sync();
	gnome_config_pop_prefix();
}


void
on_saving_unchanged_toggled            (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas)
{
	gboolean sunch;
        gnome_property_box_changed(esas);
	gnome_config_push_prefix ("/gtranslator/");
        sunch=gnome_config_get_bool("save_unchanged");
        if(sunch==FALSE)
        {
                gnome_config_set_bool("save_unchanged",TRUE);
        }
        else
        {
                gnome_config_set_bool("save_unchanged",FALSE);
        }
        gnome_config_sync();
        gnome_config_pop_prefix();
}


void
on_start_at_last_file_toggled          (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas)
{
	gboolean slast;
        gnome_property_box_changed(esas);
        gnome_config_push_prefix ("/gtranslator/");
        slast=gnome_config_get_bool("start_at_the_last_file");
        if(slast==FALSE)
        {
                gnome_config_set_bool("start_at_the_last_file",TRUE);
        }
        else
        {
                gnome_config_set_bool("start_at_the_last_file",FALSE);
        }
        gnome_config_sync();
        gnome_config_pop_prefix();
}


void
on_verbose_yes_toggled                 (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas)
{
        gnome_property_box_changed(esas);
	gnome_config_push_prefix("/gtranslator/");
	gnome_config_set_bool("verbose",TRUE);
	gnome_config_sync();
	gnome_config_pop_prefix();
}


void
on_verbose_no_toggled                  (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas)
{
        gnome_property_box_changed(esas);
	gnome_config_push_prefix("/gtranslator/");
	gnome_config_set_bool("verbose",FALSE);
	gnome_config_sync();
	gnome_config_pop_prefix();
}


gint
on_propertybox1_close                  (GnomeDialog     *gnomedialog,
                                        gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(gnomedialog));
}


void
on_ok_button_of_open_file_pressed      (GtkButton       *button,
                                        GtkFileSelection *fsel)	
{
	gtk_widget_hide(GTK_WIDGET(fsel));
	parse();
	file_opened=TRUE;
}


void
on_cancel_button_of_open_file_pressed  (GtkButton       *button,
                          		GtkFileSelection *fsel)
{
	gtk_widget_destroy(GTK_WIDGET(fsel));
}


void
on_ok_button_of_save_as_file_pressed   (GtkButton       *button,
                                        GtkFileSelection *fsel)
{
	filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fsel));
	gtk_widget_destroy(GTK_WIDGET(fsel));
}


void
on_cancel_button_of_save_as_file_pressed
                                        (GtkButton       *button,
                                         GtkFileSelection *fsel)
{
	gtk_widget_destroy(GTK_WIDGET(fsel));
}


void
on_options_button_pressed              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget* show_me_the_opts;
	show_me_the_opts=create_propertybox1();
	gtk_widget_show(show_me_the_opts);
}

void
on_compile1_activate			(GtkMenuItem	*menuitem,
					gpointer user_data)
{
	GtkWidget* comp_it;
	comp_it=create_compiling_po();
	gtk_widget_show(comp_it);
}
