#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void searchFile(FILE *f, char *searchTerm);

int main(int argc, char *argv[])
{
  FILE *f = NULL;
  
  if (argc == 1)
  {
    printf("wgrep: searchterm [file ...]\n");\
    exit(1);
  }
  else if (argc == 2)
  {
    searchFile(stdin, argv[1]);
  }
  else
  {
    int i;
    for (i = 2; i < argc; ++i)
    {
      f = fopen(argv[i], "r");

      if (!f)
      {
        printf("wgrep: cannot open file\n");
        exit(1);
      } 
      
      searchFile(f, argv[1]);
      fclose(f);
    }
  }
  
  return 0;
}

void searchFile(FILE *f, char *searchTerm)
{
  char *line = NULL;         // Buffer for line to be read in from file
  int length;                // Length of line
  size_t line_buf_size = 0; 
  
  while((length = getline(&line, &line_buf_size, f)) != -1)
  {
    if (strstr(line, searchTerm) != NULL)
    {
      printf("%s", line);
    }
  }
  
  free(line);
}