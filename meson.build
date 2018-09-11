project(
  'gtranslator',
               'c',
           version: '3.30.0',
           license: 'GPL3+',
)

if get_option('profile') == 'development'
  profile = 'Devel'
  name_suffix = ' (Development)'
  vcs_tag = run_command('git', 'rev-parse', '--short', 'HEAD').stdout().strip()
  if vcs_tag == ''
    version_suffix = '-devel'
  else
    version_suffix = '-@0@'.format (vcs_tag)
  endif
else
  profile = ''
  name_suffix = ''
  version_suffix = ''
endif

application_id = 'org.gnome.Gtranslator@0@'.format(profile)

###########
# Version #
###########

gtranslator_version = meson.project_version()
version_array = gtranslator_version.split('.')
gtranslator_major_version = version_array[0].to_int()
gtranslator_minor_version = version_array[1].to_int()
gtranslator_micro_version = version_array[2].to_int()


#################
# Default paths #
#################

gtranslator_prefix = get_option('prefix')
gtranslator_bindir = join_paths(gtranslator_prefix, get_option('bindir'))
gtranslator_localedir = join_paths(gtranslator_prefix, get_option('localedir'))
gtranslator_datadir = join_paths(gtranslator_prefix, get_option('datadir'))
gtranslator_pkgdatadir = join_paths(gtranslator_datadir, meson.project_name())
gtranslator_schemadir = join_paths(gtranslator_datadir, 'glib-2.0', 'schemas')


###########
# Options #
###########

gtranslator_buildtype = get_option('buildtype')
gtranslator_debug = gtranslator_minor_version.is_odd() or gtranslator_buildtype.contains('debug')
enable_gtk_doc = get_option('documentation')

cc = meson.get_compiler('c')
po_dep = cc.find_library('gettextpo')
iso_codes_dep = dependency('iso-codes', version: '>=0.35')

config_h = configuration_data()
# package
package_bugreport = 'https://gitlab.gnome.org/GNOME/' + meson.project_name()
set_defines = [
  ['PACKAGE', meson.project_name()],
  ['PACKAGE_DATADIR', join_paths(get_option('prefix'), get_option('datadir'))],
  ['PACKAGE_LOCALEDIR', join_paths(get_option('prefix'), get_option('datadir'), 'locale')],
  ['PACKAGE_HELPDIR', join_paths(get_option('prefix'), get_option('datadir'), 'gnome', 'help')],
  ['PACKAGE_LIBDIR', join_paths(get_option('prefix'), get_option('libdir'))],
  ['PACKAGE_ISO_CODES_PREFIX', iso_codes_dep.get_pkgconfig_variable('prefix')],
  ['PACKAGE_BUGREPORT', package_bugreport],
  ['PACKAGE_NAME', meson.project_name()],
  ['PACKAGE_STRING', '@0@ @1@'.format(meson.project_name(), gtranslator_version)],
  ['PACKAGE_TARNAME', meson.project_name()],
  ['PACKAGE_URL', 'https://wiki.gnome.org/Apps/Gtranslator'],
  ['PACKAGE_VERSION', gtranslator_version],
  ['VERSION', gtranslator_version],
  ['GETTEXT_PACKAGE', meson.project_name()]
]

foreach define: set_defines
  config_h.set_quoted(define[0], define[1])
endforeach
configure_file(output: 'config.h', configuration: config_h)

assert(cc.has_function('strerror'), '"strerror" not found')

# Compiler flags
common_flags = [
  '-DHAVE_CONFIG_H',
  '-DPACKAGE_LOCALE_DIR="@0@"'.format(gtranslator_localedir),
  '-DPACKAGE_DATA_DIR="@0@"'.format(gtranslator_pkgdatadir),
  '-DUI_DATA_DIR="@0@"'.format(join_paths(gtranslator_datadir), 'style'),
]

if gtranslator_debug
  common_flags += [
    '-DG_DISABLE_CAST_CHECKS'
  ]
elif gtranslator_buildtype == 'release'
  common_flags += [
    '-DG_DISABLE_ASSERT',
    '-DG_DISABLE_CHECKS',
    '-DG_DISABLE_CAST_CHECKS'
  ]
endif

add_project_arguments(common_flags, language: 'c')


################
# Dependencies #
################

gtk_dep = dependency('gtk+-3.0', version: '>= 3.22.20')
glib_dep = dependency('glib-2.0', version: '>=2.36.0')
gda_dep = dependency('libgda-5.0')

gtranslator_deps = [
  iso_codes_dep,
  po_dep,
  gtk_dep,
  glib_dep,
  gda_dep,
  dependency('libxml-2.0', version: '>=2.4.12'),
  dependency('gthread-2.0', version: '>=2.13.0'),
  dependency('gio-2.0', version: '>=2.36.0'),
  dependency('gtksourceview-3.0', version: '>=3.0.0'),
  dependency('gsettings-desktop-schemas')
]

gnome = import('gnome')
i18n = import('i18n')
pkg = import('pkgconfig')

top_inc = include_directories('.')

data_dir = join_paths(meson.source_root(), 'data')
po_dir = join_paths(meson.source_root(), 'po')
src_dir = join_paths(meson.source_root(), 'src')


###########
# Subdirs #
###########

subdir('build-aux')
subdir('data')
subdir('src')
subdir('po')
subdir('help')

#subdir('man')

if enable_gtk_doc
  subdir('doc/reference')
endif

output = '\n\n      GTranslator ' + gtranslator_version + '\n'
output += '      =========================\n\n'
output += '          Source ..........................: ' + meson.source_root() + '\n'
output += '          Prefix ..........................: ' + gtranslator_prefix + '\n'
output += '          Compiler ........................: ' + cc.get_id() + '\n\n'
output += '      Development options\n'
output += '          Enable Debug: ...................: ' + gtranslator_debug.to_string() + '\n'
output += '          Enable Documentation: ...........: ' + enable_gtk_doc.to_string() + '\n\n'
output += '    Now type "ninja -C ' + meson.build_root() + '" to build ' + meson.project_name() + '\n\n'
message(output)
