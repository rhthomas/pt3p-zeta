#include "zeta.h"

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

    // pinMode(SDN, OUTPUT);
    P3DIR |= SDN;

    // pinMode(IRQ, INPUT_PULLUP);
    P1DIR &= ~IRQ;
    P1REN |= IRQ; // enable pull resistor
    P1OUT |= IRQ; // pull up

    spi_cs_high();
    // digitalWrite(SDN, LOW);
    P3OUT &= ~SDN; // hold device in wake state

    // Wait for CODEC to be ready for commands.
    __delay_cycles(2.4e6); // delay 100ms
    zeta_ready();

    /* Configure device:
     * 1. Max. transmit power.
     * 2. 0xAA sync bytes (unused).
     * 3. Host baud rate 4.
     * 4. RF baud rate 6.
     */
    zeta_set_rf_power(127u);
    zeta_sync_byte(0xAA, 0xAA, 0xAA, 0xAA);
    zeta_set_baud_host(4u);
    zeta_set_baud_rf(6u);
}

void zeta_wait_irq(void)
{
    // while (digitalRead(IRQ) == HIGH)
    while (P1IN & IRQ)
        ;
}

void zeta_ready(void)
{
    // while (digitalRead(IRQ) == LOW)
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
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('M');
    zeta_write_byte(mode);
    spi_cs_high();

    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_rx_mode(uint8_t ch, uint8_t pLength)
{
    if ((ch > 15) || (pLength > 65) || (pLength < 1)) {
        // Invalid arguments.
        return;
    }
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('R');
    zeta_write_byte(ch);
    zeta_write_byte(pLength);
    spi_cs_high();

    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_sync_byte(uint8_t sync1, uint8_t sync2, uint8_t sync3, uint8_t sync4)
{
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('A');

    zeta_write_byte(reverse(sync1));
    zeta_write_byte(reverse(sync2));
    zeta_write_byte(reverse(sync3));
    zeta_write_byte(reverse(sync4));
    spi_cs_high();

    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_set_baud_host(uint8_t baud)
{
    if (baud > 4) {
        // Invalid argument.
        return;
    }
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('H');
    zeta_write_byte(baud);
    spi_cs_high();

    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_set_baud_rf(uint8_t baud)
{
    if ((baud > 6) || (baud < 1)) {
        // Invalid argument.
        return;
    }
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('B');
    zeta_write_byte(baud);
    spi_cs_high();

    // device must enter sleep and wake again w/ delay of >= 15ms
    P3OUT |= SDN; // digitalWrite(SDN, HIGH);
    __delay_cycles(4.8e5); // delay(20);
    P3OUT &= ~SDN; // digitalWrite(SDN, LOW);

    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_set_rf_power(uint8_t pwr)
{
    if ((pwr < 1) || (pwr > 127)) {
        // Invalid arguments.
        return;
    }
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('P');
    zeta_write_byte(pwr);
    spi_cs_high();

    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_enable_crc(uint8_t en)
{
    if (en > 1) {
        // Invalid arguments.
        return;
    }
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('E');
    zeta_write_byte(en);
    spi_cs_high();

    __delay_cycles(4.8e5); // delay 20ms
}

void zeta_reset_default(void)
{
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('D');
    spi_cs_high();

    __delay_cycles(4.8e5); // delay 20ms
}

//--------------------------------------
// DEBUG
//--------------------------------------

uint8_t zeta_get_rssi(void)
{
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('Q');
    spi_cs_high();

    zeta_read_byte(); // '#'
    zeta_read_byte(); // 'Q'
    return zeta_read_byte(); // RSSI value.
}

void zeta_get_vers(void)
{
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('V');
    spi_cs_high();

    // Get version from radio
    do {
        zeta_read_byte();
    } while (!(P1IN & IRQ)); // Until there is no more data.
}

void zeta_get_settings(void)
{
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('?');
    spi_cs_high();

    // Get settings from radio
    do {
        zeta_read_byte();
    } while (!(P1IN & IRQ)); // Until there is no more data.
}

//--------------------------------------
// TX
//--------------------------------------

void zeta_send_open(uint8_t ch, uint8_t pLength)
{
    if ((ch > 15) || (pLength < 1) || (pLength > 65)) {
        // Invalid arguments.
        return;
    }
    spi_cs_low();
    zeta_write_byte('A');
    zeta_write_byte('T');
    zeta_write_byte('S');
    zeta_write_byte(ch);
    zeta_write_byte(pLength + 4u);
    // NOW CALL zeta_write_byte()
}

void zeta_write_byte(uint8_t data)
{
    spi_xfer(data);
    // NOW CALL zeta_send_close()
}

void zeta_send_close(void)
{
    spi_cs_high();
    // cycles = MCLK * delay(s)
    __delay_cycles(4.8e5); // delay(20);
}

void zeta_send_packet(uint8_t packet[], uint8_t len)
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

uint8_t zeta_read_byte(void)
{
    spi_cs_low();
    uint8_t out = spi_xfer(0x00);
    spi_cs_high();
    return out;
}

void zeta_rx_packet(uint8_t packet[])
{
    uint8_t i = 0;
    // Wait for nIRQ to show there is data.
    zeta_wait_irq();
    // Get whole packet from FIFO.
    do {
        packet[i++] = zeta_read_byte();
    } while (!(P1IN & IRQ));
}
