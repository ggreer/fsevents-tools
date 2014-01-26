notifywait(1) -- Wait for a file or directory to change.
=============================================

## SYNOPSIS

`notifywait` [<options>] PATH [PATH] [PATH] [PATH] ...


## DESCRIPTION

Monitors PATHS for changes. Paths can be files or directories.


## OPTIONS

None.


## CAVEATS

If path is a directory, it must exist when the `notifywait` is started. If path is a file, its parent directory must exist at start time. This is due to a limitation in FSEvents.
