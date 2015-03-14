dnl -------------------------------------------------------------------
dnl Process this file with autoconf to produce a configure script.
dnl -------------------------------------------------------------------
dnl (C) 2000-2004 Fatih Demir <kabalak@kabalak.net>
dnl -------------------------------------------------------------------
AC_PREREQ(2.64)
AC_INIT([gtranslator],[2.91.6],[http://bugzilla.gnome.org/enter_bug.cgi?product=gtranslator],[gtranslator],[http://projects.gnome.org/gtranslator/])

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
GLIB_REQUIRED=2.36.0
LIBXML_REQUIRED=2.4.12
SOURCEVIEW_REQUIRED=3.0.0

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
	gsettings-desktop-schemas
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
dnl Enable debug builds.
dnl -------------------------------------------------------------------
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],[Turn on debugging flags (default=yes)]),
	,enable_debug=yes)
if test "x$enable_debug" = "xyes" ; then
	CFLAGS="$CFLAGS -ggdb -DDEBUG"
else
	CFLAGS="`echo $CFLAGS|sed -e s/-g.//g -e s/-ggdb//g`"
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

echo "
------------------------------------------------------------------
-- For suggestions, problems & bug reports for gtranslator please
    use http://bugzilla.gnome.org/enter_bug.cgi?product=gtranslator


Thanks for your attention and braveness in compiling gtranslator!
-- The website for gtranslator is http://projects.gnome.org/gtranslator

------------------------------------------------------------------
"
