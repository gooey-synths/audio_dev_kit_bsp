#include "qspi.hpp"
#include <system/board_defs.h>

namespace qspi {

///
/// Constructor.
///
QSpi::QSpi()
    : mQspi(QUADSPI), mMdmaCh(mdma::MDMAController::getInstance()->getChannel(0))
{
    RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;
    mQspi->CR |= QUADSPI_CR_EN;
}

///
/// Destructor.
///
QSpi::~QSpi() {
    RCC->AHB3ENR &= ~RCC_AHB3ENR_QSPIEN;
    mQspi->CR &= ~QUADSPI_CR_EN;
}

///
/// Set the device configuration
/// @param dev Desired device configuration.
/// @note this will stop the QSPI.
///
void QSpi::setDeviceConfiguration(DeviceConfiguration& dev) {
    stop();

    // Set frequency prescaler
    // This is the default kernel clock
    uint32_t kerClk = AHB_AXI_TARGET;
    uint32_t prescaler = (kerClk / dev.freq) - 1;
    mQspi->CR &= ~QUADSPI_CR_PRESCALER_Msk;
    mQspi->CR |= (prescaler << QUADSPI_CR_PRESCALER_Pos) & QUADSPI_CR_PRESCALER_Msk;

    // Set device selection
    if(dev.devSel == DUAL) {
        mQspi->CR |= QUADSPI_CR_DFM;
    } else {
        mQspi->CR &= ~QUADSPI_CR_DFM;
        if(dev.devSel == DEV_1) {
            mQspi->CR &= ~QUADSPI_CR_FSEL;
        } else {
            mQspi->CR |= QUADSPI_CR_FSEL;
        }
    }

    uint32_t dcr = 0;

    // Set number of address bits
    dcr |= ((uint32_t)(dev.nAddrBits - 1) << QUADSPI_DCR_FSIZE_Pos) & QUADSPI_DCR_FSIZE_Msk;

    // Set CS high time
    dcr |= ((uint32_t)(dev.csHighTime - 1) << QUADSPI_DCR_CSHT_Pos) & QUADSPI_DCR_CSHT_Msk;

    // Set clock polarity
    dcr |= ((uint32_t)dev.clkPol << QUADSPI_DCR_CKMODE_Pos) & QUADSPI_DCR_CKMODE_Msk;

    mQspi->DCR = dcr;
}

///
/// Set the address and alternate bytes.
/// @param head Header containing address alternate bytes.
/// @note this will stop the QSPI.
///
void QSpi::setHeader(Header& head) {
    stop();

    mQspi->AR = head.address;
    mQspi->ABR = head.alternate;
}

///
/// Begin memory mapped operation of the QSPI.
/// @note this will restart the QSPI.
///
void QSpi::startMemoryMapped() {
    stop();

    //setMode(MEMORY_MAPPED);

    mQspi->CR |= QUADSPI_CR_EN;
}

///
/// Start an indirect write operation.
/// @param head Header to use.
/// @param comm Communication cofiguration.
/// @param buf Buffer to write. If null, will only send preamble.
/// @param bufLen Length of buffer to write.
/// @note this will restart the QSPI.
///
void QSpi::startIndirectWrite(Header& head, CommunicationConfiguration& comm, uint8_t* buf, size_t bufLen) {
    stop();

    mQspi->DLR = bufLen - 1;
    setHeader(head);
    mQspi->CCR = comm.toReg() | (INDIRECT_WRITE << QUADSPI_CCR_FMODE_Pos);

    if(buf) {

        mMdmaList[0].setSource(buf, sizeof(*buf), sizeof(*buf), false);
        mMdmaList[0].setDestination((void*)&mQspi->DR, sizeof(*buf), 0, false);
        mMdmaList[0].setNumberData(bufLen, sizeof(*buf));
        mMdmaList[0].setTrigger(22, false, mdma::eTriggerMode::BUF_TRANS);
        mMdmaList[0].linkTo(nullptr);

        mMdmaCh->disable();
        mMdmaCh->configureTransfer(mMdmaList);
        mMdmaCh->enable();
    }
}

///
/// Start an indirect read operation.
/// @param head Header to use.
/// @param comm Communication cofiguration
/// @param buf Buffer to read into.
/// @param bufLen Length of buffer to read into.
/// @note this will restart the QSPI.
///
void QSpi::startIndirectRead(Header& head, CommunicationConfiguration& comm, uint8_t* buf, size_t bufLen) {
    stop();

    mQspi->DLR = bufLen - 1;
    setHeader(head);
    mQspi->CCR = comm.toReg() | (INDIRECT_READ << QUADSPI_CCR_FMODE_Pos);

    mMdmaList[0].setSource((void*)&mQspi->DR, sizeof(*buf), 0, false);
    mMdmaList[0].setDestination(buf, sizeof(*buf), sizeof(*buf), false);
    mMdmaList[0].setNumberData(bufLen, sizeof(*buf));
    mMdmaList[0].setTrigger(22, false, mdma::eTriggerMode::BUF_TRANS);
    mMdmaList[0].linkTo(nullptr);

    mMdmaCh->disable();
    mMdmaCh->configureTransfer(mMdmaList);
    mMdmaCh->enable();
 }

///
/// @param head Header to use.
/// @param comm Communication cofiguration.
/// Begin status polling mode.
/// @param spConf Desired status polling configuraiton.
/// @note this will restart the QSPI.
///
void QSpi::startStatusPolling(Header& head, CommunicationConfiguration& comm, StatusPollingConfigurtion& spConf) {
    stop();

    // Clear status polling flag
    mQspi->FCR |= QUADSPI_FCR_CSMF;

    // Set polling interval
    mQspi->PIR = spConf.interval;

    // Set status mask
    mQspi->PSMKR = spConf.mask;

    // Set status match
    mQspi->PSMAR = spConf.match;

    // Set match mode
    if(spConf.orMode) {
        mQspi->CR |= QUADSPI_CR_PMM;
    } else {
        mQspi->CR &= ~QUADSPI_CR_PMM;
    }

    setHeader(head);
    mQspi->CCR = comm.toReg() | (INDIRECT_READ << QUADSPI_CCR_FMODE_Pos);
}

///
/// Check if the status polling has matched.
/// @return True if a match has occured.
/// @note This will clear the status match flag.
///
bool QSpi::statusPollingMatch() {
    bool ret = !!(mQspi->SR & QUADSPI_SR_SMF);
    mQspi->FCR |= QUADSPI_FCR_CSMF;

    return ret;
}

} // namespace qspi

