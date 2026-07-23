# Question 4: Real-Time Log Monitoring Tool

## Objective
Design a command pipeline / tool that displays newly added log entries in 
real time, extracts ERROR messages, maintains a separate report file, and 
suppresses unnecessary output.

## Script
See `monitor.sh` in this folder.

## Steps, Commands, and Explanations

### Step 1: Create working folder

mkdir Q4
cd Q4

**Explanation:** Created a dedicated folder for Question 4.

### Step 2: Write the monitoring script

nano monitor.sh
chmod +x monitor.sh

**Explanation:** Wrote a script that pipes tail -f into grep and tee to 
monitor, filter, and log ERROR entries in real time.

### Step 3: Create an empty log file

touch system.log

**Explanation:** Created the log file that will be monitored; touch ensures 
tail -f has a valid file to follow even before any logs are written.

### Step 4: Start monitoring (Terminal A) and simulate logs (Terminal B)

./monitor.sh

echo "$(date) INFO Server started" >> system.log
echo "$(date) ERROR Disk space low" >> system.log
echo "$(date) INFO User login successful" >> system.log
echo "$(date) ERROR Database connection failed" >> system.log
echo "$(date) WARNING High memory usage" >> system.log

**Explanation:** Appended a mix of INFO, ERROR, and WARNING log lines to 
system.log from a second terminal while the monitor was running, to test 
real-time detection and filtering.

### Step 5: Observe real-time output
**Explanation:** The monitoring terminal displayed only the two ERROR lines 
the moment they were appended - INFO and WARNING lines were correctly 
filtered out by grep, and the display updated live without needing to 
re-run the command (proving tail -f's real-time following behavior).

### Step 6: Stop monitoring

Ctrl+C

**Explanation:** Terminated the continuous tail -f process.

### Step 7: Verify the report file

cat error_report.txt

**Explanation:** Confirmed the report file persistently stored only the 
ERROR entries, independent of what was shown live in the terminal.

### Step 8: Demonstrate fully silent (suppressed) logging

tail -n 5 system.log | grep --line-buffered "ERROR" | tee -a error_report.txt > /dev/null

**Explanation:** Redirected tee's terminal output to /dev/null so the ERROR 
line was still appended to error_report.txt, but nothing was printed to the 
screen - demonstrating full output suppression while still preserving the log.

## Explanation: How pipes, grep, tail, redirection, and /dev/null improve efficiency

- **tail -f**: Instead of repeatedly re-reading the entire log file (which 
  would be highly inefficient for large, constantly-growing log files), 
  tail -f uses the file's inode to watch for new data appended at the end 
  and streams only the new lines. This gives real-time monitoring with 
  minimal I/O and CPU overhead, since old content is never re-scanned.

- **Pipes (|)**: Connect the stdout of one command directly to the stdin of 
  the next, in memory, without creating intermediate temporary files. This 
  lets tail, grep, and tee work together as a single continuous stream-
  processing pipeline, processing each log line as it arrives rather than 
  waiting for the whole file.

- **grep --line-buffered "ERROR"**: Filters the stream so only lines 
  containing ERROR pass through. The --line-buffered flag is essential in a 
  pipeline context - grep normally buffers output in blocks when its output 
  isn't a terminal (e.g., when piped to tee), which would delay real-time 
  display; --line-buffered forces grep to flush its output after every 
  matching line so results appear immediately.

- **tee -a report_file**: Splits the stream into two destinations at once - 
  it prints to standard output AND appends (-a) to the report file 
  simultaneously, without needing to run the pipeline twice or manually 
  copy data. This is what allows "watch it live" and "keep a permanent 
  record" to happen from a single command.

- **Redirection to /dev/null**: /dev/null is a special "null device" that 
  discards anything written to it, at negligible cost. Redirecting tee's 
  terminal-facing output to /dev/null suppresses unnecessary console noise 
  (e.g., in automated/background monitoring or cron jobs) while still 
  preserving the actual, important output - the report file - since tee 
  writes to the file independently of what happens to its stdout copy.

Together, this pipeline is efficient because each stage does exactly one 
job and passes data forward as a stream: tail -f supplies only new data, 
grep filters it down to what matters, and tee/redirection controls where 
that filtered output goes (screen, file, or nowhere) - all without extra 
processes, temp files, or full-file re-reads, which is critical for a 
monitoring tool expected to run continuously against a live, growing log file.
