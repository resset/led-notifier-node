#include "hal.h"
#include "ch.h"

/*
 * Thread 1.
 */
THD_WORKING_AREA(waThread1, 128);
THD_FUNCTION(Thread1, arg) {

  (void)arg;

  while (true) {
    palSetPad(GPIOB, GPIOB_LED_GREEN);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOB, GPIOB_LED_GREEN);
    chThdSleepMilliseconds(500);
  }
}

/*
 * UART thread 2.
 */
THD_WORKING_AREA(waThread2, 128);
THD_FUNCTION(Thread2, arg) {

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


/*
 * Threads static table, one entry per thread. The number of entries must
 * match NIL_CFG_NUM_THREADS.
 */
THD_TABLE_BEGIN
  THD_TABLE_ENTRY(waThread1, "blinker", Thread1, NULL)
  THD_TABLE_ENTRY(waThread2, "hello", Thread2, NULL)
THD_TABLE_END

int main(void) {

  halInit();
  chSysInit();

  while (true) {
  }
}
