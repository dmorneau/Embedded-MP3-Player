/**
 * \file interface.c
 * \author Equipe 01
 * \date 10/08/2010
 *
 * Push buttons , display and audio interface
 * 
 */
 
#include "interface.h"
#include "opb_dio4_affich.h"
#include "xac97_l.h"
#include <xbasic_types.h>


typedef union {
    DIO4_state b;
    Xuint32 i;
} DIO4_state_u;

DIO4_state read_dio4_reg0(void) {
	DIO4_state_u state;
	state.i = OPB_DIO4_AFFICH_mReadSlaveReg0(DIO4_BASEADDR);
	return state.b;
}

DIO4_state read_dio4_reg1(void) {
	DIO4_state_u state;
	state.i = OPB_DIO4_AFFICH_mReadSlaveReg1(DIO4_BASEADDR);
	return state.b;
}

DIO4_state read_dio4_reg2(void) {
	DIO4_state_u state;
	state.i = OPB_DIO4_AFFICH_mReadSlaveReg2(DIO4_BASEADDR);
	return state.b;
}

void write_dio4_reg1(Xuint32 data) {
	OPB_DIO4_AFFICH_mWriteSlaveReg1(DIO4_BASEADDR, data);
}

void write_dio4_reg2(Xuint32 data) {
	OPB_DIO4_AFFICH_mWriteSlaveReg2(DIO4_BASEADDR, data);
}

void dio4_enable_interrupts(void) {
	OPB_DIO4_AFFICH_EnableInterrupt((void*)DIO4_BASEADDR);
}

void dio4_clear_interrupt(void) {
	Xuint32 IpStatus;
	IpStatus = OPB_DIO4_AFFICH_mReadReg(DIO4_BASEADDR, OPB_DIO4_AFFICH_INTR_ISR_OFFSET);
	OPB_DIO4_AFFICH_mWriteReg(DIO4_BASEADDR, OPB_DIO4_AFFICH_INTR_ISR_OFFSET, IpStatus);
}

void display(Xuint32 leds, Xuint32 segments) {
	write_dio4_reg1(leds);
	write_dio4_reg2(segments);	
}

void display_init(void){
	display(4,0);	
}
