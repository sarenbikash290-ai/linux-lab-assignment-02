# Question 3: Secure File Processing Using Linux System Calls

## Objective
Design a program using low-level Linux system calls (open, read, write, 
lseek, close) instead of standard library functions to create a file, 
write employee records, update specific records without rewriting the 
entire file, and retrieve records efficiently from any location.

## Program
See `file_processor.c` in this folder.

## Steps, Commands, and Explanations

### Step 1: Create working folder

mkdir Q3
cd Q3

**Explanation:** Created a dedicated folder for Question 3.

### Step 2: Write the C program

nano file_processor.c

**Explanation:** Implemented a program that uses fixed-size Employee records 
so any record's byte offset can be calculated directly (index * record size), 
enabling random access via lseek() instead of sequential reads.

### Step 3: Compile the program

gcc file_processor.c -o file_processor

**Explanation:** Compiled successfully with no errors, confirming correct use 
of the fcntl.h and unistd.h system call headers.

### Step 4: Run the program

./file_processor

**Explanation:** The program created employees.dat, wrote 3 fixed-size 
records, retrieved a record by index, updated one record's salary in place, 
and retrieved records again to confirm the update - all without rewriting 
the whole file.

### Step 5: Verify the file

ls -la employees.dat

**Explanation:** Confirmed the file exists with a size that is an exact 
multiple of the record size, proving records are stored as fixed-size 
binary blocks rather than variable-length text lines.

### Step 6: Inspect raw file bytes

xxd employees.dat | head -20

**Explanation:** Displayed the raw binary content of the file, confirming 
data is stored directly as struct bytes on disk (not as formatted text), 
consistent with using low-level write() rather than fprintf().

## Explanation: How open(), read(), write(), lseek(), and close() work together

- **open()**: Establishes a file descriptor - a low-level handle the OS uses 
  to track the file's state, including its current read/write offset. Flags 
  like O_CREAT, O_RDWR, and O_TRUNC control creation behavior and access mode 
  directly at the kernel level, unlike fopen()'s mode strings.

- **write()**: Writes raw bytes directly from a buffer (here, an Employee 
  struct) into the file at the current file offset, and automatically 
  advances the offset by the number of bytes written. Because every record 
  is the same fixed size, records are naturally laid out at predictable, 
  evenly spaced byte offsets in the file.

- **lseek()**: This is the key to efficient random access. Since all records 
  are fixed size, the byte offset of record N is always N * sizeof(Employee). 
  lseek(fd, offset, SEEK_SET) moves the file's internal position pointer 
  directly to that byte, in O(1) time, without needing to read through 
  preceding records. This is also what enables *updating a single record*: 
  after reading a record (which advances the offset past it), lseek() with 
  SEEK_CUR and a negative offset moves back to the start of that same record 
  so write() can overwrite just those bytes - the rest of the file is 
  untouched.

- **read()**: Reads raw bytes from the current file offset into a buffer, 
  also advancing the offset. Used both to scan for a matching emp_id during 
  update, and to fetch a record's contents during retrieval.

- **close()**: Releases the file descriptor back to the OS, flushing any 
  pending changes and freeing the resource. Important in a "secure utility" 
  context, since leaving file descriptors open unnecessarily can leak 
  resources or leave a file in an inconsistent state under concurrent access.

Together, these calls allow the program to treat the file as a direct-access 
array of records on disk: open() gets access, write() lays down fixed-size 
records, lseek() jumps to any record's exact position instantly, read() 
retrieves it, and a targeted lseek()+write() pair updates a record in place 
without touching or rewriting any other part of the file - which is exactly 
the efficient, low-level control that standard library functions like 
fopen()/fprintf() (which buffer and abstract away offsets) don't give as 
directly.
