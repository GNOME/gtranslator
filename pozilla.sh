#!/bin/sh

#
# (C) 2000-2001 Fatih Demir <kabalak@gmx.net>
#
#
# This script mails the common "oh my god, they're
#  making a release" messages to the last translators.
#

#
# Pozilla has got also releases :-)
# 
export POZILLA_RELEASE=1.0

#
# Here we do define the corresponding i18n mailing list
#  which should also get a mail message about the coming release.
#
export MAILING_LIST='GNOME I18N List <gnome-i18n@gnome.org>'

#
# The configuration dir, the mail body & mail.
#
export CONFIG_DIR="$HOME/.pozilla"
export BODY_FILE="$CONFIG_DIR/mail.body"

#
# Check for all necessary applications for pozilla.sh.
#
for app in msgfmt msgmerge make grep sed mutt
	do
		if test "z`which $app`" = "z" ; then
			echo "---------------------------------------------------------------"
			echo "[ERROR: The application \"$app\" is necessary for running pozilla.sh!]"
			echo "---------------------------------------------------------------"
				exit 1
		fi
	done	

#
# Check if a po directory is existent.
#
if ! test -d ./po ; then
	echo "---------------------------------------------------------------"
	echo "[ERROR: There is no \"po\" directory present in this directory! ]"
	echo "---------------------------------------------------------------"
		exit 1
fi

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


while [ ! -z "$1" ]
do
	option=`echo "$1"|sed -e 's/-//g' -e 's/--//g'`
	case "$option" in
	[hH]*)
	echo "---------------------------------------------------------------"
	echo " Pozilla.sh R $POZILLA_RELEASE"
	echo "---------------------------------------------------------------"
	echo " Author: Fatih Demir <kabalak@gmx.net>"
	echo "---------------------------------------------------------------"
	echo "-a --additional   Defines an additional mail address to mail to"
	echo "-d --days         Days remaining for release"
	echo "-r --release      Specifies the coming release's number"
	echo "-m --mailinglist  Changed the mailing list to the given arguments"
	echo "-v --version      Version informations"
	echo "-h --help         This help screen"
	echo "---------------------------------------------------------------"
		exit 1
	;;
	[vV]*)
	echo "---------------------------------------------------------------"
	echo " Pozilla.sh R $POZILLA_RELEASE"
	echo "---------------------------------------------------------------"
        echo " Author:  Fatih Demir <kabalak@gmx.net>"
	echo "---------------------------------------------------------------"
		exit 1
	;;
	[mM]*)
	shift 1
	if test "hehe$1" = "hehe" ; then
		echo "---------------------------------------------------------------"
		echo "No mailing list given, using default:"
		echo "$MAILING_LIST"
		echo "---------------------------------------------------------------"
	else
		echo "---------------------------------------------------------------"
		echo "Using special mailing list:"
		export MAILING_LIST="$1"
		echo "$MAILING_LIST"
		echo "---------------------------------------------------------------"
		shift 1
	fi
	;;
	[aA]*)
	shift 1
	if test "hehe$1" = "hehe" ; then
		echo "---------------------------------------------------------------"
		echo "No additional mail address given!"
		echo "---------------------------------------------------------------"
	else
		echo "---------------------------------------------------------------"
		export ADDITIONAL_MAILING_ADDRESS="$1"
		echo "Using $ADDITIONAL_MAILING_ADDRESS for additional mailing."
		echo "---------------------------------------------------------------"
		shift 1
	fi	
	;;
	[dD]*)
	shift 1
	if test "days$1" = "days" ; then
		echo "---------------------------------------------------------------"
		echo "No number of days given!"
		echo "---------------------------------------------------------------"
	else
		if test $1 -le 3 ; then
			echo "---------------------------------------------------------------"
			echo "You're giving your translators only $1 day(s); that's too less!"
			echo "Please give them more time to update their translations."
			echo "---------------------------------------------------------------"
				exit 1
		else
			export DAYS_REMAINING="$1"
			echo "---------------------------------------------------------------"
			echo "Days remaining: $DAYS_REMAINING"
			echo "---------------------------------------------------------------"
			shift 1
		fi	
	fi
	;;
	[rR]*)
	shift 1
	if test "r$1" = "r" ; then
		echo "---------------------------------------------------------------"
		echo "No release number defined! Trying to figuring it out.."
		echo "---------------------------------------------------------------"
	else
		echo "---------------------------------------------------------------"
		export MY_RELEASE="$1"
		echo "Using user-defined release string $MY_RELEASE"
		echo "---------------------------------------------------------------"
		shift 1
	fi	
	;;
	*)
		true
	;;
	esac
done	

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
export PACKAGE=`grep \^AM_INIT_AUTOMAKE configure.in|sed -e 's/^.*(//g' -e 's/,.*$//g'`

#
# This had to be more app-specific, I guess :-)
#
case "$PACKAGE"	in
gtranslator)
	if test "y$MY_RELEASE" = "y" ; then
		export `grep ^MAINVERSION configure.in`
		export `grep ^SUBVERSION configure.in`
		export RELEASE="$MAINVERSION.$SUBVERSION"
	else
		export RELEASE="$MY_RELEASE"
	fi	
;;
*)

	#
	# Hopefully the other apps are using plain version strings
	#  like "0.8" or "0.32".
	#
	if test "y$MY_RELEASE" = "y" ; then
		export RELEASE=`grep \^AM_INIT_AUTOMAKE configure.in|\
			sed -e 's/^.*(//g' -e 's/.*,//g' -e 's/).*//g' -e 's/\ //g'`
	else
		export RELEASE="$MY_RELEASE"
	fi
;;
esac

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
# Here we build up a recent pot file for the project.
#
if test -f Makefile -a -f POTFILES ; then
	[ -f $PACKAGE.pot ] && rm -f $PACKAGE.pot
	make $PACKAGE.pot
elif test -x ./update.sh ; then
	./update.sh -P
elif test -x ./update.pl ; then
	./update.pl -P
else
	echo "---------------------------------------------------------------"
	echo "[ERROR: No update.(sh|pl) or usable Makefile found!]"
	echo "---------------------------------------------------------------"
		exit 1
fi
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
		echo "it's containing fuzzy or/and untranslated entries if you get it" >> $BODY_FILE
		echo "in sync with the recent sources for $PACKAGE." >> $BODY_FILE
		echo "" >> $BODY_FILE
		if test "b$DAYS_REMAINING" != "b" ; then
			echo "$PACKAGE will release R $RELEASE in $DAYS_REMAINING days, so" >> $BODY_FILE
			echo "that you should update your translation till then." >> $BODY_FILE
		fi
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
if test "Z$DAYS_REMAINING" = "Z" ; then
	echo "The next release of $PACKAGE (R $RELEASE) is coming within the next days" >> $BODY_FILE
else
	echo "The next release of $PACKAGE (R $RELEASE) is coming in $DAYS_REMAINING days" >> $BODY_FILE
fi	
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
#
# Send the mail to the address(es).
#
if test "my$ADDITIONAL_MAILING_ADDRESS" = "my" ; then
	cat $BODY_FILE|mutt -s "$SUBJECT" "$MAILING_LIST"
else
	cat $BODY_FILE|mutt -s "$SUBJECT" "$MAILING_LIST" -c "$ADDITIONAL_MAILING_ADDRESS"
fi

#
# Exit with 0. We're all happy :-0
#
exit 0
