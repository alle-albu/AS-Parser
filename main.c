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
uint8_t csqResp[AT_COMMAND_MAX_LINE_SIZE];

STATE_MACHINE_RETURN_VALUE finalState = STATE_MACHINE_NOT_READY;

const char*  at_command_simple="AT\r\n";
const char*  at_command_csq = "AT+CSQ\r\n";
const char*  at_command_creg = "AT+CREG?\r\n";
const char*  at_command_cops = "AT+COPS?\r\n";
const char*  at_command_gsn = "AT+GSN\r\n";
const char*  at_command_gmi = "AT+GMI\r\n";
const char*  at_command_gmr = "AT+GMR\r\n";
timer_software_handler_t my_timer_handler;
uint8_t registeredFlag = 0;

// for LCD printing
LCD_PIXEL txtColor;
LCD_PIXEL bkgColor;
LCD_PIXEL yellowColor;
		txtColor.red = 0;
		txtColor.green = 0;
		txtColor.blue = 255;
       bkgColor.red = 255;
		bkgColor.green = 0;
		bkgColor.blue = 0;
	yellowColor.red = 255;
		yellowColor.green = 255;
		yellowColor.blue = 0;

// for assignment 1.8 
// i = current index of sms in SMSArray
// n = nr of elements in SMSArray
const uint8_t* const SMSArray[] = { "foo", "bar", "aaa" };
uint8_t i = 0;
size_t n = sizeof(arr) / sizeof(uint8_t*);
	

void CREG_network_registration_state(){
    char i[1];
		uint32_t index;
    i[0] = commandData.data[0][9];
    //9 sau 10 depending on the space after ,
    index = atoi(i);
    switch(index) {
        case 0: {
					strcpy((char *)registrationStateData,"not registered; not searching");
            printf("Modem is not registered in the network and is not searching for a network\n");
            break;}
        case 1: {
					strcpy((char *)registrationStateData,"registered to home network");
            printf("Modem is registered to home network\n");
					registeredFlag = 1;
            break;}
        case 2: {
					strcpy((char *)registrationStateData,"not registered;currently searching");
            printf("Modem is not registered but it is currently searching for a network\n");
            break;}
        case 3: {
            printf("Modem registration into the network was denied\n");
            break;}
        case 4: {
            printf("Unknown modem registration state\n");
        break;}
        case 5: {
            printf("Modem is registered to roaming network\n");
        break;}
    }
}
void COPS_network_operator_name(){
    uint8_t i,j,k;
    k=0;
	if(registeredFlag == 1) {
    for(i=0; i<=commandData.line_count; i++) {
        j=0;
        while(commandData.data[i][j]!=34) j++;
        for(j++; j<AT_COMMAND_MAX_LINE_SIZE && commandData.data[i][j]!=34; j++) {
            operatorData[k++] = commandData.data[i][j];
        }
    }
    printf("Operator name is: ");
    for(i=0;i<k;i++){
       printf("%c",operatorData[i]);
    }
		//memset(&operatorData, 0, sizeof (operatorData));
		printf("\n");
	}
}

void GSN_imei(){
    uint8_t j,i;
    for(j=0; commandData.data[0][j]>=32; j++) {
       imeiData[j] = commandData.data[0][j];
    }
    printf("Imei is: ");
    for(i=0;i<j;i++){
       printf("%c",imeiData[i]);
    }
		//memset(&imeiData, 0, sizeof (imeiData));
		printf("\n");
}

void GMI_manufacturer(){
    uint8_t j,i;
    for(j=0; commandData.data[0][j]>=32; j++) {
        manufacturerData[j] = commandData.data[0][j];
    }
    printf("Manufacturer name is: ");
    for(i=0;i<j;i++){
        printf("%c",manufacturerData[i]);
    }
		//memset(&manufacturerData, 0, sizeof (manufacturerData));
		printf("\n");
}

void GMR_software_version() {
    uint8_t i,j;
    j=0;
    i=0;
    while(commandData.data[0][j]!=58) j++;
    for(j=j+2; j<AT_COMMAND_MAX_LINE_SIZE && commandData.data[0][j]>=33; j++) {
        softwareData[i++] = commandData.data[0][j];
    }
    printf("Software version is: ");
    for(j=0;j<i;j++){
        printf("%c",softwareData[j]);
    }
		//memset(&softwareData, 0, sizeof (softwareData));
		printf("\n");
}

void SendCommand(const char *command)
{
 DRV_UART_FlushRX(UART_3);
 DRV_UART_FlushTX(UART_3);
 DRV_UART_Write(UART_3, (uint8_t *)command, strlen(command));
	//printf("Command sent\n");
} 

timer_software_handler_t my_handler;
// replaced the above with the global one

