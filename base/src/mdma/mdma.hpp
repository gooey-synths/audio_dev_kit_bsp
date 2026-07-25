#ifndef MDMA_HPP
#define MDMA_HPP

#include <stdint.h>
#include <stdlib.h>
#include "../system/stm32h750xx.h"
#include "string.h"

namespace mdma{

#define NUM_MDMA_CHANNELS 16

enum eTriggerMode{
    BUF_TRANS = 0, ///< Buffer transfer
    BLK_TRANS, ///< Block transfer
    RPT_BLK_TRANS, ///< Repeated block transfer
    WHOLE  ///< Transfer whole data
};

struct ListNode{
    uint32_t TCR; ///< Transmission control
    uint32_t BNDTR; ///< Number of data
    uint32_t SAR; ///< Source address
    uint32_t DAR; ///< Desttination address
    uint32_t BRUR; ///< Block repeat update
    uint32_t LAR; ///< Link address
    uint32_t TBR; ///< Trigger and bus selection
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
    inline void setSource(void* src, uint8_t size, uint8_t inc, bool ahb){
        SAR = (uint32_t) src;

        TCR &= ~(3 << 4); // clear SSIZE bits

        TCR |= (3 & (__builtin_ctz(size))) << 4; // set data SSIZE bits


        TCR &= ~(3 << 8); // clear SINCOS bits
        TCR &= ~(3 << 0); // clear SINC bits


        if(inc){
            TCR |= (3 & (__builtin_ctz(inc))) << 8; // set data SINCOS bits
            TCR |= 2 << 0; // set SINC bits
        }

        if(ahb){
            TBR |= 1<<16; // set AHB bit
        }
        else{
            TBR &= ~(1<<16); // clear AHB bit
        }
    }

    ///
    /// Set destination address
    /// @param dst Destination address
    /// @param size Destination data element size in number of bytes
    /// @param inc Destination increment in number of byte
    /// @param ahb True if destination is connected to the AHB bus, false if AXI
    ///
    inline void setDestination(void* dst, uint8_t size, uint8_t inc, bool ahb){
        DAR = (uint32_t) dst;

        TCR &= ~(3 << 6); // clear DSIZE bits

        TCR |= (3 & (__builtin_ctz(size))) << 6; // set data DSIZE bits

        TCR &= ~(3 << 10); // clear DINCOS bits
        TCR &= ~(3 << 2); // clear DINC bits

        if(inc){
            TCR |= (3 & (__builtin_ctz(inc))) << 10; // set data DINCOS bits
            TCR |= 2 << 2; // set DINC bits
        }

        if(ahb){
            TBR |= 1<<17;
        }
        else{
            TBR &= ~(1<<17);
        }
    }

    ///
    /// Link up to another list node
    /// @param node ListNode to link to next, null if finished
    ///
    inline void linkTo(ListNode* node){
        LAR = (uint32_t) node;
    }

    ///
    /// Set the number of data to transfer
    /// @param num Number of data to transfer in this node
    /// @param bytes_per_transfer Number of bytes per transfer
    ///
    inline void setNumberData(uint16_t num, uint8_t bytes_per_tansfer){
        BNDTR = num;
        TCR &= ~(0x7F << 18);
        TCR |= (0x7F & (bytes_per_tansfer -1)) << 18;
        TCR |= 3 << 18;
    }

    ///
    /// Set the trigger for transfer.
    /// @param reqNum Hardware request number
    /// @param softReq True if software request only
    /// @param mode Trigger mode
    ///
    inline void setTrigger(uint8_t reqNum, bool softReq, eTriggerMode mode){
        TCR &= ~(7 << 28); // clear software request and mode bits
        TCR |= (((uint8_t) softReq << 2) | (uint8_t)mode) << 28; // set software request and mode bits

        TBR &= ~(0x3F << 0); // clear trigger number bits
        TBR |= ((0x3F & reqNum) << 0); // set trigger number bits

    }
};

class MDMAChannel{

public:
    ///
    /// Constructor
    /// @param hw POinter to MDM
    ///
    MDMAChannel(MDMA_Channel_TypeDef* hw):
    mChannelHw(hw)
    {;}

    ///
    /// Enable the channel
    ///
    inline void enable(){
        mChannelHw->CCR |=1;
    }

    ///
    /// Disable the channel
    ///
    inline void disable(){
        mChannelHw->CCR &= ~1;
    }

    ///
    /// Trigger the channel
    ///
    inline void trigger(){
        mChannelHw->CCR |= (0xFFFF << 16);
    }

    ///
    /// Check for error.
    /// @return Erro bits from channel
    ///
    inline uint32_t getError(){
        return (uint32_t) mChannelHw->CESR;
    }

    ///
    /// Cofigure the transfer with a configured list node.
    ///
    inline void configureTransfer(ListNode* node){
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
    MDMA_Channel_TypeDef* mChannelHw;
};

class MDMAController{

public:

    ///
    /// Get a pointer to the MDMAController instance.
    /// @return Pointer to the MDMAController instance
    ///
    static MDMAController* getInstance(){
        static int a = 0;
        static MDMAController instance;        
        return &instance;
    }


    ///
    /// Get a pointer to a channel
    /// @param channel Channel to get (0-15)
    /// @return Pointer to a MDMA channel
    ///
    MDMAChannel* getChannel(uint8_t channel){
        return &mChannels[(0xF & channel)];
    }


private:
    ///
    /// Constructor
    ///
    MDMAController():
        mChannels{
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
        RCC->AHB3ENR |= 1; // Enable peripheral clock to MDMA.
    }


    MDMAChannel mChannels[NUM_MDMA_CHANNELS];
};


} // namespace mdma
#endif // MDMA_HPP