#!/bin/bash

DIR=$(dirname "$(readlink -f "$0")")

bash "$DIR/task1.sh"

less /tmp/run.log
