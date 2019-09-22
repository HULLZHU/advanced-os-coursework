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
    char *path = "/bin";
    char error_message[30] = "An error has occurred\n";
    size_t line_buf_size = 0; 

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

    while (1)
    {
        printf("wish> ");

        if(getline(&buffer, &line_buf_size, inputFile ? inputFile : stdin) == -1) // EOF reached
        {
            if (inputFile)
            {
                fclose(inputFile);
            }

            free(buffer); // from doc: buffer should be freed, even if getline fails (or EOF reached)
            exit(0);
        }
        
        char *delim = " \t\n\r";
        char *token;
        char **argv = malloc(sizeof(char*) * 1);
        int i = 0;

        while((token = strsep(&buffer, delim)) != NULL)
        {
            if (strstr(delim, token)) // do not store delim chars
            {
                continue;
            }

            if (i != 0)
            {
                argv = realloc(argv, sizeof(char*) * (i + 1)); // reallocate space for new char*
            }
            
            argv[i] = malloc((strlen(token) + 1) * sizeof(token)); // +1 to include null-terminating char
            argv[i++] = token;
        }

        argv[i] = NULL; // marks end of array

        if (!strcmp(argv[0], "exit")) // do not exit until exit is entered
        {
            if (inputFile)
            {
                fclose(inputFile);
            }

            for (int k = 0; k < i; ++k)
            {
                free(argv[k]);
            }

            free(argv);
            free(buffer);
            exit(0);        
        }
        else if (!strcmp(argv[0], "cd"))
        {
            if (i == 1 || i > 2) // 1st arg is program, but we need exactly 1 more argument to chdir
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                chdir(argv[1]);
            }
        }
        else if (!strcmp(argv[0], "path"))
        {
        }        
        else
        {
            int rc = fork();

            if (rc < 0)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            else if (rc == 0) // child process
            {
                printf("child");
                printf("%s", path);
                if(execv(strcat(strcat(path, "/"), argv[0]), argv))
                    write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
            }
            else if (rc > 0) // parent process
            {
                wait(NULL);
                printf("parent");
            }
        }

        for (int k = 0; k < i; ++k)
        {
            free(argv[k]);
        }

        free(argv);
    }

    return 0;
}