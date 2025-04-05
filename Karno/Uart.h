// Uart.h

#include <avr/io.h>

#ifndef UART_H
#define UART_H

#define F_CPU 8000000UL 
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

// прототипы функций
void USARTInit(unsigned int ubrr); 	           // Инициализация UART
void USARTTransmitChar(char c); 	           // Отправка байта
void USARTTransmitString(char str[]);     	   // Отправка строки
void USARTTransmitStringLn(char str[]);  	   // Отправка строки
char USARTReceiveChar(void); 	               // Получение байта
void USARTReceiveString(char* buf, uint8_t n); // Получение строки


#endif // UART_H
