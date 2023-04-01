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
		eDmaTransferType mTransferType; //
	};
	
	DmaChannel* getAvailableChannel();
	DmaChannel* claimChannel(uint8_t mChannelNum);
	void releaseChannel(DmaChannel* channel);

private:
	DmaController(uint8_t controller_num);

	void setRequest(uint8_t perID, uint8_t channel);

	uint8_t mControllerHwNum;

	DMA_TypeDef* mControllerHw;

	DmaChannel mChannels[DMA1_NUM_CHANNELS];
};


#endif
}// namespace dma
