#!/bin/sh
#
# (C) 2000-2003 Fatih Demir <kabalak@kabalak.net>
#
# This script mails the common "oh my god, they're
#  making a release" messages to the last translators.
#
# For full usage explanations and some usage examples, please
#  review the man page for pozilla.sh via "man pozilla.sh".
#

#
# Some utility functions:
#

print_separator_line ()  {
	echo "<--------------------==============�==============---------------------->"
}

dry_run_information_message ()  {
	print_separator_line
	echo "We're running in dry mode, any mailing option will be ignored..."
	print_separator_line
}

no_personal_information_message () {
	print_separator_line
	echo "No personal EMails are being sent to the translators, therefore"
	echo " neither po files will be sent nor will languages be ignored."
	print_separator_line
}

#
# Pozilla has got also releases :-)
# 
export POZILLA_RELEASE=6.1

#
# Here we do define the corresponding i18n mailing list which should also get
#  an EMail message about the coming release -- we do now respect the (heh,
#   logic hits me again) "MAILING_LIST" env variable.
#
export MAILING_LIST=${MAILING_LIST:-'GNOME I18N List <gnome-i18n@gnome.org>'}

#
# The configuration dir, the mail body & mail.
#
export CONFIG_DIR="$HOME/.gtranslator/pozilla.sh"
export BODY_FILE="$CONFIG_DIR/mail.body"

#
# Save the old locale evironment and unset it after that.
#
_LANG="$LANG"
_LANGUAGE="$LANGUAGE"
_LC_ALL="$LC_ALL"

export LANG=C
export LANGUAGE=C
export LC_ALL=C

#
# Set the default po directory to ./po
#
export PO_DIR="."

#
# Start the pozilla.sh internal timer with the seconds argument by now.
#
POZILLA_SH_TIMER_START=`date +%s`

#
# Check for all necessary applications for pozilla.sh.
#
for app in msgfmt msgmerge make grep sed awk
	do
		if test "z`which $app`" = "z" ; then
			print_separator_line
			echo "!ERROR�: The application \"$app\" is necessary for running pozilla.sh!"
			print_separator_line
				exit 1
		fi
	done	

#
# That's Pozilla, guy!
#
[ -d $CONFIG_DIR ] || {
	print_separator_line
	echo "!WARNING�"
	echo ""
	echo "This utility is extremely useful but you'd know what you can do"
	echo " with pozilla, so please be aware that pozilla sends emails to"
	echo "  the net automatically for it's meant purpose."
	echo ""
	echo "You'd first read the manual page for pozilla.sh to get an overview"
	echo " of pozilla.sh -- call \"man pozilla.sh\" for this purpose."
	echo ""
	print_separator_line
	
	read -p "Are you sure to use pozilla now? [y/N] " confirmation

	#
	# Make an IP test to avoid unknown use of it.
	#
	if test "%$confirmation" != "%y" ; then 
		print_separator_line
		echo "!ERROR� "
		echo ""
		echo "Confirmation not succeeded; you need to confirm the use"
		echo " of pozilla.sh to gain it's revolutionary use and/or easiness."
		echo ""
		echo "Please look into the manual page for pozilla.sh before playing"
		echo " 'round with it as it could cause heart attacks for some of"
		echo "   your package's translators ,-)"
		print_separator_line
			exit 1
	fi
	
	mkdir -p $CONFIG_DIR
}

#
# Initialize our own config file -- or touch & echo into it at least if there's
#  no currently present config file there.
#
[ -f $CONFIG_DIR/pozilla.conf ] || {

	#
	# Test if we've got any previous generations of pozilla.sh running --
	#  move the config file and remove the old directory in these cases.
	#
	if test -f "$HOME/.pozilla/pozilla.conf" ; then
		mv "$HOME/.pozilla/pozilla.conf" $CONFIG_DIR/pozilla.conf
		rm -rf "$HOME/.pozilla/"
	else
		touch $CONFIG_DIR/pozilla.conf
		echo 0 > $CONFIG_DIR/pozilla.conf
	fi
}

