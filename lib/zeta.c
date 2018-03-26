#include "zeta.h"

extern volatile uint8_t exit_loop;

uint8_t reverse(uint8_t byte)
{
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}

void zeta_init(void)
{
    // spi_init() will be called from main BEFORE zeta_init()

    P1DIR |= SDN;  // Set SDN pin as output.

    P1DIR &= ~IRQ; // Set nIRQ as input.
    P1REN |= IRQ;  // Enable pull resistor.
    P1OUT |= IRQ;  // Pull up.

#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

    P1OUT &= ~SDN; // hold device in wake state

    // Wait for CODEC to be ready for commands.
    __delay_cycles(24e6);
    zeta_ready(); // Hangs code when debugging.
    zeta_reset_default();

    /* Configure device:
     * 1. Max. transmit power.
     * 2. 0xAA sync bytes (unused).
     * 3. Host baud rate 4.
     * 4. RF baud rate 6.
     */
//    zeta_set_rf_power(127u);
//    zeta_sync_byte(0xAA, 0xAA, 0xAA, 0xAA);
//    zeta_set_baud_host(4u);
//    zeta_set_baud_rf(6u);
}

error_t zeta_wait_irq(void)
{
    timer_start();
    // Wait for nIRQ to go low.
    while (P1IN & IRQ) {
        if (exit_loop) {
            timer_stop();
            return ERROR_TIMEOUT;
        }
    }
    timer_stop();
    return ERROR_OK;
}

void zeta_ready(void)
{
    // Wait for nIRQ to go high.
    while (!(P1IN & IRQ))
        ;
}

//--------------------------------------
// CONFIG
//--------------------------------------

