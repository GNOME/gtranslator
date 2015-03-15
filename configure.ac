dnl -------------------------------------------------------------------
dnl Process this file with autoconf to produce a configure script.
dnl -------------------------------------------------------------------
dnl (C) 2000-2004 Fatih Demir <kabalak@kabalak.net>
dnl -------------------------------------------------------------------
AC_PREREQ(2.64)
AC_INIT([gtranslator],[2.91.7],[http://bugzilla.gnome.org/enter_bug.cgi?product=gtranslator],[gtranslator],[http://projects.gnome.org/gtranslator/])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

GTR_API_VERSION=3.0
AC_SUBST(GTR_API_VERSION)

AM_INIT_AUTOMAKE([1.11 tar-ustar dist-xz no-dist-gzip -Wno-portability])

AM_SILENT_RULES([yes])

dnl -------------------------------------------------------------------
dnl Check for some standard stuff.
dnl -------------------------------------------------------------------
# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL

AM_PATH_PYTHON

# Initialize libtool
LT_PREREQ([2.2.6])
LT_INIT

# Checks for header files.
AC_CHECK_HEADERS([locale.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset setlocale strchr strdup strstr])

dnl -------------------------------------------------------------------
dnl Use intltools.
dnl -------------------------------------------------------------------
IT_PROG_INTLTOOL([0.50.1])

AC_SUBST([GETTEXT_PACKAGE], [gtranslator])
AC_DEFINE([GETTEXT_PACKAGE], [PACKAGE_TARNAME], [Define to the gettext package name.])

dnl -------------------------------------------------------------------
dnl Documentation
dnl -------------------------------------------------------------------
YELP_HELP_INIT
GTK_DOC_CHECK([1.0])

dnl -------------------------------------------------------------------
dnl The standard extended GNOME compile warning options ...
dnl -------------------------------------------------------------------
GNOME_COMPILE_WARNINGS([maximum])


AC_PATH_PROG([GLIB_COMPILE_RESOURCES], [glib-compile-resources])
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])
AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])

dnl ================================================================
dnl GSettings stuff
dnl ================================================================
GLIB_GSETTINGS

dnl ==========================================================================
dnl
dnl If you add a version number here, you *must* add an AC_SUBST line for
dnl it too, or it will never make it into the spec file!
dnl
dnl ==========================================================================

GTHREAD_REQUIRED=2.13.0
GTK_REQUIRED=3.4.2
GLIB_REQUIRED=2.32.0
LIBXML_REQUIRED=2.4.12
SOURCEVIEW_REQUIRED=3.0.0
GDL_REQUIRED=3.6.0
GDA_REQUIRED=4.2.3
PEAS_REQUIRED=1.2.0
GDICT_OPTIONAL=0.11.0
GTKSPELL_OPTIONAL=3.0.0
JSON_GLIB_OPTIONAL=0.12.0

AC_ARG_WITH(gda,
	AS_HELP_STRING([--with-gda={4.0|5.0}],[Define which version of libgda to use]),,
	with_gda=5.0)

PKG_CHECK_MODULES(GTRANSLATOR, [
	libxml-2.0 >= $LIBXML_REQUIRED
	gthread-2.0 >= $GTHREAD_REQUIRED
	glib-2.0 >= $GLIB_REQUIRED
	gio-2.0 >= $GLIB_REQUIRED
	gtk+-3.0 >= $GTK_REQUIRED
	gtksourceview-3.0 >= $SOURCEVIEW_REQUIRED
	gdl-3.0 >= $GDL_REQUIRED
	gsettings-desktop-schemas
	libgda-sqlite-$with_gda >= $GDA_REQUIRED
	libpeas-1.0 >= $PEAS_REQUIRED
	libpeas-gtk-1.0 >= $PEAS_REQUIRED
])

AC_SUBST(GTRANSLATOR_CFLAGS)
AC_SUBST(GTRANSLATOR_LIBS)

dnl gettext library
AC_CHECK_LIB([gettextpo], [po_message_is_format],
             [GETTEXT_LIBS=-lgettextpo], [
              AC_MSG_ERROR(Missing gettext library)])

AC_SUBST([GETTEXT_LIBS])

