:
#!/bin/sh
#
# (C) 2000-2001 Fatih Demir <kabalak@gtranslator.org>
#
# This script mails the common "oh my god, they're
#  making a release" messages to the last translators.
#

#
# Pozilla has got also releases :-)
# 
export POZILLA_RELEASE=3.0

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
# Save the old LANG evironment and unset it after that.
#
_LANG="$LANG"
_LANGUAGE="$LANGUAGE"

unset LANG
unset LANGUAGE

#
# set the default po directory to ./po
#
export PO_DIR="."

#
# Check for all necessary applications for pozilla.sh.
#
for app in msgfmt msgmerge make grep sed awk mutt
	do
		if test "z`which $app`" = "z" ; then
			echo "---------------------------------------------------------------"
			echo "!ERROR¡: The application \"$app\" is necessary for running pozilla.sh!"
			echo "---------------------------------------------------------------"
				exit 1
		fi
	done	

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
	case "$1" in
	-h|--help)
	echo "---------------------------------------------------------------"
	echo " Pozilla.sh R $POZILLA_RELEASE"
	echo "---------------------------------------------------------------"
	echo " Author: Fatih Demir <kabalak@gtranslator.org>"
	echo "---------------------------------------------------------------"
	echo "-a --additional   Defines an additional mail address to mail to"
	echo "-d --days         Days remaining for release"
	echo "-p --podirectory  Defines the po directory location (default: ./po)"
	echo "-s --sendto       Send the merged po files to the given languages"
	echo "-i --ignore       Don't operate for these languages (ignore them)"
	echo ""
	echo "Important: Both of the \"--send\" and \"--ignore\" options do "
	echo " await a ':' separated list like \"az:tr:uk\"."
	echo ""
	echo "-r --release      Specifies the coming release's number"
	echo "-S --statistics   Print out the statistics table at the end"
	echo "-m --mailinglist  Changed the mailing list to the given arguments"
	echo "-v --version      Version informations"
	echo "-h --help         This help screen"
	echo "---------------------------------------------------------------"
		exit 1
	;;
	-v|--version)
	echo "---------------------------------------------------------------"
	echo " Pozilla.sh R $POZILLA_RELEASE"
	echo "---------------------------------------------------------------"
        echo " Author:  Fatih Demir <kabalak@gtranslator.org>"
	echo "---------------------------------------------------------------"
		exit 1
	;;
	-m|--mailinglist)
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
	-a|--additional)
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
	-d|--days)
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
	-r|--release)
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
	-i|--ignore)
	shift 1
	if test "ig$1" = "ig" ; then
		echo "---------------------------------------------------------------"
		echo "No languages to ignore given."
		echo "---------------------------------------------------------------"
	else
		echo "---------------------------------------------------------------"
		export IGNORE_LANGS="`echo $1|sed -e 's/:/\ /g'`"
		shift 1
		echo "Ignoring po files for this/these lang(s): $IGNORE_LANGS"
		echo "---------------------------------------------------------------"
	fi
	;;
	-s|--sendto)
	shift 1
	if test "sendto$1" = "sendto" ; then
		echo "---------------------------------------------------------------"
		echo "No language given to send the po file to."
		echo "---------------------------------------------------------------"
	else

		echo "---------------------------------------------------------------"
		export SENDTO_LANGS="`echo $1|sed -e 's/:/\ /g'`"
		shift 1
		echo "Sending the merged po files to this/these lang(s): $SENDTO_LANGS"
		echo "---------------------------------------------------------------"
	fi
	;;
	-S|--statistics)
		echo "---------------------------------------------------------------"
		echo "Will print out the statistics table..." 
		PRINT_TABLE=yes
		echo "---------------------------------------------------------------"
	shift 1
	;;
	-p|--podirectory)
	shift 1
	if test "po$1" = "po" ; then
		echo "---------------------------------------------------------------"
		echo "No po directory location defined. Using default location ./po ..."
		echo "---------------------------------------------------------------"
		export PO_DIR="."
	else
		echo "---------------------------------------------------------------"
		export PO_DIR="$1"
		if ! test -d "$PO_DIR" ; then
			echo "\"$PO_DIR\" is not a directory; taking default location ./po..."
		else	
			echo "Using \"$PO_DIR\" as po directory location..."
		fi
		echo "---------------------------------------------------------------"
		shift 1
	fi
	;;
	*)
		echo "---------------------------------------------------------------"
		echo "!ERROR¡: Unknown option \"$1\" given.                          "
		echo "---------------------------------------------------------------"
			exit 1
	;;
	esac
