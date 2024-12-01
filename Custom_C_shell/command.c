#ifndef COMMAND_H
#define COMMAND_H
#include "command.h"
#include "comm_log.h"
#include "display.h"
#include "procl.h"
#include "seek_c.h"
#include "reveal.h"

char *last_dir = NULL;
char resolved_path[1024];
char *token;
char *tokens[1024];
int token_count = 0;
int job_count = 0; // Number of jobs
Job jobs[1024];    // Definition of jobs array

void command_trim(char *str)
{
    if (str == NULL)
        return;

    size_t len = strlen(str);
    size_t i = 0, j = 0;

    while (i < len && isspace((unsigned char)str[i]))
    {
        i++;
    }

    if (i == len)
    {
        str[0] = ' ';
        str[1] = '\0';
        return;
    }

    while (i < len)
    {
        if (isspace((unsigned char)str[i]))
        {
            if (j > 0 && str[j - 1] != ' ')
            {
                str[j++] = ' ';
            }
        }
        else
        {
            str[j++] = str[i];
        }
        i++;
    }

    str[j] = '\0';
    size_t end = j;
    while (end > 0 && isspace((unsigned char)str[end - 1]))
    {
        end--;
    }
    str[end] = '\0';
}
// Command broken to tokens
char **tokenize_string(char *str, char *original_str)
{
    if (1)
    {
        add_command_to_log(c_log, &log_count, original_str);
        save_log(c_log, log_count);
    }
    char *token;
    token = strtok(str, ";");

    while (token != NULL)
    {
        char *start = token;
        while (*start == ' ')
            start++;

        char *end = start + strlen(start) - 1;
        while (end > start && *end == ' ')
            end--;
        end[1] = '\0';

        if (*start)
        {
            // Allocate memory and store the token in the array
            tokens[token_count] = strdup(start);
            token_count++;
        }
        token = strtok(NULL, ";");
    }
    return tokens;
}

// Command broken to tokens
char **tokenize_string_2(char *str)
{
    char *token;
    token = strtok(str, ";");

    while (token != NULL)
    {
        char *start = token;
        while (*start == ' ')
            start++;

        char *end = start + strlen(start) - 1;
        while (end > start && *end == ' ')
            end--;
        end[1] = '\0';

        if (*start)
        {
            // Allocate memory and store the token in the array
            tokens[token_count] = strdup(start);
            token_count++;
        }
        token = strtok(NULL, ";");
    }
    return tokens;
}
void execution(char **tokens, char *root, int token_count)
{
    int command_count = 0;
    char *commands[1024];
    // printf("%d\n", token_count);
    for (int i = 0; i < token_count; i++)
    {
        char *token;
        command_count = 0;
        int flag_end = 0;
        char *dup = strdup(tokens[i]);
        if (tokens[i][strlen(tokens[i]) - 1] == '&')
        {
            flag_end = 1;
        }
        token = strtok(tokens[i], "&");

        // printf("%s \n", token);
        while (token != NULL)
        {
            commands[command_count++] = strdup(token); // Store each command
            token = strtok(NULL, "&");
        }
        // printf("%s \n", commands[0]);
        // for(int i = 0; i < command_count; i++)
        // {
        //     printf("%s\n", commands[i]);
        // }

        for (int i = 0; i < command_count; i++)
        {
            char *command = commands[i];

            // Trim leading spaces
            while (*command == ' ')
                command++;
            char *end = command + strlen(command) - 1;
            while (end > command && *end == ' ')
                end--;
            end[1] = '\0';
            int is_last_command = (i == command_count - 1);

            if (is_last_command && flag_end)
            {
                exec_comm_bg(command, 1);
            }
            else if (is_last_command)
            {
                exec_comm_fg(command, root);
            }
            else
            {
                exec_comm_bg(command, 1);
            }
        }

        free(commands[i]);
    }
    token_count = 0;
}

