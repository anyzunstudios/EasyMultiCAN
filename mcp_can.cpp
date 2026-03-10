/****************************************Copyright (c)****************************************************
**                            Shenzhen SeeedStudio Co.,LTD.
**
**                                 http://www.seeedstudio.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  mcp_can.cpp
** Latest modified Date:       2026-03-09
** Latest Version:             EasyMultiCAN improved
** Descriptions:               MCP2515 class
**
*********************************************************************************************************/
#include "mcp_can.h"

#define spi_readwrite SPI.transfer
#define spi_read() spi_readwrite(0x00)

// Legacy global instance (kept for backwards compatibility)
#ifndef MCP_CAN_NO_GLOBAL
MCP_CAN CAN(MCP2515_CS_PIN);
#endif

MCP_CAN::MCP_CAN(INT8U csPin)
{
    m_csPin = csPin;
    m_clockMHz = MCP_16MHZ; // Default legacy behavior
}

/*********************************************************************************************************
** Function name:           mcp2515_reset
** Descriptions:            Reset the device
*********************************************************************************************************/
void MCP_CAN::mcp2515_reset(void)
{
    cs_select();
    spi_readwrite(MCP_RESET);
    cs_unselect();
    delay(10);
}

/*********************************************************************************************************
** Function name:           mcp2515_readRegister
** Descriptions:            Read a register
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_readRegister(const INT8U address)
{
    INT8U ret;

    cs_select();
    spi_readwrite(MCP_READ);
    spi_readwrite(address);
    ret = spi_read();
    cs_unselect();

    return ret;
}

/*********************************************************************************************************
** Function name:           mcp2515_readRegisterS
** Descriptions:            Read multiple registers
*********************************************************************************************************/
void MCP_CAN::mcp2515_readRegisterS(const INT8U address, INT8U values[], const INT8U n)
{
    INT8U i;

    cs_select();
    spi_readwrite(MCP_READ);
    spi_readwrite(address);

    // MCP2515 auto-increments the address pointer
    for (i = 0; i < n; i++) {
        values[i] = spi_read();
    }

    cs_unselect();
}

/*********************************************************************************************************
** Function name:           mcp2515_setRegister
** Descriptions:            Write a register
*********************************************************************************************************/
void MCP_CAN::mcp2515_setRegister(const INT8U address, const INT8U value)
{
    cs_select();
    spi_readwrite(MCP_WRITE);
    spi_readwrite(address);
    spi_readwrite(value);
    cs_unselect();
}

/*********************************************************************************************************
** Function name:           mcp2515_setRegisterS
** Descriptions:            Write multiple registers
*********************************************************************************************************/
void MCP_CAN::mcp2515_setRegisterS(const INT8U address, const INT8U values[], const INT8U n)
{
    INT8U i;

    cs_select();
    spi_readwrite(MCP_WRITE);
    spi_readwrite(address);

    for (i = 0; i < n; i++) {
        spi_readwrite(values[i]);
    }

    cs_unselect();
}

/*********************************************************************************************************
** Function name:           mcp2515_modifyRegister
** Descriptions:            Modify bits in a register
*********************************************************************************************************/
void MCP_CAN::mcp2515_modifyRegister(const INT8U address, const INT8U mask, const INT8U data)
{
    cs_select();
    spi_readwrite(MCP_BITMOD);
    spi_readwrite(address);
    spi_readwrite(mask);
    spi_readwrite(data);
    cs_unselect();
}

