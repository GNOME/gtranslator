#!/bin/sh
#
# (C) 2000 Fatih Demir / kabalak / kabalak@gmx.net
#
#
# A slightly changed update.sh script for the update
#  function of gtranslator.
# 
# Now named my-update.sh to avoid name clashes.
#
[ $# -lt 2 ] && {
		exit 1
}
#
# Some sed games..
#
BASEFILE=`echo $1|sed -e 's/.*\///g'`
BASEDIR=`echo $1|sed -e s/\\\/$BASEFILE//`
PACKAGE=$2
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
	my_tool="`which xml-i18n-update`"
	if test "z$my_tool" != "z" ; then
		xml-i18n-update --pot
	else
		xgettext --default-domain=$PACKAGE --directory=.. \
		  --add-comments --keyword=_ --keyword=N_ \
		  --files-from=./POTFILES.in \
		&& test ! -f $PACKAGE.po \
		   || ( rm -f ./PACKAGE.pot \
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
		rm -f $BASEFILE.old
		exit 0
	}
	#
	# No change? Return 200.
	#
	rm -f $BASEFILE.old
	exit 200
else
	#
	# There's no POTFILES.in to work with. Return 201.
	#
	exit 201
fi
