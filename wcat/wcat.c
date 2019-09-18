#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  FILE *f = NULL;
  
  if (argc > 1)
  {
    int i;
    int bufferLength = 500;
    char buffer[bufferLength];
    for (i = 1; i < argc; ++i)
    {
      f = fopen(argv[i], "r");
      
      if (!f)
      {
        printf("wcat: cannot open file\n");
        exit(1);
      }
      
      while(fgets(buffer, bufferLength, f) != NULL)
      {
        printf("%s", buffer);
      }
      
      fclose(f);
    }    
  }
  
  return 0;
}