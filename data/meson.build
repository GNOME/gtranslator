subdir('icons')
subdir('pixmaps')

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
  desktop,
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

i18n.merge_file(
  appdata,
  input: appdata_in,
  output: appdata,
  po_dir: po_dir,
  install: true,
  install_dir: join_paths(gtr_datadir, 'metainfo'),
)

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

install_data(
  'gtr-plural-forms.ini',
  install_dir: join_paths(gtr_pkgdatadir, 'ui'),
)
