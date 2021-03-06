#ifndef A_H
#define A_H

#define AT_COMMAND_MAX_LINES 100
#define AT_COMMAND_MAX_LINE_SIZE 128

typedef struct
{
 uint8_t ok;
 uint8_t data[AT_COMMAND_MAX_LINES][AT_COMMAND_MAX_LINE_SIZE + 1];
 uint32_t line_count;
}AT_COMMAND_DATA; 

typedef enum
{
 STATE_MACHINE_NOT_READY,
 STATE_MACHINE_READY_OK,
 STATE_MACHINE_READY_WITH_ERROR,
 STATE_MACHINE_SYNTAX_ERROR
}STATE_MACHINE_RETURN_VALUE;


extern AT_COMMAND_DATA commandData;
extern uint8_t imeiData[AT_COMMAND_MAX_LINE_SIZE];
extern uint8_t registrationStateData[AT_COMMAND_MAX_LINE_SIZE];
extern uint8_t operatorData[AT_COMMAND_MAX_LINE_SIZE];
extern uint8_t manufacturerData[AT_COMMAND_MAX_LINE_SIZE];
extern uint8_t softwareData[AT_COMMAND_MAX_LINE_SIZE];


STATE_MACHINE_RETURN_VALUE at_command_parse(uint8_t current_character, uint8_t special_flag); 

void print_data(AT_COMMAND_DATA data);

void reset_automaton(uint32_t *state, AT_COMMAND_DATA data);

#endif