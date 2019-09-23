#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    FILE *inputFile = NULL;
    char error_message[] = "An error has occurred\n";

    if (argc > 2)
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    else if (argc == 2)
    {
        inputFile = fopen(argv[1], "r");

        if(!inputFile)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
    }
    
    size_t line_buf_size = 0;
    char *buffer = NULL;
    char *token = NULL;
    char *itr = NULL;
    char *delim = " \t\n\r";
    char **args = malloc(sizeof(*args));
    char *path = malloc((strlen("/bin") + 1) * sizeof(*path));

    strcpy(path , "/bin");

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

            free(args);
            free(buffer); // from doc: buffer should be freed, even if getline fails (or EOF reached)
            free(path);
            exit(0);
        } 
        
        int i = 0;
        itr = buffer; // use separate pointer to iterate through buffer, so we can keep pointer to beginning of buffer

        while((token = strsep(&itr, delim)) != NULL)
        {
            if (strstr(delim, token)) // do not store delim chars
            {
                continue;
            }

            i += 1;

            args = realloc(args, (i + 1) * sizeof(*args)); // reallocate space of args to include ptr to new string 
            args[i-1] = malloc((strlen(token) + 1) * sizeof(**args)); // allocate space for new string
            args[i] = NULL; // args passed to execv must be terminated by a NULL pointer
            strcpy(args[i-1], token);            
        }        

        if (!strcmp(args[0], "exit")) // do not exit until exit is entered
        {
            if (i > 1) // we cannot have an arg with exit, first arg is "exit"
            {
                write(STDERR_FILENO, error_message, strlen(error_message));

                for (int k = 0; k <= i; ++k)
                {
                    free(args[k]);
                }

                continue;
            }

            if (inputFile)
            {
                fclose(inputFile);
            }

            for (int k = 0; k <= i; ++k)
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
                for (int k = 0; k <= i; ++k)
                {
                    free(args[k]);
                }

                continue;
            }

            if(chdir(args[1]))
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
            }

            for (int k = 0; k <= i; ++k)
            {
                free(args[k]);
            }

            continue;
        }
        else if (!strcmp(args[0], "path"))
        {
            strcpy(path, "");
            for (int k = 1; k < i; ++k)
            {
                int space = k > 1 ? 1 : 0; // adding space for multiple path args 
                path = realloc(path, (strlen(path) + strlen(args[k]) + space + 1 ) * sizeof(*path));
                
                if (space)
                {
                    strcat(path, " ");
                }

                strcat(path, args[k]);
            }

            for (int k = 0; k <= i; ++k)
            {
                free(args[k]);
            }

            continue;
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
                itr = path;
                int success = 0;
                while ((token = strsep(&itr, " ")) != NULL)
                {
                    char *program = malloc(((strlen(token) + strlen(args[0]) + 2) * sizeof(char))); // +2 - 1 for / and 1 for null-terminated char
                    strcpy(program, token);
                    strcat(strcat(program, "/"), args[0]);

                    if(!access(program, X_OK))
                    {
                        success = 1;
                        execv(program, args);
                    }
                    free(program);
                }

                if (!success)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
                exit(0);
            }
        }        
    }

    return 0;
}