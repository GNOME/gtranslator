:
#!/bin/sh

echo "---------------------------------------------------------------"
echo "Closing down gtranslator, gconf and deleting all setting files"
echo " of gtranslator..."
echo "---------------------------------------------------------------"

killall gtranslator 2>&1 >/dev/null
gconftool --shutdown 2>&1 >/dev/null
rm -rf ~/.gtranslator ~/.gconf/apps/gtranslator \
	~/.gnome/gtranslator ~/.gnome/accels/gtranslator

echo "---------------------------------------------------------------"
echo "Hopefully done!"
echo "---------------------------------------------------------------"

exit 0
