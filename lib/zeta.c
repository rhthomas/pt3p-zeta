#include "zeta.h"

static uint8_t rev_table[16] = {
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7,
    0xf};

uint8_t reverse(uint8_t byte)
{
    return (rev_table[byte & 0x0F] << 4) | rev_table[byte >> 4];
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

    // digitalWrite(SDN, LOW);
    P3OUT &= ~SDN; // hold device in wake state

    // Wait for CODEC to be ready for commands.
    zeta_ready();

    // Set host/rf baud rate.
    /// @bug Might not be in v1.01
//    zeta_set_baud_host(4u);
//    zeta_set_baud_rf(6u);
//    __delay_cycles(15e3); // Takes 15ms to complete.
    // Max. TX power.
    zeta_set_rf_power(127u);
//     Set standard sync bytes (not required).
    zeta_sync_byte(0xAA, 0xAA, 0xAA, 0xAA);
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

void zeta_select_mode(uint8_t mode)
{
    if ((mode < 1) || (mode > 3)) {
        // Invalid arguments.
        return;
    }
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('M');
    zeta_tx_byte(mode);
}

void zeta_rx_mode(uint8_t ch, uint8_t pLength)
{
    if ((ch > 15) || (pLength > 65) || (pLength < 1)) {
        // Invalid arguments.
        return;
    }
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('R');
    zeta_tx_byte(ch);
    zeta_tx_byte(pLength);
}

void zeta_sync_byte(uint8_t sync1, uint8_t sync2, uint8_t sync3, uint8_t sync4)
{
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('A');

    zeta_tx_byte(reverse(sync1));
    zeta_tx_byte(reverse(sync2));
    zeta_tx_byte(reverse(sync3));
    zeta_tx_byte(reverse(sync4));
}

void zeta_set_baud_host(uint8_t baud)
{
    if (baud > 4) {
        // Invalid argument.
        return;
    }
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('H');
    zeta_tx_byte(baud);
}

void zeta_set_baud_rf(uint8_t baud)
{
    if ((baud > 6) || (baud < 1)) {
        // Invalid argument.
        return;
    }
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('B');
    zeta_tx_byte(baud);

    // device must enter sleep and wake again w/ delay of >= 15ms
    P3OUT |= SDN; // digitalWrite(SDN, HIGH);
    __delay_cycles(100);
//    __delay_cycles(2e4); // 2e4/1e6 = 0.020s // delay(20);
    P3OUT &= ~SDN; // digitalWrite(SDN, LOW);
}

void zeta_set_rf_power(uint8_t pwr)
{
    if ((pwr < 1) || (pwr > 127)) {
        // Invalid arguments.
        return;
    }
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('P');
    zeta_tx_byte(pwr);
}

void zeta_enable_crc(uint8_t en)
{
    if (en > 1) {
        // Invalid arguments.
        return;
    }
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('E');
    zeta_tx_byte(en);
}

uint8_t zeta_get_rssi(void)
{
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('Q');

    zeta_tx_byte(0x00); // '#'
    zeta_tx_byte(0x00); // 'Q'
    uint8_t rssi = zeta_rx_byte(); // RSSI value.

    return rssi;
}

void zeta_get_vers(void)
{
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('V');

    // Get version from radio
    do {
        zeta_rx_byte();
    } while (!(P1IN & IRQ)); // Until there is no more data.
}

void zeta_get_settings(void)
{
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('?');

    // Get settings from radio
    do {
        zeta_rx_byte();
    } while (!(P1IN & IRQ)); // Until there is no more data.
}

void zeta_reset_default(void)
{
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('D');
}

void zeta_send_open(uint8_t ch, uint8_t pLength)
{
    if ((ch > 15) || (pLength < 1) || (pLength > 65)) {
        // Invalid arguments.
        return;
    }
    zeta_tx_byte('A');
    zeta_tx_byte('T');
    zeta_tx_byte('S');
    zeta_tx_byte(ch);
    zeta_tx_byte(pLength);
    // NOW CALL zeta_tx_byte()
}

void zeta_tx_byte(uint8_t data)
{
    spi_xfer(data);
    // NOW CALL zeta_send_close()
}

void zeta_send_close(void)
{
    /// @test Is this required? Test when comms are working!
    __delay_cycles(2e4); // 2e4/1e6 = 0.020 // delay(20);
//    spi_cs_high();
}

void zeta_send_packet(uint8_t packet[], uint8_t len)
{
    zeta_send_open(CHANNEL, len);
    uint8_t i;
    for (i = 0; i < len; i++) {
        zeta_tx_byte(packet[i]);
    }
    zeta_send_close();
}

uint8_t zeta_rx_byte(void)
{
    return spi_xfer(0x00);
}

void zeta_rx_packet(uint8_t packet[])
{
    uint8_t i = 0;

    // Wait for nIRQ to show there is data.
    zeta_wait_irq();
    // Get whole packet from FIFO.
    do {
        packet[i++] = zeta_rx_byte();
    } while (!(P1IN & IRQ));
}
