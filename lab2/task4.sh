#!/bin/bash

total_bytes=0
total_lines=0
n=0

echo "Files *.txt in $HOME":
while IFS= read -r -d '' f; do
	echo "  $f"
	bytes=$(stat -c %s "$f")
	lines=$(wc -l < "$f")
	total_bytes=$((total_bytes + bytes))
	total_lines=$((total_lines + lines))
	n=$((n + 1))
done < <(find "$HOME" -type f -name '*.txt' -print0 2>/dev/null)

echo "Total files: $n"
echo "Summary size: $total_bytes bytes"
echo "Summary lines: $total_lines"
