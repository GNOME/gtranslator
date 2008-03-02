#!/bin/sh

#I don't know why but you have to change #include <gtk/gtkdialog.h>
#in message-area.h to #<gtk/gtk.h>

SVN_URI=http://svn.gnome.org/svn/gedit-plugins/trunk/plugins/charmap
FILES="gedit-charmap-panel.h \
       gedit-charmap-panel.c \
       gedit-charmap-plugin.h \
       gedit-charmap-plugin.c \
       charmap.gedit-plugin.desktop.in" 

echo "Obtaining latest version of the sources"
for FILE in $FILES
do
  svn export $SVN_URI/$FILE
done

sed_it () {
	sed \
	-e 's/gedit-panel.h/panel.h/g' \
	-e 's/gedit-window.h/window.h/g' \
	-e 's/#include "gedit-debug.h"/\/\/#include "gedit-debug.h"/g' \
	-e 's/gedit/gtranslator/g' \
	-e 's/Gedit/Gtranslator/g' \
	-e 's/GEDIT/GTR/g' \
	$1
}

sed_it gedit-charmap-panel.h > charmap-panel.h
sed_it gedit-charmap-panel.c > charmap-panel.c
sed_it gedit-charmap-plugin.h > charmap-plugin.h
sed_it gedit-charmap-plugin.c > charmap-plugin.c
sed_it charmap.gedit-plugin.desktop.in > charmap.gtranslator-plugin.desktop.in

rm gedit-charmap-panel.h
rm gedit-charmap-panel.c
rm gedit-charmap-plugin.h
rm gedit-charmap-plugin.c
rm charmap.gedit-plugin.desktop.in