/*********************************************************************************************************
** Function name:           mcp2515_readStatus
** Descriptions:            Read MCP2515 status
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_readStatus(void)
{
    INT8U i;

    cs_select();
    spi_readwrite(MCP_READ_STATUS);
    i = spi_read();
    cs_unselect();

    return i;
}

/*********************************************************************************************************
** Function name:           mcp2515_setCANCTRL_Mode
** Descriptions:            Set MCP2515 mode
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_setCANCTRL_Mode(const INT8U newmode)
{
    INT8U i;

    mcp2515_modifyRegister(MCP_CANCTRL, MODE_MASK, newmode);

    i = mcp2515_readRegister(MCP_CANCTRL);
    i &= MODE_MASK;

    if (i == newmode) {
        return MCP2515_OK;
    } else {
        return MCP2515_FAIL;
    }
}

/*********************************************************************************************************
** Function name:           mcp2515_configRate
** Descriptions:            Configure CAN bit rate
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_configRate(const INT8U canSpeed)
{
    INT8U set = 1;
    INT8U cfg1 = 0;
    INT8U cfg2 = 0;
    INT8U cfg3 = 0;

    // Select timing table based on MCP2515 oscillator.
    // Default is 16 MHz for backward compatibility.
    const INT8U clk = (m_clockMHz == MCP_8MHZ) ? MCP_8MHZ : MCP_16MHZ;

    if (clk == MCP_8MHZ) {
        switch (canSpeed) {
            case CAN_125KBPS:
                cfg1 = MCP_8MHz_125kBPS_CFG1;
                cfg2 = MCP_8MHz_125kBPS_CFG2;
                cfg3 = MCP_8MHz_125kBPS_CFG3;
                break;

            case CAN_250KBPS:
                cfg1 = MCP_8MHz_250kBPS_CFG1;
                cfg2 = MCP_8MHz_250kBPS_CFG2;
                cfg3 = MCP_8MHz_250kBPS_CFG3;
                break;

            case CAN_500KBPS:
                cfg1 = MCP_8MHz_500kBPS_CFG1;
                cfg2 = MCP_8MHz_500kBPS_CFG2;
                cfg3 = MCP_8MHz_500kBPS_CFG3;
                break;

            default:
                set = 0;
                break;
        }
    } else {
        switch (canSpeed) {
            case CAN_5KBPS:
                cfg1 = MCP_16MHz_5kBPS_CFG1;
                cfg2 = MCP_16MHz_5kBPS_CFG2;
                cfg3 = MCP_16MHz_5kBPS_CFG3;
                break;

            case CAN_10KBPS:
                cfg1 = MCP_16MHz_10kBPS_CFG1;
                cfg2 = MCP_16MHz_10kBPS_CFG2;
                cfg3 = MCP_16MHz_10kBPS_CFG3;
                break;

            case CAN_20KBPS:
                cfg1 = MCP_16MHz_20kBPS_CFG1;
                cfg2 = MCP_16MHz_20kBPS_CFG2;
                cfg3 = MCP_16MHz_20kBPS_CFG3;
                break;

            case CAN_40KBPS:
                cfg1 = MCP_16MHz_40kBPS_CFG1;
                cfg2 = MCP_16MHz_40kBPS_CFG2;
                cfg3 = MCP_16MHz_40kBPS_CFG3;
                break;

            case CAN_50KBPS:
                cfg1 = MCP_16MHz_50kBPS_CFG1;
                cfg2 = MCP_16MHz_50kBPS_CFG2;
                cfg3 = MCP_16MHz_50kBPS_CFG3;
                break;

            case CAN_80KBPS:
                cfg1 = MCP_16MHz_80kBPS_CFG1;
                cfg2 = MCP_16MHz_80kBPS_CFG2;
                cfg3 = MCP_16MHz_80kBPS_CFG3;
                break;

            case CAN_100KBPS:
                cfg1 = MCP_16MHz_100kBPS_CFG1;
                cfg2 = MCP_16MHz_100kBPS_CFG2;
                cfg3 = MCP_16MHz_100kBPS_CFG3;
                break;

            case CAN_125KBPS:
                cfg1 = MCP_16MHz_125kBPS_CFG1;
                cfg2 = MCP_16MHz_125kBPS_CFG2;
                cfg3 = MCP_16MHz_125kBPS_CFG3;
                break;

            case CAN_200KBPS:
                cfg1 = MCP_16MHz_200kBPS_CFG1;
                cfg2 = MCP_16MHz_200kBPS_CFG2;
                cfg3 = MCP_16MHz_200kBPS_CFG3;
                break;

            case CAN_250KBPS:
                cfg1 = MCP_16MHz_250kBPS_CFG1;
                cfg2 = MCP_16MHz_250kBPS_CFG2;
                cfg3 = MCP_16MHz_250kBPS_CFG3;
                break;

            case CAN_500KBPS:
                cfg1 = MCP_16MHz_500kBPS_CFG1;
                cfg2 = MCP_16MHz_500kBPS_CFG2;
                cfg3 = MCP_16MHz_500kBPS_CFG3;
                break;

            default:
                set = 0;
                break;
        }
    }

    if (set) {
        mcp2515_setRegister(MCP_CNF1, cfg1);
        mcp2515_setRegister(MCP_CNF2, cfg2);
        mcp2515_setRegister(MCP_CNF3, cfg3);
        return MCP2515_OK;
    } else {
        return MCP2515_FAIL;
    }
}

/*********************************************************************************************************
** Function name:           mcp2515_initCANBuffers
** Descriptions:            Initialize CAN buffers
*********************************************************************************************************/
void MCP_CAN::mcp2515_initCANBuffers(void)
{
    INT8U i;
    INT8U a1, a2, a3;

    INT8U ext = 0;
    INT32U ulMask = 0x00;
    INT32U ulFilt = 0x00;

    mcp2515_write_id(MCP_RXM0SIDH, ext, ulMask);
    mcp2515_write_id(MCP_RXM1SIDH, ext, ulMask);

    // Set all filters to 0
    mcp2515_write_id(MCP_RXF0SIDH, ext, ulFilt);
    mcp2515_write_id(MCP_RXF1SIDH, ext, ulFilt);
    mcp2515_write_id(MCP_RXF2SIDH, ext, ulFilt);
    mcp2515_write_id(MCP_RXF3SIDH, ext, ulFilt);
    mcp2515_write_id(MCP_RXF4SIDH, ext, ulFilt);
    mcp2515_write_id(MCP_RXF5SIDH, ext, ulFilt);

    // Clear and deactivate all TX buffers
    a1 = MCP_TXB0CTRL;
    a2 = MCP_TXB1CTRL;
    a3 = MCP_TXB2CTRL;

    for (i = 0; i < 14; i++) {
        mcp2515_setRegister(a1, 0);
        mcp2515_setRegister(a2, 0);
        mcp2515_setRegister(a3, 0);
        a1++;
        a2++;
        a3++;
    }

    mcp2515_setRegister(MCP_RXB0CTRL, 0);
    mcp2515_setRegister(MCP_RXB1CTRL, 0);
}

