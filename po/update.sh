#!/bin/sh
#######################################################################
package=gtranslator
version=0.18
#######################################################################
build_pot  ()  {
echo "Building the $package.pot ..."
xgettext --default-domain=$package --directory=.. \
  --add-comments --keyword=_ --keyword=N_ \
  --files-from=./POTFILES.in \
&& test ! -f $package.po \
   || ( rm -f ./$package.pot \
    && mv $package.po ./$package.pot )
}
#######################################################################
do_it  ()  {
[ -f $package.pot ] || build_pot
msgmerge $lang.po $package.pot|sed -e 's/^#~.*$//g' |\
sed -e s/^\"PO-Revision-.*$/\"PO-Revision-Date:\ "`date +%Y-%m-%d`"\ "`date +%H:%M%z`"'\\n'\"/ |\
sed -e s/^\"Project-Id.*$/\"Project-Id-Version:\ $package\ $version'\\n'\"/ > $lang.po
}
#######################################################################
case $1 in
-h|--h*)
echo "
./update.sh -- gtranslator's dir.
	
without arguments only the $package.pot is produced.

Possible arguments

	-h --help    -- help
	[LANGUAGE]   -- build the $package.pot, check if there is a
			 [LANGUAGE].po, merge it, remove unneeded 
			  entries (#~ ..) and substitute the package
			   name and the version.

	Example: ./update.sh tr will produce the $package.pot and merge 
		the Turkish translation (tr.po) and update the version
		 number.
		

"
	exit 0
;;
[a-zA-Z]*)
lang="$1"
[ -f $lang.po ] || {
	echo "No $lang.po file found. No merge will be done."
	build_pot && exit 0
}
do_it
;;
*)
build_pot
;;
esac
#######################################################################
