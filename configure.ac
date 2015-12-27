AC_INIT(
    [fsevents-tools],
    [1.0.0],
    [https://github.com/ggreer/fsevents-tools/issues],
    [fsevents-tools],
    [http://geoff.greer.fm/fsevents/])

AC_PROG_CC
AC_PREREQ([2.59])

AM_INIT_AUTOMAKE([no-define foreign subdir-objects])

m4_ifdef(
    [AM_SILENT_RULES],
    [AM_SILENT_RULES([yes])])

# Run CFLAGS="-g" ./configure if you want debug symbols
CFLAGS="$CFLAGS -framework CoreFoundation -framework CoreServices -Wall -Wextra -Wformat=2 -Wno-format-nonliteral -Wshadow -Wpointer-arith -Wcast-qual -Wmissing-prototypes -Wno-missing-braces -std=c89 -D_GNU_SOURCE -O2"
LDFLAGS="$LDFLAGS"

AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([src/config.h])

AC_CHECK_PROGS(
    [CLANG_FORMAT],
    [clang-format-3.7 clang-format-3.6 clang-format],
    [no]
)
AM_CONDITIONAL([HAS_CLANG_FORMAT], [test x$CLANG_FORMAT != xno])
AM_COND_IF(
    [HAS_CLANG_FORMAT],
    [AC_MSG_NOTICE([clang-format found. 'make test' will detect improperly-formatted files.])],
    [AC_MSG_WARN([clang-format not found. 'make test' will not detect improperly-formatted files.])]
)

AC_OUTPUT
