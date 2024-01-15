#!/bin/bash


source_dir="raw_data"
destination_dir="data"

mkdir -p "$destination_dir"
for file in "$source_dir"/*; do
  if [ -f "$file" ]; then
    filename=$(basename "$file")
    output_file="$destination_dir/$filename"
    sed 's/  */ /g' "$file" > "$output_file"
    awk '{$1=$1};1' "$output_file" > "$output_file.tmp" && mv "$output_file.tmp" "$output_file"
  fi
done
exit 0