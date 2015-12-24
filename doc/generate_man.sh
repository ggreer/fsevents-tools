#!/bin/sh

# ronn is used to turn the markdown into a manpage.
# Get ronn at https://github.com/rtomayko/ronn or `gem install ronn`
ronn -r notifywait.1.md
