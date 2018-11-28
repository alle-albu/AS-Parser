#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <DRV\drv_sdram.h>
#include <DRV\drv_lcd.h>
#include <DRV\drv_uart.h>
#include <DRV\drv_touchscreen.h>
#include <DRV\drv_led.h>
#include <utils\timer_software.h>
#include <utils\timer_software_init.h>
#include "LPC177x_8x.h"
#include "system_LPC177x_8x.h"
#include <retarget.h>
#include "a.h"

AT_COMMAND_DATA commandData;

uint8_t imeiData[AT_COMMAND_MAX_LINE_SIZE];
uint8_t registrationStateData[AT_COMMAND_MAX_LINE_SIZE];
uint8_t operatorData[AT_COMMAND_MAX_LINE_SIZE];
uint8_t manufacturerData[AT_COMMAND_MAX_LINE_SIZE];
uint8_t softwareData[AT_COMMAND_MAX_LINE_SIZE];

STATE_MACHINE_RETURN_VALUE finalState = STATE_MACHINE_NOT_READY;

const char*  at_command_simple="AT\r\n";
const char*  at_command_csq = "AT+CSQ\r\n";
const char*  at_command_creg = "AT+CREG\r\n";
const char*  at_command_cops = "AT+COPS\r\n";
const char*  at_command_gsn = "AT+GSN\r\n";
const char*  at_command_gmi = "AT+GMI\r\n";
const char*  at_command_gmr = "AT+GMR\r\n";
timer_software_handler_t my_timer_handler;

void CREG_network_registration_state(){
    char i[1];
    i[0] = commandData.data[0][9];
    //9 sau 10 depending on the space after ,
    uint32_t index = atoi(i);
    switch(index) {
        case 0: {
            printf("Modem is not registered in the network and is not searching for a network ");
            break;}
        case 1: {
            printf("Modem is registered to home network");
            break;}
        case 2: {
            printf("Modem is not registered but it is currently searching for a network");
            break;}
        case 3: {
            printf("Modem registration into the network was denied ");
            break;}
        case 4: {
            printf("Unknown modem registration state");
        break;}
        case 5: {
            printf("Modem is registered to roaming network ");
        break;}
    }
}
void COPS_network_operator_name(){
    uint8_t i,j,k;
    k=0;
    for(i=0; i<=commandData.line_count; i++) {
        j=0;
        while(commandData.data[i][j]!=34) j++;
        for(j++; j<AT_COMMAND_MAX_LINE_SIZE && commandData.data[i][j]!=34; j++) {
            operatorData[k++] = commandData.data[i][j];
        }
    }
    printf("operator name is: ");
    for(i=0;i<k;i++){
       printf("%c",operatorData[i]);
    }
}

void GSN_imei(){
    uint8_t j,i;
    for(j=0; commandData.data[0][j]>=32; j++) {
       imeiData[j] = commandData.data[0][j];
    }
    printf("imei is: ");
    for(i=0;i<j;i++){
       printf("%c",imeiData[i]);
    }
}

void GMI_manufacturer(){
    uint8_t j,i;
    for(j=0; commandData.data[0][j]>=32; j++) {
        manufacturerData[j] = commandData.data[0][j];
    }
    printf("manufacturer name is: ");
    for(i=0;i<j;i++){
        printf("%c",manufacturerData[i]);
    }
}

void GMR_software_version() {
    uint8_t i,j;
    j=0;
    i=0;
    while(commandData.data[0][j]!=58) j++;
    for(j=j+2; j<AT_COMMAND_MAX_LINE_SIZE && commandData.data[0][j]>=33; j++) {
        softwareData[i++] = commandData.data[0][j];
    }
    printf("software version is: ");
    for(j=0;j<i;j++){
        printf("%c",softwareData[j]);
    }
}

void SendCommand(const char *command)
{
 DRV_UART_FlushRX(UART_3);
 DRV_UART_FlushTX(UART_3);
 DRV_UART_Write(UART_3, (uint8_t *)command, strlen(command));
} 

timer_software_handler_t my_handler;
// replaced the above with the global one

void GetCommandResponse()
{
  // parse and extract data
  uint8_t ch;
	
  bool ready = false;
  TIMER_SOFTWARE_reset_timer(my_handler);
  TIMER_SOFTWARE_start_timer(my_handler);
  while ((!TIMER_SOFTWARE_interrupt_pending(my_handler)) && (ready == false))
  {
		
    while (DRV_UART_BytesAvailable(UART_3) > 0)
    {
      DRV_UART_ReadByte(UART_3, &ch);
			
      if ((finalState=at_command_parse(ch,1)) != STATE_MACHINE_NOT_READY)
      {
        ready = true;
      }
    }
  }
} 

void ExecuteCommand(const char *command)
{
	SendCommand(command);
  GetCommandResponse();
} 

uint32_t ConvertAsuToDbmw(uint32_t rssi_value_asu){
  return (rssi_value_asu + 113)/2;
}

uint32_t ExtractAsuu(){
	char ab[2];
	ab[0] = commandData.data[0][6]; ab[1]=commandData.data[0][7];
	return atoi(ab);
}

bool CommandResponseValid(){
	if(finalState!=0 && finalState!=3){
		return true;
	}
	return false;
}

void BoardInit()
{
	DRV_SDRAM_Init();
	TIMER_SOFTWARE_init_system();

	initRetargetDebugSystem();
	DRV_LCD_Init();
	DRV_LCD_ClrScr();
	DRV_LCD_PowerOn();	

	printf ("Hello\n");
TIMER_SOFTWARE_Wait(1000);	
}

int main(void)
{

	uint32_t i;
  uint32_t rssi_value_asu;
  uint32_t rssi_value_dbmw;

// board init
  BoardInit();

// configure UART_3
  DRV_UART_Configure(UART_3, UART_CHARACTER_LENGTH_8, 115200, UART_PARITY_NO_PARITY, 1, TRUE, 3);
  DRV_UART_Configure(UART_0, UART_CHARACTER_LENGTH_8, 115200, UART_PARITY_NO_PARITY, 1, TRUE, 3);

// init, configure and start timer
  my_timer_handler = TIMER_SOFTWARE_request_timer(); 
	my_handler = TIMER_SOFTWARE_request_timer(); 
  if (my_timer_handler < 0) 
  {
    printf("Timer could not be init.\n");
		TIMER_SOFTWARE_Wait(1000);
  } 
  TIMER_SOFTWARE_configure_timer(my_timer_handler, MODE_1, 5000, true);
	TIMER_SOFTWARE_configure_timer(my_handler, MODE_1, 30000, true);


// establish connection to UART3 by sending AT command 3 times
  for(i=0;i<3;i++){
    DRV_UART_Write(UART_3, (uint8_t *)at_command_simple, strlen(at_command_simple));
    TIMER_SOFTWARE_Wait(1000);
  }
	
// connection established by now
// 1 time per second execute command and validate the response
	 
	TIMER_SOFTWARE_start_timer(my_timer_handler);
  while (1)
  {
    if (TIMER_SOFTWARE_interrupt_pending(my_timer_handler))
    {
      
			ExecuteCommand(at_command_csq);
      if (CommandResponseValid())
      {
			
        rssi_value_asu = ExtractAsuu();
        rssi_value_dbmw = ConvertAsuToDbmw(rssi_value_asu);
        printf("ASU value: %d; dBmW value: %d \n",rssi_value_asu,rssi_value_dbmw);
				TIMER_SOFTWARE_Wait(1000);
      }
      TIMER_SOFTWARE_clear_interrupt(my_timer_handler);
    }
  }

}