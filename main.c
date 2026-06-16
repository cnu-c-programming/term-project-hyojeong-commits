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
char csv_filename[256] = "students.csv"; //여기도 수정필요

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

int remove_student(int id) {
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

    while (fgets(line, sizeof(line), fp) != NULL) { //좀 수정 필요
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
* --------------------------------------------------------------- */
void cmd_list() {
    Student* temp;
    if (head == NULL) {
        printf("No students found.\n");
        return;
    }
    printf("ID       Name                 Score\n");
    temp = head;
    while (temp != NULL) {
        printf("%-8d %-20s %d\n", temp->id, temp->name, temp->score);
        temp = temp->next;
    }
}

void cmd_find(char* args) {
    int id;
    Student* s;

    if (args == NULL || strlen(args) == 0) {
        printf("Error: missing argument.\n");
        return;
    }

    id = atoi(args);
    s = find_by_id(id);
    if (s == NULL) {
        printf("Error: student not found.\n");
        return;
    }

    printf("ID: %d\nName: %s\nScore: %d\n", s->id, s->name, s->score);
}

void cmd_stats() {
    int count = 0, sum = 0, max = -1, min = 101;
    Student* temp;

    if (head == NULL) {
        printf("No student data available.\n");
        return;
    }

    temp = head;
    while (temp != NULL) {
        count++;
        sum += temp->score;
        if (temp->score > max) max = temp->score;
        if (temp->score < min) min = temp->score;
        temp = temp->next;
    }

    printf("Count: %d\n", count);
    printf("Average: %.1f\n", (double)sum / count);
    printf("Max: %d\n", max);
    printf("Min: %d\n", min);
} 

void cmd_reload() {
    int n;
    free_all();
    n = load_csv(csv_filename);
    if (n < 0) return;
    printf("Reloaded %d students from %s.\n", n, csv_filename);
}

void cmd_help() {
    printf("Commands:\n");
#ifdef ADMIN_MODE
    printf("  save                     Save students to CSV\n");
    printf("  reload                   Reload students from CSV\n");
    printf("  add <id> <name> <score>  Add a student\n");
    printf("  delete <id>              Delete a student\n");
    printf("  update <id> <score>      Update student score\n");
#else
    printf("  reload                   Reload students from CSV\n");
#endif
    printf("  find <id>                Find student by ID\n");
    printf("  list                     List all students\n");
    printf("  stats                    Show statistics\n");
    printf("  help                     Show this help\n");
    printf("  clear                    Clear screen\n");
    printf("  exit                     Exit program\n");
}

#ifdef ADMIN_MODE

void cmd_add(char* args) {
    char s_id[32], s_name[32], s_score[32];
    int id, score;
    int parsed;

    if (args == NULL || strlen(args) == 0) {
        printf("Error: missing arguments.\n");
        return;
    }

    parsed = sscanf(args, "%s %s %s", s_id, s_name, s_score);
    if (parsed < 3) {
        printf("Error: missing arguments.\n");
        return;
    }

    id = atoi(s_id);
    if (id <= 0) {
        printf("Error: invalid ID.\n");
        return;
    }

    score = atoi(s_score);
    if (score < 0 || score > 100) {
        printf("Error: score out of range (0-100).\n");
        return;
    }

    if (find_by_id(id) != NULL) {
        printf("Error: duplicate ID.\n");
        return;
    }

    add_to_list(id, s_name, score);
    printf("Student added.\n");
}

void cmd_delete(char* args) {
    int id;
    if (args == NULL || strlen(args) == 0) {
        printf("Error: missing argument.\n");
        return;
    }
    id = atoi(args);
    if (remove_student(id) == 0) {
        printf("Error: student not found.\n");
        return;
    }
    printf("Student deleted.\n");
}

void cmd_update(char* args) {
    char s_id[32], s_score[32];
    int id, score;
    int parsed;
    Student* s;

    if (args == NULL || strlen(args) == 0) {
        printf("Error: missing arguments.\n");
        return;
    }

    parsed = sscanf(args, "%s %s", s_id, s_score);
    if (parsed < 2) {
        printf("Error: missing arguments.\n");
        return;
    }

    id    = atoi(s_id);
    score = atoi(s_score);

    if (score < 0 || score > 100) {
        printf("Error: score out of range (0-100).\n");
        return;
    }

    s = find_by_id(id);
    if (s == NULL) {
        printf("Error: student not found.\n");
        return;
    }

    s->score = score;
    printf("Student updated.\n");
}

void cmd_save() {
    int n = save_csv(csv_filename);
    if (n < 0) return;
    printf("Saved %d students to %s.\n", n, csv_filename);
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
