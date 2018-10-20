#include <stdio.h>
#include<stdint.h>
#include <string.h>
#include "a.h"

void print_data(AT_COMMAND_DATA data){
    printf("here comes the data %d",data.ok);
}

void reset_automaton(uint32_t state, AT_COMMAND_DATA data) {
  state = 0;
  data.ok = 0;
  data.line_count = 0;
  memset(data.data, 0, sizeof data.data);
 }

STATE_MACHINE_RETURN_VALUE at_command_parse(uint8_t current_character){
 static uint32_t state = 0;
 static AT_COMMAND_DATA data;

//printf("current character is %d current state is %d \n",current_character, state );

 switch (state) {
    case 0: {
        if (current_character == 0x0D) {
        state = 1;
        }
        break;
    }
    case 1:  {
        if (current_character == 0x0A) {
            state = 2;
        }  else  {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
        break;
    }
    case 2:  {
        if (current_character == 'O') {
          //follow OK branch
            state = 3;
        } else  if (current_character == 'E') {
          //follow ERROR branch
          state = 6;
            } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
        break;
    }
    case 3:  {
        if (current_character == 'K') {
            state = 4;
        }  else  {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
        break;
    }
    case 4:  {
        if (current_character == 0x0D) {
            state = 5;
        }  else  {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
        break;
    }
    case 5: {
        if (current_character == 0x0A) {
            state = 11;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
    }
    
    case 6: {
        if (current_character == 'E') {
            state = 7;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
    }
    
    case 7: {
        if (current_character == 'R') {
            state = 8;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
    }
    
    case 8: {
        if (current_character == 'R') {
            state = 9;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
    }
    
    case 9: {
        if (current_character == 'O') {
            state = 10;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
    }
/*
although "<CR><LF>ERROR" should lead to a separate unique state, we can reuse state 4 since a "<CR><LF>" is expected on this ERROR branch
*/  
    case 10: {
        if (current_character == 'R') {
            state = 4;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
    }
    
    case 11:  {
            data.ok = 1;
            print_data(data);
            return STATE_MACHINE_READY_OK;
         break;
    }
 }
 //printf(" state is %d\n",state);
 return STATE_MACHINE_NOT_READY; 
}

// int main()
// {
//     printf("%d",at_command_parse(0x0D));
//     printf("%d",at_command_parse(0x0A));

//     printf("%d",at_command_parse('O'));
//     printf("%d",at_command_parse('K'));

//     printf("%d",at_command_parse(0x0D));
//     printf("%d",at_command_parse(0x0A));

//     return 0;
// }