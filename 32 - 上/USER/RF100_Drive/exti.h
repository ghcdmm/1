#ifndef __exti_h
#define __exti_h

extern unsigned char dht11step,humih,humil,temh,teml,chk,i,j,rad;

void mq2_Init(void);
void dht11init(void);
void raindrops_Init(void);
void dht11(void);

void dht11_gpio_input(void);
void dht11_gpio_output(void);
unsigned int dht11_scan(void);
unsigned int dht11_read_bit(void);
unsigned int dht11_read_byte(void);
unsigned int dht11_read_data(unsigned char buffer[4]);
void moterini(void);
void moter(void);
void moterb(void);

#endif
