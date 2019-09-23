#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    FILE *inputFile = NULL;
    char *buffer = NULL;
    char error_message[30] = "An error has occurred\n";
    size_t line_buf_size = 0;
    char *path = malloc((strlen("/bin")) * sizeof(*path));
    strcpy(path , "/bin");

    if (argc > 2)
    {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        free(path);
        exit(1);
    }
    else if (argc == 2)
    {
        inputFile = fopen(argv[1], "r");

        if(!inputFile)
        {
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            free(path);
            exit(1);
        }
    }

    while (1)
    {
        if (!inputFile)
        {
            printf("wish> ");
        }

        if(getline(&buffer, &line_buf_size, inputFile ? inputFile : stdin) == -1) // EOF reached
        {
            if (inputFile)
            {
                fclose(inputFile);
            }

            free(buffer); // from doc: buffer should be freed, even if getline fails (or EOF reached)
            free(path);
            exit(0);
        }
        
        char *delim = " \t\n\r";
        char *token;
        char *itr = buffer;
        char **args = malloc(sizeof(*args));
        int i = 0;
        
        while((token = strsep(&itr, delim)) != NULL)
        {
            if (strstr(delim, token)) // do not store delim chars
            {
                continue;
            }

            if (i != 0)
            {
                args = realloc(args, (i + 1) * sizeof(*args)); // reallocate space of args to include space for new string
            }
            
            args[i] = malloc((strlen(token)) * sizeof(**args));
            strcpy(args[i], token);
            i += 1;
        }

        if (!strcmp(args[0], "exit")) // do not exit until exit is entered
        {
            if (i > 1) // we cannot have an arg with exit, first arg is "exit"
            {
                write(STDERR_FILENO, error_message, strlen(error_message));

                for (int k = 0; k < i; ++k)
                {
                    free(args[k]);
                }
                free(args);
                continue;
            }

            if (inputFile)
            {
                fclose(inputFile);
            }

            for (int k = 0; k < i; ++k)
            {
                free(args[k]);
            }

            free(args);
            free(buffer);
            free(path);
            exit(0);        
        }
        else if (!strcmp(args[0], "cd"))
        {
            if (i == 1 || i > 2) // 1st arg is program, but we need exactly 1 more argument to chdir
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                for (int k = 0; k < i; ++k)
                {
                    free(args[k]);
                }
                free(args);
                continue;
            }

            if(chdir(args[1]))
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            continue;
        }
        else if (!strcmp(args[0], "path"))
        {
            strcpy(path, "");
            for (int k = 1; k < i; ++k)
            {
                path = realloc(path, (strlen(path) + strlen(args[k])) * sizeof(*path));
                strcat(path, args[k]);
            }
        }        
        else
        {
            int rc = fork();

            if (rc < 0)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            else if (rc > 0) // parent process enters here
            {
                wait(NULL);
            }
            else // child process enters here
            {
                char *program = malloc(((strlen(path) + strlen(args[0])) * sizeof(char))); 
                strcpy(program, path);
                strcat(strcat(program, "/"), args[0]);

                if(execv(program, args))
                    write(STDERR_FILENO, error_message, strlen(error_message));
                free(program);
                exit(0);
            }
        }

        for (int k = 0; k < i; ++k)
        {
            free(args[k]);
        }

        free(args);
        free(buffer);
    }

    return 0;
}