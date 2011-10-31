//////////////////////////////////////////////////////////////////////////////
// Filename:          C:\temp\gold\MyProcessorIPLib/drivers/opb_dio4_affich_v1_00_a/src/opb_dio4_affich.h
// Version:           1.00.a
// Description:       opb_dio4_affich Driver Header File
// Date:              Fri Nov 12 10:32:28 2010 (by Create and Import Peripheral Wizard)
//////////////////////////////////////////////////////////////////////////////

#ifndef OPB_DIO4_AFFICH_H
#define OPB_DIO4_AFFICH_H

/***************************** Include Files *******************************/

#include "xbasic_types.h"
#include "xstatus.h"
#include "xio.h"

/************************** Constant Definitions ***************************/


/**
 * User Logic Slave Space Offsets
 * -- SLAVE_REG0 : user logic slave module register 0
 * -- SLAVE_REG1 : user logic slave module register 1
 * -- SLAVE_REG2 : user logic slave module register 2
 */
#define OPB_DIO4_AFFICH_USER_SLAVE_SPACE_OFFSET (0x00000000)
#define OPB_DIO4_AFFICH_SLAVE_REG0_OFFSET (OPB_DIO4_AFFICH_USER_SLAVE_SPACE_OFFSET + 0x00000000)
#define OPB_DIO4_AFFICH_SLAVE_REG1_OFFSET (OPB_DIO4_AFFICH_USER_SLAVE_SPACE_OFFSET + 0x00000004)
#define OPB_DIO4_AFFICH_SLAVE_REG2_OFFSET (OPB_DIO4_AFFICH_USER_SLAVE_SPACE_OFFSET + 0x00000008)

/**
 * IPIF Reset/Mir Space Register Offsets
 * -- RST : software reset register
 * -- MIR : module identification register
 */
#define OPB_DIO4_AFFICH_IPIF_RST_SPACE_OFFSET (0x00000100)
#define OPB_DIO4_AFFICH_RST_OFFSET (OPB_DIO4_AFFICH_IPIF_RST_SPACE_OFFSET + 0x00000000)
#define OPB_DIO4_AFFICH_MIR_OFFSET (OPB_DIO4_AFFICH_IPIF_RST_SPACE_OFFSET + 0x00000000)

/**
 * IPIF Reset/Mir Masks
 * -- IPIF_MAVN_MASK   : module major version number
 * -- IPIF_MIVN_MASK   : module minor version number
 * -- IPIF_MIVL_MASK   : module minor version letter
 * -- IPIF_BID_MASK    : module block id
 * -- IPIF_BTP_MASK    : module block type
 * -- IPIF_RESET       : software reset
 */
#define IPIF_MAVN_MASK (0xF0000000UL)
#define IPIF_MIVN_MASK (0x0FE00000UL)
#define IPIF_MIVL_MASK (0x001F0000UL)
#define IPIF_BID_MASK (0x0000FF00UL)
#define IPIF_BTP_MASK (0x000000FFUL)
#define IPIF_RESET (0x0000000A)

/**
 * IPIF Interrupt Controller Space Offsets
 * -- INTR_DISR  : device (ipif) interrupt status register
 * -- INTR_DIPR  : device (ipif) interrupt pending register
 * -- INTR_DIER  : device (ipif) interrupt enable register
 * -- INTR_DIIR  : device (ipif) interrupt id (priority encoder) register
 * -- INTR_DGIER : device (ipif) global interrupt enable register
 * -- INTR_ISR   : ip (user logic) interrupt status register
 * -- INTR_IER   : ip (user logic) interrupt enable register
 */
#define OPB_DIO4_AFFICH_IPIF_INTR_SPACE_OFFSET (0x00000200)
#define OPB_DIO4_AFFICH_INTR_DISR_OFFSET (OPB_DIO4_AFFICH_IPIF_INTR_SPACE_OFFSET + 0x00000000)
#define OPB_DIO4_AFFICH_INTR_DIPR_OFFSET (OPB_DIO4_AFFICH_IPIF_INTR_SPACE_OFFSET + 0x00000004)
#define OPB_DIO4_AFFICH_INTR_DIER_OFFSET (OPB_DIO4_AFFICH_IPIF_INTR_SPACE_OFFSET + 0x00000008)
#define OPB_DIO4_AFFICH_INTR_DIIR_OFFSET (OPB_DIO4_AFFICH_IPIF_INTR_SPACE_OFFSET + 0x00000018)
#define OPB_DIO4_AFFICH_INTR_DGIER_OFFSET (OPB_DIO4_AFFICH_IPIF_INTR_SPACE_OFFSET + 0x0000001C)
#define OPB_DIO4_AFFICH_INTR_ISR_OFFSET (OPB_DIO4_AFFICH_IPIF_INTR_SPACE_OFFSET + 0x00000020)
#define OPB_DIO4_AFFICH_INTR_IER_OFFSET (OPB_DIO4_AFFICH_IPIF_INTR_SPACE_OFFSET + 0x00000028)

