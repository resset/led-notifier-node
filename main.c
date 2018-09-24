#include "hal.h"
#include "ch.h"

/*
 * Maximum speed SPI configuration (16MHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig hs_spicfg = {
  false,
  NULL,
  GPIOA,
  4,
  0,
  0
};

/*
 * Low speed SPI configuration (250kHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig ls_spicfg = {
  false,
  NULL,
  GPIOA,
  4,
  SPI_CR1_BR_2 | SPI_CR1_BR_1,
  0
};

/*
 * SPI TX and RX buffers.
 */
static uint8_t txbuf[4];
static uint8_t rxbuf[4];

/*
 * SPI thread.
 */
THD_WORKING_AREA(waThread2, 128);
THD_FUNCTION(Thread2, arg) {

  (void)arg;

  /*
   * SPI1 I/O pins setup.
   */
  palSetPadMode(GPIOB, 3, PAL_MODE_ALTERNATE(0) |
                          PAL_STM32_OSPEED_HIGHEST);       /* SCK.*/
  palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(0) |
                          PAL_STM32_OSPEED_HIGHEST);       /* MISO.*/
  palSetPadMode(GPIOB, 5, PAL_MODE_ALTERNATE(0) |
                          PAL_STM32_OSPEED_HIGHEST);       /* MOSI.*/
  palSetPadMode(GPIOA, 4, PAL_MODE_ALTERNATE(0) |
                          PAL_STM32_OSPEED_HIGHEST);       /* CSN.*/

  while (true) {
    spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
    spiStart(&SPID1, &ls_spicfg);       /* Setup transfer parameters.       */
    spiSelect(&SPID1);                  /* Slave Select assertion.          */
    spiExchange(&SPID1, 4,
                txbuf, rxbuf);          /* Atomic transfer operations.      */
    spiUnselect(&SPID1);                /* Slave Select de-assertion.       */
    spiReleaseBus(&SPID1);              /* Ownership release.               */

    chThdSleepMilliseconds(2000);
  }
}

/*
 * UART thread.
 */
THD_WORKING_AREA(waThread3, 128);
THD_FUNCTION(Thread3, arg) {

  (void)arg;

  sdStart(&SD2, NULL);
  // USART1 TX
  palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(4));
  // USART1 RX
  palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(4));

  while (true) {
    chnWrite(&SD2, (const uint8_t *)"Hello World!\r\n", 14);
    chThdSleepMilliseconds(2000);
  }
}

int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  chThdCreateStatic(waThread3, sizeof(waThread3), NORMALPRIO, Thread3, NULL);

  while (true) {
    chThdSleepMilliseconds(500);
  }
}
