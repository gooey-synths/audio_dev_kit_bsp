#pragma once

#include <cstdint>
#include <system/stm32h750xx.h>
#include <mdma/mdma.hpp>

namespace qspi {

///
/// Functional mode of the QSPI.
///
enum eMode {
    INDIRECT_WRITE = 0b00,
    INDIRECT_READ  = 0b01,
    STATUS_POLLING = 0b10,
    MEMORY_MAPPED  = 0b11
};

///
/// Number of lines to use during command phases
///
enum eNumLines {
    DISABLED = 0b00,
    ONE      = 0b01,
    TWO      = 0b10,
    FOUR     = 0b11,
};

///
/// Number of bytes that are used for the address and alternate sections of the command.
///
enum eCommandSize {
    EIGHT      = 0b00,
    SIXTEEN    = 0b01,
    TWENTYFOUR = 0b10,
    THIRTYTWO  = 0b10,
};

///
/// Determine which device is used during the transfer.
///
enum eDeviceSelection {
    DEV_1 = 0b00,
    DEV_2 = 0b01,
    DUAL,
};

///
/// Device specific configuration.
///
struct DeviceConfiguration {
    uint32_t freq           = 0;     ///< Desired clock frequency.
    eDeviceSelection devSel = DEV_1; ///< Which QSPI device to talk to during operation.
    uint8_t nAddrBits       = 1;     ///< Number of address bits (max of 6).
    uint8_t csHighTime      = 1;     ///< Number of clock cycles to hold the CS high for between commands ().
    bool clkPol             = 0;     ///< True if the clock line should idle high.
};

///
/// Command header information
///
struct Header {
    uint32_t address;   ///< Address for a given command.
    uint32_t alternate; ///< Alternate bytes for a given command.
};

///
/// Configuration for status polling mode.
///
struct StatusPollingConfigurtion {
    uint16_t interval = 0; ///< Status polling interval in number of clock cycles.
    uint32_t mask     = 0; ///< Mask to apply to data.
    uint32_t match    = 0; ///< Bits to match to.
    size_t dataLength = 0; ///< Data length (max of 4).
    bool orMode       = 0; ///< True if any bits cause a match, false if all bits cause a match.
    bool stopOnMatch  = 0; ///< Stop status polling on match.
};

///
/// Struture specifying what a command looks like.
///
struct CommunicationConfiguration {
    uint8_t instruction        = 0;          ///< Instruction to send during a command.
    eNumLines instructionMode  = DISABLED;   ///< Number of lines to use during the instruction phase.
    eNumLines addressMode      = DISABLED;   ///< Number of lines to use during the address phase.
    eCommandSize addressSize   = EIGHT;      ///< Number of bits to send for the address phase.
    eNumLines alternateMode    = DISABLED;   ///< Number of lines to use during the alternate phase.
    eCommandSize alternateSize = EIGHT;      ///< Number of bits to send for the alternate phase.
    uint8_t dummyCycles        = 0;          ///< Mumber of dummy cycles for the dummy phase.
    eNumLines dataMode         = DISABLED;   ///< Number of lines to use for the data phase.
    bool sendInstOnce          = false;      ///< True if the instruction should only be sent once.
    bool doubleDataRate        = false;      ///< True if double data rate should be used.

    ///
    /// Convert to register definition.
    /// @return Word to be loaded into the CCR register.
    /// @note This leaves the CCR functional mode bits as 0.
    ///
    uint32_t toReg() {
        uint32_t ccr = 0;

        ccr |= ((uint32_t)instruction << QUADSPI_CCR_INSTRUCTION_Pos) & QUADSPI_CCR_INSTRUCTION_Msk;
        ccr |= ((uint32_t)instructionMode << QUADSPI_CCR_IMODE_Pos) & QUADSPI_CCR_IMODE_Msk;
        ccr |= ((uint32_t)addressMode << QUADSPI_CCR_ADMODE_Pos) & QUADSPI_CCR_ADMODE_Msk;
        ccr |= ((uint32_t)addressSize << QUADSPI_CCR_ADSIZE_Pos) & QUADSPI_CCR_ADSIZE_Msk;
        ccr |= ((uint32_t)alternateMode << QUADSPI_CCR_ABMODE_Pos) & QUADSPI_CCR_ABMODE_Msk;
        ccr |= ((uint32_t)alternateSize << QUADSPI_CCR_ABSIZE_Pos) & QUADSPI_CCR_ABSIZE_Msk;
        ccr |= ((uint32_t)dummyCycles << QUADSPI_CCR_DCYC_Pos) & QUADSPI_CCR_DCYC_Msk;
        ccr |= ((uint32_t)dataMode << QUADSPI_CCR_DMODE_Pos) & QUADSPI_CCR_DMODE_Msk;
        ccr |= ((uint32_t)sendInstOnce << QUADSPI_CCR_SIOO_Pos) & QUADSPI_CCR_SIOO_Msk;
        ccr |= ((uint32_t)doubleDataRate << QUADSPI_CCR_DDRM_Pos) & QUADSPI_CCR_DDRM_Msk;
        return ccr;
    }
};

///
/// Class for communicating over QSPI.
///
class QSpi {
public:
    QSpi();

    virtual ~QSpi();


    void setDeviceConfiguration(DeviceConfiguration& dev);

    void startMemoryMapped();

    void startIndirectRead(Header& head, CommunicationConfiguration& comm, uint8_t* buf, size_t bufLen);

    void startIndirectWrite(Header& head, CommunicationConfiguration& comm, uint8_t* buf, size_t bufLen);

    void startStatusPolling(Header& head, CommunicationConfiguration& comm, StatusPollingConfigurtion& spConf);

    bool statusPollingMatch();

    ///
    /// Stop any incoming transfer.
    ///
    void stop() {
        mQspi->CR |= QUADSPI_CR_ABORT_Msk;
        while(isBusy());
    }

    ///
    /// Check if the QSPI peripheral is busy.
    /// @return True if the QSPI is busy.
    ///
    bool isBusy() {
        return mQspi->SR & QUADSPI_SR_BUSY_Msk;
    }
private:
    void setHeader(Header& head);

    QUADSPI_TypeDef* mQspi;         ///< QSPI hardware.
    mdma::MDMAChannel* mMdmaCh;     ///< MDMA channel used for indirect mode transfers.
    mdma::ListNode mMdmaList[3];    ///< MDMA list for chaining indirect mode transfers.
};

} // namespace qspi

