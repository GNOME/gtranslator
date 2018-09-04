src_inc = include_directories('.')

gtr_marshal = gnome.genmarshal('gtr-marshal',
    sources: 'gtr-marshal.list',
    prefix: 'gtr_marshal',
    install_header: true,
    install_dir: get_option('includedir'),
    extra_args: ['-UG_ENABLE_DEBUG', '--prototypes'])

gtr_marshal_c = gtr_marshal[0]
gtr_marshal_h = gtr_marshal[1]

sources = files(
	'gtr-actions-app.c',
	'gtr-actions-documents.c',
	'gtr-actions-edit.c',
	'gtr-actions-file.c',
	'gtr-actions-go.c',
	'gtr-actions-search.c',
	'gtr-actions-view.c',
	'gtr-application.c',
	'gtr-assistant.c',
	'gtr-close-button.c',
	'gtr-close-confirmation-dialog.c',
	'gtr-context.c',
	'gtr-dirs.c',
	'gtr-file-dialogs.c',
	'gtr-header.c',
	'gtr-header-dialog.c',
	'gtr-history-entry.c',
	'gtr-io-error-info-bar.c',
	'gtr-jump-dialog.c',
	'gtr-language.c',
	'gtr-languages-fetcher.c',
	'gtr-message-container.c',
	'gtr-message-table.c',
	'gtr-message-table-model.c',
	'gtr-msg.c',
	'gtr-notebook.c',
	'gtr-po.c',
	'gtr-preferences-dialog.c',
	'gtr-profile.c',
	'gtr-profile-dialog.c',
	'gtr-profile-manager.c',
	'gtr-search-dialog.c',
	'gtr-settings.c',
	'gtr-statusbar.c',
	'gtr-status-combo-box.c',
	'gtr-tab.c',
	'gtr-tab-label.c',
	'gtr-utils.c',
	'gtr-view.c',
	'gtr-window.c'
)

sources += [gtr_marshal_c, gtr_marshal_h]

header_files = files(
	'gtr-application.h',
	'gtr-context.h',
	'gtr-debug.h',
	'gtr-header.h',
	'gtr-message-container.h',
	'gtr-message-table.h',
	'gtr-msg.h',
	'gtr-notebook.h',
	'gtr-po.h',
	'gtr-profile.h',
	'gtr-statusbar.h',
	'gtr-tab-activatable.h',
	'gtr-tab.h',
	'gtr-view.h',
	'gtr-window-activatable.h',
	'gtr-window.h',
)

enum_types = 'gtr-enum-types'
sources += gnome.mkenums(
  enum_types,
     sources: header_files,
  c_template: enum_types + '.c.template',
  h_template: enum_types + '.h.template'
)

resource_data = files(
    'gtranslator-ui.xml',
    'gtranslator-menu.ui',
    'gtr-context.ui',
    'gtr-header-dialog.ui',
    'gtr-jump-dialog.ui',
    'gtr-languages-fetcher.ui',
    'gtr-message-table.ui',
    'gtr-notebook.ui',
    'gtr-preferences-dialog.ui',
    'gtr-profile-dialog.ui',
    'gtr-search-dialog.ui',
    'gtr-statusbar.ui',
    'gtr-tab.ui',
    'gtr-window.ui'
)

gresource = files('gtranslator.gresource.xml')
sources += gnome.compile_resources(
  'gtr-resources', gresource,
        source_dir: src_dir,
            c_name: 'gtr_resources',
      dependencies: resource_data,
            export: true
)

#######################
# Include directories #
#######################

incs = [
  top_inc,
  include_directories('toolbareditor'),
]

subdir('toolbareditor')


###############
# gtranslator #
###############

executable(
  meson.project_name(),
   sources + files('main.c'),
   include_directories: incs,
          dependencies: gtranslator_deps + [toolbareditor_dep],
               install: true,
           install_dir: gtranslator_bindir
)
