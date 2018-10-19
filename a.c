#include <stdio.h>
#include<stdint.h>
#include <string.h>
#include "a.h"

void print_data(AT_COMMAND_DATA data){
    printf("here comes the data %d",data.ok);
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
            state = 0;
            data.ok = 0;
            data.line_count = 0;
            memset(data.data, 0, sizeof data.data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
        break;
    }
    case 2:  {
        if (current_character == 'O') {
            state = 3;
        }  else  {
            // RESET AUTOMATON
            state = 0;
            data.ok = 0;
            data.line_count = 0;
            memset(data.data, 0, sizeof data.data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
        break;
    }
    case 3:  {
        if (current_character == 'K') {
            state = 4;
        }  else  {
            // RESET AUTOMATON
            state = 0;
            data.ok = 0;
            data.line_count = 0;
            memset(data.data, 0, sizeof data.data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
        break;
    }
    case 4:  {
        if (current_character == 0x0D) {
            state = 11;
        }  else  {
            // RESET AUTOMATON
            state = 0;
            data.ok = 0;
            data.line_count = 0;
            memset(data.data, 0, sizeof data.data);

            return STATE_MACHINE_READY_WITH_ERROR;
        }
        break;
    }
    case 5: {
        if (current_character == 0x0A) {
            state = 11;
        } else {
            // RESET AUTOMATON
            state = 0;
            data.ok = 0;
            data.line_count = 0;
            memset(data.data, 0, sizeof data.data);

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