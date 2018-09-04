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

builtins_c = gnome.mkenums('eggtypebuiltins.c',
        sources: enum_sources,
		fhead: '#include "eggtypebuiltins.h"\n\n',
		fprod: '\n/* enumerations from "@basename@" */' + 
               '\n#include "@basename@"',
		vhead: 'static const G@Type@Value _@enum_name@_values[] = {',
		vprod: '  { @VALUENAME@, "@VALUENAME@", "@valuenick@" },',
		vtail: '  { 0, NULL, NULL }\n};\n\n' +
		       'GType\n@enum_name@_get_type (void)\n{\n' +
		       '  static GType type = 0;\n\n' +
		       '  if (G_UNLIKELY (type == 0))\n' +
		       '    type = g_@type@_register_static ("@EnumName@", _@enum_name@_values);\n\n' +
		       '  return type;\n}\n\n',
)

builtins_h = gnome.mkenums('eggtypebuiltins.h',
        sources: enum_sources,
		fhead: '#ifndef __EGGTYPEBUILTINS_H__\n' +
		       '#define __EGGTYPEBUILTINS_H__ 1\n\n' +
		       '#include <glib-object.h>\n\n' +
		       'G_BEGIN_DECLS\n\n',
		ftail: 'G_END_DECLS\n\n' +
	           '#endif /* __EGGTYPEBUILTINS_H__ */\n',
		fprod: '\n/* --- @basename@ --- */',
		eprod: '#define EGG_TYPE_@ENUMSHORT@ @enum_name@_get_type()\n' +
		       'GType @enum_name@_get_type (void);\n',
)

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
    marshaller_h,
    builtins_c,
    builtins_h,
]

libtoolbar = static_library('toolbareditor',
    toolbar_sources + built_sources,
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
