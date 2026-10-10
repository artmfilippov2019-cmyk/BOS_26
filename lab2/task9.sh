#!/bin/bash
set -o pipefail

file=$1
REPORT=sales_report.txt

if [ ! -r "$file" ]; then
	echo "Error: the file '$file' was not found"
	exit 1
fi

LC_ALL=C awk -F',' -v src="$file" '
	{ sub(/\r$/, "") }
	NR == 1 && $1 !~ /^[0-9]+$/ {next}
	NF < 5 {next}
	{
		amount = $4 * $5
		total += amount
		n++
		if (n == 1 || amount > max) {
			max = amount; mid = $1; mdate = $2; mprod = $3; mq = $4; mp = $5
		}
	}
	END {
		if (n == 0) {print "Error: the file contains no sales data" > "/dev/stderr"; exit 1 }
		printf "Sales report (file: %s)\n", src
		printf "Numbers of sales: %d\n", n
		printf "1) Total sales amount: %.2f\n", total
		printf "2) Average sales price: %.2f\n", total / n
		printf "3) Most expensive sale: #%s from %s, %s x %.2f = %.2f\n",
				mid, mdate, mprod, mq, mp, max
	}
' "$file" | tee "$REPORT"