dnl ------------------------
dnl Check for win32 platform
dnl ------------------------
AC_MSG_CHECKING([for Win32 platform])
case "$host" in
  *-*-mingw*|*-*-cygwin*)
    platform_win32=yes
    AC_CHECK_TOOL(WINDRES, windres)
    ;;
  *)
    platform_win32=no
    ;;
esac
AC_MSG_RESULT([$platform_win32])
AM_CONDITIONAL(PLATFORM_WIN32, test "$platform_win32" = "yes")


dnl -----------------------------------
dnl Plugins libraries compilation flags
dnl -----------------------------------
# on win32 plugins must be able to resolve all symbols at link time
# so we create a lib
if test "$platform_win32" = yes; then
	PLUGIN_LIBTOOL_FLAGS="-module -avoid-version -no-undefined -mwindows -Wl,\$(top_builddir)/src/libgtranslator-\$(GTR_API_VERSION).a"
else
	PLUGIN_LIBTOOL_FLAGS="-module -avoid-version"
fi
AC_SUBST(PLUGIN_LIBTOOL_FLAGS)

dnl ================================================================
dnl iso-codes
dnl ================================================================

ISO_CODES_REQUIRED=0.35

PKG_CHECK_EXISTS([iso-codes >= $ISO_CODES_REQUIRED],
		 [have_iso_codes=yes],[have_iso_codes=no])

if test "x$have_iso_codes" = "xyes"; then
	AC_MSG_CHECKING([whether iso-codes has iso-639 domain])
	if $PKG_CONFIG --variable=domains iso-codes | grep 639 > /dev/null; then
		result=yes
	else
		result=no
		have_iso_codes=no
	fi
	AC_MSG_RESULT([$result])
fi

if test "x$have_iso_codes" = "xyes"; then
	AC_DEFINE_UNQUOTED([ISO_CODES_PREFIX],["`$PKG_CONFIG --variable=prefix iso-codes`"],[ISO codes prefix])
else
	AC_MSG_ERROR([iso-codes is required to build gtranslator.])
fi

dnl -------------------------------------------------------------------
dnl Check for gtkspell >= 3.0 and use it if found
dnl -------------------------------------------------------------------
have_gtkspell=no
AC_ARG_WITH(gtkspell,
            AS_HELP_STRING([--with-gtkspell],[Build with spell support]),,
            with_gtkspell=auto)

if test x"$with_gtkspell" != "xno"
then
        PKG_CHECK_MODULES([GTKSPELL], [
                gtkspell3-3.0 >= $GTKSPELL_OPTIONAL
        ], have_gtkspell=yes, have_gtkspell=no)

        if test x"$have_gtkspell" = "xyes"
        then
                AC_DEFINE([HAVE_GTKSPELL], 1, [A usable GtkSpell library was found])
        fi

fi

AM_CONDITIONAL(USE_GTKSPELL, test x"$have_gtkspell" = "xyes")

AC_SUBST(GTKSPELL_CFLAGS)
AC_SUBST(GTKSPELL_LIBS)

dnl -------------------------------------------------------------------
dnl Check for json-glib, needed for Open-tran
dnl -------------------------------------------------------------------
if test "x$enable_opentran" != "xno"; then
	PKG_CHECK_MODULES(JSON_GLIB, [
		json-glib-1.0 >= $JSON_GLIB_OPTIONAL
		], have_json_glib=yes, have_json_glib=no)
	if test x"$have_json_glib" = "xyes"; then
		AC_DEFINE(LIBJSON_GLIB_1_0, 1, [Define if libjson-glib support is enabled])
	fi
fi

AM_CONDITIONAL(USE_LIBJSON_GLIB, test x"$have_json_glib" = "xyes")

# ================================================================
# gdict
# ================================================================
have_gdict=no
AC_ARG_WITH(dictionary,
	    AS_HELP_STRING([--with-dictionary],[Build with dictionary support]),,
	    with_dictionary=auto)

if test x"$with_dictionary" != "xno"
then
	PKG_CHECK_MODULES([DICTIONARY], [
		gdict-1.0 >= $GDICT_OPTIONAL
	], have_gdict=yes, have_gdict=no)
	
	if test x"$have_gdict" = "xyes"
	then
		AC_DEFINE([HAVE_DICTIONARY], 1, [Have gdict])
	fi

