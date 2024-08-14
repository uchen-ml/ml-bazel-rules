#!/usr/bin/bash
# Check if the number of arguments is correct
if [ $# -lt 3 ]; then
    echo "Usage: $0 <source_file> <output_file> <md5_checksum>"
    exit 1
fi

# Check MD5 checksum if provided
echo "$3 $1" | md5sum --status -c
if [ $? -ne 0 ]; then
    actual_md5=$(md5sum $1 | cut -d ' ' -f 1)
    echo "MD5 checksum verification failed. Actual MD5: $actual_md5" >&2
    exit 1
fi

echo "MD5 checksum verified"
cp $1 $2
