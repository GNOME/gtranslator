#!/bin/sh

#I don't know why but you have to change #include <gtk/gtkdialog.h>
#in message-area.h to #<gtk/gtk.h>

SVN_URI=http://svn.gnome.org/svn/gedit/trunk/gedit
FILES="gedit-history-entry.h \
       gedit-history-entry.c"

echo "Obtaining latest version of the sources"
for FILE in $FILES
do
  svn export $SVN_URI/$FILE
done

sed_it () {
	sed \
	-e 's/gedit-history-entry/history-entry/g' \
	-e 's/gedit-window.h/window.h/g' \
	-e 's/#include "gedit-debug.h"/\/\/#include "gedit-debug.h"/g' \
	-e 's/gedit/gtranslator/g' \
	-e 's/Gedit/Gtranslator/g' \
	-e 's/GEDIT/GTR/g' \
	$1
}

sed_it gedit-history-entry.h > history-entry.h
sed_it gedit-history-entry.c > history-entry.c

rm gedit-history-entry.c
rm gedit-history-entry.h

