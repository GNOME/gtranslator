icon_sizes = [
  '16x16',
  '22x22',
  '24x24',
  '32x32',
  '48x48',
]

foreach icon_size: icon_sizes
  hicolor_dir = join_paths('hicolor', icon_size, 'apps')

  install_data(
    join_paths(hicolor_dir, gtr_namespace + '.png'),
    install_dir: join_paths(gtr_datadir, 'icons', hicolor_dir),
    rename: '@0@.png'.format(gtr_app_id),
  )
endforeach

hicolor_dir = join_paths('hicolor', 'scalable', 'apps')

install_data(
  join_paths(hicolor_dir, gtr_namespace + '-symbolic.svg'),
  install_dir: join_paths(gtr_datadir, 'icons', hicolor_dir),
  rename: '@0@-symbolic.svg'.format(gtr_app_id),
)