/*********************************************************************************************************
** Function name:           mcp2515_init
** Descriptions:            Initialize MCP2515
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_init(const INT8U canSpeed)
{
    INT8U res;

    mcp2515_reset();

    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if (res > 0) {
#if DEBUG_MODE
        Serial.print("Enter setting mode fail\r\n");
#endif
        return res;
    }

#if DEBUG_MODE
    Serial.print("Enter setting mode success \r\n");
#endif

    if (mcp2515_configRate(canSpeed)) {
#if DEBUG_MODE
        Serial.print("set rate fail!!\r\n");
#endif
        return res;
    }

#if DEBUG_MODE
    Serial.print("set rate success!!\r\n");
#endif

    if (res == MCP2515_OK) {
        mcp2515_initCANBuffers();

        // Enable receive interrupts
        mcp2515_setRegister(MCP_CANINTE, MCP_RX0IF | MCP_RX1IF);

#if (DEBUG_RXANY == 1)
        mcp2515_modifyRegister(
            MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK
        );
        mcp2515_modifyRegister(
            MCP_RXB1CTRL,
            MCP_RXB_RX_MASK,
            MCP_RXB_RX_ANY
        );
#else
        mcp2515_modifyRegister(
            MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK
        );
        mcp2515_modifyRegister(
            MCP_RXB1CTRL,
            MCP_RXB_RX_MASK,
            MCP_RXB_RX_STDEXT
        );
#endif

        // Enter normal mode
        res = mcp2515_setCANCTRL_Mode(MODE_NORMAL);
        if (res) {
#if DEBUG_MODE
            Serial.print("Enter Normal Mode Fail!!\r\n");
#endif
            return res;
        }

#if DEBUG_MODE
        Serial.print("Enter Normal Mode Success!!\r\n");
#endif
    }

    return res;
}

/*********************************************************************************************************
** Function name:           mcp2515_write_id
** Descriptions:            Write CAN ID
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_id(const INT8U mcp_addr, const INT8U ext, const INT32U id)
{
    uint16_t canid;
    INT8U tbufdata[4];

    canid = (uint16_t)(id & 0x0FFFF);

    if (ext == 1) {
        tbufdata[MCP_EID0] = (INT8U)(canid & 0xFF);
        tbufdata[MCP_EID8] = (INT8U)(canid / 256);
        canid = (uint16_t)(id / 0x10000L);
        tbufdata[MCP_SIDL] = (INT8U)(canid & 0x03);
        tbufdata[MCP_SIDL] += (INT8U)((canid & 0x1C) * 8);
        tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
        tbufdata[MCP_SIDH] = (INT8U)(canid / 32);
    } else {
        tbufdata[MCP_SIDH] = (INT8U)(canid / 8);
        tbufdata[MCP_SIDL] = (INT8U)((canid & 0x07) << 5);
        tbufdata[MCP_EID0] = 0;
        tbufdata[MCP_EID8] = 0;
    }

    mcp2515_setRegisterS(mcp_addr, tbufdata, 4);
}

/*********************************************************************************************************
** Function name:           mcp2515_read_id
** Descriptions:            Read CAN ID
*********************************************************************************************************/
void MCP_CAN::mcp2515_read_id(const INT8U mcp_addr, INT8U* ext, INT32U* id)
{
    INT8U tbufdata[4];

    *ext = 0;
    *id = 0;

    mcp2515_readRegisterS(mcp_addr, tbufdata, 4);

    *id = (tbufdata[MCP_SIDH] << 3) + (tbufdata[MCP_SIDL] >> 5);

    if ((tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) == MCP_TXB_EXIDE_M) {
        *id = (*id << 2) + (tbufdata[MCP_SIDL] & 0x03);
        *id <<= 16;
        *id = *id + (tbufdata[MCP_EID8] << 8) + tbufdata[MCP_EID0];
        *ext = 1;
    }
}