fi

AM_CONDITIONAL(USE_DICTIONARY, test x"$have_gdict" = "xyes")

dnl -------------------------------------------------------------------
dnl Enable debug builds.
dnl -------------------------------------------------------------------
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],[Turn on debugging flags (default=yes)]),
	,enable_debug=yes)
AC_MSG_CHECKING([if a debug friendly version should be build])
if test "x$enable_debug" = "xyes" ; then
	CFLAGS="$CFLAGS -ggdb -DDEBUG"
fi
AC_MSG_RESULT($enable_debug)

dnl -------------------------------------------------------------------
dnl Get the date for the man-page and substitute it there and anywhere.
dnl -------------------------------------------------------------------
MY_DATE=`date +%Y-%m-%d`

AC_SUBST(MY_DATE)

# Introspection
GOBJECT_INTROSPECTION_CHECK([0.9.3])

if test "$found_introspection" = "yes"; then
	enable_introspection=yes
	AC_DEFINE([ENABLE_INTROSPECTION], [1], [Define to enable GObject Introspection])
else
	enable_introspection=no
fi

dnl -------------------------------------------------------------------
dnl Put out the configured files.
dnl -------------------------------------------------------------------
AC_CONFIG_FILES([
Makefile
data/Makefile
data/gtranslator.pc
data/desktop/Makefile
data/desktop/icons/Makefile
data/org.gnome.gtranslator.gschema.xml
data/pixmaps/Makefile
doc/Makefile
doc/reference/Makefile
doc/reference/version.xml
doc/UMTF/Makefile
help/Makefile
man/Makefile
man/gtranslator.1
po/Makefile.in
plugins/Makefile
plugins/alternate-language/Makefile
plugins/charmap/Makefile
plugins/charmap/charmap/Makefile
plugins/dictionary/Makefile
plugins/dictionary/org.gnome.gtranslator.plugins.dictionary.gschema.xml
plugins/fullscreen/Makefile
plugins/insert-params/Makefile
plugins/open-tran/Makefile
plugins/open-tran/org.gnome.gtranslator.plugins.open-tran.gschema.xml
plugins/codeview/Makefile
plugins/codeview/org.gnome.gtranslator.plugins.codeview.gschema.xml
plugins/translation-memory/Makefile
plugins/translation-memory/gda/Makefile
plugins/translation-memory/org.gnome.gtranslator.plugins.translation-memory.gschema.xml
src/Makefile
src/toolbareditor/Makefile
])
AC_OUTPUT

dnl ------------------------------------------------------------------
dnl Print an information screen at the end of the process
dnl ------------------------------------------------------------------
echo "

------------------------------------------------------------------
-- gtranslator compilation options

gtranslator version	  :	${PACKAGE_VERSION}

debug friendly build	  :	${enable_debug:-no}
compilation flags	  :	${CFLAGS}
compilation warnings	  :	${WARN_CFLAGS}
GObject Introspection	  :	$enable_introspection
------------------------------------------------------------------
"

if [ test x"$have_json_glib" = "xyes" ]; then
	echo "Building open-tran plugin: .............................YES"
else
	echo "Building open-tran plugin: .............................NO"
		echo "        Requires libjson-glib >= $JSON_GLIB_OPTIONAL"
fi

if [ test x"$have_gdict" = "xyes" ]; then
	echo "Building dictionary plugin: ............................YES"
else
	echo "Building dictionary plugin: ............................NO"
		echo "        Requires gdict-1.0 >= $GDICT_OPTIONAL"
fi

if [ test x"$have_gtkspell" = "xyes" ]; then
	echo "Building spell support: ................................YES"
else
	echo "Building spell support: ................................NO"
		echo "        Requires gtkspell3 >= $GTKSPELL_OPTIONAL"
fi

echo "
------------------------------------------------------------------
-- For suggestions, problems & bug reports for gtranslator please
    use http://bugzilla.gnome.org/enter_bug.cgi?product=gtranslator


Thanks for your attention and braveness in compiling gtranslator!
-- The website for gtranslator is http://projects.gnome.org/gtranslator

------------------------------------------------------------------
"
