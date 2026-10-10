#!/bin/bash
DIR=${1:-.}

if [ ! -d "$DIR" ]; then
	echo "Error: '$DIR' is not a directory"
	exit 1
fi

find "$DIR" -type f -print0 \
	| xargs -0 -r md5sum \
	| sort \
	| awk '{
		hash = substr($0, 1, 32)
		name = substr($0, 35)
		cnt[hash]++
		files[hash] = files[hash] "\n  " name
	}
	END {
		for (h in cnt)
			if (cnt[h] > 1)
				print cnt[h] files[h]
	}'
