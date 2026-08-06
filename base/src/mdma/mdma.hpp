#ifndef MDMA_HPP
#define MDMA_HPP

#include <stdint.h>
#include <stdlib.h>
#include <system/stm32h750xx.h>

namespace mdma {

#define NUM_MDMA_CHANNELS 16
///
/// Enumerations for the trigger modes of the MDMA
///
enum eTriggerMode {
    BUF_TRANS = 0, ///< Buffer transfer
    BLK_TRANS = 1, ///< Block transfer
    RPT_BLK_TRANS = 2, ///< Repeated block transfer
    WHOLE = 3///< Transfer whole data
};

///
/// Structure for defining a single node of a list transfer
///
struct ListNode {
    uint32_t TCR; ///< Transmission control
    uint32_t BNDTR; ///< Number of data
    uint32_t SAR; ///< Source address
    uint32_t DAR; ///< Desttination address
    uint32_t BRUR; ///< Block repeat update
    uint32_t LAR; ///< Link address
    uint32_t TBR; ///< Trigger and bus selection
    uint32_t RESERVED; /// Reserved
    uint32_t MAR; ///< Mask address register
    uint32_t MDR; ///< Mask data register

    ///
    /// Constructor
    ///
    ListNode():
        TCR(0),
        BNDTR(0),
        SAR(0),
        DAR(0),
        BRUR(0),
        LAR(0),
        TBR(0),
        MAR(0),
        MDR(0)
    {;}

    ///
    /// Set the source address
    /// @param src Source address
    /// @param size Source data element size in number of bytes
    /// @param inc Source increment in number of byte
    /// @param ahb True if source is connected to the AHB bus, false if AXI
    ///
    inline void setSource(void* src, uint8_t size, uint8_t inc, bool ahb) {
        SAR = (uint32_t) src;

        TCR &= ~(MDMA_CTCR_SSIZE_Msk); // clear SSIZE bits

        TCR |= (3 & (__builtin_ctz(size))) << MDMA_CTCR_SSIZE_Pos; // set data SSIZE bits

        TCR &= ~(MDMA_CTCR_SINCOS_Msk); // clear SINCOS bits
        TCR &= ~(MDMA_CTCR_SINC_Msk); // clear SINC bits


        if(inc) {
            TCR |= (3 & (__builtin_ctz(inc))) << MDMA_CTCR_SINCOS_Pos; // set data SINCOS bits
            TCR |= MDMA_CTCR_SINC_1; // set SINC bits
        }

        if(ahb) {
            TBR |= MDMA_CTBR_SBUS_Msk; // set AHB bit
        }
        else {
            TBR &= ~(MDMA_CTBR_SBUS_Msk); // clear AHB bit
        }
    }

    ///
    /// Set destination address
    /// @param dst Destination address
    /// @param size Destination data element size in number of bytes
    /// @param inc Destination increment in number of bytes
    /// @param ahb True if destination is connected to the AHB bus, false if AXI
    ///
    inline void setDestination(void* dst, uint8_t size, uint8_t inc, bool ahb) {
        DAR = (uint32_t) dst;

        TCR &= ~(MDMA_CTCR_DSIZE_Msk); // clear DSIZE bits

        TCR |= (3 & (__builtin_ctz(size))) << MDMA_CTCR_DSIZE_Pos; // set data DSIZE bits

        TCR &= ~(MDMA_CTCR_DINCOS_Msk); // clear DINCOS bits
        TCR &= ~(MDMA_CTCR_DINC_Msk); // clear DINC bits

        if(inc) {
            TCR |= (3 & (__builtin_ctz(inc))) << MDMA_CTCR_DINCOS_Pos; // set data DINCOS bits
            TCR |= MDMA_CTCR_DINC_1; // set DINC bits
        }

        if(ahb) {
            TBR |= MDMA_CTBR_DBUS_Msk; // set AHB bit
        }
        else {
            TBR &= ~(MDMA_CTBR_DBUS_Msk); // clear AHB bit
        }
    }

    ///
    /// Link up to another list node
    /// @param node ListNode to link to next, null if finished
    ///
    inline void linkTo(ListNode* node) {
        LAR = (uint32_t) node;
    }

    ///
    /// Set the number of data to transfer
    /// @param num Total number of bytes to transfer in this node
    /// @param bytesPerTransfer Number of bytes per transfer
    ///
    inline void setNumberData(uint16_t num, uint8_t bytesPerTransfer) {
        BNDTR = num;
        TCR &= ~(MDMA_CTCR_TLEN_Msk);
        TCR |= (0x7F & (bytesPerTransfer - 1)) << MDMA_CTCR_TLEN_Pos;
    }

