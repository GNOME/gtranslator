doc_module = meson.project_name()

glib_prefix = glib_dep.get_pkgconfig_variable('prefix')
gtk_prefix = gtk_dep.get_pkgconfig_variable('prefix')

fixxref_args = [
  '--html-dir=' + join_paths(gtr_prefix, gnome.gtkdoc_html_dir(doc_module)),
  '--extra-dir=' + join_paths(glib_prefix, gnome.gtkdoc_html_dir('glib')),
  '--extra-dir=' + join_paths(glib_prefix, gnome.gtkdoc_html_dir('gio')),
  '--extra-dir=' + join_paths(gtk_prefix, gnome.gtkdoc_html_dir('gdk')),
  '--extra-dir=' + join_paths(gtk_prefix, gnome.gtkdoc_html_dir('gdk-pixbuf')),
  '--extra-dir=' + join_paths(gtk_prefix, gnome.gtkdoc_html_dir('gtk')),
]

html_images = files(
  'figures/ExtendedClassDiagram.png',
  'figures/SingleClassDiagram.png',
)

version_conf = configuration_data()
version_conf.set('VERSION', gtr_version)

version_xml = 'version.xml'

version = configure_file(
  input: version_xml + '.in',
  output: version_xml,
  configuration: version_conf,
)

gnome.gtkdoc(
  doc_module,
  main_xml: doc_module + '-docs.sgml',
  src_dir: src_inc,
  include_directories: top_inc,
  dependencies: libgtranslator_dep,
  fixxref_args: fixxref_args,
  gobject_typesfile: files(doc_module + '.types'),
  html_assets: html_images,
  content_files: version,
  install: true,
  install_dir: doc_module,
)
