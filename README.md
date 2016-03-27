# FSEvents Tools

[![Build Status](https://travis-ci.org/ggreer/fsevents-tools.svg?branch=master)](https://travis-ci.org/ggreer/fsevents-tools)

[![Floobits Status](https://floobits.com/ggreer/fsevents-tools.svg)](https://floobits.com/ggreer/fsevents-tools/redirect)

## Description

Command-line tools and scripts that use OS X's [FSEvents](http://en.wikipedia.org/wiki/FSEvents) API. Mostly useful for watching a directory and reacting to changes in it.

[This post](http://geoff.greer.fm/2015/12/25/fsevents-tools-watch-a-directory-for-changes/) explains why I made this and includes more usage examples.


## Usage examples

Alert if any files in a directory are changed.

    notifywait /path/; echo "\007"


Rebuild LESS when anything in `styles` changes. Assuming you have a script similar to this `rebuild_less.sh`:

    #!/bin/bash
    for less_file in styles/*.less
    do
      css_file=`echo $less_file | sed -E "s/.less//"`
      lessc $less_file $css_file.css
    done

...just run:

    notifyloop styles ./rebuild_less.sh


Automatically rsync files to a remote server if any of them are changed.

    autorsync . 192.168.1.127:/var/www

The above invokes `rsync` with `-avz` (archive mode, verbose, compress). To 
pass additional arguments to `rsync`, for example:

    RSYNC_OPTS='--exclude="*.pyc"' autorsync . 192.168.1.127:/var/www

To do the same thing "manually":

    notifyloop . rsync -avz --exclude="*.pyc" . 192.168.1.127:/var/www


## Building from source

### Building master

In addition to the standard automake/autoconf/make, you'll need pkg-config. `brew install pkg-config` should do the trick.

Once you have all the dependencies, run `./autogen.sh` and `make install`.


### Building a release tarball

GPG-signed releases are available [here](http://geoff.greer.fm/fsevents).

Building release tarballs requires the same dependencies, except for automake and pkg-config. Once you've installed the dependencies, run:

    ./configure
    make
    make install

Depending on permissions, `make install` may require `sudo`.


## Related software

* [inotify-tools](https://github.com/rvoicilas/inotify-tools). The original tools that inspired me to make fsevents-tools.
* [Lsyncd](https://github.com/axkibe/lsyncd). A service to keep files synced between a master and one or more slave servers.
