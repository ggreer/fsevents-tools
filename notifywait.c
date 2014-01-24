#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#define UNUSED(v) v __attribute__((__unused__))

void event_cb(ConstFSEventStreamRef UNUSED(streamRef),
              void UNUSED(*cb_data),
              size_t count,
              void *paths,
              const FSEventStreamEventFlags flags[],
              const FSEventStreamEventId ids[]) {
    size_t i;

    for (i = 0; i < count; i++) {
        /* flags are unsigned long, IDs are uint64_t */
        printf("Change %llu in %s, flags %lu\n", ids[i], ((char**)paths)[i], (long)flags[i]);
    }

    if (count > 0) {
        exit(1);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No path specified\n");
        printf("Usage: %s path/to/watch\n", argv[0]);
        exit(1);
    }

    CFMutableArrayRef paths = CFArrayCreateMutable(NULL, argc, NULL);
    int i;
    for(i = 1; i < argc; i++) {
        CFStringRef cfs_path = CFStringCreateWithCString(NULL, argv[i], kCFStringEncodingUTF8); /* pretty sure I'm leaking this */
        CFArrayAppendValue(paths, cfs_path); /* ditto */
    }

    void *cb_data = NULL;
    FSEventStreamRef stream;

    stream = FSEventStreamCreate(NULL, &event_cb, cb_data, paths, kFSEventStreamEventIdSinceNow, 0, kFSEventStreamCreateFlagNone);
    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    FSEventStreamStart(stream);

    CFRunLoopRun();
    /* We never get here */

    return(0);
}
