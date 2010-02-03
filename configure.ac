dnl -------------------------------------------------------------------
dnl Process this file with autoconf to produce a configure script.
dnl -------------------------------------------------------------------
dnl (C) 2000-2004 Fatih Demir <kabalak@kabalak.net>
dnl -------------------------------------------------------------------
AC_PREREQ(2.52)
AC_INIT(gtranslator, 1.9.7,
	http://bugzilla.gnome.org/enter_bug.cgi?product=gtranslator)

AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR ([m4])

GTR_API_VERSION=2.0
AC_SUBST(GTR_API_VERSION)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_MAINTAINER_MODE
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_INSTALL

dnl -------------------------------------------------------------------
dnl Use intltools.
dnl -------------------------------------------------------------------
IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=gtranslator
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
AC_CHECK_FUNC(bind_textdomain_codeset,,[AC_CHECK_LIB(intl,bind_textdomain_codeset)])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])

dnl -------------------------------------------------------------------
dnl Standard bla-bla checks.
dnl -------------------------------------------------------------------
AC_PROG_CC
AC_ISC_POSIX
AM_PROG_LIBTOOL
AC_TYPE_SIGNAL
GNOME_DOC_INIT
GTK_DOC_CHECK([1.0])

dnl -------------------------------------------------------------------
dnl The standard checks for the compiler.
dnl -------------------------------------------------------------------
AC_HEADER_STDC

dnl -------------------------------------------------------------------
dnl The standard extended GNOME compile warning options ...
dnl -------------------------------------------------------------------
GNOME_COMPILE_WARNINGS(maximum)

AC_ARG_ENABLE(deprecations,
              [AC_HELP_STRING([--enable-deprecations],
                              [warn about deprecated usages [default=no]])],,
              [enable_deprecations=no])

if test "x$enable_deprecations" = "xyes"; then
   DISABLE_DEPRECATED_CFLAGS="\
-DG_DISABLE_DEPRECATED \
-DGDK_DISABLE_DEPRECATED \
-DGTK_DISABLE_DEPRECATED \
-DGDK_PIXBUF_DISABLE_DEPRECATED \
-DGNOME_DISABLE_DEPRECATED"
   AC_SUBST(DISABLE_DEPRECATED_CFLAGS)
fi

dnl ==========================================================================
dnl
dnl If you add a version number here, you *must* add an AC_SUBST line for
dnl it too, or it will never make it into the spec file!
dnl
dnl ==========================================================================

GTK_REQUIRED=2.18.0
GLIB_REQUIRED=2.15.5
GCONF_REQUIRED=2.18.0
LIBXML_REQUIRED=2.4.12
SOURCEVIEW_REQUIRED=2.4.0
GDL_REQUIRED=2.26.0
GDICT_OPTIONAL=0.11.0
GTKSPELL_OPTIONAL=2.0.2
LIBSOUP_OPTIONAL=2.4.0

AC_SUBST(GTK_REQUIRED)
AC_SUBST(GLIB_REQUIRED)
AC_SUBST(GCONF_REQUIRED)
AC_SUBST(LIBXML_REQUIRED)
AC_SUBST(SOURCEVIEW_REQUIRED)
AC_SUBST(GDL_REQUIDED)
AC_SUBST(GDICT_OPTIONAL)
AC_SUBST(GTKSPELL_OPTIONAL)

PKG_CHECK_MODULES(GTRANSLATOR, [
	gtk+-2.0 >= $GTK_REQUIRED
	gconf-2.0 >= $GCONF_REQUIRED
	gtksourceview-2.0 >= $SOURCEVIEW_REQUIRED
	gio-2.0 >= $GLIB_REQUIRED
	gdl-1.0 >= $GDL_REQUIRED
])

AC_SUBST(GTRANSLATOR_CFLAGS)
AC_SUBST(GTRANSLATOR_LIBS)

AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])
AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])

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


dnl -------------------------------------------------------------------
dnl Whether we use the broken, unsupported gtk+ functions
dnl -------------------------------------------------------------------
AC_ARG_ENABLE(gtk_enable_broken, [  --enable-gtk-broken     Use broken GTK functions ],[
	gtk_enable_broken=yes
	AC_DEFINE(GTK_ENABLE_BROKEN, [yes], [Define this to use broken GTK functions])
])

dnl ------
dnl GCONF
dnl ------
AM_GCONF_SOURCE_2
 	
AC_PATH_PROG([GCONFTOOL], [gconftool-2], [no])
 	
if test "$GCONFTOOL" = "no"; then
   AC_MSG_ERROR([gconftool-2 not found])
fi 

dnl -------------------------------------------------------------------
dnl Check for gettext >= 0.17.0 - uses it's parser/writer functions
dnl -------------------------------------------------------------------
AC_CHECK_HEADER([gettext-po.h])
AC_CHECK_LIB([gettextpo], [po_format_list], [
	LDFLAGS="$LDFLAGS -lgettextpo"
],[
	AC_MSG_ERROR(Missing or inadequate gettext library. Need at least version 0.17.0)
])

