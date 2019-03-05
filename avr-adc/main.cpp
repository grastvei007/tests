#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <string.h>

#include <messagehandler.h>
#include <messagetranslationsenter.h>
#include <message.h>
#include <usart.h>
#include <tag.h>
#include <adc.h>


#define F_CPU 12000000UL

MessageHandler mh;
MessageTranslationSenter mts;
Adc adc;
volatile bool lock;
volatile bool ready;

void initTimer();

void messageCallback(Message *&msg);
void requestDeviceName();
void requestCreateTags();

void adcValueReady(int aChannel);


int main()
{
	ready = false;
	lock = true;
	cli();
	initTimer();
	USART_init();
	mh.init();
	mh.setCallback(messageCallback);

	mts.init();
	mts.setDeviceNameFunc(requestDeviceName);
	mts.setCreateTagsFunc(requestCreateTags);

	adc.init();
	adc.enable();
	adc.enableChannel(Adc::eAdc0);
	adc.setCallbackFunc(adcValueReady);

	lock = false;
	sei();


	while(true)
	{

	}

	return 0;
}


ISR(ADC_vect)
{
	adc.valueReady(); // call to the module valueReady function.
//	adc.readNext(); 
}


ISR(TIMER1_COMPA_vect)
{
    if(lock)
        return;
    lock = true;

	mh.run();

	if(!ready)
	{
		lock = false;
		return;
	}

	if(adc.isDataReady())
	{
    	float value = adc.getChannelReading(Adc::eAdc0);
    	Tag::setValue("adc0", value);
		adc.readNext();
	}

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
	Tag::createTag("deviceName", "adcTest");
}


void requestCreateTags()
{
	Tag::createTag("adc0", float(0.0));
	ready = true;
}


void adcValueReady(int aChannel)
{
	if(aChannel == 0)
	{
//		float value = adc.getChannelReading(Adc::eAdc0);
//		Tag::setValue("adc0", value);
	}
}

