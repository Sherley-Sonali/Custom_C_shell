#include "headers.h"

#define COLOR_BOLD    "\033[1m"
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"  // Green for executables
#define COLOR_WHITE   "\033[37m"  // White for regular files
#define COLOR_BLUE    "\033[34m"  // Blue for directories
#define COLOR_RED   "\033[31m"  // Red 
#define COLOR_ORANGE   "\033[33m"  // Orange

extern char *last_dir;
extern char resolved_path[1024];
extern char *token;
extern char *tokens[1024];
extern int token_count; 
typedef struct {
    pid_t pid;
    char command[1024];
    int job_id;
} Job;

extern Job jobs[1024];
extern int job_count;

char** tokenize_string(char *str, char* original_str);
char **tokenize_string_2(char *str);
void execution(char** tokens, char* root, int token_count);
int exec_comm_fg(char *command, char *root);
void exec_comm_bg(char *command, int background);
int hop_command(char *path, char *home, char **last_dir);
void command_trim(char* str);
void append_command_time(const char *command, int time_taken);
void handle_sigchld(int sig);
void capitalize_first_letter(char *str);

