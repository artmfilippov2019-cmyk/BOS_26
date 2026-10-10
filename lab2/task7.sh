#!/bin/bash

EVEN=even.txt
ODD=odd.txt
: > "$EVEN"
: > "$ODD"

processed=0
skipped=0
even=0
odd=0

for n in "$@"; do
	if ! [[ $n =~ ^-?[0-9]+$ ]]; then
		echo "'$n' is not an integer" >&2
		continue
	fi

	processed=$((processed + 1))

	if (( n % 3 == 0 )); then
		skipped=$((skipped + 1))
		continue
	fi

	if (( n % 2 == 0 )); then
		echo "$n" >> "$EVEN"
		even=$((even + 1))
	else
		echo "$n" >> "$ODD"
		odd=$((odd + 1))
	fi
done

echo "Processed numbers: $processed (multiples of 3 skipped: $skipped, even: $even, odd: $odd)"