int exec_comm_fg(char *command, char *root)
{

    time_t start, end;
    time(&start);

    // Copy of given command used
    char command_copy[1024];
    strncpy(command_copy, command, sizeof(command_copy));
    command_copy[sizeof(command_copy) - 1] = '\0';

    // Log commands
    if (strncmp(command_copy, "log", 3) == 0)
    {
        if (strlen(command_copy) == 3)
        {
            log_count--;
            display_log(c_log, log_count);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        if (strncmp(command_copy, "log purge", 9) == 0)
        {
            log_count = 0;
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        if (strncmp(command, "log execute ", 12) == 0)
        {
            log_count--;
            int index = atoi(command_copy + 12);
            // printf("Index: %d\n", index);
            // printf("Command: %s\n", c_log[index - 1]);
            // printf("%d\n", log_count);
            // for (int i = 0; i < log_count; i++)
            // {
            //     printf("%s\n", c_log[i]);
            // }
            log_execute_command(c_log, &log_count, index, root);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
    }
    // Reveal commands
    if (strncmp(command_copy, "reveal", 6) == 0)
    {

        if (strlen(command_copy) == 6)
        {
            reveal_command("~", root, last_dir);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        reveal_command(command_copy, root, last_dir);
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }
    if (strncmp(command_copy, "exit", 4) == 0)
    {
        char *s = "Exiting...\n";
        printf(" " COLOR_WHITE COLOR_BOLD "%s" COLOR_RESET "", s);
        exit(0);
    }
    // Proclore commands
    if (strncmp(command_copy, "proclore", 8) == 0)
    {

        char *token = strtok(command_copy + 8, " ");
        if (token == NULL)
        {
            proc_comm(getpid(), root);

            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }

        int pid = atoi(token);
        proc_comm(pid, root);
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }

    // Seek commands
    if (strncmp(command_copy, "seek", 4) == 0)
    {

        // Parse the seek command
        char *token = strtok(command_copy, " ");
        char *args[5] = {NULL};
        int argc = 0;

        while (token != NULL && argc < 5)
        {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }

        // Default values
        int d_flag = 0, f_flag = 0, e_flag = 0;
        char *target = NULL;
        char *start_dir = ".";
        int c = 0;
        // Process arguments and flags
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(args[i], "-d") == 0)
            {
                d_flag = 1;
                c++;
            }
            else if (strcmp(args[i], "-f") == 0)
            {
                f_flag = 1;
                c++;
            }
            else if (strcmp(args[i], "-e") == 0)
            {
                c++;
                e_flag = 1;
            }
            else if (argc == c + 2)
            {
                target = args[i];
                break;
            }
            else if (argc == c + 3)
            {
                target = args[argc - 2];
                start_dir = args[argc - 1];
            }
        }
        // printf("%d\n", argc);
        // printf("%d %d %d\n", d_flag, f_flag, e_flag);
        // printf("%s %s\n", target, start_dir);
        if (strncmp(start_dir, "~", 1) == 0)
        {
            strcpy(start_dir, strcat(root, start_dir + 1));
        }
        if (target == NULL)
        {
            char *s = "Usage: seek [-d|-f] [-e] <target> [directory]\n";
            printf(" " COLOR_ORANGE "%s" COLOR_RESET "", s);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        else if (d_flag == 1 && f_flag == 1)
        {
            char *s = "Invalid flags!\n";
            printf(" " COLOR_RED "%s" COLOR_RESET "", s);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }

        else
        {
            char resolved_dir[MAX_PATH];
            arr[0] = 0;
            arr[1] = 0;
            found[0] = NULL;
            found[1] = NULL;
            buff[0] = '\0';
            no_match_flag = 1;
            if (strncmp(start_dir, "~", 1) == 0)
            {
                strcpy(start_dir, strcat(root, start_dir + 1));
            }
            realpath(start_dir, resolved_dir);                                          // Resolve the directory path
            seek_directory(resolved_dir, target, d_flag, f_flag, e_flag, resolved_dir); // Call the seek_directory function
            if (e_flag)
                check_e_flag(arr[0], arr[1], found[0], buff, d_flag, f_flag);
            // printf("no match flag %d\n", no_match_flag);
            check_if_match_found(no_match_flag);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        return 1;
    }

    // Hop commands
    if (strncmp(command_copy, "hop", 3) == 0)
    {

        char *token = strtok(command_copy, " ");
        if (token == NULL)
        {
            hop_command("~", root, &last_dir);
            time(&end);
            time_taken = (int)difftime(end, start);
            char command_copy_2[1024];
            strncpy(command_copy_2, command, sizeof(command_copy_2));
            char *t = strtok(command_copy_2, " ");
            append_command_time(t, time_taken);
            return 1;
        }
        while (token != NULL)
        {
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                hop_command(token, root, &last_dir);
            }
        }
        time(&end);
        time_taken = (int)difftime(end, start);
        char command_copy_2[1024];
        strncpy(command_copy_2, command, sizeof(command_copy_2));
        char *t = strtok(command_copy_2, " ");
        append_command_time(t, time_taken);
        return 1;
    }
    else
    {
        // Attempt to execute as a system command
        pid_t pid = fork();
        int flag = 0;
        if (pid == 0)
        {
            // Child process
            char *argv[1024];
            int argc = 0;

            // Tokenize the command to pass to execlp
            char *token = strtok(command_copy, " ");
            while (token != NULL)
            {
                argv[argc++] = token;
                token = strtok(NULL, " ");
            }
            argv[argc] = NULL;
            int z = execvp(argv[0], argv);
            // Execute the command using execlp
            if (z == -1)
            {
                printf("\033[31mERROR : '%s' is not a valid command\033[0m\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        else if (pid > 0)
        {

            // Parent process: Wait for the child process to finish
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status))
            {
                int exit_code = WEXITSTATUS(status);
                if (exit_code == 0)
                {

                    time(&end);
                    time_taken = (int)difftime(end, start);
                    char command_copy_2[1024];
                    strncpy(command_copy_2, command, sizeof(command_copy_2));
                    char *t = strtok(command_copy_2, " ");
                    append_command_time(t, time_taken);
                }
            }
        }

        else
        {
            // Fork failed
            char *s = "ERROR : ";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            perror("Error - fork");
            return -1;
        }
    }

    return 1;
}
void exec_comm_bg(char *command, int background)
{
    pid_t pid = fork();
    char *argv[1024];

    if (pid == 0)
    {
        // Child process
        int argc = 0;
        char *token = strtok(command, " ");
        while (token != NULL)
        {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;
        if (strcmp(argv[0], "exit") == 0)
        {
            char *s = "Exiting shell...";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "\n", s);
            exit(0);
        }
        // Execute the command
        if (execvp(argv[0], argv) == -1)
        {
            char *s = "ERROR : ";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            perror("error");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
    {
        // Parent process
        if (background)
        {
            // Store the job
            jobs[job_count].pid = pid;
            strcpy(jobs[job_count].command, command);
            jobs[job_count].job_id = job_count + 1;
            job_count++;

            printf("[%d] %d\n", job_count, pid);

            // Return immediately; background process is being handled asynchronously
            return;
        }
        else
        {
            // Blocking wait for foreground process
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        // Fork failed
        char *s = "ERROR : ";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        perror("Error - fork");
    }
}

int hop_command(char *path, char *home, char **last_dir)
{
    char cwd[1024];

    // Resolve special cases for path
    if (strncmp(path, "~/", 2) == 0)
    {
        path = path + 1;
        strcpy(resolved_path, home);
        strcat(resolved_path, path);
        path = resolved_path;
    }
    else if (strcmp(path, "~") == 0)
    {
        path = home;
    }
    else if (strcmp(path, "-") == 0)
    {
        if (*last_dir != NULL)
        {
            path = *last_dir;
            if (chdir(path) == -1)
            {
                char *s = "ERROR : ";
                printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
                perror("Error - chdir");
            }
            return 1;
        }
        else
        {
            char *s = "OLDPWD not set\n";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);

            return 1;
        }
    }
    else if (strcmp(path, ".") == 0)
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            char *s = "ERROR : ";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            perror("Error - getcwd");
        }
        printf("%s\n", cwd);
        return 1; // Current directory, do nothing
    }
    else if (strcmp(path, "..") == 0)
    {
        chdir("..");
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            char *s = "ERROR : ";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            perror("Error - getcwd");
        }
        else
        {
            printf("%s\n", cwd);
        }
        return 1;
    }

    // Change directory
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        char *s = "ERROR : ";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        perror("Error - getcwd");
        return 1;
    }

    // Save current directory as last directory before changing
    if (*last_dir)
    {
        free(*last_dir);
    }
    *last_dir = strdup(cwd);

    // Change directory to the specified path
    if (chdir(path) == -1)
    {
        char *s = "ERROR : ";
        printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
        perror("Error - chdir");
    }
    else
    {
        // Print the new working directory
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            char *s = "ERROR : ";
            printf(" " COLOR_RED COLOR_BOLD "%s" COLOR_RESET "", s);
            perror("Error - getcwd");
        }
        else
        {
            printf("%s\n", cwd);
        }
    }
    return 1;
}
void append_command_time(const char *command, int time_taken)
{
    if (count < MAX_COMMANDS)
    {
        strncpy(command_store[count], command, sizeof(command_store[count]) - 1);
        command_store[count][sizeof(command_store[count]) - 1] = '\0'; // Ensure null-termination
        time_str[count] = time_taken;
        count++;
    }
    else
    {

        char *s = "Command history limit reached. Cannot store more commands.\n";
        printf(" " COLOR_ORANGE COLOR_BOLD "%s" COLOR_RESET "", s);
    }
}
void handle_sigchld(int sig)
{
    (void)sig; // Suppress unused variable warning

    int status;
    pid_t pid;

    // Reap all finished child processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // Find the job associated with this PID
        for (int i = 0; i < job_count; i++)
        {
            if (jobs[i].pid == pid)
            {
                // Print process exit information
                if (WIFEXITED(status))
                {
                    char *duplicate = strdup(jobs[i].command);
                    capitalize_first_letter(duplicate);
                    char *token = strtok(duplicate, " ");
                    printf("%s exited normally (%d)\n", token, pid);
                }
                else if (WIFSIGNALED(status))
                {
                    char *duplicate = strdup(jobs[i].command);
                    capitalize_first_letter(duplicate);
                    char *token = strtok(duplicate, " ");
                    printf("%s exited abnormally (%d)\n", token, pid);
                }
                else if (WIFSTOPPED(status))
                {
                    char *duplicate = strdup(jobs[i].command);
                    capitalize_first_letter(duplicate);
                    char *token = strtok(duplicate, " ");
                    printf("%s exited abnormally (%d)\n", token, pid);
                }
                // Remove the job from the list
                for (int j = i; j < job_count - 1; j++)
                {
                    jobs[j] = jobs[j + 1];
                }
                job_count--;
                break;
            }
        }
    }
}
void capitalize_first_letter(char *str)
{
    if (str == NULL || str[0] == '\0')
    {
        // If the string is empty or null, do nothing
        return;
    }

    // Capitalize the first character if it's a letter
    str[0] = toupper(str[0]);
}

#endif