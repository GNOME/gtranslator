/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * A woosh-boosh file ...	
 **/
	
#include <gnome.h>

/**
 * A stupid place for a character variable ...
 **/
const char *filename;

/**
 * The long list of cb-functions ...
 **/
void 
on_search_button_pressed		(GtkButton 	*button,
					gpointer user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_propertybox1_help			(GtkMenuItem	*menuitem,
					gpointer	user_data);

void
on_online_help1_activate		(GtkMenuItem	*menuitem,
					gpointer	user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cut1_activate                       (GtkMenuItem     *menuitem,
					gpointer user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
					gpointer user_data);

void
on_paste1_activate                     (GtkMenuItem     *menuitem,
					gpointer user_data);

void
on_clear1_activate                     (GtkMenuItem     *menuitem,
					gpointer user_data);

void
on_properties1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_open_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_as_button_pressed              (GtkButton       *button,
                                        gpointer         user_data);

void
on_first_button_pressed                (GtkButton       *button,
                                        gpointer         user_data);

void
on_back_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_next_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_last_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_exit_button_pressed                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_compile_button_pressed              (GtkButton       *button,
                                        gpointer         user_data);

void
on_refresh1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_first1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_back1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_next1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_last1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_trans_box_changed                   (GtkWidget	*w,
                                        gpointer       user_data);

void
on_trans_box_copy_clipboard            (GtkWidget	*w,
					gpointer user_data);

void
on_trans_box_cut_clipboard             (GtkWidget	*w,
					gpointer user_data);

void
on_trans_box_paste_clipboard           (GtkWidget	*w,
					gpointer user_data);

void
on_trans_box_activate                  (GtkWidget	*w,
					gpointer user_data);

void
on_propertybox1_apply                  (GnomePropertyBox *gnomepropertybox,
                                        gint             arg1,
                                        gpointer 	user_data);

void
on_save_last_file_toggled              (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas);

void
on_warn_for_errors_toggled             (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas);

void
on_saving_unchanged_toggled            (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas);

void
on_start_at_last_file_toggled          (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas);

void
on_verbose_yes_toggled                 (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas);

void
on_verbose_no_toggled                  (GtkToggleButton *togglebutton,
                                        GnomePropertyBox *esas);

gint
on_propertybox1_close                  (GnomeDialog     *gnomedialog,
                                        gpointer         user_data);

void
on_ok_button_of_open_file_pressed      (GtkButton       *button,
                                         GtkFileSelection *fsel);

void
on_cancel_button_of_open_file_pressed  (GtkButton       *button,
                                        GtkFileSelection *fsel);

void
on_ok_button_of_save_as_file_pressed   (GtkButton       *button,
                                         GtkFileSelection *fsel);

void
on_cancel_button_of_save_as_file_pressed
                                        (GtkButton       *button,
                                        GtkFileSelection *fsel);

void
on_options_button_pressed              (GtkButton       *button,
                                        gpointer         user_data);

void 
on_compile1_activate                   (GtkMenuItem     *menuitem,
					gpointer 	user_data);
