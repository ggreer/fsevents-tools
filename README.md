# FSEvents Tools

Command-line tools and scripts that use OS X's [FSEvents](http://en.wikipedia.org/wiki/FSEvents) API.

## Usage examples

Alert if any files in a directory are changed.

    notifywait /path/; echo "\007"

Automatically rsync files to a remote server if any of them are changed.

    auto_rsync . 192.168.1.127:/var/www

Same as above, but don't copy .pyc files.

    notify_loop ~/code/directory rsync -avz --exclude="*.pyc" ~/code/directory/ server.example.com:/stuff/

## Related software

* [inotify-tools](https://github.com/rvoicilas/inotify-tools)
* [Lsyncd](https://github.com/axkibe/lsyncd)
