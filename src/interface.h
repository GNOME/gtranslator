/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * A header for all the nice click-and-rushed dialogs ..
 *
 **/

	GtkWidget* create_app1 (void);
	GtkWidget* create_propertybox1 (void);
	GtkWidget* create_open_file (void);
	GtkWidget* create_save_file_as (void);
	GtkWidget* create_help_for_the_propbox1 (void);

/**
 * The globally needed widgets
 **/

	GtkWidget* app1;
	GtkWidget* trans_box;
	GtkWidget* text1;
	GtkWidget* appbar1; 
	GtkWidget* search_bar;
	GtkWidget* search_button,* search_again_button,* goto_button,* goto_line_button;

/**
 * Necessary for the geometry handling :
 **/

gchar *gtranslator_geometry;
