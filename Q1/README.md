# Question 1: Duplicate Detection, Backup, and Reporting Script

## Objective
Design a shell script that identifies duplicate assignment submissions, 
backs up unique files, generates a summary report, and logs errors separately.

## Script
See `dedupe_backup.sh` in this folder.

## Steps, Commands, and Explanations

### Step 1: Create working folders

mkdir linux_lab_assingment_02
cd linux_lab_assingment_02
mkdir Q1
cd Q1

**Explanation:** Created a dedicated assignment folder and a subfolder for Question 1 to keep work organized.

### Step 2: Create the script

nano dedupe_backup.sh

**Explanation:** Wrote the shell script that performs deduplication, backup, and reporting.

### Step 3: Make the script executable

chmod +x dedupe_backup.sh

**Explanation:** Granted execute permission so the script can be run directly using `./`.

### Step 4: Create sample submission files

mkdir submissions
echo "Assignment 1 content" > submissions/rohan.txt
echo "Assignment 1 content" > submissions/manoj.txt
echo "Different content" > submissions/shyam.txt

**Explanation:** Created test files simulating student submissions — rohan.txt and 
manoj.txt have identical content (duplicate), shyam.txt is unique.

### Step 5: Verify test files

ls submissions/
cat submissions/*.txt

**Explanation:** Confirmed all three files were created with the intended content.

### Step 6: Run the script

./dedupe_backup.sh

**Explanation:** Executed the script. It scanned all files in submissions/, computed 
MD5 hashes for each to detect duplicates by content, backed up unique files, and logged the results.

### Step 7: View the report

cat report.txt

**Explanation:** The report confirmed 3 total files processed, 1 duplicate detected, 
and 2 files backed up — matching the test data setup.

### Step 8: View error log

cat errors.log

**Explanation:** The error log was empty, confirming the script ran without any 
file access or copy errors.

### Step 9: Verify backup folder

ls backup/

**Explanation:** Confirmed only manoj.txt and shyam.txt were copied to backup/. 
rohan.txt was correctly excluded since it was detected as a duplicate of manoj.txt.

## Justification of Commands and Techniques

- **md5sum**: used to compare file *content* rather than filenames, since two 
  files can have different names but identical content — this is the real test 
  of a "duplicate" submission.
- **grep -q**: silently checks whether a hash was already seen; used purely for 
  its exit status to detect duplicates without printing anything.
- **Redirection operators**:
  - `>` clears report/error files at the start of each run so old data doesn't linger.
  - `>>` appends new error messages without overwriting previous ones.
  - `2>>` redirects only stderr (errors) to errors.log, keeping error output 
    separate from normal (stdout) output, as required by the task.
- **cp with $? check**: verifies each backup copy succeeded before counting it, 
  logging a failure otherwise.
- **mkdir -p**: creates the backup directory only if it doesn't already exist, 
  making the script safe to re-run.
- **for loop**: iterates over every file in submissions/, letting the script 
  scale to hundreds of files without any changes to the code.
