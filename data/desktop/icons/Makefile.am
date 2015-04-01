icon16dir = $(datadir)/icons/hicolor/16x16/apps
icon16_DATA = 16x16/gtranslator.png

icon22dir = $(datadir)/icons/hicolor/22x22/apps
icon22_DATA = 22x22/gtranslator.png

icon24dir = $(datadir)/icons/hicolor/24x24/apps
icon24_DATA = 24x24/gtranslator.png

icon32dir = $(datadir)/icons/hicolor/32x32/apps
icon32_DATA = 32x32/gtranslator.png

icon48dir = $(datadir)/icons/hicolor/48x48/apps
icon48_DATA = 48x48/gtranslator.png

svgicondir = $(datadir)/icons/hicolor/scalable/apps
svgicon_DATA = \
  scalable/gtranslator.svg \
  scalable/gtranslator-symbolic.svg

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

EXTRA_DIST = \
	$(icon16_DATA)		\
	$(icon22_DATA)		\
	$(icon24_DATA)  	\
	$(icon32_DATA)  	\
	$(icon48_DATA)  	\
	$(svgicon_DATA)
