// file_processor.c
// Purpose: Secure file-processing utility using low-level Linux system calls
// (open, read, write, lseek, close) instead of stdio library functions.
// Uses fixed-size records so any record can be accessed directly by index.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define FILENAME "employees.dat"
#define NAME_LEN 30

// Fixed-size record so each record occupies exactly sizeof(Employee) bytes.
// This is what makes direct (random) access with lseek() possible -
// record N always starts at byte offset N * sizeof(Employee).
typedef struct {
    int emp_id;
    char name[NAME_LEN];
    float salary;
} Employee;

void write_records(int fd) {
    Employee employees[] = {
        {101, "Rohan Sharma", 45000.00},
        {102, "Manoj Kumar",  52000.00},
        {103, "Shyam Verma",  48000.00}
    };

    int n = sizeof(employees) / sizeof(Employee);

    for (int i = 0; i < n; i++) {
        ssize_t bytes_written = write(fd, &employees[i], sizeof(Employee));
        if (bytes_written != sizeof(Employee)) {
            perror("write failed");
            exit(EXIT_FAILURE);
        }
    }
    printf("[Info] %d employee records written.\n", n);
}

void update_record(int fd, int emp_id, float new_salary) {
    Employee emp;
    int index = 0;

    // Reset file offset to the beginning before scanning
    lseek(fd, 0, SEEK_SET);

    while (read(fd, &emp, sizeof(Employee)) == sizeof(Employee)) {
        if (emp.emp_id == emp_id) {
            emp.salary = new_salary;

            // Move offset back to the START of this record (not the current
            // position, which is now at the START of the NEXT record after read())
            lseek(fd, -sizeof(Employee), SEEK_CUR);

            // Overwrite only this record - not the whole file
            write(fd, &emp, sizeof(Employee));
            printf("[Info] Record for emp_id %d updated to salary %.2f\n",
                   emp_id, new_salary);
            return;
        }
        index++;
    }
    printf("[Warning] emp_id %d not found.\n", emp_id);
}

void retrieve_record(int fd, int index) {
    Employee emp;

    // Directly jump to the record's byte offset - O(1) access,
    // no need to read preceding records first
    off_t offset = lseek(fd, index * sizeof(Employee), SEEK_SET);
    if (offset == -1) {
        perror("lseek failed");
        return;
    }

    ssize_t bytes_read = read(fd, &emp, sizeof(Employee));
    if (bytes_read != sizeof(Employee)) {
        printf("[Warning] Record at index %d not found.\n", index);
        return;
    }

    printf("[Record %d] ID: %d | Name: %s | Salary: %.2f\n",
           index, emp.emp_id, emp.name, emp.salary);
}

int main() {
    // O_CREAT: create file if it doesn't exist
    // O_RDWR : allow both reading and writing (needed for update in place)
    // O_TRUNC: start fresh on each run for repeatable testing
    int fd = open(FILENAME, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }
    printf("[Info] File '%s' created/opened successfully.\n", FILENAME);

    write_records(fd);

    printf("\n[Info] Retrieving record at index 1 before update:\n");
    retrieve_record(fd, 1);

    printf("\n[Info] Updating salary for emp_id 102...\n");
    update_record(fd, 102, 60000.00);

    printf("\n[Info] Retrieving record at index 1 after update:\n");
    retrieve_record(fd, 1);

    printf("\n[Info] Retrieving record at index 2 (direct access, no scan):\n");
    retrieve_record(fd, 2);

    close(fd);
    printf("\n[Info] File closed. All operations complete.\n");

    return 0;
}
