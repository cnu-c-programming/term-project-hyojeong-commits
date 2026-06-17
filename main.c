
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Student {
    int id;
    char name[32]; 
    int score;
    struct Student* next;
} Student;

Student* head = NULL;
char csv_filename[256] = "students.csv";

void add_to_list(int id, char* name, int score) {
    Student* new_student = (Student*)malloc(sizeof(Student));
    if (new_student == NULL) return;
    
    new_student->id = id;
    new_student->score = score;
    strncpy(new_student->name, name, 31);
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

Student* find_by_id(int id) {
    Student* temp = head;
    while (temp != NULL) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

int remove_student(int id) {
    Student* temp = head;
    Student* prev = NULL;
 
    while (temp != NULL) {
        if (temp->id == id) {
            if (prev == NULL) {
                head = temp->next;
            } else {
                prev->next = temp->next;
            }
            free(temp);
            return 1;
        }
        prev = temp;
        temp = temp->next;
    }
    return 0;
}

void free_all() {
    Student* temp = head;
    while (temp != NULL) {
        Student* next = temp->next;
        free(temp);
        temp = next;
    }
    head = NULL;
}



int load_csv(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Cannot open file\n");
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
        char* tok_id = strtok(line, ",\r\n");
        char* tok_name = strtok(NULL, ",\r\n");
        char* tok_score = strtok(NULL, ",\r\n");

        if (tok_id && tok_name && tok_score) {
            int id = atoi(tok_id);
            int score = atoi(tok_score);
            add_to_list(id, tok_name, score);
            count++;
        }
    }

    fclose(fp);
    return count;
}

int save_csv(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Cannot write file\n"); 
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


void cmd_list() {
    Student* temp;
    if (head == NULL) {
        printf("No students found.\n");
        return;
    }
    printf("ID     Name               Score\n");
    temp = head;
    while (temp != NULL) {
        printf("%d %s %d\n", temp->id, temp->name, temp->score);
        temp = temp->next;
    }
}

void cmd_find(char* args) {
    if (args == NULL || strlen(args) == 0) {
        printf("Error: missing argument.\n");
        return;
    }

    int id = atoi(args);
    Student* s = find_by_id(id);
    if (s == NULL) {
        printf("Error: student not found.\n");
        return;
    }

    printf("ID: %d\nName: %s\nScore: %d\n", s->id, s->name, s->score);
}

void cmd_stats() {
    int count = 0, sum = 0, max, min;
    Student* temp;

    if (head == NULL) {
        printf("No student data available.\n");
        return;
    }

    temp = head;
    max = temp->score;
    min = temp->score;

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
    free_all();
    int n = load_csv(csv_filename);
    printf("Reloaded %d students from %s.\n", n, csv_filename);
}

void cmd_help() {
    printf("Commands:\n");
#ifdef ADMIN_MODE
    printf("save                     Save students to CSV\n");
    printf("reload                   Reload students from CSV\n");
    printf("add <id> <name> <score>  Add a student\n");
    printf("delete <id>              Delete a student\n");
    printf("update <id> <score>      Update student score\n");
#else
    printf("reload                   Reload students from CSV\n");
#endif
    printf("find <id>                Find student by ID\n");
    printf("list                     List all students\n");
    printf("stats                    Show statistics\n");
    printf("help                     Show this help\n");
    printf("clear                    Clear screen\n");
    printf("exit                     Exit program\n");
}

#ifdef ADMIN_MODE

void cmd_add(char* args) {
    if (args == NULL || strlen(args) == 0) {
        printf("Error: missing arguments.\n");
        return;
    }

    char* tok_id = strtok(args, " \r\n");
    char* tok_name = strtok(NULL, " \r\n");
    char* tok_score = strtok(NULL, " \r\n");

    if (!tok_id || !tok_name || !tok_score) {
        printf("Error: missing arguments.\n");
        return;
    }

    int id = atoi(tok_id);
    if (id <= 0) {
        printf("Error: invalid ID.\n");
        return;
    }

    if (find_by_id(id) != NULL) {
        printf("Error: duplicate ID.\n");
        return;
    }

    int score = atoi(tok_score);
    if (score < 0 || score > 100) {
        printf("Error: score out of range (0-100).\n");
        return;
    }

    

    add_to_list(id, tok_name, score);
    printf("Student added.\n");
}

void cmd_delete(char* args) {
    if (args == NULL || strlen(args) == 0) {
        printf("Error: missing argument.\n");
        return;
    }
    
    char* tok_id = strtok(args, " \r\n");
    if (!tok_id) {
        printf("Error: missing argument.\n");
        return;
    }

    int id = atoi(tok_id);
    if (remove_student(id) == 0) {
        printf("Error: student not found.\n");
        return;
    }
    printf("Student deleted.\n");
}

void cmd_update(char* args) {
    if (args == NULL || strlen(args) == 0) {
        printf("Error: missing arguments.\n");
        return;
    }

    char* tok_id = strtok(args, " \r\n");
    char* tok_score = strtok(NULL, " \r\n");

    if (!tok_id || !tok_score) {
        printf("Error: missing arguments.\n");
        return;
    }

    int id = atoi(tok_id);
    int score = atoi(tok_score);

     if (find_by_id(id) == NULL) {
        printf("Error: student not found.\n");
        return;
    }
    
    if (score < 0 || score > 100) {
        printf("Error: score out of range (0-100).\n");
        return;
    }

    Student* s = find_by_id(id);
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

#endif /* ADMIN_MODE */

/* ---------------------------------------------------------------
 * 메인 쉘 명령어 분석 및 실행 루프
 * --------------------------------------------------------------- */

void run_command(char* line) {
    char* cmd =strtok(line, " \r\n");
    if (cmd ==NULL) return;

    char* args_ptr =NULL;
    char* space= strchr(line, ' ');
    if (space!=NULL) {
        args_ptr= space + 1;
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

        if (fgets(line, sizeof(line), stdin) == NULL) break;
        run_command(line);
    }
}

void run_command_file(const char *cmd_file, const char *csv_path) {
    FILE* fp = fopen(cmd_file, "r");
    char line[512];
    int lineno = 0;
 
    if (fp == NULL) {
        perror("Error: cannot open command file");
        run_shell(csv_path);
        return;
    }
 
    while (fgets(line, sizeof(line), fp) != NULL) {
        lineno++;
 
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '#') continue;
 
        line[strcspn(line, "\r\n")] = '\0';
 
        if (strlen(line) == 0) continue;
 
        printf("[command file:%d] %s\n", lineno, line);
        run_command(line);
    }
 
    fclose(fp);
    run_shell(csv_path);
}
int main(int argc, char *argv[]) {
    const char *csv_path  = "students.csv"; 
    const char *cmd_file  = NULL;           
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
    if (n <0) n =0;

#ifdef ADMIN_MODE
    printf("[Admin Program]\nLoaded %d students from %s.\n", n, csv_path);
#else
    printf("[Client Program]\nLoaded %d students from %s.\n", n, csv_path);
#endif

#ifdef ADMIN_MODE
    if (cmd_file) run_command_file(cmd_file, csv_path);
    else run_shell(csv_path);
#elif defined(CLIENT_MODE)
    if (cmd_file) run_command_file(cmd_file, csv_path);
    else run_shell(csv_path);
#else
#error "Define either -DADMIN_MODE or -DCLIENT_MODE when compiling."
#endif

    free_all();
    return 0;
}