void zeta_select_mode(uint8_t mode)
{
    if ((mode < 1) || (mode > 3)) {
        // Invalid arguments.
        return;
    }
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('M');
    zeta_write_byte(mode);
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

//    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_rx_mode(uint8_t ch, uint8_t pLength)
{
    if ((ch > 15) || (pLength > 64) || (pLength < 1)) {
        // Invalid arguments.
        return;
    }
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('R');
    zeta_write_byte(ch);
    zeta_write_byte(pLength);
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

//    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_sync_byte(uint8_t sync1, uint8_t sync2, uint8_t sync3, uint8_t sync4)
{
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('A');

    // zeta_write_byte(reverse(sync1));
    // zeta_write_byte(reverse(sync2));
    // zeta_write_byte(reverse(sync3));
    // zeta_write_byte(reverse(sync4));
    zeta_write_byte(sync1);
    zeta_write_byte(sync2);
    zeta_write_byte(sync3);
    zeta_write_byte(sync4);
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

//    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_set_baud_host(uint8_t baud)
{
    if (baud > 4) {
        // Invalid argument.
        return;
    }
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('H');
    zeta_write_byte(baud);
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

//    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_set_baud_rf(uint8_t baud)
{
    if ((baud > 6) || (baud < 1)) {
        // Invalid argument.
        return;
    }
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('B');
    zeta_write_byte(baud);
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

    // device must enter sleep and wake again w/ delay of >= 15ms
    P1OUT |= SDN; // digitalWrite(SDN, HIGH);
    __delay_cycles(4.8e5); // delay(20);
    P1OUT &= ~SDN; // digitalWrite(SDN, LOW);

//    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_set_rf_power(uint8_t pwr)
{
    if ((pwr < 1) || (pwr > 127)) {
        // Invalid arguments.
        return;
    }
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('P');
    zeta_write_byte(pwr);
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

//    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_enable_crc(uint8_t en)
{
    if (en > 1) {
        // Invalid arguments.
        return;
    }
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('E');
    zeta_write_byte(en);
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

//    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_reset_default(void)
{
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('D');
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

//    __delay_cycles(4.8e5); // delay 20ms
}

//--------------------------------------
// DEBUG
//--------------------------------------

uint8_t zeta_get_rssi(void)
{
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('Q');
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

    uint8_t i, rssi;
    for (i = 3; i > 0; i--) {
        zeta_read_byte(&rssi);
    }
    return rssi; // RSSI value.
}

void zeta_get_vers(void)
{
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('V');
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

    // Get version from radio '#V4.00'
    uint8_t cnt, out;
    for (cnt = 6; cnt > 0; cnt--) {
        if (!zeta_read_byte(&out)) {

        }
    }

    exit_loop = 0;
}

void zeta_get_settings(uint8_t settings[])
{
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('?');
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL

    // Get settings from radio '#?[8bytes]'
    uint8_t byte, out;
    for (byte = 0; byte < 10; byte++) {
        if (!zeta_read_byte(&out)) {
            settings[byte] = out;
        }
    }

    exit_loop = 0;
}

//--------------------------------------
// TX
//--------------------------------------

void zeta_send_open(uint8_t ch, uint8_t pLength)
{
    if ((ch > 15) || (pLength < 1) || (pLength > 64)) {
        // Invalid arguments.
        return;
    }
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    zeta_write_byte('A'); // A
    zeta_write_byte('T'); // T
    zeta_write_byte('S'); // S
    zeta_write_byte(ch);
    zeta_write_byte(pLength);
    // NOW CALL zeta_write_byte()
}

void zeta_write_byte(uint8_t data)
{
    spi_xfer(data);
    // NOW CALL zeta_send_close()
}

void zeta_send_close(void)
{
    // cycles = MCLK * delay(s)
    __delay_cycles(4.8e5); // delay(20);
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL
}

void zeta_send_packet(uint8_t *packet, uint8_t len)
{
    zeta_send_open(CHANNEL, len);
    uint8_t i;
    for (i = 0; i < len; i++) {
        zeta_write_byte(packet[i]);
    }
    zeta_send_close();
}

//--------------------------------------
// RX
//--------------------------------------

error_t zeta_read_byte(uint8_t *out)
{
    if (zeta_wait_irq()) {
        return ERROR_TIMEOUT;
    }
    exit_loop = 0;
#ifdef MANUAL
    spi_cs_low();
#endif // MANUAL
    *out = spi_xfer(0x00);
#ifdef MANUAL
    spi_cs_high();
#endif // MANUAL
    return ERROR_OK;
}

void zeta_rx_packet(uint8_t *packet)
{
    uint8_t i = 0, len = 0, out = 0;
    // Get whole packet from FIFO.

    //    packet[i++] = zeta_read_byte(); // '#'
    if (!zeta_read_byte(&out)) {
        packet[i++] = out;
    }
    //    packet[i++] = zeta_read_byte(); // 'R'
    if (!zeta_read_byte(&out)) {
        packet[i++] = out;
    }
    //    packet[i++] = zeta_read_byte(); // length
    if (!zeta_read_byte(&out)) {
        packet[i++] = len = out;
    }
    //    packet[i++] = zeta_read_byte(); // RSSI
    if (!zeta_read_byte(&out)) {
        packet[i++] = out;
    }

    // The actual packet contents.
    for (; len > 0; len--) {
//        packet[i++] = zeta_read_byte();
        if (!zeta_read_byte(&out)) {
            packet[i++] = out;
        }
    }
    // Clear exit_loop flag.
    exit_loop = 0;
}

//error_t zeta_rx_packet(void)
//{
//    uint8_t i = 0, len = 0, err = 0;
//    if (zeta_wait_irq()) {
//        // Clear exit_loop flag.
//        exit_loop = 0;
//        return ERROR_TIMEOUT;
//    }
//    // Get whole packet from FIFO.
//    packet[i++] = zeta_read_byte(); // '#'
//    packet[i++] = zeta_read_byte(); // 'R'
//    packet[i++] = len = zeta_read_byte(); // Length of packet.
//    packet[i++] = zeta_read_byte(); // RSSI
//    // The actual packet contents.
//    for (; len > 0; len--) {
//        packet[i++] = zeta_read_byte();
//    }
//    // Clear exit_loop flag.
//    exit_loop = 0;
//    // Return no errors.
//    return ERROR_OK;
//}
