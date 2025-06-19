subdir('icons')

# Desktop files
desktop_conf = configuration_data()
desktop_conf.set('icon', gtr_app_id)

desktop = gtr_namespace + '.desktop'

desktop_in = configure_file(
  input: desktop + '.in.in',
  output: desktop + '.in',
  configuration: desktop_conf,
)

i18n.merge_file(
  type: 'desktop',
  input: desktop_in,
  output: '@0@.desktop'.format(gtr_app_id),
  po_dir: po_dir,
  install: true,
  install_dir: join_paths(gtr_datadir, 'applications'),
)

# AppStream application metainfo
appdata_conf = configuration_data()
appdata_conf.set('appid', gtr_app_id)

appdata = gtr_namespace + '.appdata.xml'

appdata_in = configure_file(
  input: appdata + '.in.in',
  output: appdata + '.in',
  configuration: appdata_conf,
)

appstream_file = i18n.merge_file(
  input: appdata_in,
  output: appdata,
  po_dir: po_dir,
  install: true,
  install_dir: join_paths(gtr_datadir, 'metainfo'),
)

appstreamcli = find_program('appstreamcli', required: false)
if appstreamcli.found()
  test('Validate appstream file', appstreamcli,
    args: ['validate', '--no-net', appstream_file]
  )
endif

# GSettings schema
schema_conf = configuration_data()
schema_conf.set('GETTEXT_PACKAGE', meson.project_name())

schema = gtr_namespace + '.gschema.xml'

configure_file(
  input: schema + '.in',
  output: schema,
  configuration: schema_conf,
  install: true,
  install_dir: gtr_schemasdir,
)

# D-Bus service file
service_conf = configuration_data()
service_conf.set('appid', gtr_app_id)
service_conf.set('bindir', gtr_bindir)

service = gtr_namespace + '.service'

configure_file(
  input: service + '.in',
  output: '@0@.service'.format(gtr_app_id),
  configuration: service_conf,
  install_dir: join_paths(gtr_datadir, 'dbus-1', 'services'),
)

install_data(
  'gtr-plural-forms.ini',
  'gtr-languages.ini',
  install_dir: join_paths(gtr_pkgdatadir, 'ui'),
)

install_data(
  'sourceview/gtranslator.lang',
  install_dir: join_paths(gtr_datadir, 'gtksourceview-5', 'language-specs'),
)
