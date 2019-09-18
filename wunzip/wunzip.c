#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("wunzip: file1 [file2 ...]\n");
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
    
    while (1)
    {      
      if(!fread(&charCount, 4, 1, f))
        break;  
      
      if(!fread(&currentChar, 1, 1, f))
        break;
      
      int i;
      for (i = 0; i < charCount; ++i)
        printf("%c", currentChar);
    }
    
    fclose(f);
  }
  
  return 0;
}
