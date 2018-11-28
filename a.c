#include <stdio.h>
#include<stdint.h>
#include <stdlib.h>
#include <string.h>
#include "a.h"

//AT_COMMAND_DATA commandData;

// Contributors:
// - ALEXANDRA Albu
// - LEONARD Filip

// x State of network registration (AT+CREG) ->  +CREG: <n>, <stat> -> stat = {0,1,2,3,4,5}
// x Name of network operator (AT+COPS) ->  +COPS: <mode>,<format>,<op_long>,<op_short>  -> op_long
// x Modem IMEI (AT+GSN) ->  <sn> -> sn=IMEI
// x Modem Manufacturer (AT+GMI) -> <manufacturer_identity> 
// x Modem Software Version (AT+GMR) -> Revision: <revision>

// void get_info_data() {
//     COPS_network_operator_name();
//     CREG_network_registration_state();
//     GSN_imei();
//     GMI_manufacturer();
//     GMR_software_version();
// }

void print_data(AT_COMMAND_DATA data){
	uint8_t i,j;
    for(i=0; i<=data.line_count; i++) {
        for(j=0; j<AT_COMMAND_MAX_LINE_SIZE; j++) if(data.data[i][j]>=32) printf("%c",data.data[i][j]);
        printf("\n");
    }
}

void reset_automaton(uint32_t *state, AT_COMMAND_DATA data) {
  printf("syntax error occurred at state %d",*state);
  *state = 0;
  data.ok = 0;
  data.line_count = 0;
  memset(data.data, 0, sizeof data.data);
 }

STATE_MACHINE_RETURN_VALUE at_command_parse(uint8_t current_character, uint8_t special_flag){
 static uint32_t state = 0;
// use i to keep track of 1st character on every line
 static uint8_t i = 0;
 // special_flag set to 0 for normal commands; 1 for special commands (without +)

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
            reset_automaton(&state, commandData);

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
                    
                    commandData.line_count = 0;
                    commandData.data[commandData.line_count][i++] = current_character;
            } else if (current_character == 'E') {
                    state = 6;
                } else if (special_flag == 1 && ((current_character >= 48 && current_character <= 57) || (current_character >= 65 && current_character <= 90) || (current_character >= 97 && current_character <= 122) || current_character == 58 || current_character == 32 || current_character == 95)) {
                    state = 21;
                    commandData.line_count = 0;
                    commandData.data[commandData.line_count][i++] = current_character;
        
                } else {
                    // RESET AUTOMATON
                    reset_automaton(&state, commandData);

                    return STATE_MACHINE_SYNTAX_ERROR;
                }
        break;
    }
    case 3:  {
        if (current_character == 'K') {
            state = 4;
        }  else  {
            // RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 4:  {
        if (current_character == 0x0D) {
            state = 5;
        }  else  {
            // RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 5: {
        if (current_character == 0x0A) {
            state = 11;
        } else {
            // RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
    }
    
    case 11:  {
            commandData.ok = 1;
            print_data(commandData);
						state = 0;

						return STATE_MACHINE_READY_OK;
         break;
    }
    case 6: {
        if (current_character == 'R') {
            state = 7;
        } else {
            // RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 7: {
        if (current_character == 'R') {
            state = 8;
        } else {
            // RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 8: {
        if (current_character == 'O') {
            state = 9;
        } else {
            // RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 9: {
        if (current_character == 'R') {
            state = 10;
        } else {
            // RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 10: {
        if (current_character == 0x0D) {
            state = 14;
        } else {
            // RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 14: {
        if (current_character == 0x0A) {
            state = 13;
        } else {
            // RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
    }
    case 13: {
        commandData.ok = 0;
        //print_data(commandData);
				reset_automaton(&state, commandData);
        return STATE_MACHINE_READY_WITH_ERROR;
        break;
    }
    case 15: {
        if(current_character >= 32 && current_character<=124){
            state = 16;
            commandData.data[commandData.line_count][i++] = current_character;
        } else {
            //RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 16: {
        if(current_character >= 32 && current_character<=124 && current_character!= 0x0D){
            state = 16;
            commandData.data[commandData.line_count][i++] = current_character;
        } else {
            if(current_character == 0x0D) {
                state = 17;
            } else {
                // RESET AUTOMATON
                reset_automaton(&state, commandData);

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
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 18: {
        if(current_character == '+'){
            state = 15;
            commandData.data[commandData.line_count][i] = '\0';
            commandData.line_count++;
            i=0;
            commandData.data[commandData.line_count][i++] = current_character;
        } else {
            if(current_character == 0x0D){
                state = 19;
                commandData.data[commandData.line_count][i] = '\0';
            } else {
                //RESET AUTOMATON
                reset_automaton(&state, commandData);

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
            reset_automaton(&state, commandData);

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
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 21: {
        if((current_character >= 48 && current_character <= 57) || (current_character >= 65 && current_character <= 90) || (current_character >= 97 && current_character <= 122) || current_character == 58 || current_character == 32 || current_character == 95) {
            state = 21;
            commandData.data[commandData.line_count][i++] = current_character;
        } else if(current_character == 0x0D) {
            state = 22;
        } else {
            //RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 22: {
       if(current_character == 0x0A) {
            state = 23;
        } else {
            //RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 23: {
       if(current_character == 0x0D) {
            state = 24;
        } else {
            //RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 24: {
       if(current_character == 0x0A) {
            state = 25;
        } else {
            //RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
    case 25: {
        if(current_character == 'O') {
            state = 3;
        } else if(current_character == 'E') {
                state = 6;
            } else {
            //RESET AUTOMATON
            reset_automaton(&state, commandData);

            return STATE_MACHINE_SYNTAX_ERROR;
        }
        break;
    }
 }

 return STATE_MACHINE_NOT_READY;
}