#include <stdio.h>
#include<stdint.h>
#include "a.h"

void print_final_state(uint32_t result){
  printf("\n final state is %s",result==0?"STATE_MACHINE_NOT_READY":
                            result==1?"STATE_MACHINE_READY_OK":
                            result==2?"STATE_MACHINE_READY_WITH_ERROR":
                            "STATE_MACHINE_SYNTAX_ERROR");
}

int main(int argc, char* argv[])
{
  uint32_t result;

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
  uint8_t c = fgetc(fp);
  if(feof(fp)) break;
  //printf(" %c\n",c);
  result = at_command_parse(c);
  if(result == 1 || result == 2 || result== 3) break;
} while(1);
  
  fclose(fp);

  if(result == 0) {
    printf("Either transmission was never started or it never ended.");
  }
  print_final_state(result);

  return 0;
}