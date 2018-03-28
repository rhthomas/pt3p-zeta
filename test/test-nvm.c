/* Testing NVM read/writes for mailbox buffer.
 *
 * Expected output:
 * 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 add 1
 * 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 add 2
 * 0x01 0x02 0x00 0x00 0x00 0x00 0x00 0x00 add 3
 * ...
 * 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 add 9 flash
 * 0x09 0x02 0x03 0x04 0x05 0x06 0x07 0x08 add A flash
 * 0x09 0x0A 0x03 0x04 0x05 0x06 0x07 0x08 add B flash
 * ...
 */

#include "util.h"

// The mailbox buffer exists *somewhere*.
extern buffer_t mailbox;

#pragma PERSISTENT(j)
uint8_t j = 0;

int main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;
    // Unlock the system.
    PM5CTL0 &= ~LOCKLPM5;

    // System initialisations.
    io_init();
    clock_init();

    // Display contents of buffer.
    uint8_t i, out;
    for (i = 0; i < BUFFER_SIZE; i++) {
        led_set(mailbox.buffer[i]);
        __delay_cycles(6e6);
    }
    led_clear();

    // Push in new data (head data + 1).
    if (mailbox_push(j)) {
        // Flash LEDs when puffer is full.
        led_flash();
        // Pop to clear some room
        mailbox_pop(&out);
    } else {
        j++;
    }

    // Shutdown the node.
    power_off();
}
