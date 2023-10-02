project(
  'gtranslator', 'c',
  version: '45.2',
  license: 'GPL3+',
  meson_version: '>= 0.57.0',
)

###########
# Version #
###########

gtr_version = meson.project_version()
version_array = gtr_version.split('.')
gtr_major_version = version_array[0].to_int()
gtr_minor_version = version_array[1]

#################
# Default paths #
#################

gtr_prefix = get_option('prefix')
gtr_datadir = join_paths(gtr_prefix, get_option('datadir'))
gtr_includedir = join_paths(gtr_prefix, get_option('includedir'))
gtr_libdir = join_paths(gtr_prefix, get_option('libdir'))
gtr_localedir = join_paths(gtr_prefix, get_option('localedir'))

gtr_pkgdatadir = join_paths(gtr_datadir, meson.project_name())

###########
# Options #
###########

gtr_namespace = 'org.gnome.Gtranslator'

gtr_buildtype = get_option('buildtype')
gtr_debug = gtr_buildtype.contains('debug')

if gtr_debug
  profile = 'Devel'
  name_suffix = ' (Development)'
  vcs_tag = run_command(find_program('git'), 'rev-parse', '--short', 'HEAD', check: false).stdout().strip()
  version_suffix = '-' + (vcs_tag == '' ? 'devel' : vcs_tag)
else
  profile = ''
  name_suffix = ''
  version_suffix = ''
endif

gtr_app_id = gtr_namespace + profile

cc = meson.get_compiler('c')

config_h = configuration_data()

set_defines = [
  # package
  ['PACKAGE', meson.project_name()],
  ['PACKAGE_DATADIR', gtr_datadir],
  ['PACKAGE_LIBDIR', gtr_libdir],
  ['PACKAGE_LOCALEDIR', gtr_localedir],
  ['PACKAGE_URL', 'https://wiki.gnome.org/Apps/Gtranslator'],
  ['PACKAGE_ISSUES_URL', 'https://gitlab.gnome.org/GNOME/gtranslator/-/issues'],
  ['PACKAGE_VERSION', gtr_version],
  ['PACKAGE_APPID', gtr_app_id],
  ['PROFILE', get_option('profile')],
  # i18
  ['GETTEXT_PACKAGE', meson.project_name()],
  ['DL_SERVER', get_option('dl_server')],
  ['API_URL', get_option('dl_server') + '/api/v1/'],
]

foreach define: set_defines
  config_h.set_quoted(define[0], define[1])
endforeach

assert(cc.has_function('strerror'), '"strerror" not found')

# Compiler flags
common_flags = ['-DHAVE_CONFIG_H']

if gtr_debug
  common_flags += ['-DG_DISABLE_CAST_CHECKS']
elif gtr_buildtype == 'release'
  common_flags += [
    '-DG_DISABLE_ASSERT',
    #'-DG_DISABLE_CHECKS',
    '-DG_DISABLE_CAST_CHECKS',
  ]
endif

add_project_arguments(common_flags, language: 'c')

gnome = import('gnome')
i18n = import('i18n')
pkg = import('pkgconfig')

top_inc = include_directories('.')

data_dir = join_paths(meson.project_source_root(), 'data')
po_dir = join_paths(meson.project_source_root(), 'po')
src_dir = join_paths(meson.project_source_root(), 'src')

################
# Dependencies #
################

glib_dep = dependency('glib-2.0', version: '>= 2.71.3')
gtk_dep = dependency('gtk4', version: '>= 4.6.0')
libadwaita_dep = dependency('libadwaita-1', version: '>= 1.1.0')
libspell_dep = dependency('libspelling-1', required: false)

# if not libspell, we disable it
if libspell_dep.found()
  config_h.set('LIBSPELL', 1)
endif

gtr_deps = [
  glib_dep,
  gtk_dep,
  libadwaita_dep,
  libspell_dep,
  dependency('libgda-6.0'),
  dependency('gio-2.0', version: '>= 2.36.0'),
  dependency('gsettings-desktop-schemas'),
  dependency('gthread-2.0', version: '>= 2.13.0'),
  dependency('gtksourceview-5', version: '>= 5.4.0'),
  dependency('libxml-2.0', version: '>= 2.4.12'),
  dependency('libsoup-3.0'),
  dependency('json-glib-1.0', version: '>= 1.2.0'),
  cc.find_library('gettextpo'),
]

gtr_schemasdir = dependency('gio-2.0').get_variable(pkgconfig: 'schemasdir', pkgconfig_define: ['datadir', gtr_datadir])
if gtr_schemasdir == ''
  gtr_schemasdir = join_paths(gtr_datadir, 'glib-2.0', 'schemas')
  message(gtr_schemasdir)
endif

itstool = find_program  ('itstool')

###########
# Subdirs #
###########

subdir('data')
subdir('src')
subdir('po')
subdir('help')
subdir('man')


enable_gtk_doc = get_option('gtk_doc')
if enable_gtk_doc
  gtk_doc_dep = dependency('gtk-doc', version: '>= 1.28')
  gtr_deps += gtk_doc_dep
  subdir('doc/reference')
endif

configure_file(
  output: 'config.h',
  configuration: config_h,
)

gnome.post_install(
  glib_compile_schemas: true,
  gtk_update_icon_cache: true,
)

output = '\n\n      GTranslator ' + gtr_version + '\n'
output += '      =========================\n\n'
output += '          Source ..........................: ' + meson.current_source_dir() + '\n'
output += '          Prefix ..........................: ' + gtr_prefix + '\n'
output += '          Compiler ........................: ' + cc.get_id() + '\n'
if libspell_dep.found()
output += '          libspell ........................: enabled\n\n'
else
output += '          libspell ........................: disabled\n\n'
endif
output += '      Development options\n'
output += '          Enable Debug: ...................: ' + gtr_debug.to_string() + '\n'
output += '          Enable Documentation: ...........: ' + enable_gtk_doc.to_string() + '\n\n'
output += '    Now type "ninja -C ' + meson.current_build_dir() + '" to build ' + meson.project_name() + '\n\n'
message(output)
