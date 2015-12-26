ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = notifywait
notifywait_SOURCES = src/notifywait.c src/notifywait.h
dist_bin_SCRIPTS = auto_rsync notify_loop

dist_man_MANS = doc/notifywait.1

EXTRA_DIST = LICENSE NOTICE README.md doc/notifywait.1

# Uncomment this line to output gprof profiling info.
#CFLAGS=-pg

test: notifywait
	cram -v tests/*.t
if HAS_CLANG_FORMAT
	CLANG_FORMAT=${CLANG_FORMAT} ./format.sh test
else
	@echo "clang-format is not available. Skipped clang-format test."
endif