while [ ! -z "$1" ]
do
	case "$1" in
	-h|--help)
	print_separator_line
	echo " Pozilla.sh R $POZILLA_RELEASE"
	print_separator_line
	echo " Author: Fatih Demir <kabalak@kabalak.net>"
	print_separator_line
	echo "-a --additional   Defines an additional mail address to mail to"
	echo "-A --send-to-all  Send the merged po files for all languages"
	echo "-d --days         Days remaining for release"
	echo "-m --mailinglist  Changed the mailing list to the given arguments"
	echo "-p --podirectory  Defines the po directory location (default: ./po)"
	echo "-r --release      Specifies the coming release's number"
	echo "-i --ignore       Don't operate for these languages (ignore them)"
	echo "-s --send-to      Send the merged po files to the given languages"
	echo ""
	echo "Important: Both of the \"--send-to\" and \"--ignore\" options do "
	echo " await a ':' separated list like \"az:tr:uk\"."
	echo ""
	echo "-S --statistics   Print out the statistics table at the end"
	echo "-o --output-file  Print the eventual statistics table to the given file"
	echo "-D --dry-run      Don't send any EMails, create statistics (implies -S)"
	echo "-N --no-list      Don't send any EMails to the list"
	echo "-n --no-personal  Don't send personal EMails to the last translators"
	echo "-t --timer-stats  Print pozilla.sh working seconds statistic at the end"
	echo "-v --version      Version informations"
	echo "-h --help         This help screen"
	print_separator_line
	echo "Please consult pozilla.sh's manual page for more usage examples"
	echo " and informations; you can reach it via \"man pozilla.sh\"."
	print_separator_line
		exit 1
	;;
	-v|--version)
	print_separator_line
	echo " Pozilla.sh R $POZILLA_RELEASE"
	print_separator_line
        echo " Author:  Fatih Demir <kabalak@kabalak.net>"
	print_separator_line
		exit 1
	;;
	-D|--dry-run)
	shift 1
		print_separator_line
		echo "Running in dry mode -- won't send any emails out..."
		print_separator_line
		export RUN_DRY=yes
		export PRINT_TABLE=yes
	;;
	-n|--no-personal)
	shift 1
	if test "say_$RUN_DRY" = "say_yes" ; then
		dry_run_information_message
	else
		print_separator_line
		echo "Not sending any personal EMails; only sending to the list..."
		print_separator_line
		export NO_PERSONAL=yes
	fi
	;;
	-N|--no-list)
	shift 1
	if test "say_$RUN_DRY" = "say_yes" ; then
		dry_run_information_message
	else
		print_separator_line
		echo "Not sending any EMails to the list..."
		print_separator_line
		export NO_LIST=yes
	fi
	;;
	-t|--timer-stats)
	shift 1
		export TIMER_STATS=yes
	;;
	-o|--output-file)
	shift 1
		if test "file$1" = "file" ; then
			print_separator_line
			echo "!WARNING�"
			echo ""
			echo "No output filename given! Will print an eventual statistics"
			echo " table to stdout."
			print_separator_line
		else
			if test -d "$1" ; then
				export OUTPUT_FILE="$1.statistics"
				
				print_separator_line
				echo "\"$1\" is a directory! Will take \"$OUTPUT_FILE\" as filename."
				print_separator_line
			else
				export OUTPUT_FILE="$1"
			fi

			shift 1
			
			print_separator_line
			echo "Will print an eventual statistics table to the following file:"
			echo "\"$OUTPUT_FILE\""
			print_separator_line
		fi
	;;
	-m|--mailinglist)
	shift 1
	if test "say_$RUN_DRY" = "say_yes" ; then
		dry_run_information_message

		if test "q$1" != "q" ; then
			shift 1
		fi
	else
		if test "hehe$1" = "hehe" ; then
			print_separator_line
			echo "No mailing list given, using default:"
			echo "$MAILING_LIST"
			print_separator_line
		else
			print_separator_line
			echo "Using special mailing list:"
			export MAILING_LIST="$1"
			echo "$MAILING_LIST"
			print_separator_line
			shift 1
		fi
	fi
	;;
	-a|--additional)
	shift 1
	if test "say_$RUN_DRY" = "say_yes" ; then
		dry_run_information_message

		if test "q$1" != "q" ; then
			shift 1
		fi
	else
		if test "hehe$1" = "hehe" ; then
			print_separator_line
			echo "No additional mail address given!"
			print_separator_line
		else
			print_separator_line
			export ADDITIONAL_MAILING_ADDRESS="$1"
			echo "Using $ADDITIONAL_MAILING_ADDRESS for additional mailing."
			print_separator_line
			shift 1
		fi
	fi
	;;
	-d|--days)
	shift 1
	if test "days$1" = "days" ; then
		print_separator_line
		echo "No number of days given! Assuming \"7\" days to go..."
		print_separator_line
		export DAYS_REMAINING=7
	else
		if test $1 -le 3 ; then
			print_separator_line
			echo "You're giving your translators only $1 day(s); that's too less!"
			echo "Please give them more time to update their translations."
			print_separator_line
				exit 1
		else
			export DAYS_REMAINING="$1"
			print_separator_line
			echo "Days remaining: $DAYS_REMAINING"
			print_separator_line
			shift 1
		fi
	fi
	;;
	-r|--release)
	shift 1
	if test "r$1" = "r" ; then
		print_separator_line
		echo "No release number defined! Trying to figuring it out.."
		print_separator_line
	else
		print_separator_line
		export MY_RELEASE="$1"
		echo "Using user-defined release string $MY_RELEASE"
		print_separator_line
		shift 1
	fi	
	;;
	-i|--ignore)
	shift 1
	if test "say_$RUN_DRY" = "say_yes" ; then
		dry_run_information_message

		if test "q$1" != "q" ; then
			shift 1
		fi
	else
		if test "say_$NO_PERSONAL" = "say_yes" ; then
			no_personal_information_message

			if test "%$1" != "%" ; then shift 1 ; fi
		else
			if test "ig$1" = "ig" ; then
				print_separator_line
				echo "No languages to ignore given."
				print_separator_line
			else
				print_separator_line
				export IGNORE_LANGS="`echo $1|sed -e 's/:/\ /g'`"
				shift 1
				echo "Ignoring po files for this/these lang(s): $IGNORE_LANGS"
				print_separator_line
			fi
		fi
	fi
	;;
	-A|--send-to-all)
	shift 1
	if test "say_$RUN_DRY" = "say_yes" ; then
		dry_run_information_message
	else
		if test "say_$NO_PERSONAL" = "say_yes" ; then
			no_personal_information_message
		else
			print_separator_line
			echo "Sending po files to all languages..."
			print_separator_line
			export SEND_TO_ALL_LANGUAGES=yes
		fi
	fi
	;;
	-s|--send-to)
	shift 1
	if test "say_$NO_PERSONAL" = "say_yes" ; then
		no_personal_information_message

		if test "%$1" != "%" ; then shift 1 ; fi
	else
		if test "q$SEND_TO_ALL_LANGUAGES" != "q" ; then
			print_separator_line
			echo "Sending the po files to all languages switch is already active.."
			echo "Ignoring arguments.."
			print_separator_line
			
			if test "foo$1" != "foo" ; then shift 1 ; fi
		else	
			if test "sendto$1" = "sendto" ; then
				print_separator_line
				echo "No language given to send the po file to."
				print_separator_line
			else
				print_separator_line
				export SEND_TO_LANGS="`echo $1|sed -e 's/:/\ /g'`"
				shift 1
				echo "Sending the merged po files to this/these lang(s): $SEND_TO_LANGS"
				print_separator_line
			fi
		fi
	fi
	;;
	-S|--statistics)
	if test "say_$RUN_DRY" = "say_yes" ; then
		print_separator_line
		echo "We're already running in dry-modus -- the statistics table will"
		echo " be printed at the end of the process."
		print_separator_line
	else
		print_separator_line
		echo "Will print out the statistics table..." 
		export PRINT_TABLE=yes
		print_separator_line
	fi
	shift 1
	;;
	-p|--podirectory)
	shift 1
	if test "po$1" = "po" ; then
		print_separator_line
		echo "No po directory location defined. Using default location ./po ..."
		print_separator_line
		export PO_DIR="."
	else
		print_separator_line
		export PO_DIR="$1"
		if ! test -d "$PO_DIR" ; then
			echo "\"$PO_DIR\" is not a directory; taking default location ./po..."
		else	
			echo "Using \"$PO_DIR\" as po directory location..."
		fi
		print_separator_line
		shift 1
	fi
	;;
	*)
		print_separator_line
		echo "!ERROR�: Unknown option \"$1\" given.                          "
		print_separator_line
			exit 1
	;;
	esac
