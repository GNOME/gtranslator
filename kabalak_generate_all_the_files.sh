#!/bin/sh
#######################################################################
# The kabalak'sh answer to ./autogen.sh!
# (C) 2000-2005 Fatih Demir <kabalak@kabalak.net>
#######################################################################
package=gtranslator
#######################################################################
echo "I don't check for the stuff I'll use ;-) "
a="echo -n "
echo "Starting to process the files ..."
echo ""
#
# Clean up these files from previous
#  configurations.
#
[ -f config.log ] && rm -f config.log
[ -f config.cache ] && rm -f config.cache
[ -f config.h ] && rm -f config.h
#
# Set up the $ACLOCAL_FLAGS
#
export ACLOCAL_FLAGS=" -I . $ACLOCAL_FLAGS "
#
# Test if the ./macros dir is present
test -d macros
case $? in
	0)
	$a "aclocal -> " && aclocal $ACLOCAL_FLAGS -I macros
	;;
	*)
	$a "aclocal -> " && aclocal $ACLOCAL_FLAGS
	;;
esac
grep -sq LIBTOOL configure.in && {
$a "libtoolize -> " && echo "no"|libtoolize --copy --force 2>&1 1>/dev/null
}
grep -sq GETTEXT configure.in && {
$a "gettextize -> " && echo "no"|gettextize --copy --force 2>&1 1>/dev/null
$a "intltoolize -> " && intltoolize --copy --force 2>&1 1>/dev/null
}
$a "autoheader -> " && autoheader
echo ""
$a "automake -> " && automake -a
$a "autoconf -> :-) " && autoconf
echo -e "\n\nYou can now do 2 things :\n"
echo "1) Hope that you did $0 ANY_CONFIGURE_FLAGS "
echo "--------------------------------- OR -------------------------------------"
echo "2) Press Ctrl-C and call './configure --help' and then call "
echo "    './configure' with the desired flags ."
echo ""
t=5
echo "Waiting for your 'open-mind' decision : "
while [ $t -gt 1 ]
	do
	$a "$t "
	sleep 1
	t=$[ $t -1 ]
	done
$a " last chance (1) " && sleep 1 && echo ""
./configure --enable-debug=yes --enable-deprecated \
	"$@" && echo -e "\n\tHappy $package-ing\n" && exit 0
#######################################################################
