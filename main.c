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
void run_command(char* line) {
    char cmd[256];
    char args_buf[256];
    char* args_ptr = NULL;
    int i, j, k;
    int len;

    remove_newline(line);
    len = strlen(line);
    if (len == 0) return;
    j = 0;
    for (i = 0; i < len; i++) {
        if (line[i] == ' ') {
            break;
        }
        cmd[j++] = line[i];
    }
    cmd[j] = '\0';

    while (i < len && line[i] == ' ') {
        i++;
    }

    if (i < len) {
        k = 0;
        for (; i < len; i++) {
            args_buf[k++] = line[i];
        }
        args_buf[k] = '\0';
        
        if (strlen(args_buf) > 0) {
            args_ptr = args_buf;
        }
    }

    if (strcmp(cmd, "list") == 0) {
        cmd_list();
    } else if (strcmp(cmd, "find") == 0) {
        cmd_find(args_ptr);
    } else if (strcmp(cmd, "stats") == 0) {
        cmd_stats();
    } else if (strcmp(cmd, "reload") == 0) {
        cmd_reload();
    } else if (strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "clear") == 0) {
        printf("\033[2J\033[H");
    } else if (strcmp(cmd, "exit") == 0) {
        printf("Goodbye.\n");
        free_all();
        exit(0);
#ifdef ADMIN_MODE
    } else if (strcmp(cmd, "add") == 0) {
        cmd_add(args_ptr);
    } else if (strcmp(cmd, "delete") == 0) {
        cmd_delete(args_ptr);
    } else if (strcmp(cmd, "update") == 0) {
        cmd_update(args_ptr);
    } else if (strcmp(cmd, "save") == 0) {
        cmd_save();
#endif
    } else {
        printf("Unknown command or permission denied.\n");
    }
}



void run_shell(const char *csv_path) {
    char line[512];
    (void)csv_path;
    while (1) {
#ifdef ADMIN_MODE
        printf("admin> ");
#else
        printf("client> ");
#endif
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        run_command(line);
    }
}


/* ---------------------------------------------------------------
 * TODO: Implement batch mode – read commands from a file.
 *   - Open cmd_file for reading.
 *   - Execute each line as a command (same logic as run_shell).
 *   - Close the file when done.
 * --------------------------------------------------------------- */
void run_command_file(const char *cmd_file, const char *csv_path) {
    FILE* fp = fopen(cmd_file, "r");
    char line[512];
    int lineno = 0;
    (void)csv_path; 

    if (fp == NULL) {
        perror("Error: cannot open command file");
        run_shell(csv_path);
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        lineno++;
        remove_newline(line);
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }

        printf("[command file:%d] %s\n", lineno, line);
        run_command(line);
    }

    if (ferror(fp)) {
        fprintf(stderr, "Error: command file read error.\n");
    }

    fclose(fp);
    run_shell(csv_path);
}

int main(int argc, char *argv[]) {
    const char *csv_path  = "students.csv"; /* default CSV file */
    const char *cmd_file  = NULL;           /* -f <file> argument */
    int i, n;

    if (argc < 2) {
#ifdef ADMIN_MODE
        fprintf(stderr, "Usage: ./admin_shell <csv_file> [-f command_file]\n");
#else
        fprintf(stderr, "Usage: ./client_shell <csv_file> [-f command_file]\n");
#endif
        return 1;
    }

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            cmd_file = argv[++i];
        } else {
            csv_path = argv[i];
        }
    }

    strcpy(csv_filename, csv_path);
    n = load_csv(csv_path);
    if (n < 0) {
        n = 0;
    }

#ifdef ADMIN_MODE
    printf("[Admin Program] Loaded %d students from %s.\n", n, csv_path);
#else
    printf("[Client Program] Loaded %d students from %s.\n", n, csv_path);
#endif


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

    /*free_all();*/
    return 0;
}
