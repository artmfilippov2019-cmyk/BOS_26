#!/bin/bash
LOG=./trap.log
trap 'echo "$(date +%T) [$$] SIGINT caught - continuing" >> "$LOG"' INT
trap 'echo "$(date +%T) [$$] SIGTERM caught - exiting" >> "$LOG"; exit 143' TERM
trap 'echo "$(date +%T) [$$] EXIT: terminating, code=$?" >> "$LOG"' EXIT

echo "Script PID: $$, log: $LOG"
while true; do
	echo "$(date +%T) [$$] working" >> "$LOG"
	sleep 1 &
	wait $!
done
