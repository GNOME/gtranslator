#!/bin/sh
#
# (C) 2001-2003 Fatih Demir <kabalak@kabalak.net>
#		Juan RP <juan@xtraeme.UnixBSD.org>
#
# cleanup-gtranslator-settings.sh -- Cleans up gtranslator's settings and
#  "personal data" directories for the user running this script -- it simply
#    removes all of the corresponding files in the user's home directory.
#

#
# We should always show some information on a call with "--help" as the arg.
#
case $1 in
--[hH][eE][lL][pP]|-[hH])
	echo "---------------------------------------------------------------"
	echo "\"$0\""
	echo " does allow you to completely clean up all gtranslator data and"
	echo "  settings from your home directory. Be careful with it!"
	echo "---------------------------------------------------------------"
		exit 1
;;
esac

remove_commands ()  {
	# Default path for killall
	KILLALL=/usr/bin/killall

	# Check if we have killall or pkill
	case `uname` in
	FreeBSD)
		KILLALL=/usr/bin/killall
	;;
	NetBSD|SunOS)
		KILLALL=/usr/bin/pkill
	;;
	Linux)
		if [ -x /usr/bin/pkill ]; then
			KILLALL=/usr/bin/pkill
		elif [ -x /usr/bin/killall ]; then
		KILLALL=/usr/bin/killall
		fi
	;;
        *)
		echo "Unknown OS, using default path: /usr/bin/killall"
        ;;
	esac

	$KILLALL gtranslator 2>&1 >/dev/null ; sleep 3 && \
	gconftool --shutdown 2>&1 >/dev/null && sleep 3 && \
	rm -rf ~/.gtranslator ~/.gconf/apps/gtranslator \
		~/.gnome/gtranslator ~/.gnome/accels/gtranslator

	echo "---------------------------------------------------------------"
	echo "All of your gtranslator settings have been (hopefully) deleted!"
	echo "---------------------------------------------------------------"
}

echo "---------------------------------------------------------------"
echo "Closing down gtranslator, gconf and deleting all setting files"
echo " of gtranslator..."
echo "---------------------------------------------------------------"
echo "Are you really sure to delete all person. settings/data created"
echo " by gtranslator?"
echo "---------------------------------------------------------------"
read -p "[y/N]? " answer

case $answer in
[yY]*|[zZ]*)
	remove_commands	
;;
*)
	echo "---------------------------------------------------------------"
	echo "Wise answer :-) Your settings are not removed/cleaned up now..."
	echo "---------------------------------------------------------------"
		exit 1
;;
esac

exit 0