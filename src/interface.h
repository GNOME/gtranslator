/**
 * Fatih Demir [ kabalak@gmx.net ]
 *
 * (C) 2000 Published under GNU GPL V 2.0+
 *
 * A header for all the nice click-and-rushed dialogs ..
 *
 **/


	GtkWidget* create_app1 (void);
	GtkWidget* create_about1 (void);
	GtkWidget* create_propertybox1 (void);
	GtkWidget* create_open_file (void);
	GtkWidget* create_save_file_as (void);
	GtkWidget* create_errors_in_the_po (void);
	GtkWidget* create_save_unchanged_po (void);
	GtkWidget* create_compiling_po (void);
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

/**
 * A simple define since I don't know
 *  if gtk_widget_set_name(x) ,gtk_widget_ref(X) , ....
 *  which glade always have put to the interface files 
 *   are needed .....
 **/

#define ka_widget_show(x) gtk_widget_set_name(x, "x"); \
gtk_widget_ref(x); \
gtk_object_set_data_full(GTK_OBJECT(app1), "x", x, \
			(GtkDestroyNotify) gtk_widget_unref);\
gtk_widget_show(x)

/**
 * AND : If this #define isn't needed , we 
 *  could simply uncomment the following line :
 **/

/*#define ka_widget_show(x) gtk_widget_show(x)*/
