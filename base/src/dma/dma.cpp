#include <cstdint>

#include "dma.hpp"
#include "../system/stm32h750xx.h"
#include "../system/core_cm7.h"

using namespace dma;

/// Message when unable to claim a DMA channel.
static const char* const scCouldNotClaim = "Could not claim DMA channel";
/// Message when datasize is invalid.
static const char* const scInvalidSize = "Invalid data size";

///
/// Constructor
///
DmaController::DmaController(uint8_t controller_num):
	mControllerHwNum(controller_num)
	{
	if(controller_num  > 2 || controller_num < 1) {
		throw "Invalid DMA controller";
	}
	for (uint8_t iChannel = 0; iChannel < DMA1_NUM_CHANNELS; iChannel++){
		mChannels[iChannel].mChannelNum = iChannel + 1;
		mChannels[iChannel].mChannelClaimed = 0;
		mChannels[iChannel].mChannelController = this;
		switch(mControllerHwNum){
			case 1:
				mChannels[iChannel].mStreamHw = DMA1_Stream0+iChannel; // NOLINT
				mControllerHw = DMA1; // NOLINT
				break;
			case 2:
				mChannels[iChannel].mStreamHw = DMA2_Stream0+iChannel; // NOLINT
				mControllerHw = DMA2; // NOLINT
				break;
		}

	}

	RCC->AHB1ENR |= 1 << (mControllerHwNum -1); // enable DMA controller
	RCC->AHB1RSTR |= 1 << (mControllerHwNum -1); // reset DMA controller
	RCC->AHB1RSTR &= ~(1 << (mControllerHwNum -1)); // reset DMA controller
}

///
/// Set the peripheral request for a specific dma channel.
/// @param perID Request ID
/// @param channel channel
///
void DmaController::setRequest(uint8_t perID, uint8_t channel){
	perID &= 0x3F; // keep it to 6 bits.

	*(&(DMAMUX1_Channel0->CCR)+(channel-1) + ((mControllerHwNum-1)*8)) = perID;  // NOLINT
}


///
/// Claim a DMA channel.
/// @param channelNum Channel number to claim (1-8)
/// @return Pointer to Dma channel object or NULL if channel not available.
///
DmaController::DmaChannel* DmaController::claimChannel(uint8_t channelNum){
	if(channelNum < 1 || channelNum > DMA1_NUM_CHANNELS){
		throw scInvaliChannel;
	}
	if(mChannels[channelNum-1].mChannelClaimed == 0){
		mChannels[channelNum-1].claim();
		return &mChannels[channelNum-1];
	}
	throw scCouldNotClaim;
}

///
/// Release a DMA channel.
/// @param channel Pointer to channel to free
///
void DmaController::releaseChannel(DmaController::DmaChannel* channel){
	if(!channel){
		return; // Null pointer
	}
	channel->release();
}

///
/// Claim and get pointer to the next available DMA channel or NULL if all are taken
/// @return Next availble channel or NULL if none available
///
DmaController::DmaChannel* DmaController::claimAvailableChannel(){
	for(uint8_t iChannel = 0; iChannel < DMA1_NUM_CHANNELS; iChannel++){
		if(!mChannels[iChannel].mChannelClaimed){
			return claimChannel(iChannel+1);
		}
	}
	throw scCouldNotClaim;
}

///
/// Constructor.
///
DmaController::DmaChannel::DmaChannel(){
	mStreamHw->CR |= 1<<20;
}

///
/// Claim the DMA channel
///
void DmaController::DmaChannel::claim(){
	mChannelClaimed = 1;
}

///
/// Release the DMA channel
///
void DmaController::DmaChannel::release(){
	mStreamHw->CR &= ~(1 << 0);
	mChannelClaimed = 0;
}

///
/// Set the type of transfer
/// @param type Type of the transfer PER2MEM, etc
/// @param dma_flow True if the DMA channel should control the flow
///
void DmaController::DmaChannel::setTransferType(eDmaTransferType type, bool dma_flow){
	uint8_t bits = 0; // default is per2mem
	if(type == MEM2MEM){
		bits = 2;
		mStreamHw->CR |= 3 << 16;
		mStreamHw->FCR |= 3 << 0;
	}
	else if(type == MEM2PER){
		bits = 1;
	}
	mStreamHw->CR &= ~(3<<6); // clear dir bits.
	mStreamHw->CR |= bits << 6; // set dir bits

	if(dma_flow){
		mStreamHw->CR &= ~(1<<5); // Dma channel is the flow controller
	}
	else{
		mStreamHw->CR |= (1<<5); // Target peripheral is the flow controller
	}

	mTransferType = type;
}

