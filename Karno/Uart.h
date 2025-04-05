// Uart.h

#include <avr/io.h>

#ifndef UART_H
#define UART_H

#define F_CPU 8000000UL 
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

// ��������� �������
void USARTInit(unsigned int ubrr); 	           // ������������� UART
void USARTTransmitChar(char c); 	           // �������� �����
void USARTTransmitString(char str[]);     	   // �������� ������
void USARTTransmitStringLn(char str[]);  	   // �������� ������
char USARTReceiveChar(void); 	               // ��������� �����
void USARTReceiveString(char* buf, uint8_t n); // ��������� ������


#endif // UART_H
