#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int sameFile(FILE *f1, FILE *f2);
void readLines(FILE *f, char ***lineList, int *numLines);
void printLines(FILE *f, char **lineList, int numLines);

int main (int argc, char *argv[])
{
  FILE *inFile = NULL;     // File ptr for input file
  FILE *outFile = NULL;    // File ptr for output file
  char **lineList = NULL;  // List of lines to be read in from file (or stdin)
  int numLines = 0;        // Total number of lines in lineList
  
  if (argc > 3)
  {
    fprintf(stderr, "usage: reverse <input> <output>\n");
    exit(1);
  }
  
  if (argc > 2)
  {    
    outFile = fopen(argv[2], "w");
    
    if (!outFile)
    {
      fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
      exit(1);
    }
  }
  
  if (argc > 1)
  {
    inFile = fopen(argv[1], "r");
    
    if (!inFile)
    {
      fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
      exit(1);
    }
  }  
  
  if (sameFile(inFile, outFile))
  {
    fprintf(stderr, "reverse: input and output file must differ\n");
    exit(1);
  }

  readLines(inFile, &lineList, &numLines);
  
  if (inFile)
    fclose(inFile);
  
  printLines(outFile, lineList, numLines);
  
  if (outFile)
    fclose(outFile);
  
  return 0;
}

int sameFile(FILE *f1, FILE *f2)
{
  if (!f1 || !f2)
    return 0;
  
  // Comparing file location instead of just strings in case of a symbolic link
  struct stat stat1, stat2;
  fstat(fileno(f1), &stat1);
  fstat(fileno(f2), &stat2);
  return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}

void readLines(FILE *f, char ***lineList, int *numLines)
{
  char *line = NULL;         // Buffer for line to be read in from file
  int length;                // Length of line
  size_t line_buf_size = 0; 
  
  while((length = getline(&line, &line_buf_size, f != NULL ? f : stdin)) != -1)
  {
    if (numLines == 0)
    {
      *lineList = malloc(1 * sizeof(char*));      
    }
    else
    {
      *lineList = realloc(*lineList, (*numLines + 1) * sizeof(char*));
    }
    
    if (*lineList == NULL)
    {
      fprintf(stderr, "malloc failed");
      exit(1);
    }
    
    (*lineList)[(*numLines)] = malloc((length + 1) * sizeof(char));  // +1 to account for null terminated char
    
    if ((*lineList)[*numLines] == NULL)
    {
      fprintf(stderr, "malloc failed");
      exit(1);
    }
        
    strcpy((*lineList)[(*numLines)++], line);
  }
  free(line);
}

void printLines(FILE *f, char **lineList, int numLines)
{
  int i;
  for (i = numLines-1; i >= 0; --i)
  {
    fprintf(f != NULL ? f : stdout, "%s", lineList[i]);
    free(lineList[i]);
  }
  free(lineList);
}
