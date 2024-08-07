# Gtranslator (-the GNOME translation making program-)

gtranslator is a quite comfortable gettext po/po.gz/(g)mo files editor for the
GNOME platform with many features. It's evolving quite fast and many useful
functions are already implemented; gtranslator aims to be a very complete
editing environment for translation issues within the GNU gettext/GNOME desktop
world.

You can check for the latest news and releases on the gtranslator Gitlab repository:

    https://gitlab.gnome.org/GNOME/gtranslator/

In the case you found a bug or wanted to give us a "wish-feature" of your own,
please file a report under the gtranslator product in GNOME gitlab at:

    https://gitlab.gnome.org/GNOME/gtranslator/issues/

If you have got any problems, need support, want to give any ideas, hints,
blame us, spam us or declare your love to gtranslator you can do this via the
GNOME support forum:

    https://discourse.gnome.org/c/applications/

We've also a Matrix channel, you can contact us directly in the #gtranslator:gnome.org channel:

    https://matrix.to/#/#gtranslator:gnome.org

Please don't hesitate to file bug reports on
https://gitlab.gnome.org/GNOME/gtranslator/issues/ or to share your comments,
ideas, suggestions with us on our mailing list - we do really, really, really
appreciate any kind of feedback and user comments very much in the
community/gtranslator field :-)

## How to build

If you get this code from the git repository the easier way to build is using
gnome-builder. There's a flatpak manifest that gnome-builder uses to build all
deps and launch the application sandboxed so you don't need to worry about
dependencies.

If you want to build by hand, you should follow the meson build commands:

```
meson _build
ninja -C _build
ninja -C _build install
```
