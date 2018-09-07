memory_resource_data = files(
    'gtr-translation-memory-dialog.ui',
)

memory_resource = files('gtr-translation-memory.gresource.xml')

sources += gnome.compile_resources(
  'gtr-translation-memory-resources', memory_resource,
        source_dir: src_dir,
      dependencies: memory_resource_data,
            export: true
)

sources += files(
  'gtr-translation-memory.c',
  'gtr-translation-memory-dialog.c',
  'gtr-translation-memory-ui.c',
  'gtr-translation-memory-utils.c',
)

subdir('gda')

# GSettings schema
memory_schema_conf = configuration_data()
memory_schema_conf.set('GETTEXT_PACKAGE', meson.project_name())

memory_schema = 'org.gnome.gtranslator.plugins.translation-memory.gschema.xml'

schema_xml = configure_file(
  input: memory_schema + '.in',
  output: memory_schema,
  install: true,
  install_dir: gtranslator_schemadir,
  configuration: memory_schema_conf
)

