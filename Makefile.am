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

MAINTAINERCLEANFILES = 		\
	aclocal.m4		\
	config.guess		\
	config.h.in		\
	config.sub		\
	depcomp			\
	gtk-doc.make		\
	install-sh		\
	ltmain.sh		\
	missing			\
	mkinstalldirs		\
	m4/gnome-doc-utils.m4	\
	m4/gtk-doc.m4		\
	m4/intltool.m4		\
	m4/libtool.m4		\
	m4/ltoptions.m4		\
	m4/ltsugar.m4		\
	m4/ltversion.m4		\
	m4/lt~obsolete.m4	\
	`find "$(srcdir)" -type f -name Makefile.in -print`

DISTCHECK_CONFIGURE_FLAGS = --disable-scrollkeeper --enable-gtk-doc
-include $(top_srcdir)/git.mk
