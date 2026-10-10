#!/bin/bash

usage() {
	cat <<EOF
Usage: $(basename "$0") [-h] [-v]  [-o file] -a|-s|-m|-d|-p number1 number2 [...]

Options:
-h show this help
-v detailed output (calculation process)
-o <file> write the result to a file
Operations (exactly one):
-a addition
-s subtraction
-m multiplication
-d division
-p exponentiation

The operation is applied to the numbers from left to right:
$(basename "$0") -s 10 3 2 -> (10 - 3) - 2 = 5
$(basename "$0") -p 2 3 2 -> (2 ^ 3) ^ 2 = 64
EOF
}

verbose=0
outfile=""
op=""

set_op() {
	if [ -n "$op" ]; then
		echo "Error: only one operation can be specified" >&2
		exit 1
	fi
	op=$1
}

while getopts ":hvo:asmdp" opt; do
	case $opt in
		h) usage; exit 0 ;;
		v) verbose=1 ;;
		o) outfile=$OPTARG ;;
		a) set_op "+" ;;
		s) set_op "-" ;;
		m) set_op "*" ;;
		d) set_op "/" ;;
		p) set_op "^" ;;
		:) echo "Error: option -$OPTARG requires an argument" >&2; exit 1 ;;
		\?) echo "Error: unknown option -$OPTARG" >&2; usage >&2; exit 1 ;;
	esac
done

shift $((OPTIND - 1))

if [ -z "$op" ]; then
	echo "no operation specified (-a, -s, -m, -d or -p)" >&2
	exit 1
fi

if [ $# -lt 2 ]; then
	echo "Error: at least two numbers are required"
	exit 1
fi

for n in "$@"; do
	if ! [[ $n =~ ^-?[0-9]+([.][0-9]+)?$ ]]; then
		echo "Error: '$n' is not a number"
		exit 1
	fi
done

log() { [ "$verbose" -eq 1 ] && echo "[v] $*" >&2; }

log "Operation: $op, numbers: $*"

calc() {
	LC_ALL=C awk -v a="$1" -v b="$2" -v op="$3" 'BEGIN {
		if (op == "+") r = a + b
		else if (op == "-") r = a - b
		else if (op == "*") r = a * b
		else if (op == "/") r = a / b
		else if (op == "^") r = a ^ b
		printf "%.10g\n", r
	}'
}

result=$1
shift
for n in "$@"; do
	if [ "$op" = "/" ] && LC_ALL=C awk -v x="$n" 'BEGIN {exit !(x == 0)}'; then
		echo "Error: division by zero" >&2
		exit 1
	fi
	new=$(calc "$result" "$n" "$op")
	log "$result $op $n = $new"
	result=$new
done

if [ -n "$outfile" ]; then
	echo "$result" > "$outfile"
	log "Result written to file $outfile"
	echo "Result: $result (saved to $outfile)"
else
	echo "Result: $result"
fi
