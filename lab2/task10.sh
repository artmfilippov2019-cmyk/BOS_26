#!/bin/bash

format=txt
gen_sql=0

while getopts ":f:gh" opt; do
	case $opt in
		f) format=$OPTARG ;;
		g) gen_sql=1 ;;
		h) usage; exit 0 ;;
		:) echo "Error: option -$OPTARG requires an argument (txt or csv)" >&2; exit 1 ;;
		\?) echo "Error: unknown option"
	esac
done
shift $((OPTIND - 1))

if [ "$format" != "txt" ] && [ "$format" != "csv" ]; then
	echo "Error: unknown format '$format' (acceptable txt, csv)"
	exit 1
fi

if [ $# -ne 1 ]; then
	usage >&2
	exit 1
fi

file=$1
if [ ! -r "$file" ]; then
	echo "Error: the file '$file' was not found or is not readable" >&2
	exit 1
fi

stats=$(LC_ALL=C awk -F',' '
	{ sub(/\r$/, "") }
	NR == 1 && $1 !~ /^[0-9]+$/ { next }
	NF < 5 { next }
	{
		amount = $4 * $5
		total += amount
		n++
		if (n == 1 || count > max) {
			max = amount; mid = $1; mdate = $2; mprod = $3; mq = $4; mp = $5
		}
	}
	END {
		if (n == 0) exit 1
		printf "%d\t%.2f\t%.2f\t%s\t%s\t%s\t%s\t%.2f\t%.2f\n",
			n, total, total / n, mid, mdate, mprod, mq, mp, max
	}
' "$file")

if [ $? -ne 0 ] || [ -z "$stats" ]; then
	echo "Error: the file '$file' contains no sales data" >&2
	exit 1
fi

IFS=$'\t' read -r count total avg mid mdate mprod mq mp mamount <<< "$stats"

csv_quote() {
	local s=${1//\"/\"\"}
	printf '"%s"' "$s"
}

print_txt() {
	echo "Sales report (file: $file)"
	echo "1) Total sales amount: $total"
	echo "2) Average sale amount: $avg"
	echo "3) Most expensive sale: #$mid from #mdate, $mprod, $mq x $mp = $mamount"
}

print_csv() {
	echo "source_file,sales_count,total_sum,avg_sale,max_sale_id,max_sale_date,max_sale_product,max_sale_quantity,max_sale_price,max_sale_amount"
	echo "$(csv_quote "$file"),$count,$total,$avg,$mid,$mdate,$(csv_quote "$mprod"),$mq,$mp,$mamount"
}

if [ "$format" = "txt" ]; then
	print_txt | tee sales_report.txt
else
	print_csv | tee sales_report.csv
fi

if [ "$gen_sql" -eq 1 ]; then
	if [ "$format" != "csv" ]; then
		print_csv > sales_report.csv
	fi

	cat > sales_report.sql <<'SQL'
-- Import sales_report.csv into PostgreSQL:
--   psql -d <db_name> -f sales_report.sql

BEGIN;

CREATE TABLE IF NOT EXISTS sales_report (
	id		SERIAL PRIMARY KEY,
	source_file	TEXT,
	sales_count	INTEGER,
	total_sum	NUMERIC(14,2),
	avg_sale	NUMERIC(14,2),
	max_sale_id	INTEGER,
	max_sale_product TEXT,
	max_sale_quantity INTEGER,
	max_sale_price  NUMERIC(14,2),
	max_sale_amount  NUMERIC(14,2),
	created_at	TIMESTAMP DEFAULT now()
);

\copy sales_report (source_file, sales_count_ total_sum, avg_sale, max_sale_id, max_sale_date, max_sale_product, max_sale_quatity, max_sale_price, max_sale_amount) FROM 'sales_report.csv' WITH (FORMAT csv, HEADER true)

COMMIT;

SELECT * FROM sales_report ORDER BY id DESC LIMIT 5;
SQL
	echo "SQL import script saved to sales_report.sql" >&2
fi
