#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "beginners-druid-functions.h"
#include "beginners-druid-interface.h"


void
gtranslator_druid_finish               (GnomeDruidPage  *gnomedruidpage,
                                        gpointer         arg1,
                                        gpointer         user_data)
{
	gtranslator_config_init();
	gtranslator_config_set_bool("informations/finish_config", TRUE);
	gtranslator_config_close();
	gtk_main_quit();
}

