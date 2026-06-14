/*
 * main.c  –  Mini Student Shell
 *
 * TODO: Implement admin_shell and client_shell.
 *
 * Build:
 *   make admin   →  admin_shell  (compiled with -DADMIN_MODE)
 *   make client  →  client_shell (compiled with -DCLIENT_MODE)
 *
 * Usage:
 *   ./admin_shell [students.csv]
 *   ./admin_shell -f commands.txt [students.csv]
 *   ./client_shell [students.csv]
 *   ./client_shell -f commands.txt [students.csv]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TODO: Add your own header includes here */
/* #include "student.h"  */
/* #include "file_io.h"  */
/* #include "command.h"  */

typedef struct Student {
    int id;
    char name[23];
    int score;
    struct Student* next;
} Student;

Student* head = NULL;
char csv_filename[256] = "students.csv";

void add(int id, char* name, int score){
    Student* new_student = (Student*)malloc(sizeof(Student));
    new_student->id = id;
    new_student->score = score;
    strcpy(new_student->name, name);
    new_student->next = NULL;

    if (head == NULL) {
        head = new_student;
        return;
    }

    Student* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_student;
}

Student* find(int id) {
    Student* temp = head;
    while (temp != NULL) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

int delete(int id) {
    if (head == NULL) {
        return 0; 
    }
    
    if (head->id == id) {
        Student *delh = head;    
        head = head->next;      
        free(delh);           
        return 1;     
    }

    Student *p = head; 
    while (1) {
        if (p->next == NULL) {
            break; 
        }
        
        if (p->next->id == id) {
            Student *de = p->next;             
            p->next = de->next;     
            free(de);               
            return 1; 
        }
        p = p->next;
    }
    
    return 0; 
}
int load_csv(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: cannot open file\n");
        return -1;
    }

    char header[256];
    
    if (fgets(header, sizeof(header), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL) {
        int id, score;
        char name[32];
        int result = sscanf(line, "%d,%[^,],%d", &id, name, &score);
        add_to_list(id, name, score);
        count++;
    }

    fclose(fp);
    return count;
}

int save_csv(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: cannot write to file\n"); 
        return -1;
    }

    fprintf(fp, "id,name,score\n");

    int count = 0;
    Student* temp = head;
    while (temp != NULL) {
  
        fprintf(fp, "%d,%s,%d\n", temp->id, temp->name, temp->score);
        
        count++;
        temp = temp->next;
    }

    fclose(fp);
    return count;
}
/* ---------------------------------------------------------------
 * TODO: Implement the interactive shell loop.
 *   - Print a prompt and read a line from stdin.
 *   - Parse the line into a command and arguments.
 *   - Dispatch to the appropriate handler function.
 *   - Loop until the user types "exit" or EOF.
 * --------------------------------------------------------------- */
void run_shell(const char *csv_path) {
    /* TODO */
    (void)csv_path;
}

/* ---------------------------------------------------------------
 * TODO: Implement batch mode – read commands from a file.
 *   - Open cmd_file for reading.
 *   - Execute each line as a command (same logic as run_shell).
 *   - Close the file when done.
 * --------------------------------------------------------------- */
void run_command_file(const char *cmd_file, const char *csv_path) {
    /* TODO */
    (void)cmd_file;
    (void)csv_path;
}

int main(int argc, char *argv[]) {
    const char *csv_path  = "students.csv"; /* default CSV file */
    const char *cmd_file  = NULL;           /* -f <file> argument */

    /* TODO: Parse command-line arguments.
     *   Supported flags:
     *     -f <file>   run commands from <file> instead of stdin
     *   Remaining positional argument (if any): path to students CSV.
     *
     *   Example parsing skeleton:
     *
     *   for (int i = 1; i < argc; i++) {
     *       if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
     *           cmd_file = argv[++i];
     *       } else {
     *           csv_path = argv[i];
     *       }
     *   }
     */
    (void)argc;
    (void)argv;

#ifdef ADMIN_MODE
    /* Admin shell: supports add, delete, update, save, load, sort, list, find, help, exit */
    if (cmd_file) {
        run_command_file(cmd_file, csv_path);
    } else {
        run_shell(csv_path);
    }

#elif defined(CLIENT_MODE)
    /* Client shell: supports find, list, help, exit  (read-only) */
    if (cmd_file) {
        run_command_file(cmd_file, csv_path);
    } else {
        run_shell(csv_path);
    }

#else
#error "Define either -DADMIN_MODE or -DCLIENT_MODE when compiling."
#endif

    return 0;
}
