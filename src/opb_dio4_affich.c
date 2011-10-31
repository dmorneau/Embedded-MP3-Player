//////////////////////////////////////////////////////////////////////////////
// Filename:          C:\temp\gold\MyProcessorIPLib/drivers/opb_dio4_affich_v1_00_a/src/opb_dio4_affich.c
// Version:           1.00.a
// Description:       opb_dio4_affich Driver Source File
// Date:              Fri Nov 12 10:32:28 2010 (by Create and Import Peripheral Wizard)
//////////////////////////////////////////////////////////////////////////////


/***************************** Include Files *******************************/

#include "opb_dio4_affich.h"

/************************** Function Definitions ***************************/

/**
 *
 * Enable all possible interrupts from OPB_DIO4_AFFICH device.
 *
 * @param   baseaddr_p is the base address of the OPB_DIO4_AFFICH device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void OPB_DIO4_AFFICH_EnableInterrupt(void * baseaddr_p)
{
  Xuint32 baseaddr;
  baseaddr = (Xuint32) baseaddr_p;

  /*
   * Enable all interrupt source from user logic.
   */
  OPB_DIO4_AFFICH_mWriteReg(baseaddr, OPB_DIO4_AFFICH_INTR_IER_OFFSET, 0x00000001);

  /*
   * Enable all possible interrupt sources from device.
   */
  OPB_DIO4_AFFICH_mWriteReg(baseaddr, OPB_DIO4_AFFICH_INTR_DIER_OFFSET,
    INTR_TERR_MASK
    | INTR_DPTO_MASK
    | INTR_IPIR_MASK
    );

  /*
   * Set global interrupt enable.
   */
  OPB_DIO4_AFFICH_mWriteReg(baseaddr, OPB_DIO4_AFFICH_INTR_DGIER_OFFSET, INTR_GIE_MASK);
}

/**
 *
 * Example interrupt controller handler for OPB_DIO4_AFFICH device.
 * This is to show example of how to toggle write back ISR to clear interrupts.
 *
 * @param   baseaddr_p is the base address of the OPB_DIO4_AFFICH device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void OPB_DIO4_AFFICH_Intr_DefaultHandler(void * baseaddr_p)
{
  Xuint32 baseaddr;
  Xuint32 IntrStatus;
Xuint32 IpStatus;

  baseaddr = (Xuint32) baseaddr_p;

  /*
   * Get status from Device Interrupt Status Register.
   */
  IntrStatus = OPB_DIO4_AFFICH_mReadReg(baseaddr, OPB_DIO4_AFFICH_INTR_DISR_OFFSET);

  xil_printf("Device Interrupt! DISR value : 0x%08x \n\r", IntrStatus);

  /*
   * Verify the source of the interrupt is the user logic and clear the interrupt
   * source by toggle write baca to the IP ISR register.
   */
  if ( (IntrStatus & INTR_IPIR_MASK) == INTR_IPIR_MASK )
  {
    xil_printf("User logic interrupt! \n\r");
    IpStatus = OPB_DIO4_AFFICH_mReadReg(baseaddr, OPB_DIO4_AFFICH_INTR_ISR_OFFSET);
    OPB_DIO4_AFFICH_mWriteReg(baseaddr, OPB_DIO4_AFFICH_INTR_ISR_OFFSET, IpStatus);
  }

}

