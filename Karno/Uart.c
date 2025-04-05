#include "Uart.h"

#include <stdlib.h> // Для функции itoa

void USARTInit(unsigned int ubrr) {
  //  нормальный асинхронный двунаправленный режим работы
  //  UBRR = f / (16 * band)
  //  Установка скорости
  UBRRH = (unsigned char)(ubrr>>8);
  UBRRL = (unsigned char)(ubrr);

  // Настройка формата кадра: 8 бит данных, 1 стоп-бит, без четности
  UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

  // Включение приемника и передатчика
  UCSRB = (1 << RXEN) | (1 << TXEN);
}

//  Отправка байта
void USARTTransmitChar(char c) {
  //  Устанавливается, когда регистр свободен
  while(!( UCSRA & (1<<UDRE)));
  UDR = c;
}

//  Отправка строки
void USARTTransmitString(char str[]) {
  register char i = 0;
  while(str[i]) {
    USARTTransmitChar(str[i++]);
  }
}

//  Отправка строки
void USARTTransmitStringLn(char str[]) {
  USARTTransmitString(str);
  USARTTransmitChar((char)13);
  USARTTransmitChar((char)10);
}

//  Получение байта
char USARTReceiveChar(void) {
  //  Устанавливается, когда регистр свободен
  while(!(UCSRA & (1<<RXC)));
  return UDR;
}

// Получение строки
void USARTReceiveString(char* buf, uint8_t n) {
  uint8_t bufIdx = 0;
  char c;

  do
  {
    c = USARTReceiveChar();   // получаем символ

    buf[bufIdx++] = c; // Записываем символ в строку
  }
  while((bufIdx < n) && (c != '\r'));

  buf[bufIdx-1] = 0;     // Завершаем строку нулём-терминатором
}
