#ifndef NOTIFYWAIT_H
#define NOTIFYWAIT_H

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

typedef struct {
    size_t len;
    size_t size;
    char **paths;
} file_paths_t;


void add_file(file_paths_t *file_paths, char *path);

void event_cb(ConstFSEventStreamRef streamRef,
              void *ctx,
              size_t count,
              void *paths,
              const FSEventStreamEventFlags flags[],
              const FSEventStreamEventId ids[]);

#endif
