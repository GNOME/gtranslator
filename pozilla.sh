#!/bin/sh

#
# (C) 2000-2001 Fatih Demir <kabalak@gmx.net>
#
#
# This script mails the common "oh my god, they're
#  making a release" messages to the last translaters.
#

#
# Pozilla has got also releases :-)
# 
export POZILLA_RELEASE=0.5

#
# Here we do define the corresponding i18n mailing list
#  which should also get a mail message about the coming release.
#
export MAILING_LIST='GNOME I18N List <gnome-i18n@gnome.org>'

#
# The configuration dir, the mail body & mail.
#
export CONFIG_DIR="$HOME/.kabalak/pozilla"
export BODY_FILE="$CONFIG_DIR/mail.body"

#
# That's Pozilla, guy!
#
[ -d $CONFIG_DIR ] || {
	mkdir -p $CONFIG_DIR
}

#
# Read out the current "configuration", if there's any.
#
[ -f $CONFIG_DIR/pozilla.conf ] || {
	touch $CONFIG_DIR/pozilla.conf
	echo 0 > $CONFIG_DIR/pozilla.conf
}
echo -n "" > $CONFIG_DIR/pozilla.output


case $1 in
-[hH]*|--[hH]*)
	echo "----------------------------------------------"
	echo "Pozilla.sh R $POZILLA_RELEASE"
	echo "----------------------------------------------"
	echo "Author:  Fatih Demir <kabalak@gmx.net>"
	echo "----------------------------------------------"
	echo "-v --version      Version informations"
	echo "-h --help         This help screen"
	echo "----------------------------------------------"
		exit 1
;;
-[vV]*|--[vV]*)
	echo "----------------------------------------------"
	echo "Pozilla.sh R $POZILLA_RELEASE"
	echo "----------------------------------------------"
        echo "Author:  Fatih Demir <kabalak@gmx.net>"
	echo "----------------------------------------------"
		exit 1
;;
*)
	true
;;
esac

#
# Get the current task no, increment it and write
#  it at least into the config-file.
#
POZILLA_NO=`cat $CONFIG_DIR/pozilla.conf`
export POZILLA_NO=$[ $POZILLA_NO + 1 ]
echo $POZILLA_NO > $CONFIG_DIR/pozilla.conf

#
# Get the common values.
#
export PACKAGE=`grep \^AM_INIT_AUTOMAKE configure.in|\
	sed -e 's/^.*(//g' -e 's/,.*$//g'`
#
# This had to be more app-specific, I guess :-)
#
export `grep ^MAINVERSION configure.in`
export `grep ^SUBVERSION configure.in`
export RELEASE="$MAINVERSION.$SUBVERSION"

#
# If you're adepting pozilla.sh for your app and have got
#  a _plain_ version string like "0.5" or "0.32" then you
#   can comment out the lines above and uncomment the lines
#    below. Thanks.
#
#export RELEASE=`grep \^AM_INIT_AUTOMAKE configure.in|\
#	sed -e 's/^.*(//g' -e 's/.*,\ //g' -e 's/).*//g'`

#
# Go to the po-dir and get the list of all po-files.
#
cd po
export PO_FILES=`ls *.po`

#
# Set the subject
#
export SUBJECT="[ Pozilla #$POZILLA_NO ] $PACKAGE R $RELEASE"

#
# Now get for every po-file the last translator
#
./update.pl -P

#
# Possibly you want to use the new xml-i18n-tools here,
#  so you comment out the above call and uncomment the following.
# 
# ./update.sh -P
#
for i in $PO_FILES
	do
	AUTHOR=`grep ^\"Last $i|sed -e 's/.*:\ //g' \
		-e 's/\\\n.*$//g' -e 's/\,//g'`
	#
	# And evaluate the statistics for the po-file.
	#
	cp $i $i.backup
	msgmerge $i $PACKAGE.pot -o $i
	msgfmt -vv $i 2>/dev/null
	case $? in
	1)
		echo "You should update your $i po-file for $PACKAGE," > $BODY_FILE
		echo "it's containg fuzzy or/and untranslated entries if you get it" >> $BODY_FILE
		echo "in sync with the recent sources for $PACKAGE." >> $BODY_FILE
		echo "" >> $BODY_FILE
		echo "Your po-file $i's statistics are:" >> $BODY_FILE
		msgfmt -v $i 2>>$BODY_FILE
		echo "" >> $BODY_FILE
		echo "Have fun within the po-updating :-)" >> $BODY_FILE
	;;
	*)
		echo "Congratulations! $PACKAGE is making R $RELEASE and:" > $BODY_FILE
		echo "your $i is up-to-date :-)" >> $BODY_FILE
	;;
	esac
        rm -f $i ; mv $i.backup $i
	echo  "" >> $BODY_FILE
	echo "--" >> $BODY_FILE
	echo "This is a mail send by Pozilla R $POZILLA_RELEASE." >> $BODY_FILE
	echo "For questions concerning Pozilla or your translator's faith" >> $BODY_FILE
	echo "- the po-files - send a mail to Fatih Demir <kabalak@gmx.net>" >> $BODY_FILE
	echo "Thanks." >> $BODY_FILE
	cat $BODY_FILE|mutt -s "$SUBJECT" "$AUTHOR"
        done

#
# Send a mail to the mailing list.
#
echo "Dear translators of $PACKAGE:" > $BODY_FILE
echo "" >> $BODY_FILE
echo "The next release of $PACKAGE (R $RELEASE) is coming within the next days" >> $BODY_FILE
echo "and you all should update your translator for it please." >> $BODY_FILE
echo "" >> $BODY_FILE
echo "Possibly you'll also get a \"private\" message from pozilla informing" >> $BODY_FILE
echo "you about the coming release with the specs/status of your po-file." >> $BODY_FILE
echo "" >> $BODY_FILE
echo "--" >> $BODY_FILE
echo "This is a mail send by Pozilla R $POZILLA_RELEASE." >> $BODY_FILE
echo "For questions concerning Pozilla or your translator's faith" >> $BODY_FILE
echo "- the po-files - send a mail to Fatih Demir <kabalak@gmx.net>" >> $BODY_FILE
echo "Thanks." >> $BODY_FILE
cat $BODY_FILE|mutt -s "$SUBJECT" "$MAILING_LIST"

#
# Exit with 0. We're all happy :-0
#
exit 0
