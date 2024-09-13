#include "dgh_esp8266.h"

void TestFunction(uint32_t delay)
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	HAL_Delay(delay);
}

//GLOBAL VARIABLES
uint8_t esp_Rdata[200];
char esp_Tdata[2];
uint8_t direction[10];
uint8_t tempData[1];
char str[80];
uint16_t dbg = 0;
uint8_t flag = 0;

HAL_StatusTypeDef ESP_Reset(void)
{
	ESP_ClearRdata(200);
	HAL_UART_Receive_IT(&huart1, tempData, 1);
	
	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+RST\r\n", strlen("AT+RST\r\n"), 1000);
	//search of "ready" in Rdata
	while( strstr( (char*) esp_Rdata, "ready") == 0 ){}  

	return HAL_OK;
}

HAL_StatusTypeDef ESP_IsDeviceReady(void)
{
	ESP_ClearRdata(200);
	HAL_UART_Receive_IT(&huart1, tempData, 1);
	
	HAL_UART_Transmit(&huart1, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
	//search of "OK" in Rdata
	while( strstr( (char*) esp_Rdata, "OK") == 0 ){}  

	return HAL_OK;
}

HAL_StatusTypeDef ESP_ConnectToAP(const char* name, const char* pass)
{
	ESP_ClearRdata(200);
	HAL_UART_Receive_IT(&huart1, tempData, 1);
	
	sprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n", name, pass);
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
	
	//search of "OK" in Rdata
	while( strstr( (char*) esp_Rdata, "OK") == 0 ){}  
		
	return HAL_OK;
}

void ESP_Transmit(char xyz, char pn)
{
	ESP_ClearRdata(200);
	HAL_UART_Receive_IT(&huart1, tempData, 1);
	dbg = 1;
	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPSEND=2\r\n", strlen("AT+CIPSEND=2\r\n"), 1000);
	dbg = 2;
	//search of "OK" in Rdata
	while( strstr( (char*) esp_Rdata, "OK") == 0 ){} 
	dbg = 3;
	sprintf(esp_Tdata, "%c%c", xyz, pn);
	HAL_Delay(5);
	HAL_UART_Transmit(&huart1, (uint8_t*) esp_Tdata, 2, 1000);
	dbg = 4;
	//search of "OK" in Rdata
	while( strstr( (char*) esp_Rdata, "bytes") == 0 )
	{
		if(strstr( (char*) esp_Rdata, "busy") != 0)
			break;
	}  
	dbg = 5;
}

HAL_StatusTypeDef ESP_CheckStatus(void)
{
	ESP_ClearRdata(200);
	HAL_UART_Receive_IT(&huart1, tempData, 1);
	
	sprintf(str, "AT+CIPSTATUS\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
	
	//search of "STATUS:2" in Rdata
	while( strstr( (char*) esp_Rdata, "STATUS:2") == 0 ){}  
		
	return HAL_OK;
}

HAL_StatusTypeDef ESP_StartConnection(const char* ip, uint16_t port)
{
	ESP_ClearRdata(200);
	HAL_UART_Receive_IT(&huart1, tempData, 1);
	
	sprintf(str, "AT+CIPSTART=\"TCP\",\"%s\",%i\r\n", ip, port);
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
//	uint8_t size = strlen("AT+CIPSTART=\"TCP\",\"192.168.43.229\",5005\r\n");
//	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPSTART=\"TCP\",\"192.168.43.229\",5005\r\n", size, 1000);
	
	while( strstr( (char*) esp_Rdata, "OK") == 0 )
	{
		if( strstr( (char*) esp_Rdata, "ERROR") != 0)
			return HAL_ERROR;
	} 
	return HAL_OK;
}

void ESP_ClearRdata(uint16_t cnt)
{
	for(uint16_t i=0; i<cnt; i++)  
		esp_Rdata[i] = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	if(robot_status == 'i' && esp_Rdata[199] == 0)
//	{
		strcat( (char*) esp_Rdata, (char*) tempData);
		HAL_UART_Receive_IT(&huart1, tempData, 1);
//	}
//	else
//	{
//		dbg+=1;
//		esp_Rdata[199] = 1; //flag
//		robot_status = direction[8];
//		
//		for(uint8_t i=0; i<10; i++)
//			direction[i] = 0;
//		HAL_UART_Receive_IT(&huart1, direction, 10);
//	}
}
