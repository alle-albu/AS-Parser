#include <stdio.h>
#include<stdint.h>
#include <string.h>
#include "a.h"

// Contributors:
// - ALEXANDRA Albu
// - LEONARD Filip

void print_data(AT_COMMAND_DATA data){
    for(uint8_t i=0; i<=data.line_count; i++) {
        for(uint8_t j=0; j<AT_COMMAND_MAX_LINE_SIZE; j++) if(data.data[i][j]>=32) printf("%c",data.data[i][j]);
        printf("\n");
    }
}

void reset_automaton(uint32_t state, AT_COMMAND_DATA data) {
  printf("syntax error occurred at state %d",state);
  state = 0;
  data.ok = 0;
  data.line_count = 0;
  memset(data.data, 0, sizeof data.data);
 }

STATE_MACHINE_RETURN_VALUE at_command_parse(uint8_t current_character){
 static uint32_t state = 0;
 static AT_COMMAND_DATA data;
// use i to keep track of 1st character on every line
 static uint8_t i = 0;

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

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 2:  {
        if (current_character == 'O') {
          //follow OK branch
            state = 3;
        }  else if(current_character == '+') {
                    state = 15;
                    
                    data.line_count = 0;
                    data.data[data.line_count][i++] = current_character;
            } else if (current_character == 'E') {
                    state = 6;
                } else {
                    // RESET AUTOMATON
                    reset_automaton(state, data);

                    return STATE_MACHINE_SYNTAX_ERROR;
                }
        break;
    }
    case 3:  {
        if (current_character == 'K') {
            state = 4;
        }  else  {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 4:  {
        if (current_character == 0x0D) {
            state = 5;
        }  else  {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 5: {
        if (current_character == 0x0A) {
            state = 11;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
    }
    
    case 11:  {
            data.ok = 1;
            print_data(data);
            return STATE_MACHINE_READY_OK;
         break;
    }
    case 6: {
        if (current_character == 'R') {
            state = 7;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 7: {
        if (current_character == 'R') {
            state = 8;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 8: {
        if (current_character == 'O') {
            state = 9;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 9: {
        if (current_character == 'R') {
            state = 10;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 10: {
        if (current_character == 0x0D) {
            state = 14;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 14: {
        if (current_character == 0x0A) {
            state = 13;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
    }
    case 13: {
        data.ok = 0;
        print_data(data);
        return STATE_MACHINE_READY_WITH_ERROR;
        break;
    }
    case 15: {
        if(current_character >= 32 && current_character<=124){
            state = 16;
            data.data[data.line_count][i++] = current_character;
        } else {
            //RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 16: {
        if(current_character >= 32 && current_character<=124 && current_character!= 0x0D){
            state = 16;
            data.data[data.line_count][i++] = current_character;
        } else {
            if(current_character == 0x0D) {
                state = 17;
            } else {
                // RESET AUTOMATON
                reset_automaton(state, data);

                return STATE_MACHINE_SYNTAX_ERROR;
            }
        }
        break;
    }
    case 17: {
        if(current_character == 0x0A){
            state = 18;
        } else {
            // RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 18: {
        if(current_character == '+'){
            state = 15;
            data.data[data.line_count][i] = '\0';
            data.line_count++;
            i=0;
            data.data[data.line_count][i++] = current_character;
        } else {
            if(current_character == 0x0D){
                state = 19;
                data.data[data.line_count][i] = '\0';
            } else {
                //RESET AUTOMATON
                reset_automaton(state, data);

                return STATE_MACHINE_SYNTAX_ERROR;
            }
        }
        break;
    }
    case 19: {
        if(current_character == 0x0A) {
            state = 20;
        } else {
            //RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 20: {
        if(current_character == 'O') {
            state = 3;
        } else if(current_character == 'E') {
                state = 6;
            } else {
            //RESET AUTOMATON
            reset_automaton(state, data);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
 }
 return STATE_MACHINE_NOT_READY;
}
