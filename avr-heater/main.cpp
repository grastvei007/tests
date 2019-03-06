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
#include <pwm.h>


#define F_CPU 12000000UL

MessageHandler mh;
MessageTranslationSenter mts;
Adc adc;
Pwm pwm;
volatile int heartBeat;
volatile int heartBeatCounter;
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
	heartBeat = 0;
	heartBeatCounter = 0;
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

	pwm.init();
	pwm.enable(Pwm::eChanPb3);

//	lock = false;
	sei();
	lock = false;
	pwm.setDutyCycle(Pwm::eChanPb3, 0);

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

	heartBeatCounter++;
	if(heartBeatCounter >= 50)
	{
		heartBeat++;
		Tag::setValue("heartBeat", heartBeat);
		if(heartBeat >= 1000)
			heartBeat = 0;

		heartBeatCounter = 0;
	}

	mh.run();

	if(!ready)
	{
		lock = false;
		return;
	}

	if(adc.isDataReady())
	{
//    	float value = adc.getChannelReading(Adc::eAdc0);
//    	Tag::setValue("adc0", value);
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
	Tag::createTag("deviceName", "heater");
}


void requestCreateTags()
{
	Tag::createTag("heartBeat", heartBeat);
	Tag::createTag("adc0", float(0.0));
	ready = true;
}


void adcValueReady(int aChannel)
{
	if(aChannel == 0)
	{
		float value = adc.getChannelReading(Adc::eAdc0);
		Tag::setValue("adc0", value);

		int c = (int)(value*51);
		pwm.setDutyCycle(Pwm::eChanPb3, c);
	}
}