done	

#
# Finally, check for existance of mutt if $RUN_DRY is not defined.
#
if test "z`which mutt`" = "z" -a "say_$RUN_DRY" != "say_yes" ; then
	print_separator_line
	echo "!ERROR�: The application \"mutt\" is necessary for running pozilla.sh!"
	print_separator_line
	exit 1
fi

#
# Check if a po directory is existent in the $PO_DIR.
#
if ! test -d $PO_DIR/po ; then
	print_separator_line
	echo "!ERROR�: There is no \"po\" directory present in \"$PO_DIR\"!"
	print_separator_line
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
# Check if we've got a configure.in present and operate accordingly.
#
if test -f configure.in ; then
	export PACKAGE=`grep \^AM_INIT_AUTOMAKE configure.in|sed -e 's/^.*(//g' -e 's/,.*$//g'`

	if test "o$PACKAGE" = "oAC_PACKAGE_NAME" ; then
	
		export PACKAGE=`grep \^AC_INIT configure.in|sed -e 's/^.*(//g' -e 's/,.*$//g'`
	fi

	#
	# Test if we'd get a PACKAGE and try some other plays if we'dn't get it.
	#
	if test "z$PACKAGE" = "z" ; then
		export PACKAGE=`basename ${PWD:-`pwd`}`

		print_separator_line
		echo "Couldn't automatically determine package name, taking local"
		echo " directory name \"$PACKAGE\" instead..."
		print_separator_line
	fi
