#!/bin/sh

SVN_URI=http://svn.gnome.org/svn/gedit/trunk/gedit
FILES="gedit-module.h \
       gedit-module.c \
       gedit-plugin-info-priv.h \
       gedit-plugin-info.c \
       gedit-plugin-info.h \
       gedit-plugin.h \
       gedit-plugin.c \
       gedit-plugin-manager.c \
       gedit-plugin-manager.h \
       gedit-plugins-engine.c \
       gedit-plugins-engine.h"

echo "Obtaining latest version of the sources"
for FILE in $FILES
do
  svn export $SVN_URI/$FILE
done

sed_it () {
	sed \
	-e 's/gedit-module/module/g' \
	-e 's/gedit-plugin-info/plugin-info/g' \
	-e 's/gedit-plugin/plugin/g' \
	-e 's/gedit-panel.h/panel.h/g' \
	-e 's/gedit-window.h/window.h/g' \
	-e 's/gedit-utils.h/utils_gui.h/g' \
	-e 's/#include <gedit\/gedit-debug.h>/\/\/#include <gedit\/gedit-debug.h>/g' \
	-e 's/#include "gedit-debug.h"/\/\/#include "gedit-debug.h"/g' \
	-e 's/#include "gedit-app.h"/#include "application.h"/g' \
	-e 's/gedit_debug_message (DEBUG_PLUGINS,/g_warning(/g' \
	-e 's/gedit_debug/\/\/gedit_debug/g' \
	-e 's/gedit_app/gtranslator_application/g' \
	-e 's/.gnome2\/gedit\/plugins\//.gtranslator\/plugins\//g' \
	-e 's/\/apps\/gedit-2\/plugins/\/apps\/gtranslator\/plugins/g' \
	-e 's/gedit/gtranslator/g' \
	-e 's/Gedit/Gtranslator/g' \
	-e 's/GEDIT/GTR/g' \
	$1
}

sed_it gedit-module.h > module.h
sed_it gedit-module.c > module.c
sed_it gedit-plugin-info-priv.h > plugin-info-priv.h
sed_it gedit-plugin-info.h > plugin-info.h
sed_it gedit-plugin-info.c > plugin-info.c
sed_it gedit-plugin.c > plugin.c
sed_it gedit-plugin.h > plugin.h
sed_it gedit-plugin-manager.h > plugin-manager.h
sed_it gedit-plugin-manager.c > plugin-manager.c
sed_it gedit-plugins-engine.c > plugins-engine.c
sed_it gedit-plugins-engine.h > plugins-engine.h

rm gedit-module.h
rm gedit-module.c
rm gedit-plugin-info-priv.h
rm gedit-plugin-info.h
rm gedit-plugin-info.c
rm gedit-plugin.c
rm gedit-plugin.h
rm gedit-plugin-manager.c
rm gedit-plugin-manager.h
rm gedit-plugins-engine.c
rm gedit-plugins-engine.h

