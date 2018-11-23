#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "utils/timer_software_init.h"
#include "utils/timer_software.h"
#include "drv/drv_led.h"
#include "drv/drv_uart.h"
#include "drv/drv_sdram.h"
#include "drv/drv_lcd.h"
#include "b.h"
#include "a.h"

AT_COMMAND_DATA data_structure;
const uint8_t*  at_command_simple="AT\r\n";
const uint8_t*  at_command_csq = "AT+CSQ\r\n";
timer_software_handler_t my_timer_handler;
timer_software_handler_t response_timer_handler;

void ExecuteCommand(const uint8_t *command)
{
 SendCommand(command);
 GetCommandResponse(command);
} 

void SendCommand(const uint8_t *command)
{
 DRV_UART_FlushRX(UART_3);
 DRV_UART_FlushTX(UART_3);
 char * my_command = strdup(command);
 DRV_UART_Write(UART_3, my_command, strlen(my_command));
} 

//timer_software_handler_t my_handler;
// replaced the above with the global one

void GetCommandResponse()
{
  // parse and extract data
  uint8_t ch;
  bool ready = false;
  TIMER_SOFTWARE_reset_timer(response_timer_handler);
  TIMER_SOFTWARE_start_timer(response_timer_handler);
  while ((!TIMER_SOFTWARE_interrupt_pending(response_timer_handler)) && (ready == false))
  {
    while (DRV_UART_BytesAvailable(UART_3) > 0)
    {
      DRV_UART_ReadByte(UART_3, &ch);
      if (at_command_parse(ch) != STATE_MACHINE_NOT_READY)
      {
        ready = true;
      }
    }
  }
} 

uint32_t ConvertAsuToDbmw(uint32_t rssi_value_asu){
  return (rssi_value_asu + 113)/2;
}

uint32_t ExtractAsu(AT_COMMAND_DATA *data_structure){
  //  @TODO: implement this

	//idea: add an data_structure parameter to the parser function, and define it globally here in order to have it available for the function 
}

bool CommandResponseValid(){
  // @TODO: implement this
	//idea: define a global variable of type STATE_MACHINE_RETURN_VALUE, instantiate it in GetCommandResponse() and return true here only if it is STATE_MACHINE_READY_OK, STATE_MACHINE_READY_WITH_ERROR
}

void BoardInit()
{
	DRV_SDRAM_Init();
	
	initRetargetDebugSystem();
	DRV_LCD_Init();
	DRV_LCD_ClrScr();
	DRV_LCD_PowerOn();	

	printf ("Hello\n");		
}

int main(void)
{

  uint32_t rssi_value_asu;
  uint32_t rssi_value_dbmw;

// board init
  BoardInit();

// configure UART_3
  DRV_UART_Configure(UART_3, UART_CHARACTER_LENGTH_8, 115200, UART_PARITY_NO_PARITY, 1, TRUE, 3);

// init, configure and start timer
  TIMER_SOFTWARE_init_system(); 
  my_timer_handler = TIMER_SOFTWARE_request_timer();
  response_timer_handler = TIMER_SOFTWARE_request_timer();
  if (my_timer_handler < 0 || response_timer_handler < 0) 
  {
    printf("Timers could not be init.");
  } 
  //a request to the GSM modem should be sent every 1 sec (therefore timer is configured with 1000ms)
  TIMER_SOFTWARE_configure_timer(my_timer_handler, MODE_1, 1000, true);
  //internal timer configuration, if GSM Modem gets blocked after the request, execution will still continue after 500ms
  TIMER_SOFTWARE_configure_timer(response_timer_handler, MODE_1, 500, true);
  TIMER_SOFTWARE_start_timer(my_timer_handler); 


// establish connection to UART3 by sending AT command 3 times / configure the GSM modem AUTOBAUD by sending the AUTOBAUD pattern 3 times 
  for(int i=0;i<3;i++){
    char * my_at_command_simple = strdup(at_command_simple);
    DRV_UART_Write(UART_3, my_at_command_simple, strlen(my_at_command_simple));
    TIMER_SOFTWARE_Wait(1000);
  }


// connection established by now
// 1 time per second execute command and validate the response
  while (1)
  {
    if (TIMER_SOFTWARE_interrupt_pending(my_timer_handler))
    {
      ExecuteCommand(at_command_csq);
      if (CommandResponseValid())
      {
        rssi_value_asu = ExtractAsu(&data_structure);
        rssi_value_dbmw = ConvertAsuToDbmw(rssi_value_asu);
        printf("ASU value: %d; dBmW value: %d \n",rssi_value_asu,rssi_value_dbmw);
      }
      TIMER_SOFTWARE_clear_interrupt(my_timer_handler);
    }
  }

} 