//#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static const char error_message[] = "An error has occurred\n";
static char *path;
static FILE *inputFile;
//static pthread_t threads[10];
//static pthread_mutex_t lock;

void process_command(void *buffer);

int main(int argc, char *argv[])
{
    inputFile = NULL;

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
    path = malloc((strlen("/bin") + 1) * sizeof(*path));
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

            free(buffer); // from doc: buffer should be freed, even if getline fails (or EOF reached)
            free(path);
            exit(0);
        } 
        
        char *itr = buffer;
        char *token = NULL;
        //int i = 0;
        token = strsep(&itr, "&");

        if (itr != NULL) // & found
        {
            // int rc = pthread_mutex_init(&lock, NULL);
            // assert(rc == 0); // always check success!
            int rc;
            do
            {
                rc = fork();

                if (rc < 0)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
                else if (rc > 0) // parent process enters here
                {
                    //wait(NULL);

                    //pthread_mutex_unlock(&lock);
                }
                else
                {
                    process_command(token);
                    exit(0);
                }
                //pthread_create(&threads[i++], NULL, process_command, token);
            } while ((token = strsep(&itr, "&")) != NULL);

            if (rc > 0) // parent process
            {
                wait(NULL);
            }
            // for (int k = 0; k < i; ++k)
            // {
            //     pthread_join(threads[k], NULL);
            // }
            
            //pthread_mutex_destroy(&lock);
        }
        else
        {
            process_command(buffer);
        }                
    }

    return 0;
}

void process_command(void *buffer)
{
    char *buf = (char*)buffer; 

    const char *delim = " \t\n\r\v\f";  
    int redirection = 0;
    int stdout_copy = dup(1);
    char *itr;
    char *redirect = buf;
    char *token = NULL;
    char **args = malloc(sizeof(*args));

    buf = strsep(&redirect, ">"); // split for case of redirection  
    itr = buf;
    int i = 0;

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

    if (i == 0)
    {
        if(redirect) // no left-hand side
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }

        free(args);
        return;
    }

    if (!strcmp(args[0], "exit")) // do not exit until exit is entered
    {
        if (i > 1) // we cannot have an arg with exit, first arg is "exit"
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else
        {
            //pthread_mutex_lock(&lock);
            if (inputFile)
            {
                fclose(inputFile);
            }
            //pthread_mutex_unlock(&lock);

            for (int k = 0; k <= i; ++k)
            {
                free(args[k]);
            }

            free(args);
            free(path);
            free(buffer);
            exit(0); 
        }   
    }
    else if (!strcmp(args[0], "cd"))
    {
        if (i == 1 || i > 2) // 1st arg is program, but we need exactly 1 more argument to chdir
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else if(chdir(args[1]))
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
    else if (!strcmp(args[0], "path"))
    {
        //pthread_mutex_lock(&lock);
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
        //pthread_mutex_unlock(&lock);
    }    
    else
    {
        //pthread_mutex_lock(&lock);

        int count = 0;
        if (redirect != NULL)
        {
            char *fileName = NULL;
            while((token = strsep(&redirect, delim)) != NULL)
            {
                if (strstr(delim, token)) // do not store delim chars
                {
                    continue;
                }

                if (count == 0)
                {
                    fileName = token;
                }

                count += 1;
            }

            if (count != 1) // only 1 argument allowed on right hand side of '>'
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                free(args);
                //pthread_mutex_unlock(&lock);

                return;
            }
            
            close(STDOUT_FILENO);
            redirection = open(fileName, O_CREAT|O_RDWR|O_TRUNC, S_IRWXU);
        }    

        int rc = fork();

        if (rc < 0)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else if (rc > 0) // parent process enters here
        {
            wait(NULL);
            
            if (redirection > 0)
            {
                dup2(stdout_copy, 1);
            }

            //pthread_mutex_unlock(&lock);
        }
        else // child process enters here
        {
            itr = path;
            int success = 0;
            while ((token = strsep(&itr, " ")) != NULL)
            {
                char *program = malloc((strlen(token) + strlen(args[0]) + 2) * sizeof(char)); // +2 - 1 for / and 1 for null-terminated char
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

    for (int k = 0; k <= i; ++k)
    {
        free(args[k]);
    }

    free(args);
    return;
}