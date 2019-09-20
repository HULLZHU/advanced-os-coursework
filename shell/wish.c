#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    FILE *inputFile = NULL;
    char *buffer = NULL;
    char error_message[30] = "An error has occurred\n";
    size_t line_buf_size = 0; 

    if (argc > 2)
    {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        return 1;
    }
    else if (argc == 2)
    {
        inputFile = fopen(argv[1], "r");

        if(!inputFile)
        {
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            return 1;
        }

        fclose(inputFile);
    }

    while (1)
    {
        printf("wish> ");
        getline(&buffer, &line_buf_size, stdin);

        if (!strcmp(buffer, "exit\n")) // do not exit until exit is entered
            exit(0);
        printf("%s", buffer);
    }

    return 0;
}