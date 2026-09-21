#include "qspi.hpp"

namespace qspi {

///
/// Constructor.
///
QSpi::QSpi()
    : mMdmaCh(mdma::MDMAController::getInstance()->getChannel(0))
{
    RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;
}

///
/// Destructor.
///
QSpi::~QSpi() {
    RCC->AHB3ENR &= ~RCC_AHB3ENR_QSPIEN;
}

///
/// Set the communication cofiguration.
/// @param comm Desired communication configuration.
/// @note This will stop the QSPI.
///
void QSpi::setCommunicationConfiguration(CommunicationConfiguration& comm) {
    stop();
    uint32_t funcModeBits = QUADSPI->CCR & QUADSPI_CCR_FMODE;
    uint32_t ccr = comm.toReg();
    QUADSPI->CCR = funcModeBits | ccr;
}

///
/// Set the device configuration
/// @param dev Desired device configuration.
/// @note this will stop the QSPI.
///
void QSpi::setDeviceConfiguration(DeviceConfiguration& dev) {
    stop();

    // TODO: Set frequency

    // Set device selection.
    if(dev.devSel == DUAL) {
        QUADSPI->CR |= QUADSPI_CR_DFM;
    } else {
        QUADSPI->CR &= ~QUADSPI_CR_DFM;
        if(dev.devSel == DEV_1) {
            QUADSPI->CR |= QUADSPI_CR_FSEL;
        } else {
            QUADSPI->CR &= ~QUADSPI_CR_FSEL;
        }
    }

    uint32_t dcr = 0;

    // Set number of address bits
    dcr |= ((uint32_t)(dev.nAddrBits - 1) << QUADSPI_DCR_FSIZE_Pos) & QUADSPI_DCR_FSIZE_Msk;

    // Set CS high time
    dcr |= ((uint32_t)(dev.csHighTime - 1) << QUADSPI_DCR_CSHT_Pos) & QUADSPI_DCR_CSHT_Msk;

    // Set clock polarity
    dcr |= ((uint32_t)dev.clkPol << QUADSPI_DCR_CKMODE_Pos) & QUADSPI_DCR_CKMODE_Msk;

    QUADSPI->DCR = dcr;
}

///
/// Set the address and alternate bytes.
/// @param head Header containing address alternate bytes.
/// @note this will stop the QSPI.
///
void QSpi::setHeader(Header& head) {
    stop();

    QUADSPI->AR = head.address;
    QUADSPI->ABR = head.alternate;
}

///
/// Begin memory mapped operation of the QSPI.
/// @note this will restart the QSPI.
///
void QSpi::startMemoryMapped() {
    stop();

    setMode(MEMORY_MAPPED);

    QUADSPI->CR |= QUADSPI_CR_EN;
}

///
/// Start an indirect write operation.
/// @param buf Buffer to write.
/// @param bufLen Length of buffer to write.
/// @note this will restart the QSPI.
///
void QSpi::startIndirectWrite(uint8_t* buf, size_t bufLen) {
    stop();

    setMode(INDIRECT_WRITE);

    QUADSPI->DLR = bufLen - 1;

    mMdmaList[0].setSource(buf, sizeof(*buf), 1, true);
    mMdmaList[0].setDestination((void*)&QUADSPI->DR, sizeof(*buf), 0, true);
    mMdmaList[0].setNumberData(bufLen, 1);
    mMdmaList[0].setTrigger(22, false, mdma::eTriggerMode::BUF_TRANS);
    mMdmaList[0].linkTo(nullptr);

    mMdmaCh->disable();
    mMdmaCh->configureTransfer(mMdmaList);
    mMdmaCh->enable();
}

///
/// Start an indirect read operation.
/// @param buf Buffer to read into.
/// @param bufLen Length of buffer to read into.
/// @note this will restart the QSPI.
///
void QSpi::startIndirectRead(uint8_t* buf, size_t bufLen) {
    stop();

    setMode(INDIRECT_READ);

    QUADSPI->DLR = bufLen - 1;

    mMdmaList[0].setSource((void*)&QUADSPI->DR, sizeof(*buf), 0, true);
    mMdmaList[0].setDestination(buf, sizeof(*buf), 1, true);
    mMdmaList[0].setNumberData(bufLen, 1);
    mMdmaList[0].setTrigger(22, false, mdma::eTriggerMode::BUF_TRANS);
    mMdmaList[0].linkTo(nullptr);

    mMdmaCh->disable();
    mMdmaCh->configureTransfer(mMdmaList);
    mMdmaCh->enable();

    QUADSPI->CR |= QUADSPI_CR_EN;
}

///
/// Begin status polling mode.
/// @param spConf Desired status polling configuraiton.
/// @note this will restart the QSPI.
///
void QSpi::startStatusPolling(StatusPollingConfigurtion& spConf) {
    stop();

    setMode(INDIRECT_READ);

    QUADSPI->PIR = spConf.interval;
    QUADSPI->PSMKR = spConf.mask;
    QUADSPI->PSMAR = spConf.match;
    if(spConf.orMode) {
        QUADSPI->CR |= QUADSPI_CR_PMM;
    } else {
        QUADSPI->CR &= ~QUADSPI_CR_PMM;
    }
}

///
/// Check if the status polling has matched.
/// @return True if a match has occured.
/// @note This will clear the status match flag.
///
bool QSpi::statusPollingMatch() {
    bool ret = !!(QUADSPI->SR & QUADSPI_SR_SMF);
    QUADSPI->FCR |= QUADSPI_FCR_CSMF;

    return ret;
}

} // namespace qspi

