//////////////////////////////////////////////////////////////////////////////
// Filename:          C:\temp\gold\MyProcessorIPLib/drivers/opb_dio4_affich_v1_00_a/src/opb_dio4_affich_selftest.c
// Version:           1.00.a
// Description:       Contains a diagnostic self-test function for the opb_dio4_affich driver
// Date:              Fri Nov 12 10:32:28 2010 (by Create and Import Peripheral Wizard)
//////////////////////////////////////////////////////////////////////////////


/***************************** Include Files *******************************/

#include "opb_dio4_affich.h"

/************************** Constant Definitions ***************************/


/************************** Variable Definitions ****************************/


/************************** Function Definitions ***************************/

/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the OPB_DIO4_AFFICH instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus OPB_DIO4_AFFICH_SelfTest(void * baseaddr_p)
{
   int     Index;
   Xuint32 baseaddr;
   Xuint8  Reg8Value;
   Xuint16 Reg16Value;
   Xuint32 Reg32Value;
   Xuint64 Reg64Value;

   /*
    * Assert the argument
    */
   XASSERT_NONVOID(baseaddr_p != XNULL);
   baseaddr = (Xuint32) baseaddr_p;

   xil_printf("******************************\n\r");
   xil_printf("* User Peripheral Self Test\n\r");
   xil_printf("******************************\n\n\r");

   /*
    * Reset the device to get it back to its default state and get module identification value
    */
   xil_printf("RST/MIR test...\n\r");
   OPB_DIO4_AFFICH_mReset(baseaddr);
   xil_printf("   - write 0x%08x to software reset register \n\r", IPIF_RESET);
   Reg32Value = OPB_DIO4_AFFICH_mReadMIR(baseaddr);
   if ( Reg32Value == 0x30220301 )
   {
      xil_printf("   - read 0x%08x (expected) from module identification register \n\r", Reg32Value);
      xil_printf("   - RST/MIR write/read passed\n\n\r");
   }
   else
   {
      xil_printf("   - read 0x%08x (unexpected) from module identification register, expect 0x30220301 \n\r", Reg32Value);
      xil_printf("   - RST/MIR write/read failed\n\n\r");
      return XST_FAILURE;
   }

   /*
    * Write to user logic slave module register(s) and read back
    */
   xil_printf("User logic slave module test...\n\r");
   xil_printf("   - write 1 to slave register 0\n\r");
   OPB_DIO4_AFFICH_mWriteSlaveReg0(baseaddr, 1);
   Reg32Value = OPB_DIO4_AFFICH_mReadSlaveReg0(baseaddr);
   xil_printf("   - read %d from register 0\n\r", Reg32Value);
   if ( Reg32Value != (Xuint32) 1 )
   {
      xil_printf("   - slave register 0 write/read failed\n\r");
      return XST_FAILURE;
   }
   xil_printf("   - write 2 to slave register 1\n\r");
   OPB_DIO4_AFFICH_mWriteSlaveReg1(baseaddr, 2);
   Reg32Value = OPB_DIO4_AFFICH_mReadSlaveReg1(baseaddr);
   xil_printf("   - read %d from register 1\n\r", Reg32Value);
   if ( Reg32Value != (Xuint32) 2 )
   {
      xil_printf("   - slave register 1 write/read failed\n\r");
      return XST_FAILURE;
   }
   xil_printf("   - write 3 to slave register 2\n\r");
   OPB_DIO4_AFFICH_mWriteSlaveReg2(baseaddr, 3);
   Reg32Value = OPB_DIO4_AFFICH_mReadSlaveReg2(baseaddr);
   xil_printf("   - read %d from register 2\n\r", Reg32Value);
   if ( Reg32Value != (Xuint32) 3 )
   {
      xil_printf("   - slave register 2 write/read failed\n\r");
      return XST_FAILURE;
   }
   xil_printf("   - slave register write/read passed\n\n\r");

   /*
    * Enable all possible interrupts and clear interrupt status register(s)
    */
   xil_printf("Interrupt controller test...\n\r");
   Reg32Value = OPB_DIO4_AFFICH_mReadReg(baseaddr, OPB_DIO4_AFFICH_INTR_ISR_OFFSET);
   xil_printf("   - IP (user logic) interrupt status : 0x%08x \n\r", Reg32Value);
   xil_printf("   - clear IP (user logic) interrupt status register\n\r");
   OPB_DIO4_AFFICH_mWriteReg(baseaddr, OPB_DIO4_AFFICH_INTR_ISR_OFFSET, Reg32Value);
   Reg32Value = OPB_DIO4_AFFICH_mReadReg(baseaddr, OPB_DIO4_AFFICH_INTR_DISR_OFFSET);
   xil_printf("   - Device (peripheral) interrupt status : 0x%08x \n\r", Reg32Value);
   xil_printf("   - clear Device (peripheral) interrupt status register\n\r");
   OPB_DIO4_AFFICH_mWriteReg(baseaddr, OPB_DIO4_AFFICH_INTR_DISR_OFFSET, Reg32Value);
   xil_printf("   - enable all possible interrupt(s)\n\r");
   OPB_DIO4_AFFICH_EnableInterrupt(baseaddr_p);
   xil_printf("   - write/read interrupt register passed \n\n\r");

   return XST_SUCCESS;
}
