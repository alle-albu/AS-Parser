#ifndef B_H
#define B_H

#include "a.h"
#include<stdbool.h>

void ExecuteCommand(const uint8_t *command);

void SendCommand(const uint8_t *command);

void GetCommandResponse();

uint32_t ConvertAsuToDbmw(uint32_t rssi_value_asu);

uint32_t ExtractAsu(AT_COMMAND_DATA *data_structure);

bool CommandResponseValid();

#endif