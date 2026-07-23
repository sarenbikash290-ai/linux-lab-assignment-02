#!/bin/bash
# dedupe_backup.sh
# Purpose: Detect duplicate assignment submissions, back up unique files,
# generate a summary report, and log errors separately.

SRC_DIR="./submissions"
BACKUP_DIR="./backup"
REPORT_FILE="./report.txt"
ERROR_LOG="./errors.log"
HASH_FILE="./hashes.tmp"

# Clear old logs/reports so re-runs don't append stale data
> "$REPORT_FILE"
> "$ERROR_LOG"
> "$HASH_FILE"

# Validate source directory exists
if [ ! -d "$SRC_DIR" ]; then
    echo "Error: Source directory $SRC_DIR does not exist." >> "$ERROR_LOG"
    exit 1
fi

mkdir -p "$BACKUP_DIR" 2>> "$ERROR_LOG"

total_files=0
duplicate_files=0
backed_up_files=0

# Loop through all files in submissions directory
for file in "$SRC_DIR"/*; do
    if [ -f "$file" ]; then
        total_files=$((total_files+1))

        # Compute MD5 hash of file content to detect duplicates
        hash=$(md5sum "$file" 2>>"$ERROR_LOG" | awk '{print $1}')

        if [ -z "$hash" ]; then
            echo "Error: Could not compute hash for $file" >> "$ERROR_LOG"
            continue
        fi

        # Check if this hash has already been seen
        if grep -q "^$hash$" "$HASH_FILE" 2>>"$ERROR_LOG"; then
            duplicate_files=$((duplicate_files+1))
        else
            echo "$hash" >> "$HASH_FILE"
            cp "$file" "$BACKUP_DIR"/ 2>>"$ERROR_LOG"
            if [ $? -eq 0 ]; then
                backed_up_files=$((backed_up_files+1))
            else
                echo "Error: Failed to back up $file" >> "$ERROR_LOG"
            fi
        fi
    fi
done

# Generate report
{
    echo "===== Submission Processing Report ====="
    echo "Date: $(date)"
    echo "Total files processed : $total_files"
    echo "Duplicate files found : $duplicate_files"
    echo "Files backed up       : $backed_up_files"
    echo "=========================================="
} > "$REPORT_FILE"

rm -f "$HASH_FILE"

echo "Processing complete. See $REPORT_FILE for summary and $ERROR_LOG for errors."