void GetCommandResponse()
{
  // parse and extract data
  uint8_t ch;
	
  bool ready = false;
	//printf("Waiting 4 response\n");
  TIMER_SOFTWARE_reset_timer(my_handler);
  TIMER_SOFTWARE_start_timer(my_handler);
  while ((!TIMER_SOFTWARE_interrupt_pending(my_handler)) && (ready == false))
  {
		
    while (DRV_UART_BytesAvailable(UART_3) > 0)
    {
      DRV_UART_ReadByte(UART_3, &ch);
			//printf("response char is: --%c-- \n", ch);
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
	//printf("final state is %d\n", finalState);
	if(finalState!=0 && finalState!=3){
		return true;
	}
	return false;
}



void CSQ() {
	uint32_t rssi_value_asu;
  uint32_t rssi_value_dbmw;
	rssi_value_asu = ExtractAsuu();
	rssi_value_dbmw = ConvertAsuToDbmw(rssi_value_asu);
	
	printf("ASU value: %d; dBmW value: %d \n",rssi_value_asu,rssi_value_dbmw);
	//strcpy((char *)csqResp,strcat((char *)rssi_value_asu,(char *)rssi_value_dbmw));
	
	
	TIMER_SOFTWARE_Wait(1000);
	sprintf((char *)csqResp, "asu: %d, dbmw: %d", rssi_value_asu, rssi_value_dbmw);
}

void drawBtn(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2,char * txt, LCD_PIXEL txtColor, LCD_PIXEL bkgColor){
	uint32_t i;
	uint32_t j;
	for(i=x1;i<=x2;i++)
		for(j=y1;j<=y2;j++)
			DRV_LCD_PutPixel(i,j,0,255,0);
	
	bkgColor.red = 0;
	bkgColor.green = 255;
	
	DRV_LCD_Puts(txt,y1+(y2-y1)/2,x1+(x2-x1)/2,txtColor,bkgColor,false);
}

void goToNextEl(){
    if(i+1 == n) i = 0;
    else i = i+1;
}

void goToPrevEl(){
    if(i==0) i = n-1;
    else i = i-1;
}

void deleteEl(){
    uint8_t c;
     for (c = i ; c < n - 1; c++)
    	SMSArray[c] = SMSArray[c+1];
    n = n-1;
    while(i>=n) i--; 
}

void processTouch(uint8_t x,uint8_t y){
    const uint8_t* staticSMS = "this is a randomly generated sms";
	
	//@TODO: complete this
	if(x<100&&x>20 && y>120&&y<150) prev;
	..
		
		
		// for prev
		goToPrevEl();
		DRV_LCD_Puts((char *)SMSAray[i],20,0,txtColor,bkgColor,true);
		// for next
		goToNextEl();
		DRV_LCD_Puts((char *)SMSAray[i],20,0,txtColor,bkgColor,true);
		// for send static SMS
		DRV_LCD_Puts((char *)staticSMS,20,0,txtColor,bkgColor,true);
		// for delete SMS ; I assumed position of SMS to be deleted in SMSArray is i
		deleteEl();
		DRV_LCD_Puts((char *)SMSAray[i],20,0,txtColor,bkgColor,true);
}

void printLCD(){
	
	DRV_LCD_ClrScr();
	DRV_LCD_Puts((char *)registrationStateData,20,0,txtColor,bkgColor,true);
	DRV_LCD_Puts((char *)operatorData,20,30,txtColor,bkgColor,true);
	DRV_LCD_Puts((char *)csqResp,20,60,txtColor,bkgColor,true);
	
	//SMS buttons 
	// @TODO: delete txtColor, bkgColor params because they are now global
	drawBtn(120,20,150,100,"prev",txtColor,bkgColor);
	drawBtn(120,120,150,200,"next",txtColor,bkgColor);
	drawBtn(120,220,150,300,"del",txtColor,bkgColor);
	drawBtn(120,320,150,400,"send",txtColor,bkgColor);
	
	//SMS preview
	drawBtn(170,20,250,400,"SMS preview",txtColor,yellowColor);
}

void MyTouchScreenCallBack(TouchResult* touchData){
printf("touched X=%3d Y=%3d\n", touchData->X, touchData->Y); 
	TIMER_SOFTWARE_Wait(1000);	
	processTouch(touchData->X,touchData->Y);
}

void BoardInit()
{
	DRV_SDRAM_Init();
	TIMER_SOFTWARE_init_system();

	initRetargetDebugSystem();
	DRV_LCD_Init();
	DRV_LCD_ClrScr();
	DRV_LCD_PowerOn();	
	
	//config touchscreen init & callback
	 DRV_TOUCHSCREEN_Init(); 
   DRV_TOUCHSCREEN_SetTouchCallback(MyTouchScreenCallBack); 

	printf ("Hello\n");
TIMER_SOFTWARE_Wait(1000);	
}

int main(void)
{

	uint32_t i;
  //uint32_t rssi_value_asu;
  //uint32_t rssi_value_dbmw;

// board init
  BoardInit();

	//DRV_LCD_TestFillColor(255,0,0);
	//DRV_LCD_Puts("test string",100,100,txtColor,bkgColor,true);
	
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
			TIMER_SOFTWARE_Wait(1000);
      if (CommandResponseValid())
      {
				CSQ();
      }
			
			memset(&commandData, 0, sizeof (commandData));
			
			ExecuteCommand(at_command_creg);
			TIMER_SOFTWARE_Wait(1000);
      if (CommandResponseValid())
      {
				CREG_network_registration_state();
      }	
			
			memset(&commandData, 0, sizeof (commandData));
			
			ExecuteCommand(at_command_gsn);
			TIMER_SOFTWARE_Wait(1000);
      if (CommandResponseValid())
      {
				GSN_imei();
      }
			
			memset(&commandData, 0, sizeof (commandData));
	
			ExecuteCommand(at_command_gmi);
			TIMER_SOFTWARE_Wait(200);
      if (CommandResponseValid())
      {
				GMI_manufacturer();
      }
			
			memset(&commandData, 0, sizeof (commandData));
			
			ExecuteCommand(at_command_gmr);
			TIMER_SOFTWARE_Wait(1000);
      if (CommandResponseValid())
      {
				GMR_software_version();
      }

			memset(&commandData, 0, sizeof (commandData));
			
			 
			
			ExecuteCommand(at_command_cops);
			TIMER_SOFTWARE_Wait(1000);
      if (CommandResponseValid())
      {
				COPS_network_operator_name();
      }
			
			memset(&commandData, 0, sizeof (commandData));
			
			 printLCD();
			
      TIMER_SOFTWARE_clear_interrupt(my_timer_handler);
    }
			DRV_TOUCHSCREEN_Process();
  }
}