else
	print_separator_line
	echo "No configure.in found in ${PWD:-`pwd`}, so you do now"

	read -p " please enter the package name: " package_name

	if test "z$package_name" != "z" ; then
		export PACKAGE=$package_name

		echo ""
		echo "Setting package name to \"$PACKAGE\"..."
	else
		print_separator_line
		echo "!ERROR� You didn't enter any package name!"
		print_separator_line
			exit 1
	fi
	
	print_separator_line
fi

#
# Hopefully the other apps are using plain version strings
#  like "0.8" or "0.32" -- if not, query the admin.
#
if test "y$MY_RELEASE" = "y" ; then
	export RELEASE=`grep \^AM_INIT_AUTOMAKE configure.in|\
		sed -e 's/^.*(//g' -e 's/.*,//g' -e 's/).*//g' -e 's/\ //g'`

	if test "r$RELEASE" = "rAC_PACKAGE_VERSION" ; then
		export RELEASE=`grep \^AC_INIT configure.in|sed -e 's/^.*(//g' -e s/$PACKAGE,//g -e 's/,//g' -e 's/\ //g'`
	fi
	
	if test "r$RELEASE" = "r" ; then
		print_separator_line
		echo "Couldn't automatically determine package version and as no"
		echo " special version string was specified using the \"-r\" switch,"
		read -p "  please enter the version : " version_string

		if test "v$version_string" != "v" ; then
			export RELEASE=$version_string
			
			echo ""
			echo "Taking \"$RELEASE\" as version..."
		else
			print_separator_line
			echo "!ERROR�"
			echo ""
			echo "No version detected/entered, please try using the \"-r\" switch"
			echo " switch from the command line next time."
			print_separator_line
				exit 1
		fi
		
		print_separator_line
	fi
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
if test "q`which intltool-update`" != "q" ; then
	intltool-update --pot 2>&1 >/dev/null
