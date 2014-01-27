AC_INIT(
    [fsevents-tools],
    [0.2.0],
    [https://github.com/ggreer/fsevents-tools/issues],
    [fsevents-tools],
    [http://geoff.greer.fm/fsevents/])

AC_PROG_CC
AC_PREREQ([2.59])

AM_INIT_AUTOMAKE([no-define foreign subdir-objects])

m4_ifdef(
    [AM_SILENT_RULES],
    [AM_SILENT_RULES([yes])])

CFLAGS="$CFLAGS -framework CoreFoundation -framework CoreServices -Wall -Wextra -std=c89 -D_GNU_SOURCE"
LDFLAGS="$LDFLAGS"

AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([src/config.h])

AC_OUTPUT
