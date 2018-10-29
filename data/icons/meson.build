scalable_dir = join_paths('hicolor', 'scalable', 'apps')

install_data(
  join_paths(scalable_dir, gtr_namespace + '.svg'),
  install_dir: join_paths(gtr_datadir, 'icons', scalable_dir),
  rename: '@0@.svg'.format(gtr_app_id),
)

symbolic_dir = join_paths('hicolor', 'symbolic', 'apps')

install_data(
  join_paths(symbolic_dir, gtr_namespace + '-symbolic.svg'),
  install_dir: join_paths(gtr_datadir, 'icons', symbolic_dir),
  rename: '@0@-symbolic.svg'.format(gtr_app_id),
)