elif test "q`which xml-i18n-update`" != "q" ; then
	xml-i18n-update --pot 2>&1 >/dev/null
elif test -f Makefile -a -f POTFILES.in ; then
	[ -f $PACKAGE.pot ] && rm -f $PACKAGE.pot
	[ -f POTFILES ] && rm -f POTFILES
	make POTFILES 2>&1 >/dev/null
	make $PACKAGE.pot 2>&1 >/dev/null
elif test -x ./update.pl ; then
	./update.pl -P 2>&1 >/dev/null
elif test -x ./update.sh ; then
	./update.sh -P 2>&1 >/dev/null
else
	print_separator_line
	echo "!ERROR�:"
	echo ""
	echo "Neither \"xml-i18n-update\" nor usable Makefile/update scripts"
	echo " like \"./update.pl\" or \"./update.sh\" found!"
	print_separator_line
		exit 1
fi

#
# Test if we could build the pot file.
#
[ -f $PACKAGE.pot ] || {
	print_separator_line
	echo "!ERROR�: Couldn't build \"$PACKAGE.pot\" file!"
	print_separator_line
		exit 1
}

#
# Setup the table-header .-)
#
STAT_TABLE="
<=======--------==============--==========------==========------======>
Language	Total messages	Translated	Percentage	Missing
<=======--------==============--==========------==========------======>
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

	if echo $merge_status|grep -sq invalid\ control ; then
		extra_args="OLD_PO_FILE_INPUT=yes"
		merge_status=`$extra_args msgmerge $i $PACKAGE.pot -o $i 2>&1`
	fi
	
	if echo $merge_status|grep -sq warning ; then
		mv $i.backup $i

		STAT_TABLE="$STAT_TABLE
$language\t\t<------====== Failure due to a warning =====------>"

		continue
	elif echo $merge_status|grep -sq error ; then
		mv $i.backup $i

		STAT_TABLE="$STAT_TABLE
$language\t\t<------====== Failure due to an error ======------>"

		continue
	fi
	
	#
	# Get the values for the messages statistics.
	#
	statistics=(`$extra_args msgfmt -v $i 2>&1`)
	translated=${statistics[0]}
	fuzzy=${statistics[3]:-0}
	untranslated=${statistics[6]:-0}

	#
	# Arg, I'm really insane without any doubt -- also act for the case
	#  where the output of msgmerge etc. doesn't look very good -- happening
	#   to the "az.po" of gtranslator; well, as I dunno why, I just try to
	#    get it slaned-by.
	#
	if `echo "$translated"|grep -sq "[[:alpha:]]"` ; then
		continue
	elif `echo "$fuzzy"|grep -sq "[[:alpha:]]"` ; then
		continue
	elif `echo "$untranslated"|grep -sq "[[:alpha:]]"` ; then
		continue
	else
		#
		# Hurray! A working case .-)
		#
		messages=$[ $translated + $fuzzy + $untranslated ]
	fi

	missing=$[ $fuzzy + $untranslated ]
	centil=`awk "{ print $messages / 100 }" $CONFIG_DIR/pozilla.conf`
	percent=`awk "{ printf \"%.2f\", $translated / $centil }" $CONFIG_DIR/pozilla.conf`

	case $percent in
	?.*)
	STAT_TABLE="$STAT_TABLE
