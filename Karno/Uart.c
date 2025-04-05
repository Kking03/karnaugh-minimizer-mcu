#include "Uart.h"

#include <stdlib.h> // ��� ������� itoa

void USARTInit(unsigned int ubrr) {
  //  ���������� ����������� ��������������� ����� ������
  //  UBRR = f / (16 * band)
  //  ��������� ��������
  UBRRH = (unsigned char)(ubrr>>8);
  UBRRL = (unsigned char)(ubrr);

  // ��������� ������� �����: 8 ��� ������, 1 ����-���, ��� ��������
  UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

  // ��������� ��������� � �����������
  UCSRB = (1 << RXEN) | (1 << TXEN);
}

//  �������� �����
void USARTTransmitChar(char c) {
  //  ���������������, ����� ������� ��������
  while(!( UCSRA & (1<<UDRE)));
  UDR = c;
}

//  �������� ������
void USARTTransmitString(char str[]) {
  register char i = 0;
  while(str[i]) {
    USARTTransmitChar(str[i++]);
  }
}

//  �������� ������
void USARTTransmitStringLn(char str[]) {
  USARTTransmitString(str);
  USARTTransmitChar((char)13);
  USARTTransmitChar((char)10);
}

//  ��������� �����
char USARTReceiveChar(void) {
  //  ���������������, ����� ������� ��������
  while(!(UCSRA & (1<<RXC)));
  return UDR;
}

// ��������� ������
void USARTReceiveString(char* buf, uint8_t n) {
  uint8_t bufIdx = 0;
  char c;

  do
  {
    c = USARTReceiveChar();   // �������� ������

    buf[bufIdx++] = c; // ���������� ������ � ������
  }
  while((bufIdx < n) && (c != '\r'));

  buf[bufIdx-1] = 0;     // ��������� ������ ����-������������
}