    ///
    /// Set the trigger for transfer.
    /// @param reqNum Hardware request number
    /// @param softReq True if software request only
    /// @param mode Trigger mode
    ///
    inline void setTrigger(uint8_t reqNum, bool softReq, eTriggerMode mode) {
        TCR &= ~(MDMA_CTCR_TRGM_Msk | MDMA_CTCR_SWRM_Msk); // clear software request and mode bits
        TCR |= (((uint8_t) softReq << 2) | (uint8_t)mode) << MDMA_CTCR_TRGM_Pos; // set software request and mode bits

        TBR &= ~(MDMA_CTBR_TSEL_Msk); // clear trigger number bits
        TBR |= ((0x3F & reqNum) << MDMA_CTBR_TSEL_Pos); // set trigger number bits

    }
};

///
/// Single channel of the MDMA controller
///
class MDMAChannel {
public:
    ///
    /// Constructor
    /// @param hw Pointer to MDMA
    ///
    MDMAChannel(MDMA_Channel_TypeDef* hw):
    mChannelHw(hw)
    {;}

    ///
    /// Enable the channel
    ///
    inline void enable() {
        mChannelHw->CCR |= MDMA_CCR_EN;
    }

    ///
    /// Disable the channel
    ///
    inline void disable() {
        mChannelHw->CCR &= ~MDMA_CCR_EN;
    }

    ///
    /// Trigger the channel
    ///
    inline void trigger() {
        mChannelHw->CCR |= MDMA_CCR_SWRQ;
    }

    ///
    /// Check for error.
    /// @return Error bits from channel
    ///
    inline uint32_t getError() {
        return (uint32_t) mChannelHw->CESR;
    }

    ///
    /// Cofigure the transfer with a configured list node.
    ///
    inline void configureTransfer(ListNode* node) {
        mChannelHw->CTCR = node->TCR;
        mChannelHw->CBNDTR = node->BNDTR;
        mChannelHw->CSAR = node->SAR;
        mChannelHw->CDAR = node->DAR;
        mChannelHw->CBRUR = node->BRUR;
        mChannelHw->CLAR = node->LAR;
        mChannelHw->CTBR = node->TBR;
        mChannelHw->CMAR = node->MAR;
        mChannelHw->CMDR = node->MDR;
}

private:
    friend class MDMAController;
    MDMA_Channel_TypeDef* mChannelHw; ///< Pointer to HW registers of the MDMA channel
};

///
/// Class for defining an MDMA controller.
///
class MDMAController {
public:
    ///
    /// Get a pointer to the MDMAController instance.
    /// @return Pointer to the MDMAController instance
    ///
    static MDMAController* getInstance() {
        static MDMAController instance;
        return &instance;
    }

    ///
    /// Get a pointer to a channel
    /// @param channel Channel to get (0-15)
    /// @return Pointer to a MDMA channel
    ///
    MDMAChannel* getChannel(uint8_t channel) {
        return &mChannels[(0xF & channel)];
    }

private:
    ///
    /// Constructor
    ///
    MDMAController():
        mChannels {
           MDMAChannel(MDMA_Channel0),
           MDMAChannel(MDMA_Channel1),
           MDMAChannel(MDMA_Channel2),
           MDMAChannel(MDMA_Channel3),
           MDMAChannel(MDMA_Channel4),
           MDMAChannel(MDMA_Channel5),
           MDMAChannel(MDMA_Channel6),
           MDMAChannel(MDMA_Channel7),
           MDMAChannel(MDMA_Channel8),
           MDMAChannel(MDMA_Channel9),
           MDMAChannel(MDMA_Channel10),
           MDMAChannel(MDMA_Channel11),
           MDMAChannel(MDMA_Channel12),
           MDMAChannel(MDMA_Channel13),
           MDMAChannel(MDMA_Channel14),
           MDMAChannel(MDMA_Channel15),
        }
    {
        RCC->AHB3ENR |= RCC_AHB3ENR_MDMAEN; // Enable peripheral clock to MDMA.
    }

    MDMAChannel mChannels[NUM_MDMA_CHANNELS]; ///< MDMA channels of the controller.
};


} // namespace mdma
#endif // MDMA_HPP