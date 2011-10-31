#ifndef _DIO4_H__
#define _DIO4_H__
#include <xbasic_types.h>
#include "xac97_l.h"
#include "xparameters.h"

#define AC97_BASEADDR XPAR_AUDIO_CODEC_BASEADDR
#define DIO4_BASEADDR XPAR_OPB_DIO4_AFFICH_0_BASEADDR
typedef struct {
	unsigned char btn1 : 1;
	unsigned char btn2 : 1;
	unsigned char btn3 : 1;
	unsigned char btn4 : 1;
	unsigned char btn5 : 1;
	unsigned int padding : (32 - 5);
} DIO4_state;

typedef void (*DIO4_callback) (void*);

//reads and returns the value in reg0 (buttons)
DIO4_state read_dio4_reg0(void);
//reads and returns the value in reg1 (leds)
DIO4_state read_dio4_reg1(void);
//reads and returns the value in reg2 (7 segments)
DIO4_state read_dio4_reg2(void);

//writes on the register1 of the opb controller (LEDS)
void write_dio4_reg1(Xuint32 data);

//writes on the register2 of the opb controller (7segments)
void write_dio4_reg2(Xuint32 data);

void dio4_enable_interrupts(void);

void dio4_clear_interrupt(void);

//displays the volume (leds) and the time (7segments) on the dio4
void display(Xuint32 leds, Xuint32 segments);

//initializes the display with default values
void display_init(void);

#endif
