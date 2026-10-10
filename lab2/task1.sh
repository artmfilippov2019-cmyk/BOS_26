#!/bin/bash

LOG=/tmp/run.log

if [[ -f "$LOG" ]]; then
	prev=$(wc -l < "$LOG")
else
	prev=0
fi

date >> "$LOG"
echo "Hello, world!"
echo "$prev" >&2