/*********************************************************************************************************
** Function name:           mcp2515_write_canMsg
** Descriptions:            Write CAN message to TX buffer
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_canMsg(const INT8U buffer_sidh_addr)
{
    INT8U mcp_addr = buffer_sidh_addr;

    mcp2515_setRegisterS(mcp_addr + 5, m_nDta, m_nDlc);

    if (m_nRtr == 1) {
        m_nDlc |= MCP_RTR_MASK;
    }

    mcp2515_setRegister(mcp_addr + 4, m_nDlc);
    mcp2515_write_id(mcp_addr, m_nExtFlg, m_nID);
}

/*********************************************************************************************************
** Function name:           mcp2515_read_canMsg
** Descriptions:            Read CAN message from RX buffer
*********************************************************************************************************/
void MCP_CAN::mcp2515_read_canMsg(const INT8U buffer_sidh_addr)
{
    INT8U mcp_addr = buffer_sidh_addr;
    INT8U ctrl;

    mcp2515_read_id(mcp_addr, &m_nExtFlg, &m_nID);

    ctrl = mcp2515_readRegister(mcp_addr - 1);
    m_nDlc = mcp2515_readRegister(mcp_addr + 4);

    if (ctrl & 0x08) {
        m_nRtr = 1;
    } else {
        m_nRtr = 0;
    }

    m_nDlc &= MCP_DLC_MASK;
    mcp2515_readRegisterS(mcp_addr + 5, &(m_nDta[0]), m_nDlc);
}

/*********************************************************************************************************
** Function name:           mcp2515_start_transmit
** Descriptions:            Request transmission
*********************************************************************************************************/
void MCP_CAN::mcp2515_start_transmit(const INT8U mcp_addr)
{
    mcp2515_modifyRegister(mcp_addr - 1, MCP_TXB_TXREQ_M, MCP_TXB_TXREQ_M);
}

/*********************************************************************************************************
** Function name:           mcp2515_getNextFreeTXBuf
** Descriptions:            Get next free TX buffer
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_getNextFreeTXBuf(INT8U *txbuf_n)
{
    INT8U res;
    INT8U i;
    INT8U ctrlval;
    INT8U ctrlregs[MCP_N_TXBUFFERS] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL };

    res = MCP_ALLTXBUSY;
    *txbuf_n = 0x00;

    for (i = 0; i < MCP_N_TXBUFFERS; i++) {
        ctrlval = mcp2515_readRegister(ctrlregs[i]);
        if ((ctrlval & MCP_TXB_TXREQ_M) == 0) {
            *txbuf_n = ctrlregs[i] + 1; // Return TXBnSIDH address
            res = MCP2515_OK;
            return res;
        }
    }

    return res;
}

/*********************************************************************************************************
** Function name:           begin
** Descriptions:            Initialize CAN with default 16 MHz clock
*********************************************************************************************************/
INT8U MCP_CAN::begin(INT8U speedset)
{
    INT8U res;

    pinMode(m_csPin, OUTPUT);
    digitalWrite(m_csPin, HIGH);

#ifdef SS
    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);