dnl -------------------------------------------------------------------
dnl Check for libdb >= 4.3
dnl -------------------------------------------------------------------
AX_BERKELEY_DB(4.3, [
	AC_DEFINE([HAVE_DB_BERKELEY], 1, [A usable libdb library was found])
	AC_DEFINE_UNQUOTED(DB_HEADER, ["$DB_HEADER"])
	LDFLAGS="$LDFLAGS $DB_LIBS"
],[
	AC_MSG_ERROR([Cannot find Berkeley DB >= 4.3])
])

dnl -------------------------------------------------------------------
dnl Check for gtkspell >= 2.0 and use it if found
dnl -------------------------------------------------------------------
have_gtkspell=no
AC_ARG_WITH(gtkspell,
            AC_HELP_STRING([--with-gtkspell],
                           [Build with spell support]),,
            with_gtkspell=auto)

if test x"$with_gtkspell" != "xno"
then
        PKG_CHECK_MODULES([GTKSPELL], [
                gtkspell-2.0 >= $GTKSPELL_OPTIONAL
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
dnl Check for libsoup, needed for Open-tran
dnl -------------------------------------------------------------------
if test "x$enable_opentran" != "xno"; then
	PKG_CHECK_MODULES(SOUP,                            \
		libsoup-2.4,
		have_libsoup=yes,
		have_libsoup=no)
	if test x"$have_libsoup" = "xno"; then
		PKG_CHECK_MODULES(SOUP,
			libsoup-2.4,
			have_libsoup=yes,
			have_libsoup=no)
		if test x"$have_libsoup" = "xyes"; then
			AC_DEFINE(LIBSOUP_2_4, 1, [Define if libsoup support is enabled])
		fi
	else
		AC_DEFINE(LIBSOUP_2_2, 1, [Define if libsoup support is enabled])
	fi
fi

AM_CONDITIONAL(USE_LIBSOUP, test x"$have_libsoup" = "xyes")

# ================================================================
# gdict
# ================================================================
have_gdict=no
AC_ARG_WITH(dictionary,
	    AC_HELP_STRING([--with-dictionary],
	    		   [Build with dictionary support]),,
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

# ================================================================
# gucharmap
# ================================================================
have_gucharmap=no
GUCHARMAP2_OPTIONAL=2.23.0
GUCHARMAP_OPTIONAL=1.6.0
PKG_CHECK_MODULES([GUCHARMAP],[gucharmap-2 >= $GUCHARMAP2_OPTIONAL],
                  [have_gucharmap=yes have_gucharmap_2=yes],[have_gucharmap_2=no])
if test "$have_gucharmap_2" = "no"; then
	PKG_CHECK_MODULES([GUCHARMAP], [gucharmap >= $GUCHARMAP_OPTIONAL],
        		[have_gucharmap=yes],[have_gucharmap=no])
fi

if test "$have_gucharmap_2" = "yes"; then
	AC_DEFINE([HAVE_GUCHARMAP_2],[1],[Define if gucharmap API is version 2])
fi

AM_CONDITIONAL(USE_CHARMAP, test x"$have_gucharmap" = "xyes")

dnl -------------------------------------------------------------------
dnl Enable debug builds.
dnl -------------------------------------------------------------------
AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug],[Turn on debugging flags (default=yes)]),
	,enable_debug=yes)
AC_MSG_CHECKING([if a debug friendly version should be build])
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

dnl -------------------------------------------------------------------
dnl Put out the configured files.
dnl -------------------------------------------------------------------
AC_OUTPUT([
Makefile
gtranslator.spec
data/Makefile
data/gtranslator.pc
data/desktop/Makefile
data/desktop/gtranslator.desktop.in
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
plugins/dictionary/Makefile
plugins/fullscreen/Makefile
plugins/insert-params/Makefile
plugins/insert-tags/Makefile
plugins/open-tran/Makefile
plugins/source-code-view/Makefile
src/Makefile
src/dialogs/Makefile
src/toolbareditor/Makefile
src/plugin-system/Makefile
src/translation-memory/Makefile
src/translation-memory/berkeley/Makefile
])

dnl ------------------------------------------------------------------
dnl Print an information screen at the end of the process
dnl ------------------------------------------------------------------
echo "

------------------------------------------------------------------
-- gtranslator compilation options

gtranslator version	  :	${VERSION}

debug friendly build	  :	${enable_debug:-no}
build with deprecation 	  :	${enable_deprecated:-no}
compilation flags      	  :	${CFLAGS}
------------------------------------------------------------------
"

if [ test x"$have_gucharmap" = "xyes" ]; then
	echo "Building charmap plugin: ...............................YES"
else
	echo "Building charmap plugin: ...............................NO"
		echo "        Requires gucharmap >= $GUCHARMAP_OPTIONAL"
fi

if [ test x"$have_libsoup" = "xyes" ]; then
	echo "Building open-tran plugin: .............................YES"
else
	echo "Building open-tran plugin: .............................NO"
		echo "        Requires libsoup >= $LIBSOUP_OPTIONAL"
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
		echo "        Requires gtkspell >= $GTKSPELL_OPTIONAL"
fi

echo "
------------------------------------------------------------------
-- For suggestions, problems & bug reports for gtranslator please
    use http://bugzilla.gnome.org/enter_bug.cgi?product=gtranslator


Thanks for your attention and braveness in compiling gtranslator!
-- The website for gtranslator is http://gtranslator.sourceforge.net

------------------------------------------------------------------
"
