#! /bin/sh
VERSION=

if [ "x$1" != "x" ]; then
  VERSION=-$1
fi

autoheader \
&& aclocal$VERSION \
&& libtoolize \
&& automake$VERSION --add-missing \
&& autoconf