#endif

    SPI.begin();

    m_clockMHz = MCP_16MHZ;
    res = mcp2515_init(speedset);

    if (res == MCP2515_OK) {
        return CAN_OK;
    } else {
        return CAN_FAILINIT;
    }
}

/*********************************************************************************************************
** Function name:           begin
** Descriptions:            Initialize CAN with explicit clock
*********************************************************************************************************/
INT8U MCP_CAN::begin(INT8U speedset, INT8U clockset)
{
    INT8U res;

    pinMode(m_csPin, OUTPUT);
    digitalWrite(m_csPin, HIGH);

#ifdef SS
    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);
#endif

    SPI.begin();

    if (clockset == MCP_8MHZ) {
        m_clockMHz = MCP_8MHZ;
    } else {
        m_clockMHz = MCP_16MHZ;
    }

    res = mcp2515_init(speedset);

    if (res == MCP2515_OK) {
        return CAN_OK;
    } else {
        return CAN_FAILINIT;
    }
}

/*********************************************************************************************************
** Function name:           init_Mask
** Descriptions:            Initialize mask
*********************************************************************************************************/
INT8U MCP_CAN::init_Mask(INT8U num, INT8U ext, INT32U ulData)
{
    INT8U res = MCP2515_OK;

#if DEBUG_MODE
    Serial.print("Begin to set Mask!!\r\n");
#endif

    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if (res > 0) {
#if DEBUG_MODE
        Serial.print("Enter setting mode fail\r\n");
#endif
        return res;
    }

    if (num == 0) {
        mcp2515_write_id(MCP_RXM0SIDH, ext, ulData);
    } else if (num == 1) {
        mcp2515_write_id(MCP_RXM1SIDH, ext, ulData);
    } else {
        res = MCP2515_FAIL;
    }

    res = mcp2515_setCANCTRL_Mode(MODE_NORMAL);
    if (res > 0) {
#if DEBUG_MODE
        Serial.print("Enter normal mode fail\r\n");
#endif
        return res;
    }

#if DEBUG_MODE
    Serial.print("set Mask success!!\r\n");
#endif

    return res;
}

/*********************************************************************************************************
** Function name:           init_Filt
** Descriptions:            Initialize filter
*********************************************************************************************************/
INT8U MCP_CAN::init_Filt(INT8U num, INT8U ext, INT32U ulData)
{
    INT8U res = MCP2515_OK;

#if DEBUG_MODE
    Serial.print("Begin to set Filter!!\r\n");
#endif

    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if (res > 0) {
#if DEBUG_MODE
        Serial.print("Enter setting mode fail\r\n");
#endif
        return res;
    }

    switch (num) {
        case 0:
            mcp2515_write_id(MCP_RXF0SIDH, ext, ulData);
            break;

        case 1:
            mcp2515_write_id(MCP_RXF1SIDH, ext, ulData);
            break;

        case 2:
            mcp2515_write_id(MCP_RXF2SIDH, ext, ulData);
            break;

        case 3:
            mcp2515_write_id(MCP_RXF3SIDH, ext, ulData);
            break;

        case 4:
            mcp2515_write_id(MCP_RXF4SIDH, ext, ulData);
            break;

        case 5:
            mcp2515_write_id(MCP_RXF5SIDH, ext, ulData);
            break;

        default:
            res = MCP2515_FAIL;
            break;
    }

    res = mcp2515_setCANCTRL_Mode(MODE_NORMAL);
    if (res > 0) {
#if DEBUG_MODE
        Serial.print("Enter normal mode fail\r\nSet filter fail!!\r\n");
#endif
        return res;
    }

#if DEBUG_MODE
    Serial.print("set Filter success!!\r\n");
#endif

    return res;
}

/*********************************************************************************************************
** Function name:           setMsg
** Descriptions:            Set CAN message fields
*********************************************************************************************************/
INT8U MCP_CAN::setMsg(INT32U id, INT8U ext, INT8U len, INT8U *pData)
{
    INT8U i;

    m_nExtFlg = ext;
    m_nID = id;

    if (len > MAX_CHAR_IN_MESSAGE) {
        len = MAX_CHAR_IN_MESSAGE;
    }

    m_nDlc = len;

    for (i = 0; i < len; i++) {
        m_nDta[i] = pData[i];
    }

    for (i = len; i < MAX_CHAR_IN_MESSAGE; i++) {
        m_nDta[i] = 0x00;
    }

    return MCP2515_OK;
}

