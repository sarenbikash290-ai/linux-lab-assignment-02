#!/bin/bash
# monitor.sh
# Purpose: Continuously monitor a log file in real time, extract ERROR
# messages, save them to a report file, and suppress unnecessary output.

LOG_FILE="system.log"
REPORT_FILE="error_report.txt"

# Ensure log file exists so tail -f doesn't fail
touch "$LOG_FILE"

echo "Monitoring $LOG_FILE for ERROR entries... (Press Ctrl+C to stop)"
echo "Errors will be logged to $REPORT_FILE"

# tail -f          : follow the file in real time as new lines are appended
# grep --line-buffered "ERROR" : filter only lines containing ERROR, 
#                    flushing output line-by-line instead of buffering
# tee -a            : print matched lines to terminal AND append them to
#                    the report file simultaneously
# > /dev/null       : discard tee's terminal copy so only intended messages
#                    reach the terminal (suppressing duplicate/unnecessary output)
tail -f "$LOG_FILE" | grep --line-buffered "ERROR" | tee -a "$REPORT_FILE"