done	

#
# Check if a po directory is existent in the $PO_DIR.
#
if ! test -d $PO_DIR/po ; then
	echo "---------------------------------------------------------------"
	echo "!ERROR¡: There is no \"po\" directory present in \"$PO_DIR\"!"
	echo "---------------------------------------------------------------"
		exit 1
fi

#
# Get the current task no, increment it and write
#  it at least into the config-file.
#
POZILLA_NO=`cat $CONFIG_DIR/pozilla.conf`
export POZILLA_NO=$[ $POZILLA_NO + 1 ]
echo $POZILLA_NO > $CONFIG_DIR/pozilla.conf

#
# Change to the basic directory of the po directory (. if ./po is used as po
#  directory or the real base directory for other po directory locations).
#
if test "o$PO_DIR" != "o`pwd`" ; then
	cd $PO_DIR
fi	

#
# Get the common values.
#
export PACKAGE=`grep \^AM_INIT_AUTOMAKE configure.in|sed -e 's/^.*(//g' -e 's/,.*$//g'`

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

#
# Go to the po-dir and get the list of all po-files.
#
cd $PO_DIR/po
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
	make $PACKAGE.pot 2>&1 >/dev/null
elif test -x ./update.sh ; then
	./update.sh -P 2>&1 >/dev/null
elif test -x ./update.pl ; then
	./update.pl -P 2>&1 >/dev/null
else
	echo "---------------------------------------------------------------"
	echo "!ERROR¡: No update.(sh|pl) or usable Makefile found!"
	echo "---------------------------------------------------------------"
		exit 1
fi

#
# Test if we could build the pot file.
#
[ -f $PACKAGE.pot ] || {
	echo "---------------------------------------------------------------"
	echo "!ERROR¡: Couldn't build \"$PACKAGE.pot\" file!"
	echo "---------------------------------------------------------------"
		exit 1
}

#
# Setup the table-header .-)
#
STAT_TABLE="
Language	Total messages	Translated	Percentage	Missing
========--------==============--==========------==========------=======
"

#
# Put up all the mail messages and statistics.
#
for i in $PO_FILES
	do
	AUTHOR=`grep ^\"Last $i|sed -e 's/.*:\ //g' \
		-e 's/\\\n.*$//g' -e 's/\,//g'`
	#
	# Test if the current po file should be ignored.
	#
	if test "i$IGNORE_LANGS" != "i" ; then
		_PO_LANG=`echo $i|sed -e 's/.po//g'`
		echo $IGNORE_LANGS|grep -sq $_PO_LANG && {
			continue
		}
	fi
	
	#
	# And evaluate the statistics for the po-file, but be more fail-safe
	#  then in previous release -- jump over the fallible po file and
	#   move the file back to it's original right place/name.
	#
	cp $i $i.backup
	language=`basename $i .po|sed -e s/\.Big5//g -e s/\.GB2312//g`
	
	merge_status=`msgmerge $i $PACKAGE.pot -o $i 2>&1`
	
	if echo $merge_status|grep -sq warning ; then
		mv $i.backup $i

		STAT_TABLE="$STAT_TABLE
$language\t\t------------- Failure due to a warning ------------"

		continue
	elif echo $merge_status|grep -sq error ; then
		mv $i.backup $i

		STAT_TABLE="$STAT_TABLE
