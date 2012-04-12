#!/bin/sh

if [ $# -lt 2 ]
then
    echo "Usage: $0 path command"
    exit 1
fi

WATCH_PATH=$1
shift
COMMAND=$@

echo "Watching $WATCH_PATH"

while true
do
    notifywait $WATCH_PATH
    echo "Running $COMMAND"
    $COMMAND
done
