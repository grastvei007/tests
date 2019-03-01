#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <string.h>

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

void onBoolValueChanged(char *aKey, bool aValue);

int main()
{
	lock = true;
	cli();
	initTimer();
	USART_init();
	mh.init();
	mh.setCallback(messageCallback);

	mts.init();
	mts.setDeviceNameFunc(requestDeviceName);
	mts.setCreateTagsFunc(requestCreateTags);

	mts.setCallbackBoolValue(onBoolValueChanged);

	lock = false;
	sei();

	DDRB |= (1<<PB0);

	while(true)
	{

	}

	return 0;
}


void onBoolValueChanged(char *aKey, bool aValue)
{
	if(strcmp(aKey, "pb0") == 0)
	{
		if(aValue)
		{
			//USART_putstring("pb0 on");
			 PORTB |= (1<<PB0);
		}
		else
		{
		//	USART_putstring("pb0 off");
			PORTB &= ~(1<<PB0);
		}
	}
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
	msg->destroy();
	free(msg);	
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


	Message pwm;
	pwm.init();
	int val = 0;
	pwm.add("pwm_0", val);
	pwm.finnish();
	size = pwm.getSize() + 1;
	msg = (char*)malloc(size);
	pwm.getMessageData(msg);

	USART_putMessage(msg, size);
	pwm.destroy();
	free(msg);
}

