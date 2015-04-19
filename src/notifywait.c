#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include "config.h"
#include "notifywait.h"


void add_file(file_paths_t *file_paths, char *path) {
    printf("adding %s length %lu size %lu\n", path, (unsigned long)file_paths->len, (unsigned long)file_paths->size);
    if (file_paths->len == file_paths->size) {
        file_paths->size = file_paths->size * 1.5;
        file_paths->paths = realloc(file_paths->paths, file_paths->size * sizeof(char *));
    }
    file_paths->paths[file_paths->len] = strdup(path);
    file_paths->len++;
}


void event_cb(ConstFSEventStreamRef streamRef,
              void *ctx,
              size_t count,
              void *paths,
              const FSEventStreamEventFlags flags[],
              const FSEventStreamEventId ids[]) {

    file_paths_t *file_paths = (file_paths_t *)ctx;
    size_t i;
    size_t ignored_paths = 0;

    for (i = 0; i < count; i++) {
        char *path = ((char **)paths)[i];
        /* flags are unsigned long, IDs are uint64_t */
        printf("Change %llu in %s, flags %lu\n", ids[i], path, (long)flags[i]);
        size_t j;
        for (j = 0; j < file_paths->len; j++) {
            char *file_path = file_paths->paths[j];
            printf("%s %s\n", file_path, path);
            if (strcmp(file_path, path) == 0) {
                printf("File %s changed.\n", file_path);
                FSEventStreamStop((FSEventStreamRef)streamRef);
                FSEventStreamScheduleWithRunLoop(streamRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
                exit(0);
            }
        }
        /* TODO: this logic is wrong */
        ignored_paths++;
    }
    if (count > ignored_paths) {
        /* OS X occasionally leaks event streams. Manually stop the stream just to make sure. */
        FSEventStreamStop((FSEventStreamRef)streamRef);
        FSEventStreamScheduleWithRunLoop(streamRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        exit(0);
    }
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No path specified.\n");
        printf("Usage: %s path/to/watch\n", argv[0]);
        exit(1);
    }

    CFMutableArrayRef paths = CFArrayCreateMutable(NULL, argc, NULL);
    CFStringRef cfs_path;
    char *dir_path;
    char *file_name;
    char *path;
    int i;
    int rv;
    struct stat s;

    file_paths_t *file_paths = malloc(sizeof(file_paths_t));
    file_paths->len = 0;
    file_paths->size = 2;
    file_paths->paths = malloc(file_paths->size * sizeof(char *));

    for (i = 1; i < argc; i++) {
        dir_path = NULL;
        path = realpath(argv[i], NULL);
        if (path == NULL) {
            path = dirname(argv[i]);
            if (strcmp(path, ".") == 0) {
                /* realpath(".") returns a useless path like /User */
                dir_path = realpath("./", NULL);
            } else {
                dir_path = realpath(path, NULL);
            }
            if (dir_path == NULL) {
                fprintf(stderr, "Error %i in realpath(\"%s\"): %s\n", errno, path, strerror(errno));
                exit(1);
            }
            file_name = basename(argv[i]);
            asprintf(&path, "%s/%s", dir_path, file_name);
        }

        /* At this point, path should be an absolute path. */
        printf("Path is %s\n", path);
        rv = stat(path, &s);
        if (rv < 0) {
            if (errno != 2) {
                fprintf(stderr, "Error %i stat()ing %s: %s\n", errno, path, strerror(errno));
                goto cleanup;
            }
            /* File doesn't exist yet. Watch parent dir instead. */
            s.st_mode = S_IFREG;
        }

        if (s.st_mode & S_IFREG) {
            /* FSEvents can only watch directories, not files. Watch parent dir. */
            dir_path = dirname(path);
            file_name = basename(path);
            asprintf(&path, "%s/%s", dir_path, file_name);
            add_file(file_paths, path);
        } else if (s.st_mode & S_IFDIR) {
            /* Yay a directory! */
            dir_path = path;
        } else {
            fprintf(stderr, "I don't know what to do with %u\n", s.st_mode);
            goto cleanup;
        }

        cfs_path = CFStringCreateWithCString(NULL, dir_path, kCFStringEncodingUTF8);
        printf("Watching %s\n", path);
        CFArrayAppendValue(paths, cfs_path); /* pretty sure I'm leaking this */

    cleanup:
        ;
        if (dir_path != path) {
            free(dir_path);
        }
        free(path);
    }

    FSEventStreamContext ctx = {
        0,
        file_paths,
        NULL,
        NULL,
        NULL
    };
    FSEventStreamRef stream;
    FSEventStreamCreateFlags flags = kFSEventStreamCreateFlagFileEvents;

    stream = FSEventStreamCreate(NULL, &event_cb, &ctx, paths, kFSEventStreamEventIdSinceNow, 0, flags);
    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    FSEventStreamStart(stream);

    CFRunLoopRun();
    /* We never get here */

    return (0);
}
