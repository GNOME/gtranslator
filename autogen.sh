#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="gtranslator"

(test -f $srcdir/configure.ac \
  && test -d $srcdir/src \
  && test -f $srcdir/src/main.c) || {
    echo -n "**Error**: Directory "\`$srcdir\'" is NOT the top source"
    echo " tree level of the gtranslator directories! Oooppsia!"
    exit 1
}

which gnome-autogen.sh || {
    echo "You need to install gnome-common module and make"
    echo "sure the gnome-autogen.sh script is in your \$PATH."
    exit 1
}

autopoint --force || exit $?

USE_GNOME2_MACROS=1 . gnome-autogen.sh
