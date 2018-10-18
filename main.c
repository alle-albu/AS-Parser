#include <stdio.h>
#include<stdint.h>
#include "a.h"

int main(int argc, char* argv[])
{
  if(argc==1){
    printf("Test file name missing.");
    return -1;
  } 

  FILE *fp = fopen(argv[1],"rb");
  if(fp == NULL){
    printf("Error in opening file");
    return -2;
  } 

do {
  char c = fgetc(fp);
  if(feof(fp)) break;
  printf(" %c\n",c);
  printf("in main: %d", at_command_parse(c));
} while(1);
  
  fclose(fp);

  return 0;
}