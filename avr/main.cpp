#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>

#include <messagehandler.h>
#include <messagetranslationsenter.h>
#include <message.h>
#include <usart.h>

#define F_CPU 12000000UL

MessageHandler mh;
MessageTranslationSenter mts;
volatile bool lock;

void initTimer();

void messageCallback(Message *&msg);
void requestDeviceName();
void requestCreateTags();

int main()
{
	initTimer();
	USART_init();
	mh.init();
	mh.setCallback(messageCallback);

	mts.init();
	mts.setDeviceNameFunc(requestDeviceName);
	mts.setCreateTagsFunc(requestCreateTags);

	lock = false;
	sei();

	while(true)
	{

	}

	return 0;
}

ISR(TIMER1_COMPA_vect)
{
    if(lock)
        return;
    lock = true;

	mh.run();
	lock = false;
}


void messageCallback(Message *&msg)
{
	mts.translateMessage(msg);
	
}


ISR(USART_RX_vect)
{
    while(!(UCSR0A&(1<<RXC0))){};
    char c = UDR0;
	mh.insertChar(c);
}


void initTimer()
{
    OCR1A = F_CPU / 256 / 10; // 10 Hz
    TCCR1B |= 1 << WGM12;
    TCCR1B |= 1 << CS12;
    TIMSK1 |= 1 << OCIE1A;
}


void requestDeviceName()
{
	Message m;
	m.init();
	m.add("deviceName", "junetest");
	m.finnish();
	int size = m.getSize() + 1;
	char *msg = (char*)malloc(size);
	m.getMessageData(msg);

	USART_putMessage(msg, size);
	m.destroy();
	free(msg);
}


void requestCreateTags()
{
	Message pb0;
	pb0.init();
	bool on = false;
	pb0.add("pb0", on);
	pb0.finnish();
	int size = pb0.getSize() +1;
	char *msg = (char*)malloc(size);
	pb0.getMessageData(msg);

	USART_putMessage(msg, size);
	pb0.destroy();
	free(msg);
}

