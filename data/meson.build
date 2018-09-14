subdir('icons')
subdir('pixmaps')

# Desktop files
desktop = 'org.gnome.Gtranslator.desktop'
appdata = 'org.gnome.Gtranslator.appdata.xml'

i18n.merge_file(
      desktop,
         type: 'desktop',
        input: desktop + '.in',
       output: desktop,
       po_dir: po_dir,
      install: true,
  install_dir: join_paths(gtranslator_datadir, 'applications')
)

i18n.merge_file(
      appdata,
         type: 'xml',
        input: appdata + '.in',
       output: appdata,
       po_dir: po_dir,
      install: true,
  install_dir: join_paths(gtranslator_datadir, 'metainfo')
)

# GSettings schema
schema_conf = configuration_data()
schema_conf.set('GETTEXT_PACKAGE', meson.project_name())

schema = 'org.gnome.Gtranslator.gschema.xml'

schema_xml = configure_file(
  input: schema + '.in',
  output: schema,
  install: true,
  install_dir: gtranslator_schemadir,
  configuration: schema_conf
)

output = 'gschemas.compiled'

# for unit tests - gnome.compile_schemas() only looks in srcdir
compiled_schemas = custom_target(
   output,
    input: schema_xml,
   output: output,
  command: [find_program('glib-compile-schemas'), meson.current_build_dir()]
)

install_data ('gtr-plural-forms.ini', install_dir: join_paths (gtranslator_pkgdatadir, 'ui'))

