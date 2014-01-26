#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>


typedef struct {
    size_t len;
    size_t size;
    char **paths;
} file_paths_t;


void event_cb(ConstFSEventStreamRef streamRef,
              void *cb_data,
              size_t count,
              void *paths,
              const FSEventStreamEventFlags flags[],
              const FSEventStreamEventId ids[]) {
    size_t i;
    file_paths_t* file_paths = cb_data;
    for (i = 0; i < count; i++) {
        char *path = ((char**)paths)[i];
        /* flags are unsigned long, IDs are uint64_t */
        printf("Change %llu in %s, flags %lu\n", ids[i], path, (long)flags[i]);
        size_t j;
        for (j = 0; j < file_paths->len; j++) {
            char *file_path = file_paths->paths[i];
            if (strncmp(file_path, path, strlen(file_path)) == 0) {
                printf("File %s changed.\n", file_path);
            } else {
                count--;
            }
        }
    }
    if (count > 0) {
        /* OS X occasionally leaks event streams. Manually stop the stream just to make sure. */
        FSEventStreamStop((FSEventStreamRef)streamRef);
        exit(0);
    }
}

char *dirname(const char *path) {
    char *d_name = strdup(path);
    int i;
    for (i = strlen(d_name); i > 0; i--) {
        if (d_name[i] == '/') {
            d_name[i] = '\0';
            break;
        }
    }
    return d_name;
};


void add_file(file_paths_t* file_paths, char *path) {
    if (file_paths->len == file_paths->size) {
        file_paths->size = file_paths->size * 1.5;
        file_paths->paths = realloc(file_paths->paths, file_paths->size);
    }
    file_paths->paths[file_paths->len] = path;
    file_paths->len++;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No path specified.\n");
        printf("Usage: %s path/to/watch\n", argv[0]);
        exit(1);
    }

    int i;
    int rv;
    char *path;
    struct stat s;
    CFMutableArrayRef paths = CFArrayCreateMutable(NULL, argc, NULL);
    CFStringRef cfs_path;

    file_paths_t file_paths;
    file_paths.len = 0;
    file_paths.size = 2;
    file_paths.paths = malloc(file_paths.size);

    for (i = 1; i < argc; i++) {
        path = argv[i];
        rv = stat(path, &s);
        if (rv < 0) {
          if (errno != 2) {
            fprintf(stderr, "Error %i stat()ing %s: %s\n", errno, path, strerror(errno));
            continue;
          }
          /* File doesn't exist. Watch parent dir instead. */
          add_file(&file_paths, dirname(path));
        }
        switch (s.st_mode) {
            case S_IFDIR:
            break;
            case S_IFREG:
            /* FSEvents can only watch directories, not files. Watch parent dir. */
            add_file(&file_paths, dirname(path));
            break;
            default:
                printf("Don't know what to do with %u\n", s.st_mode);
                continue;
        }
        cfs_path = CFStringCreateWithCString(NULL, path, kCFStringEncodingUTF8);
        CFArrayAppendValue(paths, cfs_path); /* pretty sure I'm leaking this */
    }

    void *cb_data = &file_paths;
    FSEventStreamRef stream;
    FSEventStreamCreateFlags flags = kFSEventStreamCreateFlagNone;

    stream = FSEventStreamCreate(NULL, &event_cb, cb_data, paths, kFSEventStreamEventIdSinceNow, 0.1, flags);
    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    FSEventStreamStart(stream);

    CFRunLoopRun();
    /* We never get here */

    return(0);
}
