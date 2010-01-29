#!/bin/sh

FILES="gedit-history-entry.h \
       gedit-history-entry.c"

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