///
/// Set the source of the DMA transfer
/// @param src Pointer to the souce of the trasfer, buffer or register address
/// @param size size of the tranfer uint8_t, uin16_t, uint32_t
/// @param inc Should the source be incremented every transfer.
/// @note The transfer type must be configured first using @ref dma::DmaController::DmaChannel::setmTransferType
///
void DmaController::DmaChannel::setSource(void* src, size_t size, uint8_t inc){
	if(size < 1 || size > 4 || size == 3){
		throw scInvalidSize;
	}

	uint8_t mincpinc, msizepsize;

	if(mTransferType == PER2MEM || mTransferType == MEM2MEM){
		mStreamHw->PAR = reinterpret_cast<intptr_t>(src); // set addr

		mincpinc = 9;
		msizepsize = 11;
	}
	else if(mTransferType == MEM2PER){
		mStreamHw->M0AR = reinterpret_cast<intptr_t>(src);

		mincpinc = 10;
		msizepsize = 13;
	}

	mStreamHw->CR &= ~(0x03 << msizepsize); // clear msize or psize
	mStreamHw->CR |= (__builtin_ctz(size) << msizepsize); // set msize or psize
	if(inc){
		mStreamHw->CR |= (1 << mincpinc); // set minc or pinc
	}
	else{
		mStreamHw->CR &= ~(1 << mincpinc); // clear minc or pinc
	}

}

///
/// Set the destination of the DMA transfer
/// @param dst Pointer to the destination of the trasfer, buffer or register address
/// @param size size of the tranfer uint8_t, uin16_t, uint32_t
/// @param inc Should the destination be incremented every transfer.
/// @note The transfer type must be configured first using @ref dma::DmaController::DmaChannel::setmTransferType
///
void DmaController::DmaChannel::setDest(void* dst, size_t size, uint8_t inc){
	if(size < 1 || size > 4 || size == 3){
		throw scInvalidSize;
	}

	uint8_t mincpinc, msizepsize; 

	if(mTransferType == PER2MEM || mTransferType == MEM2MEM){
		mStreamHw->M0AR = reinterpret_cast<intptr_t>(dst);
		mincpinc = 10;
		msizepsize = 13;
	}
	else if(mTransferType == MEM2PER){
		mStreamHw->PAR = reinterpret_cast<intptr_t>(dst); // set addr
		mincpinc = 9;
		msizepsize = 11;
	}

	mStreamHw->CR &= ~(0x03 << msizepsize); // clear msize or psize
	mStreamHw->CR |= (__builtin_ctz(size) << msizepsize); // set msize or psize
	if(inc){
		mStreamHw->CR |= (1 << mincpinc); // set minc or pinc
	}
	else{
		mStreamHw->CR &= ~(1 << mincpinc); // clear minc or pinc
	}

}

///
/// Set the number of transfers for the DMA channel
/// @param numTransfers NUmber of transfers to dox1
/// @param circ True if you want the dma transfer to repeat after it is finished
///
void DmaController::DmaChannel::setNumTransfers(uint16_t numTransfers, uint8_t circ){
	if(circ){
		mStreamHw->CR |= 1 << 8; // set circ bit
	}
	else{
		mStreamHw->CR &= ~(1 << 8); // clear circ bit
	}
	mStreamHw->NDTR = numTransfers;
}

///
/// Begin the transfer
///
uint8_t DmaController::DmaChannel::begin(){	
	mChannelController->clearInterrupt(0x3f, mChannelNum);

	mStreamHw->CR |= 1 << 0;
	return 0;
}

///
/// Disable the channel
///
void DmaController::DmaChannel::disable(){
	mStreamHw->CR &= ~(1 << 0);
	while(mStreamHw->CR & (1 << 0)); // wait for bit to go to 0
}

///
/// Set the reuquest ID for a given channel
/// @param perID ID of the peripheral.
///
void DmaController::DmaChannel::setRequest(uint8_t perID){
	mChannelController->setRequest(perID, mChannelNum);
}