/****************************************Copyright (c)****************************************************
**                            Shenzhen SeeedStudio Co.,LTD.
**
**                                 http://www.seeedstudio.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  mcp_can.h
** Latest modified Date:       2012-4-24
** Latest Version:             v0.1
** Descriptions:	       mcp_can class .h file
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 loovee
** Created date:               2012-4-24
** Version:                    v0.1
** Descriptions:               mcp2515 class

*********************************************************************************************************/
#ifndef _MCP2515_H_
#define _MCP2515_H_

#include "mcp_can_dfs.h"
#define MAX_CHAR_IN_MESSAGE 8

class MCP_CAN
{
    private:
    
    INT8U   m_nExtFlg;                                                  /* identifier xxxID             */
                                                                        /* either extended (the 29 LSB) */
                                                                        /* or standard (the 11 LSB)     */
    INT32U  m_nID;                                                      /* can id                       */
    INT8U   m_nDlc;                                                     /* data length:                 */
    INT8U   m_nDta[MAX_CHAR_IN_MESSAGE];                            	/* data                         */
    INT8U   m_nRtr;                                                     /* rtr                          */
    INT8U   m_nfilhit;

    // MCP2515 oscillator in MHz (8 or 16). Old versions assumed 16MHz.
    INT8U   m_clockMHz;

    // Chip Select pin for this MCP2515 instance.
    INT8U   m_csPin;

    // Select / unselect helpers (per-instance CS)
    inline void cs_select()   { digitalWrite(m_csPin, LOW); }
    inline void cs_unselect() { digitalWrite(m_csPin, HIGH); }

/*
*  mcp2515 driver function 
*/
   // private:
   private:

    void mcp2515_reset(void);                                           /* reset mcp2515                */

    INT8U mcp2515_readRegister(const INT8U address);                    /* read mcp2515's register      */
    
    void mcp2515_readRegisterS(const INT8U address, 
	                       INT8U values[], 
                               const INT8U n);
    void mcp2515_setRegister(const INT8U address,                       /* set mcp2515's register       */
                             const INT8U value);

    void mcp2515_setRegisterS(const INT8U address,                      /* set mcp2515's registers      */
                              const INT8U values[],
                              const INT8U n);
    
    void mcp2515_initCANBuffers(void);
    
    void mcp2515_modifyRegister(const INT8U address,                    /* set bit of one register      */
                                const INT8U mask,
                                const INT8U data);

    INT8U mcp2515_readStatus(void);                                     /* read mcp2515's Status        */
    INT8U mcp2515_setCANCTRL_Mode(const INT8U newmode);                 /* set mode                     */
    INT8U mcp2515_configRate(const INT8U canSpeed);                     /* set boadrate                 */
    INT8U mcp2515_init(const INT8U canSpeed);                           /* mcp2515init                  */

    void mcp2515_write_id( const INT8U mcp_addr,                        /* write can id                 */
                               const INT8U ext,
                               const INT32U id );

    void mcp2515_read_id( const INT8U mcp_addr,                        /* read can id                  */
                                    INT8U* ext,
                                    INT32U* id );

    void mcp2515_write_canMsg( const INT8U buffer_sidh_addr );          /* write can msg                */
    void mcp2515_read_canMsg( const INT8U buffer_sidh_addr);            /* read can msg                 */
    void mcp2515_start_transmit(const INT8U mcp_addr);                  /* start transmit               */
    INT8U mcp2515_getNextFreeTXBuf(INT8U *txbuf_n);                     /* get Next free txbuf          */

/*
*  can operator function
*/    

    INT8U setMsg(INT32U id, INT8U ext, INT8U len, INT8U *pData);    /* set message                  */  
    INT8U clearMsg();                                               /* clear all message to zero    */
    INT8U readMsg();                                                /* read message                 */
    INT8U sendMsg();                                                /* send message                 */
public:
    // Constructor allows using multiple MCP2515 devices on the same SPI bus.
    // If not provided, it falls back to the legacy default (MCP2515_CS_PIN).
    MCP_CAN(INT8U csPin = MCP2515_CS_PIN);

    // Backward compatible init (defaults to 16MHz oscillator)
    INT8U begin(INT8U speedset);                              /* init can                     */
    // New init that lets you select MCP2515 oscillator (MCP_8MHZ / MCP_16MHZ)
    INT8U begin(INT8U speedset, INT8U clockset);

    // Optional: change CS pin after construction (must be called before begin()).
    void setCSPin(INT8U csPin) { m_csPin = csPin; }
    INT8U init_Mask(INT8U num, INT8U ext, INT32U ulData);           /* init Masks                   */
    INT8U init_Filt(INT8U num, INT8U ext, INT32U ulData);           /* init filters                 */
    INT8U sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf);  /* send buf                     */
    INT8U readMsgBuf(INT8U *len, INT8U *buf);                       /* read buf                     */
    INT8U checkReceive(void);                                       /* if something received        */
    INT8U checkError(void);                                         /* if something error           */
    INT32U getCanId(void);                                          /* get can id when receive      */
};

// ------------------------------------------------------------
// Optional global instance
//
// Default behaviour (legacy-friendly): the library provides a
// global object named `CAN`.
//
// - Single node: use `CAN` and optionally override CS with
//   `#define MCP2515_CS_PIN <pin>` BEFORE including <mcp_can.h>
// - Multi-node: define `MCP_CAN_NO_GLOBAL` BEFORE including
//   <mcp_can.h> and create your own instances (CAN0/CAN1/etc).
// ------------------------------------------------------------
#ifndef MCP_CAN_NO_GLOBAL
extern MCP_CAN CAN;
#endif
#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
