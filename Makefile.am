## the main Makefile.am

SUBDIRS = data help man src doc plugins po

EXTRA_DIST= \
		DEPENDS \
		HACKING \
		gtranslator.spec.in \
		gtranslator.spec \
		omf.make \
		xmldocs.make \
		gnome-doc-utils.make

DISTCLEANFILES = 		\
	gnome-doc-utils.make    

DISTCHECK_CONFIGURE_FLAGS = --disable-scrollkeeper --enable-gtk-doc
-include $(top_srcdir)/git.mk