$language\t\t$messages\t\t$translated\t\t  $percent%\t\t$missing"
	;;
	??.*)
	STAT_TABLE="$STAT_TABLE
$language\t\t$messages\t\t$translated\t\t $percent%\t\t$missing"	
	;;
	*)
	STAT_TABLE="$STAT_TABLE
$language\t\t$messages\t\t$translated\t\t$percent%\t\t$missing"
	;;
	esac

	#
	# Test if the previously parsed author string is valuable at all -- it
	#  should contain at least an EMail address and therefore '<', '>' and
	#   '@' characters should be present -- if not, jump to the next file.
	#
	echo "$AUTHOR"|grep -sq "<" || {
		continue
	}

	echo "$AUTHOR"|grep -sq ">" || {
		continue
	}

	echo "$AUTHOR"|grep -sq "@" || {
		continue
	}
	
	#
	# Only operate if we don't need to run drily or to send personal mails.
	#
	if test "say_$RUN_DRY" != "say_yes" ; then
		if test "say_$NO_PERSONAL" != "say_yes" ; then
	#
	# Compile the current merged po file.
	#
	$extra_args msgfmt -vv $i 2>/dev/null
	
	case $? in
	1)
		echo "You should update your $i po-file for $PACKAGE," > $BODY_FILE
		echo " it's containing fuzzy or/and untranslated entries if you get it" >> $BODY_FILE
		echo "  in sync with the recent sources for $PACKAGE." >> $BODY_FILE
		echo "" >> $BODY_FILE
		if test "b$DAYS_REMAINING" != "b" ; then
			echo "$PACKAGE will release R $RELEASE in $DAYS_REMAINING days, so" >> $BODY_FILE
			echo " that you should update your translation till then." >> $BODY_FILE
			echo "" >> $BODY_FILE
		fi
		echo "Your po-file $i's statistics are:" >> $BODY_FILE

		case $percent in
		?.*)
			echo "${statistics[*]} [  $percent%]" >>$BODY_FILE
		;;
		??.*)
			echo "${statistics[*]} [ $percent%]" >>$BODY_FILE
		;;
		*)
			echo "${statistics[*]} [$percent%]" >>$BODY_FILE
		;;
		esac

		echo "" >> $BODY_FILE
		if test "q$SEND_TO_ALL_LANGUAGES" != "q" ; then
			if test "s$SEND_TO_LANGS" != "s" ; then
				_lang=`echo $i|sed -e 's/.po//g'`
				if  test "q$SEND_TO_ALL_LANGUAGES" != "q" ; then
					gzip --best -cf < $i > $PACKAGE.$i.gz
					echo "An updated and merged $i file is attached to this message, so that you can" >> $BODY_FILE
					echo " immediately start with your update of $i." >> $BODY_FILE
					echo "" >> $BODY_FILE
				elif echo $SEND_TO_LANGS|grep -sq $_lang ; then
					gzip --best -cf < $i > $PACKAGE.$i.gz
					echo "An updated and merged $i file is attached to this message, so that you can" >> $BODY_FILE
					echo " immediately start with your update of $i." >> $BODY_FILE
					echo "" >> $BODY_FILE
				fi
			fi
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
	echo "  - the po-files - send a mail to Fatih Demir <kabalak@kabalak.net>" >> $BODY_FILE
	echo "   Thanks." >> $BODY_FILE
	if test -f $PACKAGE.$i.gz ; then
		cat $BODY_FILE|mutt -s "$SUBJECT" "$AUTHOR" -a $PACKAGE.$i.gz
	else	
		cat $BODY_FILE|mutt -s "$SUBJECT" "$AUTHOR"
	fi
	[ -f $PACKAGE.$i.gz ] && rm -f $PACKAGE.$i.gz

		fi
	fi

	#
	# Clean up any resting backup file (due to --dry-run/--no-personal).
	#
	if test "say_$NO_LIST" != "say_yes" ; then 
		if test -f $i -a -f $i.backup ; then 
			rm -f $i
			mv $i.backup $i
		fi
	fi
	
        done

