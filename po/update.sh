#!/bin/sh

xgettext --default-domain=gtranslator --directory=.. \
  --add-comments --keyword=_ --keyword=N_ \
  --files-from=./POTFILES.in \
&& test ! -f gtranslator.po \
   || ( rm -f ./gtranslator.pot \
    && mv gtranslator.po ./gtranslator.pot )