/**
 * IPIF Interrupt Controller Masks
 * -- INTR_TERR_MASK : transaction error
 * -- INTR_DPTO_MASK : data phase time-out
 * -- INTR_IPIR_MASK : ip interrupt requeset
 * -- INTR_DMA0_MASK : dma channel 0 interrupt request
 * -- INTR_DMA1_MASK : dma channel 1 interrupt request
 * -- INTR_RFDL_MASK : read packet fifo deadlock interrupt request
 * -- INTR_WFDL_MASK : write packet fifo deadlock interrupt request
 * -- INTR_IID_MASK  : interrupt id
 * -- INTR_GIE_MASK  : global interrupt enable
 * -- INTR_NOPEND    : the DIPR has no pending interrupts
 */
#define INTR_TERR_MASK (0x00000001UL)
#define INTR_DPTO_MASK (0x00000002UL)
#define INTR_IPIR_MASK (0x00000004UL)
#define INTR_DMA0_MASK (0x00000008UL)
#define INTR_DMA1_MASK (0x00000010UL)
#define INTR_RFDL_MASK (0x00000020UL)
#define INTR_WFDL_MASK (0x00000040UL)
#define INTR_IID_MASK (0x000000FFUL)
#define INTR_GIE_MASK (0x80000000UL)
#define INTR_NOPEND (0x80)

/**************************** Type Definitions *****************************/


/***************** Macros (Inline Functions) Definitions *******************/

/**
 *
 * Write a value to a OPB_DIO4_AFFICH register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the OPB_DIO4_AFFICH device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void OPB_DIO4_AFFICH_mWriteReg(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Data)
 *
 */
#define OPB_DIO4_AFFICH_mWriteReg(BaseAddress, RegOffset, Data) \
 	XIo_Out32((BaseAddress) + (RegOffset), (Xuint32)(Data))

/**
 *
 * Read a value from a OPB_DIO4_AFFICH register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the OPB_DIO4_AFFICH device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	Xuint32 OPB_DIO4_AFFICH_mReadReg(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define OPB_DIO4_AFFICH_mReadReg(BaseAddress, RegOffset) \
 	XIo_In32((BaseAddress) + (RegOffset))


/**
 *
 * Write/Read value to/from OPB_DIO4_AFFICH user logic slave registers.
 *
 * @param   BaseAddress is the base address of the OPB_DIO4_AFFICH device.
 * @param   Value is the data written to the register.
 *
 * @return  Data is the data from the user logic slave register.
 *
 * @note
 * C-style signature:
 * 	Xuint32 OPB_DIO4_AFFICH_mReadSlaveRegn(Xuint32 BaseAddress)
 *
 */
#define OPB_DIO4_AFFICH_mWriteSlaveReg0(BaseAddress, Value) \
 	XIo_Out32((BaseAddress) + (OPB_DIO4_AFFICH_SLAVE_REG0_OFFSET), (Xuint32)(Value))
#define OPB_DIO4_AFFICH_mWriteSlaveReg1(BaseAddress, Value) \
 	XIo_Out32((BaseAddress) + (OPB_DIO4_AFFICH_SLAVE_REG1_OFFSET), (Xuint32)(Value))
#define OPB_DIO4_AFFICH_mWriteSlaveReg2(BaseAddress, Value) \
 	XIo_Out32((BaseAddress) + (OPB_DIO4_AFFICH_SLAVE_REG2_OFFSET), (Xuint32)(Value))

#define OPB_DIO4_AFFICH_mReadSlaveReg0(BaseAddress) \
 	XIo_In32((BaseAddress) + (OPB_DIO4_AFFICH_SLAVE_REG0_OFFSET))
#define OPB_DIO4_AFFICH_mReadSlaveReg1(BaseAddress) \
 	XIo_In32((BaseAddress) + (OPB_DIO4_AFFICH_SLAVE_REG1_OFFSET))
#define OPB_DIO4_AFFICH_mReadSlaveReg2(BaseAddress) \
 	XIo_In32((BaseAddress) + (OPB_DIO4_AFFICH_SLAVE_REG2_OFFSET))

/**
 *
 * Reset OPB_DIO4_AFFICH via software.
 *
 * @param   BaseAddress is the base address of the OPB_DIO4_AFFICH device.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void OPB_DIO4_AFFICH_mReset(Xuint32 BaseAddress)
 *
 */
#define OPB_DIO4_AFFICH_mReset(BaseAddress) \
 	XIo_Out32((BaseAddress)+(OPB_DIO4_AFFICH_RST_OFFSET), IPIF_RESET)

/**
 *
 * Read module identification information from OPB_DIO4_AFFICH device.
 *
 * @param   BaseAddress is the base address of the OPB_DIO4_AFFICH device.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	Xuint32 OPB_DIO4_AFFICH_mReadMIR(Xuint32 BaseAddress)
 *
 */
#define OPB_DIO4_AFFICH_mReadMIR(BaseAddress) \
 	XIo_In32((BaseAddress)+(OPB_DIO4_AFFICH_MIR_OFFSET))

/************************** Function Prototypes ****************************/


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
void OPB_DIO4_AFFICH_EnableInterrupt(void * baseaddr_p);

/**
 *
 * Example interrupt controller handler.
 *
 * @param   baseaddr_p is the base address of the OPB_DIO4_AFFICH device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void OPB_DIO4_AFFICH_Intr_DefaultHandler(void * baseaddr_p);

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
XStatus OPB_DIO4_AFFICH_SelfTest(void * baseaddr_p);

#endif // OPB_DIO4_AFFICH_H