#
# Send a mail to the mailing list -- if we're running in "wet-modus".
#
if test "say_$RUN_DRY" != "say_yes" ; then
	if test "say_$NO_LIST" != "say_yes" ; then

echo "Dear translators of $PACKAGE:" > $BODY_FILE
echo "" >> $BODY_FILE
if test "Z$DAYS_REMAINING" = "Z" ; then
	echo "The next release of $PACKAGE (R $RELEASE) is coming within the next days" >> $BODY_FILE
else
	echo "The next release of $PACKAGE (R $RELEASE) is coming in $DAYS_REMAINING days" >> $BODY_FILE
fi
echo "and you all should update your translator for it please." >> $BODY_FILE
echo "" >> $BODY_FILE
if test "say_$NO_PERSONAL" != "say_yes" ; then
echo "Possibly you'll also get a \"private\" message from pozilla informing" >> $BODY_FILE
echo "you about the coming release with the specs/status of your po-file." >> $BODY_FILE
fi
if test "Q$SEND_TO_ALL_LANGUAGES" != "Q" ; then
echo "" >> $BODY_FILE
echo "Updated and merged po files have been sent to all last translators" >> $BODY_FILE
echo "of $PACKAGE." >> $BODY_FILE
fi
echo "" >> $BODY_FILE
echo "Current po files statistics table:" >> $BODY_FILE
echo -e "$STAT_TABLE" >> $BODY_FILE
echo "" >> $BODY_FILE
echo "--" >> $BODY_FILE
echo "This is a mail send by Pozilla R $POZILLA_RELEASE." >> $BODY_FILE
echo "For questions concerning Pozilla or your translator's faith -- the po files --" >> $BODY_FILE
echo "   send an EMail to Fatih Demir <kabalak@kabalak.net>." >> $BODY_FILE
echo "Thanks for using Pozilla!" >> $BODY_FILE

#
# Send the mail to the address(es).
#
if test "my$ADDITIONAL_MAILING_ADDRESS" = "my" ; then
	cat $BODY_FILE|mutt -s "$SUBJECT" "$MAILING_LIST"
else
	cat $BODY_FILE|mutt -s "$SUBJECT" "$MAILING_LIST" -c "$ADDITIONAL_MAILING_ADDRESS"
fi

	fi
fi

#
# Clean up the rest of the mailfiles.
#
[ -f $BODY_FILE ] && rm -f $BODY_FILE

#
# Print out the statistics table if necessary.
#
if test "&$PRINT_TABLE" = "&yes" ; then
	if test "q$OUTPUT_FILE" != "q" ; then
		echo -e "$STAT_TABLE" > $OUTPUT_FILE
	else
		echo -e "$STAT_TABLE"
	fi
fi

#
# Stop our internal timer by now -- we should know now how many seconds we did
#  work for the current call.
#
POZILLA_SH_TIMER_STOP=`date +%s`
POZILLA_SH_WORKING_TIME=$[ $POZILLA_SH_TIMER_STOP - $POZILLA_SH_TIMER_START ]

#
# If we should print a small working information about our "working seconds"
#  then it's now the best place to do so.
#
if test "say_$TIMER_STATS" = "say_yes" ; then
	print_separator_line
	echo "  pozilla.sh R $POZILLA_RELEASE worked $POZILLA_SH_WORKING_TIME seconds long for you."
	print_separator_line
fi

#
# Recover the old LANG environment flags.
#
export LANG="$_LANG"
export LANGUAGE="$_LANGUAGE"
export LC_ALL="$_LC_ALL"

#
# Exit with 0. We're all happy :-0
#
exit 0