/*********************************************************************************************************
** Function name:           clearMsg
** Descriptions:            Clear internal message buffer
*********************************************************************************************************/
INT8U MCP_CAN::clearMsg()
{
    INT8U i;

    m_nID = 0;
    m_nDlc = 0;
    m_nExtFlg = 0;
    m_nRtr = 0;
    m_nfilhit = 0;

    for (i = 0; i < MAX_CHAR_IN_MESSAGE; i++) {
        m_nDta[i] = 0x00;
    }

    return MCP2515_OK;
}

/*********************************************************************************************************
** Function name:           sendMsg
** Descriptions:            Send message
*********************************************************************************************************/
INT8U MCP_CAN::sendMsg()
{
    INT8U res;
    INT8U txbuf_n;
    const uint32_t txTimeoutUs = 2000UL; // 2 ms timeout

    uint16_t uiTimeOut = 0;

    do {
        res = mcp2515_getNextFreeTXBuf(&txbuf_n);
        uiTimeOut++;
    } while (res == MCP_ALLTXBUSY && (uiTimeOut < TIMEOUTVALUE));

    if (uiTimeOut == TIMEOUTVALUE) {
        return CAN_GETTXBFTIMEOUT;
    }

    mcp2515_write_canMsg(txbuf_n);
    mcp2515_start_transmit(txbuf_n);

    // Wait until TXREQ is cleared without using delayMicroseconds().
    // txbuf_n points to TXBnSIDH, so TXBnCTRL is (txbuf_n - 1).
    const uint32_t startUs = micros();

    while (mcp2515_readRegister(txbuf_n - 1) & MCP_TXB_TXREQ_M) {
        if ((uint32_t)(micros() - startUs) > txTimeoutUs) {
            return CAN_SENDMSGTIMEOUT;
        }
    }

    return CAN_OK;
}

/*********************************************************************************************************
** Function name:           sendMsgBuf
** Descriptions:            Send buffer
*********************************************************************************************************/
INT8U MCP_CAN::sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf)
{
    setMsg(id, ext, len, buf);
    return sendMsg();
}

/*********************************************************************************************************
** Function name:           readMsg
** Descriptions:            Read message from RX buffer
*********************************************************************************************************/
INT8U MCP_CAN::readMsg()
{
    INT8U stat;
    INT8U res;

    stat = mcp2515_readStatus();

    if (stat & MCP_STAT_RX0IF) {
        mcp2515_read_canMsg(MCP_RXBUF_0);
        mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
        res = CAN_OK;
    } else if (stat & MCP_STAT_RX1IF) {
        mcp2515_read_canMsg(MCP_RXBUF_1);
        mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
        res = CAN_OK;
    } else {
        res = CAN_NOMSG;
    }

    return res;
}

/*********************************************************************************************************
** Function name:           readMsgBuf
** Descriptions:            Read message buffer
*********************************************************************************************************/
INT8U MCP_CAN::readMsgBuf(INT8U *len, INT8U buf[])
{
    INT8U rc = readMsg();

    if (rc != CAN_OK) {
        return rc;
    }

    *len = m_nDlc;

    for (INT8U i = 0; i < m_nDlc; i++) {
        buf[i] = m_nDta[i];
    }

    return CAN_OK;
}

/*********************************************************************************************************
** Function name:           checkReceive
** Descriptions:            Check if a message is available
*********************************************************************************************************/
INT8U MCP_CAN::checkReceive(void)
{
    INT8U res;

    res = mcp2515_readStatus();

    if (res & MCP_STAT_RXIF_MASK) {
        return CAN_MSGAVAIL;
    } else {
        return CAN_NOMSG;
    }
}

/*********************************************************************************************************
** Function name:           checkError
** Descriptions:            Check controller error flags
*********************************************************************************************************/
INT8U MCP_CAN::checkError(void)
{
    INT8U eflg = mcp2515_readRegister(MCP_EFLG);

    if (eflg & MCP_EFLG_ERRORMASK) {
        return CAN_CTRLERROR;
    } else {
        return CAN_OK;
    }
}

/*********************************************************************************************************
** Function name:           getCanId
** Descriptions:            Get last received CAN ID
*********************************************************************************************************/
INT32U MCP_CAN::getCanId(void)
{
    return m_nID;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/