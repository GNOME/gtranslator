src_inc = include_directories('.')

subdir('translation-memory')
subdir('codeview')

gtr_build_ident_h = vcs_tag(
     fallback: meson.project_version(),
        input: 'gtr-build-ident.h.in',
       output: 'gtr-build-ident.h',
)

enum_headers = files(
  'gtr-application.h',
  'gtr-context.h',
  'gtr-header.h',
  'gtr-message-container.h',
  'gtr-message-table.h',
  'gtr-message-table-row.h',
  'gtr-msg.h',
  'gtr-po.h',
  'gtr-profile.h',
  'gtr-tab.h',
  'gtr-view.h',
  'gtr-window.h',
  'gtr-search-bar.h',
)

sources = files(
  'gtr-actions-app.c',
  'gtr-actions-edit.c',
  'gtr-actions-file.c',
  'gtr-actions-go.c',
  'gtr-actions-search.c',
  'gtr-application.c',
  'gtr-close-confirmation-dialog.c',
  'gtr-context.c',
  'gtr-dirs.c',
  'gtr-file-dialogs.c',
  'gtr-header.c',
  'gtr-header-dialog.c',
  'gtr-io-error-info-bar.c',
  'gtr-language.c',
  'gtr-languages-fetcher.c',
  'gtr-message-container.c',
  'gtr-message-table.c',
  'gtr-message-table-row.c',
  'gtr-msg.c',
  'gtr-po.c',
  'gtr-preferences-dialog.c',
  'gtr-profile.c',
  'gtr-profile-dialog.c',
  'gtr-profile-manager.c',
  'gtr-settings.c',
  'gtr-tab.c',
  'gtr-utils.c',
  'gtr-view.c',
  'gtr-projects.c',
  'gtr-dl-teams.c',
  'gtr-drop-down-option.c',
  'gtr-lang-combo-row.c',
  'gtr-progress.c',
  'gtr-window.c',
  'gtr-search-bar.c',
  'gtr-upload-dialog.c',
  'gtr-greeter.c',
)

marshal = 'gtr-marshal'

sources += gnome.genmarshal(
  marshal,
  sources: marshal + '.list',
  prefix: marshal.underscorify(),
  internal: true,
)

enum_types = 'gtr-enum-types'

sources += gnome.mkenums(
  enum_types,
  sources: enum_headers,
  c_template: enum_types + '.c.template',
  h_template: enum_types + '.h.template',
)

resource_data = files(
  'gtr-context.ui',
  'gtr-header-dialog.ui',
  'gtr-languages-fetcher.ui',
  'gtr-message-table.ui',
  'gtr-message-table-row.ui',
  'gtr-preferences-dialog.ui',
  'gtr-profile-dialog.ui',
  'gtr-tab.ui',
  'gtr-window.ui',
  'gtr-search-bar.ui',
  'gtr-upload-dialog.ui',
)

sources += gnome.compile_resources(
  'gtr-resources',
  'gtranslator.gresource.xml',
  c_name: 'gtr_resources',
  dependencies: resource_data,
  export: true,
)

#######################
# Include directories #
#######################

incs = [
  top_inc,
  translation_mem_inc,
  codeview_inc,
]

c_args = [
  '-DG_LOG_DOMAIN="@0@"'.format(gtr_app_id),
]

libgtranslator = shared_library(
  meson.project_name(),
  sources: sources + translation_mem_sources + codeview_sources + [gtr_build_ident_h],
  include_directories: incs,
  dependencies: gtr_deps,
  c_args: c_args,
)

libgtranslator_dep = declare_dependency(
  include_directories: src_inc,
  link_with: libgtranslator,
)

###############
# gtranslator #
###############

executable(
  meson.project_name(),
  'main.c',
  c_args: c_args,
  include_directories: top_inc,
  dependencies: gtr_deps,
  objects: libgtranslator.extract_all_objects(recursive: true),
  install: true,
)
