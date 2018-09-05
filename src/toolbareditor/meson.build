marshallers = gnome.genmarshal('eggmarshalers',
    sources: 'eggmarshalers.list',
    prefix: '_egg_marshal',
    install_header: true,
    install_dir: get_option('includedir'),
    extra_args: ['-UG_ENABLE_DEBUG', '--prototypes'])

marshaller_c = marshallers[0]
marshaller_h = marshallers[1]

enum_sources = files(
	'egg-editable-toolbar.h',
	'egg-toolbars-model.h',
	'egg-toolbar-editor.h',
)

builtins = gnome.mkenums_simple('eggtypebuiltins', sources: enum_sources)

toolbar_sources = files(
	'egg-editable-toolbar.c',
	'egg-toolbars-model.c',
	'egg-toolbar-editor.c',
	'egg-editable-toolbar.h',
	'egg-toolbars-model.h',
	'egg-toolbar-editor.h',
)

built_sources = [
    marshaller_c,
    marshaller_h
] + builtins

libtoolbar = static_library('toolbareditor',
    toolbar_sources + built_sources,
   include_directories: top_inc,
    dependencies: [
        dependency('glib-2.0'),
        dependency('gtk+-3.0'),
        dependency('libxml-2.0'),
    ],
)

toolbareditor_dep = declare_dependency(
              sources: sources + built_sources,
            link_with: libtoolbar,
)