$language\t\t------------- Failure due to an error -------------"

		continue
	fi
	
	#
	# Get the values for the messages statistics.
	#
	statistics=(`msgfmt -v $i 2>&1`)
	translated=${statistics[0]}
	fuzzy=${statistics[3]:-0}
	untranslated=${statistics[6]:-0}
	messages=$[ $translated + $fuzzy + $untranslated ]
	missing=$[ $fuzzy + $untranslated ]
	centil=`awk "{ print $messages / 100 }" $CONFIG_DIR/pozilla.conf`
	percent=`awk "{ printf \"%.2f\", $translated / $centil }" $CONFIG_DIR/pozilla.conf`

	STAT_TABLE="$STAT_TABLE
$language\t\t$messages\t\t$translated\t\t$percent%\t\t$missing"

	#
	# Compile the current merged po file.
	#
	msgfmt -vv $i 2>/dev/null
	
	case $? in
	1)
		echo "You should update your $i po-file for $PACKAGE," > $BODY_FILE
		echo " it's containing fuzzy or/and untranslated entries if you get it" >> $BODY_FILE
		echo "  in sync with the recent sources for $PACKAGE." >> $BODY_FILE
		echo "" >> $BODY_FILE
		if test "b$DAYS_REMAINING" != "b" ; then
			echo "$PACKAGE will release R $RELEASE in $DAYS_REMAINING days, so" >> $BODY_FILE
			echo " that you should update your translation till then." >> $BODY_FILE
		fi
		echo "" >> $BODY_FILE
		echo "Your po-file $i's statistics are:" >> $BODY_FILE
		echo "${statistics[*]} [$percent%]" >>$BODY_FILE
		echo "" >> $BODY_FILE
		if test "s$SENDTO_LANGS" != "s" ; then
			_lang=`echo $i|sed -e 's/.po//g'`
			echo $SENDTO_LANGS|grep -sq $_lang && {
				gzip --best -cf < $i > $PACKAGE.$i.gz
			echo "An updated and merged $i file is attached to this message, so that you can" >> $BODY_FILE
			echo " immediately start with your update of $i." >> $BODY_FILE
			echo "" >> $BODY_FILE
			}
		fi
		echo "Have fun with po-updating :-)" >> $BODY_FILE
	;;
	*)
		echo "Congratulations! $PACKAGE is making R $RELEASE and:" > $BODY_FILE
		echo " your $i is up-to-date :-)" >> $BODY_FILE
	;;
	esac
        [ -f $i ] && rm -f $i 
	mv $i.backup $i
	echo  "" >> $BODY_FILE
	echo "--" >> $BODY_FILE
	echo "This is a mail send by Pozilla R $POZILLA_RELEASE." >> $BODY_FILE
	echo " For questions concerning Pozilla or your translator's faith" >> $BODY_FILE
	echo "  - the po-files - send a mail to Fatih Demir <kabalak@gtranslator.org>" >> $BODY_FILE
	echo "   Thanks." >> $BODY_FILE
	if test -f $PACKAGE.$i.gz ; then
		cat $BODY_FILE|mutt -s "$SUBJECT" "$AUTHOR" -a $PACKAGE.$i.gz
	else	
		cat $BODY_FILE|mutt -s "$SUBJECT" "$AUTHOR"
	fi
	[ -f $PACKAGE.$i.gz ] && rm -f $PACKAGE.$i.gz
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
echo "Current po files statistics table:" >> $BODY_FILE
echo "$STAT_TABLE" >> $BODY_FILE
echo "" >> $BODY_FILE
echo "--" >> $BODY_FILE
echo "This is a mail send by Pozilla R $POZILLA_RELEASE." >> $BODY_FILE
echo "For questions concerning Pozilla or your translator's faith" >> $BODY_FILE
echo "- the po-files - send a mail to Fatih Demir <kabalak@gtranslator.org>" >> $BODY_FILE
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
# Clean up the rest of the mailfiles.
#
[ -f $BODY_FILE ] && rm -f $BODY_FILE

#
# Print out the statistics table if necessary.
#
if test "&$PRINT_TABLE" = "&yes" ; then
	echo "$STAT_TABLE"
fi

#
# Recover the old LANG environment flags.
#
export LANG="$_LANG"
export LANGUAGE="$_LANGUAGE"

#
# Exit with 0. We're all happy :-0
#
exit 0
