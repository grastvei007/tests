#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <string.h>

#include <messagehandler.h>
#include <messagetranslationsenter.h>
#include <message.h>
#include <usart.h>
#include <pwm.h>
#include <tag.h>


#define F_CPU 12000000UL

MessageHandler mh;
MessageTranslationSenter mts;
Pwm pwm;
volatile bool lock;

void initTimer();

void messageCallback(Message *&msg);
void requestDeviceName();
void requestCreateTags();

void onBoolValueChanged(char *aKey, bool aValue);
void onIntValueChanged(char *aKey, int aValue);

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
	mts.setCallbackIntValue(onIntValueChanged);

	pwm.init();
	pwm.enable(Pwm::eChanPb3);
	pwm.setDutyCycle(Pwm::eChanPb3, 0);

	lock = false;
	sei();

	DDRB |= (1<<PB0);
	DDRB |= (1<<PB2);

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


void onIntValueChanged(char *aKey, int aValue)
{
	if(strcmp(aKey, "pwm_0") == 0)
	{
		int value = aValue;
		if(value < 0)
			value = 0;
		if(value > 255)
			value = 255;

		pwm.setDutyCycle(Pwm::eChanPb3, value);
		USART_putstring(" pwm ");
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
	Tag::createTag("deviceName", "junetest");
}


void requestCreateTags()
{
	Tag::createTag("pb0", false);
	Tag::createTag("pwm_0", int(0));
}

