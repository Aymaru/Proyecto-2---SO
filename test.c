

#include <stdio.h>  /* defines FILENAME_MAX */
#include <unistd.h>
#define GetCurrentDir getcwd

int main(){
  char buff[FILENAME_MAX];
  GetCurrentDir( buff, FILENAME_MAX );
  printf("Current working dir: %s\n", buff);
  return 1;
}

