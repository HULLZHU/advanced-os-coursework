#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("wzip: file1 [file2 ...]\n");
    exit(1);
  }
  
  FILE *f = NULL;
  int i;
  char currentChar = NULL;
  int charCount = 0;
  for (i = 1; i < argc; ++i)
  {
    f = fopen(argv[i], "r");
    
    if(!f)
    {
      printf("wgrep: cannot open file\n");
      exit(1);
    }
    
    int c;
    while ((c = getc(f)) != EOF)
    {      
      if (currentChar != c && charCount != 0)  // will be 0 if this is first time running through loop
      {
        fwrite(&charCount, 4, 1, stdout);        
        fwrite(&currentChar, 1, 1, stdout);
        charCount = 0;
      }
      
      currentChar = c;
      ++charCount;
    }
    
    fclose(f);
  }
  
    // print out last char in last file; EOF condition causes file to exit before compressing
    fwrite(&charCount, 4, 1, stdout);        
    fwrite(&currentChar, 1, 1, stdout);
  
  return 0;
}
