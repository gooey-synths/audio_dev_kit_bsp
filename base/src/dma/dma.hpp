#ifndef DMA_CHANNEL_HPP
#define DMA_CHANNEL_HPP

#include <cstddef>
#include "../system/stm32h750xx.h"

namespace dma{

#define DMA1_NUM_CHANNELS 8

#if 0
#define DMA1_CLEAR_INT_FLAG(channel, int_type) DMA1->IFCR |= 1 << ((channel-1)*4+(int_type+1))
#endif

enum eDmaTransferType{
	PER2MEM = 0,
	MEM2PER,
	MEM2MEM

};

enum eDmaErrorType{
	TRANSFER,
	HALF_TRANSFER,
	ERROR
};

enum eDmaInterruptFlags{
	FIFO_ERROR_INT = 1<<0,
	RESERVED_INT = 1<<1,
	DIR_MODE_ERR_INT = 1<<2,
	TRAN_ERR_INT = 1<<3,
	HALF_TRANS_INT = 1<<4,
	TRANS_COMPLETE = 1<<5,
};

class DmaController{
public:

	///
	/// Get singleton instance of A DMA controller.
	/// @param controller_num Controller 1 or 2.
	/// @return Pointer to DmaController instance.
	///
	static DmaController* getInstance(uint8_t controller_num){
		static DmaController instance1(1);
		static DmaController instance2(2);

		switch(controller_num){
			case 1:
				return &instance1;
			case 2:
				return &instance2;
		}

		return NULL;
	
	}
	class DmaChannel{
	public:	
		DmaChannel();
		void setRequest(uint8_t perID);
		void setTransferType(eDmaTransferType type, bool dma_flow);
		void setSource(void* src, size_t size, uint8_t inc);
		void setDest(void* dst, size_t size, uint8_t inc);
		void setNumTransfers(uint16_t numTransfers, uint8_t circ);
		eDmaErrorType getError();
		uint8_t begin();
		void disable();

		///
		/// Get the number of transfers left on the chennel
		/// 
		uint16_t getNumTrasfers(){
			return mStreamHw->NDTR;
		}

	protected:
		uint8_t mChannelNum; ///< Channel number, 1 indexed to align with reference manual
		uint8_t mChannelClaimed; ///< 1 if channel is claimed, 0 otherwise
		uint8_t mChannelPriority; ///< Priority of channel
		DmaController* mChannelController; ///< Pointer to controller for this channel
		DMA_Stream_TypeDef* mStreamHw; ///< pointer to either DMA1 or DMA2 strean
		void claim();
		void release();

	private:
		friend class DmaController;
		eDmaTransferType mTransferType; ///< Transfer type that the channel is configured for
	};
	
	DmaChannel* claimAvailableChannel();
	DmaChannel* claimChannel(uint8_t mChannelNum);
	void releaseChannel(DmaChannel* channel);

private:
	DmaController(uint8_t controller_num);

	void setRequest(uint8_t perID, uint8_t channel);

	///
	/// Clear interrupts on a certain channel.
	/// @param intMask Bitmask representing the interrupt flags to clear see @ref dma::eDmaInterruptFlags
	/// @param channel Channel to clear interrupts on (1-8)
	///
	inline void clearInterrupt(uint8_t intMask, uint8_t channel){
		if(channel < 1 || channel > DMA1_NUM_CHANNELS){
			return; // Invalid channel
		}
		channel--;
		intMask &= 0x3F;

		if(channel < 2){
			mControllerHw->LIFCR |= intMask << channel*6;
		}
		else if(channel < 4){
			mControllerHw->LIFCR |= intMask << (channel*6)+4;
		}
		else if(channel < 6){
			mControllerHw->HIFCR |= intMask << (channel-4)*6;
		}
		else if(channel < 8){
			mControllerHw->HIFCR |= intMask << ((channel-4)*6)+4;
		}
	}

	uint8_t mControllerHwNum; ///< Number of the DMA controller (1 or 2)

	DMA_TypeDef* mControllerHw; ///< DMA controller registers

	DmaChannel mChannels[DMA1_NUM_CHANNELS]; ///< DMA channels
};


#endif
}// namespace dma
