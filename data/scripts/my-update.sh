#!/bin/sh
#
# (C) 2000-2001 Fatih Demir <kabalak@gtranslator.org>
#
#
# A slightly changed update.sh script for the update
#  function of gtranslator.
# 
# Now named my-update.sh to avoid name clashes.
#
[ $# -lt 3 ] && {
		exit 1
}

#
# Help!
#
case $1 in
--[hH][eE][lL][pP]|-[hH])
	echo "---------------------------------------------------------------"
	echo "\"$0\""
	echo " is a gtranslator specific implementation of a po file updating"
	echo "  script which uses xml-i18n-tools/intltools or simple msgmerge"
	echo "   according to what is detected on the current system."
	echo ""
	echo "For outside-of-gtranslator-usage: It requires 3 arguments to be"
	echo " given to this script:"
	echo ""
	echo "  - 1. The po file name by itself (e.g. \"tr.po\")"
	echo "  - 2. The directory where the po files reside (e.g. \"po\" dir)"
	echo "  - 3. The package name of the po file to be updated (e.g. \"mc\")"
	echo ""
	echo "---------------------------------------------------------------"
		exit 1
;;
esac

#
# Here we do take over the arguments' list to our own variables.
#
BASEFILE="$1"
BASEDIR="$2"
PACKAGE="$3"

#
# Test the given arguments for sanity.
#
[ -z $BASEFILE ] && {
	echo "---------------------------------------------------------------"
	echo "No \"base file name\" like \"tr.po\" given! Exitting..."
	echo "---------------------------------------------------------------"
		exit 1
}

[ -z $BASEDIR ] &&  {
	echo "---------------------------------------------------------------"
	echo "No \"base directory\" like \"po\" given! Exitting..."
	echo "---------------------------------------------------------------"
		exit 1 
}

[ -z $PACKAGE ] && {
	echo "---------------------------------------------------------------"
	echo "No package name like \"mc\" given! Exitting..."
	echo "---------------------------------------------------------------"
		exit 1
}

#
# Change to the given package directory.
#
cd $BASEDIR && \
#
# Check is there's a POTFILES.in present.
#
if test -f "POTFILES.in" -a -n "POTFILES.in" ; then
	#
	# Now build the updated po-file.
	#

	if test "z`which intltool-update`" != "z" ; then
		intltool-update --pot 2>&1 >/dev/null
	elif test "z`which xml-i18n-update`" != "z" ; then
		xml-i18n-update --pot 2>&1 >/dev/null
	else
		xgettext --default-domain=$PACKAGE --directory=.. \
		  --add-comments --keyword=_ --keyword=N_ \
		  --files-from=./POTFILES.in \
		&& test ! -f $PACKAGE.po \
		   || ( rm -f ./$PACKAGE.pot \
		&& mv $PACKAGE.po ./$PACKAGE.pot );
	fi 

	#
	# Now check the updated file for changes.
	#
	mv $BASEFILE $BASEFILE.old && \

	#
	# Merge the file with the new pot file.
	#
	msgmerge $BASEFILE.old $PACKAGE.pot -o $BASEFILE 2>&1 1>/dev/null

	#
	# Is there any difference ?
	#
	diff -q -I '^"POT.*' $BASEFILE $BASEFILE.old|grep -sq differ && {
		rm -f $BASEFILE
		mv $BASEFILE.old $BASEFILE

		exit 0
	}

	#
	# No change? Return 200.
	#
	rm -f $BASEFILE
	mv $BASEFILE.old $BASEFILE

	exit 200
else
	#
	# There's no POTFILES.in to work with. Return 201.
	#
	exit 201
fi
