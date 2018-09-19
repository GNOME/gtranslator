translation_mem_inc = include_directories('.')

translation_mem_sources = files(
  'gda/gda-utils.c',
  'gda/gtr-gda.c',
  'gtr-translation-memory.c',
  'gtr-translation-memory-dialog.c',
  'gtr-translation-memory-ui.c',
  'gtr-translation-memory-utils.c',
)

resource_data = files('gtr-translation-memory-dialog.ui')

translation_mem_sources += gnome.compile_resources(
  'gtr-translation-memory-resources',
  'gtr-translation-memory.gresource.xml',
  dependencies: resource_data,
  export: true,
)

# GSettings schema
translation_mem_schema_conf = configuration_data()
translation_mem_schema_conf.set('GETTEXT_PACKAGE', meson.project_name())

schema = 'org.gnome.gtranslator.plugins.translation-memory.gschema.xml'

configure_file(
  input: schema + '.in',
  output: schema,
  configuration: translation_mem_schema_conf,
  install: true,
  install_dir: gtr_schemasdir,
)
