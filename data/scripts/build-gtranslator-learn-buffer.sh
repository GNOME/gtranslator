:
#!/bin/sh
#
# (C) 2002 Fatih Demir <kabalak@gtranslator.org>
#
# build-gtranslator-learn-buffer.sh -- Builds a learn buffer within a half-wise
#  automated process -- if you have got a central CVS checkout dir then you'd
#   get a cup of coffee and watch gtranslator and this script working.
#

#
# Also feature a small help screen if the user wants to know more about this
#  script.
#
case $1 in
--[hH][eE][lL][pP]|-[hH])
	echo "---------------------------------------------------------------"
	echo "\"$0\""
	echo " is a script which does try to semiautomatically set up a learn"
	echo "  buffer for gtranslator with the data you're entering and with"
	echo "   po files beneath the current directory."
	echo "---------------------------------------------------------------"
		exit 1
;;
esac

#
# The functions used in our little, small script.
#
read_po_filename ()  {
	echo "---------------------------------------------------------------"
	echo "Welcome to build-gtranslator-learn-buffer.sh -- please enter"
	echo " now the filename scheme of the po files which should be all"
	echo "  learned now by gtranslator to build up a learn buffer:"
	read -p "Po filename (e.g. \"tr.po\"): " po_filename
	echo "---------------------------------------------------------------"
}

build_up_learn_buffer ()  {
	for i in `find . -type f -name $po_filename`
		do

			[ -f "$i" ] && gtranslator -n -l "$i"
		done
}

#
# Now we're going to the action part of our script.
#
read_po_filename

echo "---------------------------------------------------------------"
echo "Are you sure to let gtranslator learn all $po_filename files"
echo " below the current \"${PWD:-`pwd`}\" directory?"
read -p "[y/N] " answer
echo "---------------------------------------------------------------"

case "$answer" in
[yY]|[zZ])
	echo "---------------------------------------------------------------"
	echo "Starting to build up a learn buffer with gtranslator..."
	echo "---------------------------------------------------------------"
	build_up_learn_buffer
;;
*)
	echo "---------------------------------------------------------------"
	echo "Ok, _not_ building a learn buffer for gtranslator."
	echo "---------------------------------------------------------------"
		exit 1
;;
esac

exit 0
