codeview_inc = include_directories('.')

codeview_sources = files(
  'gtr-codeview.c',
)

codeview_sources += gnome.compile_resources(
  'gtr-codeview-resources',
  'gtr-codeview.gresource.xml',
  dependencies: resource_data,
  export: true,
)
