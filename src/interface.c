/*
*	-> Fatih Demir [ kabalak@gmx.net ]
* 	The interface file , totally catastrophed by me ...
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

#include <gnome.h>

#include "callbacks.h"
#include "interface.h"

static GnomeUIInfo file1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("_Compile"),
    NULL,
    on_compile1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CONVERT,
    GDK_C, GDK_MOD1_MASK, NULL
  },
  GNOMEUIINFO_MENU_OPEN_ITEM (on_open1_activate, NULL),
  GNOMEUIINFO_MENU_SAVE_ITEM (on_save1_activate, NULL),
  GNOMEUIINFO_MENU_SAVE_AS_ITEM (on_save_as1_activate, NULL),
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_MENU_EXIT_ITEM (on_exit1_activate, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo edit1_menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_CUT_ITEM (on_cut1_activate, NULL),
  GNOMEUIINFO_MENU_COPY_ITEM (on_copy1_activate, NULL),
  GNOMEUIINFO_MENU_PASTE_ITEM (on_paste1_activate, NULL),
  GNOMEUIINFO_MENU_CLEAR_ITEM (on_clear1_activate, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo view1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Refresh"),
    NULL,
    on_refresh1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_REFRESH,
    GDK_F5, 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo messages1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("_First"),
    NULL,
    on_first1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FIRST,
    GDK_Home, GDK_CONTROL_MASK, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("_Back"),
    NULL,
    on_back1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BACK,
    GDK_Page_Up, 0, NULL
  },
  GNOMEUIINFO_SEPARATOR,
  {
    GNOME_APP_UI_ITEM, N_("_Next"),
    NULL,
    on_next1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
    GDK_Page_Down, 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("_Last"),
    NULL,
    on_last1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_LAST,
    GDK_End, GDK_CONTROL_MASK, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo settings1_menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_PREFERENCES_ITEM (on_preferences1_activate, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo help1_menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_ABOUT_ITEM (on_about1_activate, NULL),
  {
    GNOME_APP_UI_ITEM, N_("_Online help"),
    NULL,
    on_online_help1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_OPEN,
    GDK_F1, 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] =
{
  GNOMEUIINFO_MENU_FILE_TREE (file1_menu_uiinfo),
  GNOMEUIINFO_MENU_EDIT_TREE (edit1_menu_uiinfo),
  GNOMEUIINFO_MENU_VIEW_TREE (view1_menu_uiinfo),
  {
    GNOME_APP_UI_SUBTREE, N_("_Messages"),
    NULL,
    messages1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, N_("_Messages"),
    0, 0, NULL
  },
  GNOMEUIINFO_MENU_SETTINGS_TREE (settings1_menu_uiinfo),
  GNOMEUIINFO_MENU_HELP_TREE (help1_menu_uiinfo),
  GNOMEUIINFO_END
};

GtkWidget*
create_app1 (void)
{
  GtkWidget *dock1;
  GtkWidget *toolbar1;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *compile_button;
  GtkWidget *open_button;
  GtkWidget *save_button;
  GtkWidget *save_as_button;
  GtkWidget *first_button;
  GtkWidget *back_button;
  GtkWidget *options_button;
  GtkWidget *next_button;
  GtkWidget *last_button;
  GtkWidget *exit_button;
  GtkWidget *vbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *scrolledwindow2;

  app1 = gnome_app_new ("gtranslator", _("gtranslator"));
  gtk_widget_set_name (app1, "app1");
  gtk_object_set_data (GTK_OBJECT (app1), "app1", app1);
  gtk_window_set_wmclass (GTK_WINDOW (app1), "gtranslator", "gtranslator");

  dock1 = GNOME_APP (app1)->dock;
  gtk_widget_set_name (dock1, "dock1");
  gtk_widget_ref (dock1);
  gtk_object_set_data_full (GTK_OBJECT (app1), "dock1", dock1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (dock1);

  gnome_app_create_menus (GNOME_APP (app1), menubar1_uiinfo);

  gtk_widget_set_name (menubar1_uiinfo[0].widget, "file1");
  gtk_widget_ref (menubar1_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "file1",
                            menubar1_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (file1_menu_uiinfo[0].widget, "compile1");
  gtk_widget_ref (file1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "compile1",
                            file1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (file1_menu_uiinfo[1].widget, "open1");
  gtk_widget_ref (file1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "open1",
                            file1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (file1_menu_uiinfo[2].widget, "save1");
  gtk_widget_ref (file1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "save1",
                            file1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (file1_menu_uiinfo[3].widget, "save_as1");
  gtk_widget_ref (file1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "save_as1",
                            file1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (file1_menu_uiinfo[4].widget, "separator1");
  gtk_widget_ref (file1_menu_uiinfo[4].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "separator1",
                            file1_menu_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (file1_menu_uiinfo[5].widget, "exit1");
  gtk_widget_ref (file1_menu_uiinfo[5].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "exit1",
                            file1_menu_uiinfo[5].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (menubar1_uiinfo[1].widget, "edit1");
  gtk_widget_ref (menubar1_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "edit1",
                            menubar1_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (edit1_menu_uiinfo[0].widget, "cut1");
  gtk_widget_ref (edit1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "cut1",
                            edit1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (edit1_menu_uiinfo[1].widget, "copy1");
  gtk_widget_ref (edit1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "copy1",
                            edit1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (edit1_menu_uiinfo[2].widget, "paste1");
  gtk_widget_ref (edit1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "paste1",
                            edit1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (edit1_menu_uiinfo[3].widget, "clear1");
  gtk_widget_ref (edit1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "clear1",
                            edit1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (menubar1_uiinfo[2].widget, "view1");
  gtk_widget_ref (menubar1_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "view1",
                            menubar1_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (view1_menu_uiinfo[0].widget, "refresh1");
  gtk_widget_ref (view1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "refresh1",
                            view1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (menubar1_uiinfo[3].widget, "messages1");
  gtk_widget_ref (menubar1_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "messages1",
                            menubar1_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (messages1_menu_uiinfo[0].widget, "first1");
  gtk_widget_ref (messages1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "first1",
                            messages1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (messages1_menu_uiinfo[1].widget, "back1");
  gtk_widget_ref (messages1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "back1",
                            messages1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (messages1_menu_uiinfo[2].widget, "separator1");
  gtk_widget_ref (messages1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "separator1",
                            messages1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (messages1_menu_uiinfo[3].widget, "next1");
  gtk_widget_ref (messages1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "next1",
                            messages1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (messages1_menu_uiinfo[4].widget, "last1");
  gtk_widget_ref (messages1_menu_uiinfo[4].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "last1",
                            messages1_menu_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (menubar1_uiinfo[4].widget, "settings1");
  gtk_widget_ref (menubar1_uiinfo[4].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "settings1",
                            menubar1_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (settings1_menu_uiinfo[0].widget, "preferences1");
  gtk_widget_ref (settings1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "preferences1",
                            settings1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (menubar1_uiinfo[5].widget, "help1");
  gtk_widget_ref (menubar1_uiinfo[5].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "help1",
                            menubar1_uiinfo[5].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (help1_menu_uiinfo[0].widget, "about1");
  gtk_widget_ref (help1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "about1",
                            help1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_set_name (help1_menu_uiinfo[1].widget, "online_help1");
  gtk_widget_ref (help1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (app1), "online_help1",
                            help1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  toolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
  gtk_widget_set_name (toolbar1, "toolbar1");
  gtk_widget_ref (toolbar1);
  gtk_object_set_data_full (GTK_OBJECT (app1), "toolbar1", toolbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar1);
  gnome_app_add_toolbar (GNOME_APP (app1), GTK_TOOLBAR (toolbar1), "toolbar1",
                                GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
                                GNOME_DOCK_TOP, 1, 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (toolbar1), 1);
  gtk_toolbar_set_space_size (GTK_TOOLBAR (toolbar1), 16);
  gtk_toolbar_set_space_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_SPACE_LINE);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar1), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_CONVERT);
  compile_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Compile"),
                                _("New File"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (compile_button, "compile_button");
  gtk_widget_ref (compile_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "compile_button", compile_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (compile_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_OPEN);
  open_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Open"),
                                _("Open File"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (open_button, "open_button");
  gtk_widget_ref (open_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "open_button", open_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (open_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_SAVE);
  save_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Save"),
                                _("Save File"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (save_button, "save_button");
  gtk_widget_ref (save_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "save_button", save_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (save_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_SAVE_AS);
  save_as_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Save as"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (save_as_button, "save_as_button");
  gtk_widget_ref (save_as_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "save_as_button", save_as_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (save_as_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_FIRST);
  first_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("First"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (first_button, "first_button");
  gtk_widget_ref (first_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "first_button", first_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (first_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_BACK);
  back_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Back"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (back_button, "back_button");
  gtk_widget_ref (back_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "back_button", back_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (back_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_PREFERENCES);
  options_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Options"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (options_button, "options_button");
  gtk_widget_ref (options_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "options_button", options_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (options_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_FORWARD);
  next_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Next"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (next_button, "next_button");
  gtk_widget_ref (next_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "next_button", next_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (next_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_LAST);
  last_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Last"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (last_button, "last_button");
  gtk_widget_ref (last_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "last_button", last_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (last_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app1, GNOME_STOCK_PIXMAP_EXIT);
  exit_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Exit"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_set_name (exit_button, "exit_button");
  gtk_widget_ref (exit_button);
  gtk_object_set_data_full (GTK_OBJECT (app1), "exit_button", exit_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit_button);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox1, "vbox1");
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (app1), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gnome_app_set_contents (GNOME_APP (app1), vbox1);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_name (scrolledwindow1, "scrolledwindow1");
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (app1), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  text1 = gtk_text_new (NULL, NULL);
  gtk_widget_set_name (text1, "text1");
  gtk_widget_ref (text1);
  gtk_object_set_data_full (GTK_OBJECT (app1), "text1", text1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), text1);
  gtk_widget_realize (text1);
  /*gtk_text_insert(GTK_TEXT(text1), NULL, NULL, NULL,
                   _("The msgids will appear here ."), 29);*/

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_name (scrolledwindow2, "scrolledwindow2");
  gtk_widget_ref (scrolledwindow2);
  gtk_object_set_data_full (GTK_OBJECT (app1), "scrolledwindow2", scrolledwindow2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow2, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  trans_box = gtk_text_new (NULL, NULL);
  gtk_widget_set_name (trans_box, "trans_box");
  gtk_widget_ref (trans_box);
  gtk_object_set_data_full (GTK_OBJECT (app1), "trans_box", trans_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  /*gtk_text_insert(GTK_TEXT(trans_box), NULL, NULL, NULL, _("Msgstrs go here ."),20);*/

  gtk_widget_show (trans_box);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), trans_box);
  gtk_text_set_editable (GTK_TEXT (trans_box), TRUE);
  gtk_widget_realize (trans_box);

  appbar1 = gnome_appbar_new (TRUE, TRUE, GNOME_PREFERENCES_NEVER);
  gtk_widget_set_name (appbar1, "appbar1");
  gtk_widget_ref (appbar1);
  gtk_object_set_data_full (GTK_OBJECT (app1), "appbar1", appbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (appbar1);
  gnome_app_set_statusbar (GNOME_APP (app1), appbar1);

  gtk_signal_connect (GTK_OBJECT (compile_button), "pressed",
                      GTK_SIGNAL_FUNC (on_compile_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (open_button), "pressed",
                      GTK_SIGNAL_FUNC (on_open_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (save_button), "pressed",
                      GTK_SIGNAL_FUNC (on_save_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (save_as_button), "pressed",
                      GTK_SIGNAL_FUNC (on_save_as_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (first_button), "pressed",
                      GTK_SIGNAL_FUNC (on_first_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (back_button), "pressed",
                      GTK_SIGNAL_FUNC (on_back_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (options_button), "pressed",
                      GTK_SIGNAL_FUNC (on_options_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (next_button), "pressed",
                      GTK_SIGNAL_FUNC (on_next_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (last_button), "pressed",
                      GTK_SIGNAL_FUNC (on_last_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit_button), "pressed",
                      GTK_SIGNAL_FUNC (on_exit_button_pressed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (trans_box), "changed",
                      GTK_SIGNAL_FUNC (on_trans_box_changed),
                      GTK_TEXT(trans_box));
  gtk_signal_connect (GTK_OBJECT (trans_box), "copy_clipboard",
                      GTK_SIGNAL_FUNC (on_trans_box_copy_clipboard),
                      GTK_TEXT(trans_box));
  gtk_signal_connect (GTK_OBJECT (trans_box), "cut_clipboard",
                      GTK_SIGNAL_FUNC (on_trans_box_cut_clipboard),
                      GTK_TEXT(trans_box));
  gtk_signal_connect (GTK_OBJECT (trans_box), "paste_clipboard",
                      GTK_SIGNAL_FUNC (on_trans_box_paste_clipboard),
                      GTK_TEXT(trans_box));
  gtk_signal_connect (GTK_OBJECT (trans_box), "activate",
                      GTK_SIGNAL_FUNC (on_trans_box_activate),
                      GTK_TEXT(trans_box));

  return app1;
}

GtkWidget*
create_about1 (void)
{
  const gchar *authors[] = {
    "Fatih Demir",
    NULL
  };
  GtkWidget *about1;

  about1 = gnome_about_new ("gtranslator", VERSION,
                        _("(C) 1999-2000 Fatih Demir ( kabalak@gmx.net )"),
                        authors,
                        _("gtranslator is a GnomeApplication for translating .po-files more comfortable than in (X)Emacs .\nTill now you can't see much , but that will change !"),
                        NULL);
  gtk_widget_set_name (about1, "about1");
  gtk_object_set_data (GTK_OBJECT (about1), "about1", about1);
  gtk_window_set_modal (GTK_WINDOW (about1), TRUE);
  gtk_window_set_wmclass (GTK_WINDOW (about1), "gtranslator", "gtranslator");

  return about1;
}

GtkWidget*
create_propertybox1 (void)
{
  GtkWidget *propertybox1;
  GtkWidget *notebook1;
  GtkWidget *vbox3;
  GtkWidget *warn_for_errors;
  GtkWidget *saving_unchanged;
  GtkWidget *start_at_last_file;
  GtkWidget *label1;
  GtkWidget *vbox2;
  GSList *verbose_group = NULL;
  GtkWidget *verbose_yes;
  GtkWidget *verbose_no;
  GtkWidget *label2;

  propertybox1 = gnome_property_box_new ();
  gtk_widget_set_name (propertybox1, "propertybox1");
  gtk_object_set_data (GTK_OBJECT (propertybox1), "propertybox1", propertybox1);

  notebook1 = GNOME_PROPERTY_BOX (propertybox1)->notebook;
  gtk_widget_set_name (notebook1, "notebook1");
  gtk_object_set_data (GTK_OBJECT (propertybox1), "notebook1", notebook1);
  gtk_widget_show (notebook1);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox3, "vbox3");
  gtk_widget_ref (vbox3);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "vbox3", vbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox3);

  warn_for_errors = gtk_check_button_new_with_label (_("Warn me if there are errors in the .po-file"));
  gtk_widget_set_name (warn_for_errors, "warn_for_errors");
  gtk_widget_ref (warn_for_errors);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "warn_for_errors", warn_for_errors,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (warn_for_errors);
  gtk_box_pack_start (GTK_BOX (vbox3), warn_for_errors, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (warn_for_errors), TRUE);

  saving_unchanged = gtk_check_button_new_with_label (_("Ask for saving unchanged .po-files "));
  gtk_widget_set_name (saving_unchanged, "saving_unchanged");
  gtk_widget_ref (saving_unchanged);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "saving_unchanged", saving_unchanged,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (saving_unchanged);
  gtk_box_pack_start (GTK_BOX (vbox3), saving_unchanged, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (saving_unchanged), TRUE);

  start_at_last_file = gtk_check_button_new_with_label (_("gtranslator should start at the last edited file"));
  gtk_widget_set_name (start_at_last_file, "start_at_last_file");
  gtk_widget_ref (start_at_last_file);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "start_at_last_file", start_at_last_file,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (start_at_last_file);
  gtk_box_pack_start (GTK_BOX (vbox3), start_at_last_file, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (start_at_last_file), TRUE);

  label1 = gtk_label_new (_("Common settings "));
  gtk_widget_set_name (label1, "label1");
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox2, "vbox2");
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox2);

  verbose_yes = gtk_radio_button_new_with_label (verbose_group, _("Show me the output of msgfmt --statistics"));
  verbose_group = gtk_radio_button_group (GTK_RADIO_BUTTON (verbose_yes));
  gtk_widget_set_name (verbose_yes, "verbose_yes");
  gtk_widget_ref (verbose_yes);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "verbose_yes", verbose_yes,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (verbose_yes);
  gtk_box_pack_start (GTK_BOX (vbox2), verbose_yes, FALSE, FALSE, 0);

  verbose_no = gtk_radio_button_new_with_label (verbose_group, _("Don't  show me any messages of msgfmt "));
  verbose_group = gtk_radio_button_group (GTK_RADIO_BUTTON (verbose_no));
  gtk_widget_set_name (verbose_no, "verbose_no");
  gtk_widget_ref (verbose_no);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "verbose_no", verbose_no,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (verbose_no);
  gtk_box_pack_start (GTK_BOX (vbox2), verbose_no, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (verbose_no), TRUE);

  label2 = gtk_label_new (_("Compiling  points ..."));
  gtk_widget_set_name (label2, "label2");
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

  gtk_signal_connect (GTK_OBJECT (propertybox1), "apply",
                      GTK_SIGNAL_FUNC (on_propertybox1_apply),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (propertybox1), "help",
                      GTK_SIGNAL_FUNC (on_propertybox1_help),
                      propertybox1);
  gtk_signal_connect (GTK_OBJECT (propertybox1), "close",
                      GTK_SIGNAL_FUNC (on_propertybox1_close),
                      propertybox1);
  gtk_signal_connect (GTK_OBJECT (warn_for_errors), "toggled",
                      GTK_SIGNAL_FUNC (on_warn_for_errors_toggled),
                      propertybox1);
  gtk_signal_connect (GTK_OBJECT (saving_unchanged), "toggled",
                      GTK_SIGNAL_FUNC (on_saving_unchanged_toggled),
                      propertybox1);
  gtk_signal_connect (GTK_OBJECT (start_at_last_file), "toggled",
                      GTK_SIGNAL_FUNC (on_start_at_last_file_toggled),
                      propertybox1);
  gtk_signal_connect (GTK_OBJECT (verbose_yes), "toggled",
                      GTK_SIGNAL_FUNC (on_verbose_yes_toggled),
                      propertybox1);
  gtk_signal_connect (GTK_OBJECT (verbose_no), "toggled",
                      GTK_SIGNAL_FUNC (on_verbose_no_toggled),
                      propertybox1);

  return propertybox1;
}

GtkWidget*
create_open_file (void)
{
  GtkWidget *open_file;
  GtkWidget *ok_button_of_open_file;
  GtkWidget *cancel_button_of_open_file;

  open_file = gtk_file_selection_new (_("Select your .po-file to open"));
  gtk_widget_set_name (open_file, "open_file");
  gtk_object_set_data (GTK_OBJECT (open_file), "open_file", open_file);
  gtk_container_set_border_width (GTK_CONTAINER (open_file), 10);
  gtk_window_set_wmclass (GTK_WINDOW (open_file), "gtranslator", "gtranslator");
  gtk_file_selection_hide_fileop_buttons (GTK_FILE_SELECTION (open_file));

  ok_button_of_open_file = GTK_FILE_SELECTION (open_file)->ok_button;
  gtk_widget_set_name (ok_button_of_open_file, "ok_button_of_open_file");
  gtk_object_set_data (GTK_OBJECT (open_file), "ok_button_of_open_file", ok_button_of_open_file);
  gtk_widget_show (ok_button_of_open_file);
  GTK_WIDGET_SET_FLAGS (ok_button_of_open_file, GTK_CAN_DEFAULT);

  cancel_button_of_open_file = GTK_FILE_SELECTION (open_file)->cancel_button;
  gtk_widget_set_name (cancel_button_of_open_file, "cancel_button_of_open_file");
  gtk_object_set_data (GTK_OBJECT (open_file), "cancel_button_of_open_file", cancel_button_of_open_file);
  gtk_widget_show (cancel_button_of_open_file);
  GTK_WIDGET_SET_FLAGS (cancel_button_of_open_file, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (ok_button_of_open_file), "pressed",
                      GTK_SIGNAL_FUNC (on_ok_button_of_open_file_pressed),
                      open_file);
  gtk_signal_connect (GTK_OBJECT (cancel_button_of_open_file), "pressed",
                      GTK_SIGNAL_FUNC (on_cancel_button_of_open_file_pressed),
                      open_file);

  return open_file;
}

GtkWidget*
create_save_file_as (void)
{
  GtkWidget *save_file_as;
  GtkWidget *ok_button_of_save_as_file;
  GtkWidget *cancel_button_of_save_as_file;

  save_file_as = gtk_file_selection_new (_("Select your file to save to"));
  gtk_widget_set_name (save_file_as, "save_file_as");
  gtk_object_set_data (GTK_OBJECT (save_file_as), "save_file_as", save_file_as);
  gtk_container_set_border_width (GTK_CONTAINER (save_file_as), 10);
  gtk_window_set_wmclass (GTK_WINDOW (save_file_as), "gtranslator", "gtranslator");

  ok_button_of_save_as_file = GTK_FILE_SELECTION (save_file_as)->ok_button;
  gtk_widget_set_name (ok_button_of_save_as_file, "ok_button_of_save_as_file");
  gtk_object_set_data (GTK_OBJECT (save_file_as), "ok_button_of_save_as_file", ok_button_of_save_as_file);
  gtk_widget_show (ok_button_of_save_as_file);
  GTK_WIDGET_SET_FLAGS (ok_button_of_save_as_file, GTK_CAN_DEFAULT);

  cancel_button_of_save_as_file = GTK_FILE_SELECTION (save_file_as)->cancel_button;
  gtk_widget_set_name (cancel_button_of_save_as_file, "cancel_button_of_save_as_file");
  gtk_object_set_data (GTK_OBJECT (save_file_as), "cancel_button_of_save_as_file", cancel_button_of_save_as_file);
  gtk_widget_show (cancel_button_of_save_as_file);
  GTK_WIDGET_SET_FLAGS (cancel_button_of_save_as_file, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (ok_button_of_save_as_file), "pressed",
                      GTK_SIGNAL_FUNC (on_ok_button_of_save_as_file_pressed),
                      save_file_as);
  gtk_signal_connect (GTK_OBJECT (cancel_button_of_save_as_file), "pressed",
                      GTK_SIGNAL_FUNC (on_cancel_button_of_save_as_file_pressed),
                      save_file_as);

  return save_file_as;
}

GtkWidget*
create_errors_in_the_po (void)
{
  GtkWidget *errors_in_the_po;
  GtkWidget *dialog_vbox1;
  GtkWidget *button11;
  GtkWidget *dialog_action_area1;

  errors_in_the_po = gnome_message_box_new (_("An error occured in your .po-file as it \ncouldn't be compiled by mfgfmt !"),
                              GNOME_MESSAGE_BOX_ERROR, NULL);
  gtk_widget_set_name (errors_in_the_po, "errors_in_the_po");
  gtk_object_set_data (GTK_OBJECT (errors_in_the_po), "errors_in_the_po", errors_in_the_po);
  GTK_WINDOW (errors_in_the_po)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_policy (GTK_WINDOW (errors_in_the_po), FALSE, FALSE, FALSE);
  gtk_window_set_wmclass (GTK_WINDOW (errors_in_the_po), "gtranslator", "gtranslator");
  gnome_dialog_set_close (GNOME_DIALOG (errors_in_the_po), TRUE);
  gnome_dialog_close_hides (GNOME_DIALOG (errors_in_the_po), TRUE);

  dialog_vbox1 = GNOME_DIALOG (errors_in_the_po)->vbox;
  gtk_widget_set_name (dialog_vbox1, "dialog_vbox1");
  gtk_object_set_data (GTK_OBJECT (errors_in_the_po), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  gnome_dialog_append_button (GNOME_DIALOG (errors_in_the_po), GNOME_STOCK_BUTTON_OK);
  button11 = g_list_last (GNOME_DIALOG (errors_in_the_po)->buttons)->data;
  gtk_widget_set_name (button11, "button11");
  gtk_widget_ref (button11);
  gtk_object_set_data_full (GTK_OBJECT (errors_in_the_po), "button11", button11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button11);
  GTK_WIDGET_SET_FLAGS (button11, GTK_CAN_DEFAULT);

  dialog_action_area1 = GNOME_DIALOG (errors_in_the_po)->action_area;
  if (dialog_action_area1 != NULL)
    gtk_widget_set_name (dialog_action_area1, "dialog_action_area1");
  gtk_widget_ref (dialog_action_area1);
  gtk_object_set_data_full (GTK_OBJECT (errors_in_the_po), "dialog_action_area1", dialog_action_area1,
                            (GtkDestroyNotify) gtk_widget_unref);

  return errors_in_the_po;
}

GtkWidget*
create_save_unchanged_po (void)
{
  GtkWidget *save_unchanged_po;
  GtkWidget *dialog_vbox2;
  GtkWidget *button12;
  GtkWidget *dialog_action_area2;

  save_unchanged_po = gnome_message_box_new (_("You try to save an unchanged .po-file !\nHow useless ."),
                              GNOME_MESSAGE_BOX_WARNING, NULL);
  gtk_widget_set_name (save_unchanged_po, "save_unchanged_po");
  gtk_object_set_data (GTK_OBJECT (save_unchanged_po), "save_unchanged_po", save_unchanged_po);
  GTK_WINDOW (save_unchanged_po)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_policy (GTK_WINDOW (save_unchanged_po), FALSE, FALSE, FALSE);
  gtk_window_set_wmclass (GTK_WINDOW (save_unchanged_po), "gtranslator", "gtranslator");
  gnome_dialog_set_close (GNOME_DIALOG (save_unchanged_po), TRUE);
  gnome_dialog_close_hides (GNOME_DIALOG (save_unchanged_po), TRUE);

  dialog_vbox2 = GNOME_DIALOG (save_unchanged_po)->vbox;
  gtk_widget_set_name (dialog_vbox2, "dialog_vbox2");
  gtk_object_set_data (GTK_OBJECT (save_unchanged_po), "dialog_vbox2", dialog_vbox2);
  gtk_widget_show (dialog_vbox2);

  gnome_dialog_append_button (GNOME_DIALOG (save_unchanged_po), GNOME_STOCK_BUTTON_OK);
  button12 = g_list_last (GNOME_DIALOG (save_unchanged_po)->buttons)->data;
  gtk_widget_set_name (button12, "button12");
  gtk_widget_ref (button12);
  gtk_object_set_data_full (GTK_OBJECT (save_unchanged_po), "button12", button12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button12);
  GTK_WIDGET_SET_FLAGS (button12, GTK_CAN_DEFAULT);

  dialog_action_area2 = GNOME_DIALOG (save_unchanged_po)->action_area;
  if (dialog_action_area2 != NULL)
    gtk_widget_set_name (dialog_action_area2, "dialog_action_area2");
  gtk_widget_ref (dialog_action_area2);
  gtk_object_set_data_full (GTK_OBJECT (save_unchanged_po), "dialog_action_area2", dialog_action_area2,
                            (GtkDestroyNotify) gtk_widget_unref);

  return save_unchanged_po;
}

GtkWidget*
create_compiling_po (void)
{
  GtkWidget *compiling_po;
  GtkWidget *dialog_vbox3;
  GtkWidget *button13;
  GtkWidget *dialog_action_area3;

  compiling_po = gnome_message_box_new (_("Your .po-file is being compiled by \nmsgfmt now ...."),
                              GNOME_MESSAGE_BOX_INFO, NULL);
  gtk_widget_set_name (compiling_po, "compiling_po");
  gtk_object_set_data (GTK_OBJECT (compiling_po), "compiling_po", compiling_po);
  GTK_WINDOW (compiling_po)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_policy (GTK_WINDOW (compiling_po), FALSE, FALSE, FALSE);
  gtk_window_set_wmclass (GTK_WINDOW (compiling_po), "gtranslator", "gtranslator");
  gnome_dialog_set_close (GNOME_DIALOG (compiling_po), TRUE);
  gnome_dialog_close_hides (GNOME_DIALOG (compiling_po), TRUE);

  dialog_vbox3 = GNOME_DIALOG (compiling_po)->vbox;
  gtk_widget_set_name (dialog_vbox3, "dialog_vbox3");
  gtk_object_set_data (GTK_OBJECT (compiling_po), "dialog_vbox3", dialog_vbox3);
  gtk_widget_show (dialog_vbox3);

  gnome_dialog_append_button (GNOME_DIALOG (compiling_po), GNOME_STOCK_BUTTON_OK);
  button13 = g_list_last (GNOME_DIALOG (compiling_po)->buttons)->data;
  gtk_widget_set_name (button13, "button13");
  gtk_widget_ref (button13);
  gtk_object_set_data_full (GTK_OBJECT (compiling_po), "button13", button13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button13);
  GTK_WIDGET_SET_FLAGS (button13, GTK_CAN_DEFAULT);

  dialog_action_area3 = GNOME_DIALOG (compiling_po)->action_area;
  if (dialog_action_area3 != NULL)
    gtk_widget_set_name (dialog_action_area3, "dialog_action_area3");
  gtk_widget_ref (dialog_action_area3);
  gtk_object_set_data_full (GTK_OBJECT (compiling_po), "dialog_action_area3", dialog_action_area3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gnome_execute_shell(NULL,"msgfmt tr.po");
  return compiling_po;
}

GtkWidget*
create_wait_for_help (void)
{
  GtkWidget *wait_for_help;
  GtkWidget *dialog_vbox4;
  GtkWidget *button14;
  GtkWidget *dialog_action_area4;

  wait_for_help = gnome_message_box_new (_("Please wait for the help system .\n( I'd to learn going along with gtkxmhtml ..)"),
                              GNOME_MESSAGE_BOX_GENERIC, NULL);
  gtk_widget_set_name (wait_for_help, "wait_for_help");
  gtk_object_set_data (GTK_OBJECT (wait_for_help), "wait_for_help", wait_for_help);
  GTK_WINDOW (wait_for_help)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_policy (GTK_WINDOW (wait_for_help), FALSE, FALSE, FALSE);
  gtk_window_set_wmclass (GTK_WINDOW (wait_for_help), "gtranslator", "gtranslator");
  gnome_dialog_set_close (GNOME_DIALOG (wait_for_help), TRUE);
  gnome_dialog_close_hides (GNOME_DIALOG (wait_for_help), TRUE);

  dialog_vbox4 = GNOME_DIALOG (wait_for_help)->vbox;
  gtk_widget_set_name (dialog_vbox4, "dialog_vbox4");
  gtk_object_set_data (GTK_OBJECT (wait_for_help), "dialog_vbox4", dialog_vbox4);
  gtk_widget_show (dialog_vbox4);

  gnome_dialog_append_button (GNOME_DIALOG (wait_for_help), GNOME_STOCK_BUTTON_OK);
  button14 = g_list_last (GNOME_DIALOG (wait_for_help)->buttons)->data;
  gtk_widget_set_name (button14, "button14");
  gtk_widget_ref (button14);
  gtk_object_set_data_full (GTK_OBJECT (wait_for_help), "button14", button14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button14);
  GTK_WIDGET_SET_FLAGS (button14, GTK_CAN_DEFAULT);

  dialog_action_area4 = GNOME_DIALOG (wait_for_help)->action_area;
  if (dialog_action_area4 != NULL)
    gtk_widget_set_name (dialog_action_area4, "dialog_action_area4");
  gtk_widget_ref (dialog_action_area4);
  gtk_object_set_data_full (GTK_OBJECT (wait_for_help), "dialog_action_area4", dialog_action_area4,
                            (GtkDestroyNotify) gtk_widget_unref);

  return wait_for_help;
}

GtkWidget*
create_help_for_the_propbox1 (void)
{
  GtkWidget *help_for_the_propbox1;
  GtkWidget *dialog_vbox5;
  GtkWidget *button15;
  GtkWidget *dialog_action_area5;

  help_for_the_propbox1 = gnome_message_box_new (_("Here you can select various options of\ngtranslator which will make gtranslator\nmore work like you want it to ."),
                              GNOME_MESSAGE_BOX_INFO, NULL);
  gtk_widget_set_name (help_for_the_propbox1, "help_for_the_propbox1");
  gtk_object_set_data (GTK_OBJECT (help_for_the_propbox1), "help_for_the_propbox1", help_for_the_propbox1);
  GTK_WINDOW (help_for_the_propbox1)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_policy (GTK_WINDOW (help_for_the_propbox1), FALSE, FALSE, FALSE);
  gtk_window_set_wmclass (GTK_WINDOW (help_for_the_propbox1), "gtranslator", "gtranslator");
  gnome_dialog_set_close (GNOME_DIALOG (help_for_the_propbox1), TRUE);
  gnome_dialog_close_hides (GNOME_DIALOG (help_for_the_propbox1), TRUE);

  dialog_vbox5 = GNOME_DIALOG (help_for_the_propbox1)->vbox;
  gtk_widget_set_name (dialog_vbox5, "dialog_vbox5");
  gtk_object_set_data (GTK_OBJECT (help_for_the_propbox1), "dialog_vbox5", dialog_vbox5);
  gtk_widget_show (dialog_vbox5);

  gnome_dialog_append_button (GNOME_DIALOG (help_for_the_propbox1), GNOME_STOCK_BUTTON_OK);
  button15 = g_list_last (GNOME_DIALOG (help_for_the_propbox1)->buttons)->data;
  gtk_widget_set_name (button15, "button15");
  gtk_widget_ref (button15);
  gtk_object_set_data_full (GTK_OBJECT (help_for_the_propbox1), "button15", button15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button15);
  GTK_WIDGET_SET_FLAGS (button15, GTK_CAN_DEFAULT);

  dialog_action_area5 = GNOME_DIALOG (help_for_the_propbox1)->action_area;
  if (dialog_action_area5 != NULL)
    gtk_widget_set_name (dialog_action_area5, "dialog_action_area5");
  gtk_widget_ref (dialog_action_area5);
  gtk_object_set_data_full (GTK_OBJECT (help_for_the_propbox1), "dialog_action_area5", dialog_action_area5,
                            (GtkDestroyNotify) gtk_widget_unref);

  return help_for_the_propbox1;
}

