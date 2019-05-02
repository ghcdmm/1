#ifndef __LCD1602_H
#define __LCD1602_H

enum rs1602{ins=0,dat};
enum rw1602{wt=0,rd};

extern enum rs1602 RS1602;
extern enum rw1602 RW1602;

void LCD1602_ins(unsigned char rs,unsigned char rw,unsigned char data);
void LCD1602_init(void);
#endif
