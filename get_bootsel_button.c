#include  <stdint.h>
#include  <stdbool.h>
#include  "common.h"

#include  "hardware/sync.h"            // interrupts
#include  "hardware/structs/ioqspi.h"  // IOqSPI

//+============================================================================
// https://github.com/raspberrypi/pico-examples/blob/master/picoboard/button/button.c
//
// Picoboard has a button attached to the flash CS pin, which the bootrom
// checks, and jumps straight to the USB bootcode if the button is pressed
// (pulling flash CS low). We can check this pin in by jumping to some code in
// SRAM (so that the XIP interface is not required), floating the flash CS
// pin, and observing whether it is pulled low.
//
// This doesn't work if others are trying to access flash at the same time,
// e.g. XIP streamer, or the other core.
//
bool  __no_inline_not_in_flash_func( get_bootsel_button ) (void)
{
	const uint  CS_PIN_INDEX = 1;

    // Must disable interrupts, as interrupt handlers may be in flash, and we
	// are about to temporarily disable flash access!
	uint32_t  flags = save_and_disable_interrupts();

	// Set chip select to Hi-Z
	hw_write_masked( &ioqspi_hw->io[CS_PIN_INDEX].ctrl,
	                 GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
	                 IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS );

	// Note we can't call into any sleep functions in flash right now
	for (volatile int i = 0;  i < 1000;  ++i) ;

	// The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
	// Note the button pulls the pin *low* when pressed.
#if PICO_RP2040
    #define CS_BIT (1u << 1)
#else
    #define CS_BIT SIO_GPIO_HI_IN_QSPI_CSN_BITS
#endif
    bool  button_state = !(sio_hw->gpio_hi_in & CS_BIT);

    // Need to restore the state of chip select, else we are going to have a
    // bad time when we return to code in flash!
    hw_write_masked( &ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                     GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                     IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS );

    restore_interrupts(flags);

    return button_state;
}
