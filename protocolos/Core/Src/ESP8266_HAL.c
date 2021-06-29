/*
 *
 * Protocolo de comunicacion
 * Ing. Josue Huaman
 *
 */


#include "UartRingbuffer_multi.h"
#include "ESP8266_HAL.h"
#include "stdio.h"
#include "vehiculo.h"
#include "string.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern myQueue01Handle;

#define wifi_uart &huart1
#define pc_uart &huart2

void vehiculo(int accion);

char buffer[20];

char *Basic_inclusion =
		"<!DOCTYPE html> <html>\n<head><meta name=\"viewport\"\
		content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n\
		<title>LED CONTROL</title>\n<style>html { font-family: Helvetica; \
		display: inline-block; margin: 0px auto; text-align: center;}\n\
		body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\
		h3 {color: #444444;margin-bottom: 50px;}\n.button {display: block;\
		width: 80px;background-color: #1abc9c;border: none;color: white;\
		padding: 13px 30px;text-decoration: none;font-size: 25px;\
		margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n\
		.button-on {background-color: #1abc9c;}\n.button-on:active \
		{background-color: #16a085;}\n.button-off {background-color: #34495e;}\n\
		.button-off:active {background-color: #2c3e50;}\np {font-size: 14px;color: #888;margin-bottom: 10px;}\n\
		</style>\n</head>\n<body>\n<h1>Vehiculo controlado por WiFi</h1>\n";

/*****************************************************************************************************************************************/

/*
 *
 * Inicializa ESP...
 *
 * */
void ESP_Init(char *SSID, char *PASSWD) {
	char data[80];

	Ringbuf_init();

	Uart_sendstring("AT+RST\r\n", wifi_uart);
	Uart_sendstring("RESETTING.", pc_uart);
	for (int i = 0; i < 5; i++) {
		Uart_sendstring(".", pc_uart);
		HAL_Delay(1000);
	}

	/********* AT **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT\r\n", wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)))
		;
	Uart_sendstring("AT---->OK\n\r", pc_uart);

	/********* AT+CWMODE=1 **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT+CWMODE=1\r\n", wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)))
		;
	Uart_sendstring("CW MODE---->1\n\r", pc_uart);

	/********* AT+CWJAP="SSID","PASSWD" **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("connecting... to the provided AP\n\r", pc_uart);
	sprintf(data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)))
		;
	sprintf(data, "Connected to,\"%s\"\n\r", SSID);
	Uart_sendstring(data, pc_uart);

	/********* AT+CIFSR **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT+CIFSR\r\n", wifi_uart);
	while (!(Wait_for("CIFSR:STAIP,\"", wifi_uart)))
		;
	while (!(Copy_upto("\"", buffer, wifi_uart)))
		;
	while (!(Wait_for("OK\r\n", wifi_uart)))
		;
	int len = strlen(buffer);
	buffer[len - 1] = '\0';
	sprintf(data, "IP ADDR: %s\n\r", buffer);
	Uart_sendstring(data, pc_uart);

	/********* AT+CIPMUX **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT+CIPMUX=1\r\n", wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)))
		;
	Uart_sendstring("CIPMUX---->OK\n\r", pc_uart);

	/********* AT+CIPSERVER **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT+CIPSERVER=1,80\r\n", wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)))
		;
	Uart_sendstring("CIPSERVER---->OK\n\r", pc_uart);

	Uart_sendstring("Now Connect to the IP ADRESS\n\r", pc_uart);

}

int Server_Send(char *str, int Link_ID) {
	int len = strlen(str);
	char data[80];
	sprintf(data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for(">", wifi_uart)))
		;
	Uart_sendstring(str, wifi_uart);
	while (!(Wait_for("SEND OK", wifi_uart)))
		;
	sprintf(data, "AT+CIPCLOSE=5\r\n");
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)));
	return 1;
}

void Server_Handle(char *str, int Link_ID) {
	char datatosend[1024] = { 0 };
	if (!(strcmp(str, "/retrocede"))) {
		sprintf(datatosend, Basic_inclusion);
		strcat(datatosend, "<h2>El carro retrocede</h2>");
		strcat(datatosend, "</body></html>");
		Server_Send(datatosend, Link_ID);
	}
	else if (!(strcmp(str, "/avanza"))) {
		sprintf(datatosend, Basic_inclusion);
		strcat(datatosend, "<h2>El carro avanza</h2>");
		strcat(datatosend, "</body></html>");
		Server_Send(datatosend, Link_ID);
	}
	else if (!(strcmp(str, "/detente"))) {
		sprintf(datatosend, Basic_inclusion);
		strcat(datatosend, "<h2>El carro se detuvo</h2>");
		strcat(datatosend, "</body></html>");
		Server_Send(datatosend, Link_ID);
	}
	else {
		sprintf(datatosend, Basic_inclusion);
		strcat(datatosend, "<h2>De una orden al carro.</h2>");
		strcat(datatosend, "</body></html>");
		Server_Send(datatosend, Link_ID);
	}
}

void Server_Start(void) {
	int a;
	char buftocopyinto[64] = { 0 };
	char Link_ID;
	/*recibe la informacion y lo almacena en el buffer del uart */
	while (!(Get_after("+IPD,", 1, &Link_ID, wifi_uart)));
	Link_ID -= 48;
	/* ya esta en el buffer del UART */
	while (!(Copy_upto(" HTTP/1.1", buftocopyinto, wifi_uart)));
	//Uart_sendstring(buftocopyinto, pc_uart);
	//Uart_sendstring("\n\n\r", pc_uart);

	if (Look_for("/retrocede", buftocopyinto) == 1) {
		vehiculo(0);
		Server_Handle("/retrocede", Link_ID);
		a = 1;
		osMessageQueuePut(myQueue01Handle, &a, 0, 0);
	}

	else if (Look_for("/avanza", buftocopyinto) == 1) {
		vehiculo(1);
		Server_Handle("/avanza", Link_ID);
		a = 0;
		osMessageQueuePut(myQueue01Handle, &a, 0, 0);
	}

	else if (Look_for("/detente", buftocopyinto) == 1){
		vehiculo(2);
		Server_Handle("/detente", Link_ID);
		a = 2;
		osMessageQueuePut(myQueue01Handle, &a, 0, 0);
	}
}
