:
#!/bin/sh

echo "---------------------------------------------------------------"
echo "Closing down gtranslator, gconf and deleting all setting files"
echo " of gtranslator..."
echo "---------------------------------------------------------------"

killall gtranslator 2>&1 >/dev/null && sleep 3 && \
gconftool --shutdown 2>&1 >/dev/null && sleep 3 && \
rm -rf ~/.gtranslator ~/.gconf/apps/gtranslator \
	~/.gnome/gtranslator ~/.gnome/accels/gtranslator

echo "---------------------------------------------------------------"
echo "Hopefully done! All of your gtranslator settings have been"
echo " deleted."
echo "---------------------------------------------------------------"

exit 0
