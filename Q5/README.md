# Question 5: Evaluating vi Recovery Mechanisms After a Crash

## Objective
Evaluate the recovery mechanisms available in vi (swap files, undo history, 
registers, backup files, auto-recovery), and propose the most reliable 
recovery strategy for a crash occurring before a save.

## Steps, Commands, and Explanations

### Step 1: Create working folder

mkdir Q5
cd Q5

**Explanation:** Created a dedicated folder for Question 5.

### Step 2: Open a file in vi and add unsaved content

vi config.txt

**Explanation:** Entered insert mode and typed configuration lines without 
saving, to simulate a developer actively editing a critical file.

### Step 3: Identify and kill the vi process (simulating a crash)

ps aux | grep vi
kill -9 <PID>

**Explanation:** Force-killed the vi process the way a system crash would 
terminate it abruptly, without giving vi a chance to save or clean up normally.

### Step 4: Check for the swap file

ls -la

**Explanation:** Found a hidden .config.txt.swp file, which vi had been 
continuously updating in the background during editing - this file survived 
the crash since it exists independently on disk, not just in memory.

### Step 5: Recover the file using the swap file

vi -r config.txt

**Explanation:** vi detected the swap file and restored the buffer to its 
state at the time of the crash, recovering the unsaved edits.

### Step 6: Save recovered content and verify

:wq
cat config.txt

**Explanation:** Confirmed the recovered content matched what was typed 
before the crash, proving the swap file preserved unsaved work.

### Step 7: Confirm swap file cleanup after a clean save

ls -la

**Explanation:** The .swp file was automatically removed after a proper 
save and exit, confirming swap files exist only to protect against 
abnormal termination, not as permanent storage.

## Evaluation of vi's Recovery Mechanisms

### 1. Swap Files (.swp)
vi periodically writes the in-memory edit buffer to a hidden swap file 
(e.g., `.filename.swp`) in the same directory as the file being edited. 
This happens continuously during editing, independent of whether the user 
saves. If vi terminates abnormally (crash, killed process, power loss), the 
swap file remains on disk and can be used to reconstruct the buffer exactly 
as it was at the last swap-write, via `vi -r filename`. 
**Strength:** Survives full process/system crashes since it's persisted to 
disk continuously, not held only in memory. 
**Weakness:** If the disk itself is what fails, or the swap file is deleted/
inaccessible, recovery is impossible; also only recovers up to the last 
swap sync point, so a few keystrokes right before the crash could be lost.

### 2. Undo History
vi/Vim maintains an in-memory undo tree (`u` to undo, Ctrl+R to redo) during 
an editing session, and in modern Vim, an optional persistent undo file 
(`undofile` setting) that survives across sessions. 
**Strength:** Excellent for reverting unwanted *intentional* edits within a 
session or across sessions if persistent undo is enabled. 
**Weakness:** Standard undo history is memory-only and is lost entirely on 
a crash unless persistent undo was explicitly configured beforehand - it 
does not help recover from an abrupt process kill by default.

### 3. Registers
Registers store copied/deleted text (like a clipboard) for reuse within an 
editing session, and can be inspected with `:registers`. 
**Strength:** Useful for recovering recently deleted/yanked chunks of text 
during an active session. 
**Weakness:** Registers exist only in vi's process memory; once the process 
is killed, register contents are gone. They also don't represent the file's 
overall state, only recently manipulated text snippets - not a real recovery 
mechanism for a crash.

### 4. Backup Files
When configured (`set backup`, `set backupdir`), vi creates a copy of the 
file's *previous saved version* (e.g., `filename~`) before overwriting it 
on save. 
**Strength:** Protects against a *bad save* (e.g., accidentally overwriting 
good content with a mistaken edit) by preserving the last saved version. 
**Weakness:** Only captures the state as of the last successful save - by 
definition, it cannot help recover unsaved edits made after that save, 
which is exactly the scenario in this problem (crash before saving).

### 5. Auto-Recovery (vi -r)
This is the mechanism that actively uses the swap file to reconstruct the 
buffer, and is what a user is prompted to do the next time they try to edit 
a file for which a swap file already exists ("swap file already exists" 
warning). 
**Strength:** This is the direct, built-in recovery path vi provides 
specifically for the crash-before-save scenario. 
**Weakness:** Depends entirely on the swap file existing and not being 
corrupted; recovery quality is only as good as how recently the swap file 
was last synced to disk.

## Most Reliable Recovery Strategy

**The swap file, accessed via `vi -r`, is the most reliable recovery 
mechanism for this specific scenario** (a crash occurring before the file 
was saved), because:

1. It is the only mechanism among the five that is written to disk 
   *continuously during editing*, independent of any explicit save action 
   by the user - undo history and registers are memory-only and vanish on 
   a crash, while backup files only capture data from the *last save*, 
   which is not helpful since, in this scenario, no save had occurred yet.
2. It requires no prior special configuration (unlike persistent undo, 
   which must be explicitly enabled beforehand) - swap files are created by 
   default whenever vi opens a file.
3. It directly reconstructs the buffer state closest to the moment of the 
   crash, which is precisely what "recovering unsaved work" means.

**Recommended practice:** Combine swap-file recovery (the primary safety 
net for crash scenarios) with periodically enabling persistent undo 
(`set undofile`) and backups (`set backup`) as supplementary layers - swap 
files handle the crash-before-save case, while backups and persistent undo 
protect against *bad saves* and *unwanted edits* respectively. Regularly 
saving work (`:w`) frequently during long editing sessions also minimizes 
how much a swap file would even need to recover, reducing overall risk.
