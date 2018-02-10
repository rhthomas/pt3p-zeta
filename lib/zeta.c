#include "zeta.h"

void zeta_init(void)
{
    // spi_init() will be called from main BEFORE zeta_init()

    // pinMode(CS, OUTPUT); pinMode(SDN, OUTPUT);
    P2DIR |= SDN;

    // pinMode(IRQ, INPUT_PULLUP);
    P2DIR &= ~IRQ;
    P2REN |= IRQ; // enable pull resistor
    P2OUT |= IRQ; // pull up

    // digitalWrite(CS, HIGH);
    spi_cs_high();

    // P2OUT &= ~SDN; // digitalWrite(SDN, LOW);
    P2OUT &= ~SDN; // hold device in wake state
}

void zeta_wait_irq(void)
{
    // while (digitalRead(IRQ) == HIGH)
    while (!(P2IN & IRQ))
        ;
}

void zeta_ready(void)
{
    // while (digitalRead(IRQ) == LOW)
    while (P2IN & IRQ)
        ;
}

void zeta_select_mode(uint8_t mode)
{
    if ((mode < 1) || (mode > 3)) {
        // Invalid arguments.
        return;
    }
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('M');
    spi_xfer(mode);
    spi_cs_high(); // digitalWrite(CS, HIGH);
}

void zeta_rx_mode(uint8_t ch, uint8_t pLength)
{
    if ((ch > 15) || (pLength > 65) || (pLength < 1)) {
        // Invalid arguments.
        return;
    }
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('R');
    spi_xfer(ch);
    spi_xfer(pLength);
    spi_cs_high(); // digitalWrite(CS, HIGH);
}

void zeta_sync_byte(uint8_t sync1, uint8_t sync2, uint8_t sync3, uint8_t sync4)
{
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('A');

    spi_xfer(sync1);
    spi_xfer(sync2);
    spi_xfer(sync3);
    spi_xfer(sync4);
    spi_cs_high(); // digitalWrite(CS, HIGH);
}

void zeta_set_baud_host(uint8_t baud)
{
    if (baud > 4) {
        // Invalid argument.
        return;
    }
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('H');
    spi_xfer(baud);
    spi_cs_high(); // digitalWrite(CS, HIGH);
}

void zeta_set_baud_rf(uint8_t baud)
{
    if ((baud > 4) || (baud < 1)) {
        // Invalid argument.
        return;
    }
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('B');
    spi_xfer(baud);
    spi_cs_high(); // digitalWrite(CS, HIGH);

    // device must enter sleep and wake again w/ delay of >= 15ms
    P2OUT |= SDN; // digitalWrite(SDN, HIGH);
    __delay_cycles(480000); // 48e4/24e6 = 0.020 // delay(20);
    P2OUT &= ~SDN; // digitalWrite(SDN, LOW);
}

void zeta_set_rf_power(uint8_t pwr)
{
    if ((pwr < 1) || (pwr > 127)) {
        // Invalid arguments.
        return;
    }
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('P');
    spi_xfer(pwr);
    spi_cs_high(); // digitalWrite(CS, LOW);
}

void zeta_enable_crc(uint8_t en)
{
    if (en > 1) {
        // Invalid arguments.
        return;
    }
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('E');
    spi_xfer(en);
    spi_cs_high(); // digitalWrite(CS, HIGH);
}

uint8_t zeta_get_rssi(void)
{
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('Q');

    spi_xfer(0x00); // '#'
    spi_xfer(0x00); // 'Q'
    uint8_t rssi = spi_xfer(0x00); // RSSI value.
    spi_cs_high(); // digitalWrite(CS, HIGH);

    return rssi;
}

void zeta_reset_default(void)
{
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('D');
    spi_cs_high(); // digitalWrite(CS, HIGH);
}

void zeta_send_open(uint8_t ch, uint8_t pLength)
{
    if ((ch > 15) || (pLength < 1) || (pLength > 65)) {
        // Invalid arguments.
        return;
    }
    spi_cs_low(); // digitalWrite(CS, LOW);
    spi_xfer('A');
    spi_xfer('T');
    spi_xfer('S');
    spi_xfer(ch);
    spi_xfer(pLength);
    // NOW CALL zeta_write_byte()
}

void zeta_write_byte(uint8_t data)
{
    spi_xfer(data);
    // NOW CALL zeta_send_close()
}

void zeta_send_close(void)
{
    __delay_cycles(480000); // 48e4/24e6 = 0.020 // delay(20);
    spi_cs_high(); // digitalWrite(CS, HIGH);
}

void zeta_send_packet(uint8_t* packet, uint8_t len)
{
    zeta_send_open(CHANNEL, len);
    uint8_t i;
    for (i = 0; i < len; i++) {
        zeta_write_byte(packet[i]);
    }
    zeta_send_close();
}

uint8_t zeta_rx_byte(void)
{
    zeta_wait_irq();
    spi_cs_low(); // digitalWrite(CS, LOW);
    uint8_t out = spi_xfer(0x00);
    spi_cs_high(); // digitalWrite(CS, HIGH);

    return out;
}

void zeta_rx_packet(uint8_t* packet)
{
    uint8_t count = 0;

    packet[0] = zeta_rx_byte(); // '#'
    packet[1] = zeta_rx_byte(); // 'R'
    packet[2] = count = zeta_rx_byte() - 4; // packet length
    packet[3] = zeta_rx_byte(); // RSSI

    // subsequent bytes are message data
    for (; count > 0; count--) {
        packet[4 + (4u - count)] = zeta_rx_byte();
    }
}
