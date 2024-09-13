#ifndef DGH_ESP8266_H
#define DGH_ESP8266_H

#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "string.h"


extern UART_HandleTypeDef huart1;
extern char robot_status[2];

void TestFunction(uint32_t delay);
void ESP_ClearRdata(uint16_t cnt);  


HAL_StatusTypeDef ESP_Reset(void);   
HAL_StatusTypeDef ESP_IsDeviceReady(void);  //OK
HAL_StatusTypeDef ESP_ConnectToAP(const char* name, const char* pass);
HAL_StatusTypeDef ESP_CheckStatus(void);  //OK
HAL_StatusTypeDef ESP_StartConnection(const char* ip, uint16_t port);
void ESP_Transmit(char xyz, char pn);
HAL_StatusTypeDef ESP_TCPServer();

	
#endif
