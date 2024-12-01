#ifndef COMM_LOG_H
#define COMM_LOG_H
#include "comm_log.h"
#include "command.h"

char c_log[15][1024]; // Define the log array
int log_count = 0;    // Initialize the log count

void load_log(char c_log[MAX_LOG_SIZE][1024], int *log_count)
{
    FILE *file = fopen(LOG_FILE_PATH, "r");
    if (!file)
        return;

    *log_count = 0;
    while (fgets(c_log[*log_count], 1024, file))
    {
        c_log[*log_count][strcspn(c_log[*log_count], "\n")] = 0; // Remove newline
        (*log_count)++;
    }
    fclose(file);
}

void save_log(char c_log[MAX_LOG_SIZE][1024], int log_count)
{
    FILE *file = fopen(LOG_FILE_PATH, "w");
    if (!file)
    {
        char *s = "ERROR : ";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        perror("Error opening log file");
        return;
    }

    for (int i = 0; i < log_count; i++)
    {
        fprintf(file, "%s\n", c_log[i]);
    }
    fclose(file);
}

void add_command_to_log(char c_log[MAX_LOG_SIZE][1024], int *log_count, const char *command)
{
    if (*log_count > 0 && strcmp(c_log[*log_count - 1], command) == 0)
    {
        return; // Skip if the command is the same as the previous one
    }

    if (*log_count < MAX_LOG_SIZE)
    {
        strcpy(c_log[*log_count], command);
        (*log_count)++;
    }
    else
    {
        // Shift all commands up to make room for the new command
        for (int i = 1; i < MAX_LOG_SIZE; i++)
        {
            strcpy(c_log[i - 1], c_log[i]);
        }
        strcpy(c_log[MAX_LOG_SIZE - 1], command);
    }
}

void display_log(char c_log[MAX_LOG_SIZE][1024], int log_count)
{
    for (int i = 0; i < log_count; i++)
    {
        printf("%s\n", c_log[i]);
    }
}

void execute_command(char *command, char *home)
{
    token_count = 0;
    char *dup_command = strdup(command);
    command_trim(dup_command);
    char **tokens = tokenize_string_2(dup_command);
    execution(tokens, home, token_count);
    for (int i = 0; i < token_count; i++)
    {
        free(tokens[i]);
    }
}

void log_execute_command(char c_log[MAX_LOG_SIZE][1024], int *log_count, int index, char *home)
{
    if (index < 1 || index > *log_count)
    {
        char *s = "Invalid index\n";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        // printf("Invalid index\n");
        return;
    }

    char *command_to_execute = c_log[*log_count - index];
    // printf("Executing command: %s -- %d\n", command_to_execute, *log_count - index);

    // Execute the command
    execute_command(command_to_execute, home);
}

#endif