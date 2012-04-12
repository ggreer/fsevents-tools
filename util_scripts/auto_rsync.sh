#!/bin/sh

if [ $# -lt 2 ]
then
    echo "Usage: $0 path command"
    exit 1
fi

PATH=$1
shift
COMMAND=$@

while true
do
    notifywait $PATH
    echo "Running $COMMAND"
    $COMMAND
done
