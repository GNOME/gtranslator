#!/bin/sh
#
# (C) 2000 Fatih Demir / kabalak / kabalak@gmx.net
#
#
# A slightly changed update.sh script for the update
#  function of gtranslator.
#
[ $# -lt 2 ] && {
	echo "Not enough parameters given!"
		exit 1
}
#
BASEFILE=`echo $1|sed -e 's/.*\///g'`
BASEDIR=`echo $1|sed -e s/\\\/$BASEFILE//`
PACKAGE=$2
#
cd $BASEDIR && \
xgettext --default-domain=$PACKAGE --directory=.. \
  --add-comments --keyword=_ --keyword=N_ \
  --files-from=./POTFILES.in \
&& test ! -f $PACKAGE.po \
   || ( rm -f ./PACKAGE.pot \
&& mv $PACKAGE.po ./$PACKAGE.pot );
#
mv $BASEFILE $BASEFILE.old && \
	msgmerge $BASEFILE.old $PACKAGE.pot -o $BASEFILE 2>&1 1>/dev/null
diff -q $BASEFILE $BASEFILE.old|grep -sq differ && {
	rm -f $BASEFILE.old
	exit 0
}
mv $BASEFILE.old $BASEFILE
exit 1
