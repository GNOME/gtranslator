#!/bin/sh

FILES="gedit-history-entry.h \
       gedit-history-entry.c \
       gedit-status-combo-box.h \
       gedit-status-combo-box.c"

sed_it () {
	sed \
	-e 's/gedit-history-entry/gtr-history-entry/g' \
	-e 's/gedit-window.h/gtr-window.h/g' \
	-e 's/#include "gedit-debug.h"/\/\/#include "gedit-debug.h"/g' \
	-e 's/gedit/gtr/g' \
	-e 's/Gedit/Gtr/g' \
	-e 's/GEDIT/GTR/g' \
	$1
}

for i in $FILES
do
	if [ -f $i ]; then
		sed_it $i > `echo $i | sed -e s/gedit/gtr/g`
		rm $i
	fi
done
