#include <msp430.h>

typedef unsigned char uint8_t;

#define SET_COLUMN_ADDRESS_LSB        0x00
#define SET_COLUMN_ADDRESS_MSB        0x10
#define SET_PAGE_ADDRESS              0xB0

#define SET_SEG_DIRECTION             0xA1
#define SET_COM_DIRECTION             0xC0

#define SET_POWER_CONTROL             0x2F // ”правление питанием. PC[0] Ц усилитель, PC[1] Ч регул€тор, PC[2] Ч повторитель. 0 Ч отключено, 1 Ч включено
#define SET_SCROLL_LINE               0x40 // ”становка начальной линии скроллинга SL=0..63
#define SET_VLCD_RESISTOR_RATIO       0x27 // ”становка уровн€ внутреннего резисторного делител€ PC = [0..7].»спользуетс€ дл€ управлени€ контрастом.
#define SET_ELECTRONIC_VOLUME_MSB     0x81 // –егулировка контраста. ƒвухбайтна€ команда. PM[5..0] PM = 0..63.
#define SET_ELECTRONIC_VOLUME_LSB     0x0F
#define SET_ALL_PIXEL_ON              0xA4 // ¬ключение всех пикселей. 0 Ц отображение содержимого пам€ти, 1 Ц все пиксели включены (содержимое пам€ти сохран€етс€).
#define SET_INVERSE_DISPLAY           0xA6 // ¬ключение инверсного режима. 0 Ч нормальное отображение содержимого пам€ти, 1 Ч инверсное.
#define SET_DISPLAY_ENABLE            0xAF // ќтключение экрана. 0 Ч экран отключен, 1 Ч включен.
#define SET_LCD_BIAS_RATIO            0xA2 // —мещение напр€жени€ делител€: 0 Ц 1/9, 1 Ц 1/7.
#define SET_ADV_PROGRAM_CONTROL0_MSB  0xFA // –асширенное управление. “— Ч температурна€ компенсаци€ 0 = -0.05, 1 = -0.11 % / ∞—;
#define SET_ADV_PROGRAM_CONTROL0_LSB  0x90 // WC Ц циклический сдвиг столбцов 0 = нет, 1 = есть; WP Цциклический сдвиг страниц 0 = нет, 1 = есть.

#define CD              BIT6
#define CS              BIT4

uint8_t Dogs102x6_initMacro[] = {
    SET_SCROLL_LINE,
    SET_SEG_DIRECTION,
    SET_COM_DIRECTION,
    SET_ALL_PIXEL_ON,
    SET_INVERSE_DISPLAY,
    SET_LCD_BIAS_RATIO,
    SET_POWER_CONTROL,
    SET_VLCD_RESISTOR_RATIO,
    SET_ELECTRONIC_VOLUME_MSB,
    SET_ELECTRONIC_VOLUME_LSB,
    SET_ADV_PROGRAM_CONTROL0_MSB,
    SET_ADV_PROGRAM_CONTROL0_LSB,
    SET_DISPLAY_ENABLE,
    SET_PAGE_ADDRESS,
    SET_COLUMN_ADDRESS_MSB,
    SET_COLUMN_ADDRESS_LSB
};

uint8_t MODE_COMMANDS[2][1] = { {SET_SEG_DIRECTION}, {SET_SEG_DIRECTION | 1} };

int COLUMN_START_ADDRESS = 0;
int CURRENT_NUMBER = +9826;
int SUM_NUMBER = -721;
int CURRENT_SCROLL_LINE = 0;
int CURRENT_BUTTON = 0;

uint8_t symbols[12][11] = {
        {0x10, 0x10, 0x10, 0x10, 0x10, 0xFE, 010, 0x10, 0x10, 0x10, 0x10}, // plus
        {0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00}, // minus
        {0xFE, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xFE, 0xFE}, // num0
        {0x18, 0x38, 0x78, 0xF8, 0xF8, 0x38, 0x38, 0x38, 0x38, 0xFE, 0xFE}, // num1
        {0xFE, 0xFE, 0x0E, 0x0E, 0x0E, 0xFE, 0xFE, 0xE0, 0xE0, 0xFE, 0xFE}, // num2
        {0xFE, 0xFE, 0x0E, 0x0E, 0x0E, 0xFE, 0x0E, 0x0E, 0x0E, 0xFE, 0xFE}, // num3
        {0xC6, 0xC6, 0xC6, 0xC6, 0xFE, 0xFE, 0x06, 0x06, 0x06, 0x06, 0x06}, // num4
        {0xFE, 0xFE, 0xE0, 0xE0, 0xFE, 0xFE, 0x0E, 0x0E, 0x0E, 0xFE, 0xFE}, // num5
        {0xFE, 0xFE, 0xC0, 0xC0, 0xC0, 0xFE, 0xFE, 0xC6, 0xC6, 0xFE, 0xFE}, // num6
        {0xFE, 0xFE, 0xFE, 0x06, 0x0E, 0x1E, 0x3C, 0x78, 0xF0, 0xE0, 0xC0}, // num7
        {0xFE, 0xFE, 0xC6, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0xFE, 0xFE}, // num8
        {0xFE, 0xFE, 0xC6, 0xC6, 0xC6, 0xFE, 0xFE, 0x06, 0x06, 0xFE, 0xFE}  // num9
};

int lenHelper(int number);
int abs(int number);
int pow(int base, int exponent);
void printNumber(void);
void startTimerA1(void);

void Dogs102x6_clearScreen(void);
void Dogs102x6_scrollLine(uint8_t lines);
void Dogs102x6_setAddress(uint8_t pa, uint8_t ca, uint8_t maxp);
void Dogs102x6_writeData(uint8_t* sData, uint8_t i);
void Dogs102x6_writeCommand(uint8_t* sCmd, uint8_t i);
void Dogs102x6_backlightInit(void);
void Dogs102x6_init(void);

#pragma vector = PORT1_VECTOR
__interrupt void buttonS1(void)
{
    CURRENT_BUTTON = 0;
    startTimerA1();
}

#pragma vector = PORT2_VECTOR
__interrupt void buttonS2(void)
{
   CURRENT_BUTTON = 1;
   startTimerA1();
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    P1DIR &= ~BIT7;
    P1OUT |= BIT7;
    P1REN |= BIT7;
    P1IE |= BIT7;
    P1IES |= BIT7;
    P1IFG = 0;

    P2DIR &= ~BIT2;
    P2OUT |= BIT2;
    P2REN |= BIT2;
    P2IE |= BIT2;
    P2IES |= BIT2;
    P2IFG = 0;

    Dogs102x6_init();
    Dogs102x6_backlightInit();
    Dogs102x6_clearScreen();
    printNumber();

    __bis_SR_register(GIE);

    return 0;
}

void printNumber(void) {
    int nDigits = lenHelper(CURRENT_NUMBER);
    int number = CURRENT_NUMBER;

    Dogs102x6_setAddress(0, COLUMN_START_ADDRESS, nDigits);
    Dogs102x6_writeData(number > 0 ? symbols[0] : symbols[1], 11);

    int i = 0;
    int divider = pow(10, nDigits - 1);

    number = abs(number);

    for (i = 1; i <= nDigits; i++) {
        int digit = number / divider;

        Dogs102x6_setAddress(i, COLUMN_START_ADDRESS, nDigits);
        Dogs102x6_writeData(symbols[digit + 2], 11);

        number = number % divider;
        divider /= 10;
    }
}

int lenHelper(int number) {
    number = abs(number);

    if (number >= 1000000) return 7;
    if (number >= 100000) return 6;
    if (number >= 10000) return 5;
    if (number >= 1000) return 4;
    if (number >= 100) return 3;
    if (number >= 10) return 2;
    return 1;
}

int abs(int number) {
    return number > 0 ? number : number * (-1);
}

int pow(int base, int exponent) {
    int i = 0;
    int result = base;

    for (i = 0; i < exponent - 1; i++) {
        result *= base;
    }

    return result;
}

void Dogs102x6_clearScreen(void)
{
    uint8_t LcdData[] = { 0x00 };
    uint8_t p, c;

    // 8 total pages in LCD controller memory
    for (p = 0; p < 8; p++)
    {
        Dogs102x6_setAddress(p, 0, 7);
        // 132 total columns in LCD controller memory
        for (c = 0; c < 132; c++)
        {
            Dogs102x6_writeData(LcdData, 1);
        }
    }
}

void Dogs102x6_scrollLine(uint8_t lines)
{
    uint8_t cmd[] = {SET_SCROLL_LINE};

    //check if parameter is in range
    if (lines > 0x3F)
    {
        cmd[0] |= 0x3F;
    }
    else
    {
        cmd[0] |= lines;
    }

    Dogs102x6_writeCommand(cmd, 1);
}

void Dogs102x6_setAddress(uint8_t pa, uint8_t ca, uint8_t maxp)
{
    uint8_t cmd[1];

    if (pa > 7)
    {
        pa = 7;
    }

    if (ca > 101)
    {
        ca = 101;
    }

    cmd[0] = SET_PAGE_ADDRESS + (maxp - pa);
    uint8_t H = 0x00;
    uint8_t L = 0x00;
    uint8_t ColumnAddress[] = { SET_COLUMN_ADDRESS_MSB, SET_COLUMN_ADDRESS_LSB };

    L = (ca & 0x0F);
    H = (ca & 0xF0);
    H = (H >> 4);

    ColumnAddress[0] = SET_COLUMN_ADDRESS_LSB + L;
    ColumnAddress[1] = SET_COLUMN_ADDRESS_MSB + H;

    Dogs102x6_writeCommand(cmd, 1);
    Dogs102x6_writeCommand(ColumnAddress, 2);
}

void Dogs102x6_writeData(uint8_t* sData, uint8_t i)
{
    P7OUT &= ~CS;
    P5OUT |= CD;

    while (i)
    {
        while (!(UCB1IFG & UCTXIFG));

        UCB1TXBUF = *sData;

        sData++;
        i--;
    }

    while (UCB1STAT & UCBUSY);
    // Dummy read to empty RX buffer and clear any overrun conditions
    UCB1RXBUF;

    P7OUT |= CS;
}

void Dogs102x6_writeCommand(uint8_t* sCmd, uint8_t i)
{
    P7OUT &= ~CS;
    P5OUT &= ~CD;

    while (i)
    {
        while (!(UCB1IFG & UCTXIFG));

        UCB1TXBUF = *sCmd;

        sCmd++;
        i--;
    }

    while (UCB1STAT & UCBUSY);
    // Dummy read to empty RX buffer and clear any overrun conditions
    UCB1RXBUF;

    P7OUT |= CS;
}

void Dogs102x6_backlightInit(void)
{
    P7DIR |= BIT6; // питание подсветки
    P7OUT |= BIT6;
    P7SEL &= ~BIT6;
}

void Dogs102x6_init(void)
{
    // Port initialization for LCD operation
    P5DIR |= BIT7;
    P5OUT &= BIT7;
    P5OUT |= BIT7;

    P7DIR |= CS;

    P5DIR |= CD;
    P5OUT &= ~CD;

    P4SEL |= BIT1;
    P4DIR |= BIT1;

    P4SEL |= BIT3;
    P4DIR |= BIT3;

    UCB1CTL1 = UCSSEL_2 + UCSWRST;
    UCB1CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC;
    //UCCKPH = 1 - Data is captured on the first UCLK edge and changed on the following edge.
    //UCMSB - Mode: MSB first  0:LSB / 1:MSB
    //UCMST - Sync. Mode: Master Select
    //UCMODE_0 - 3-pin SPI
    //UCSYNC - Sync-Mode 0:UART-Mode / 1:SPI-Mode

    UCB1BR0 = 0x02;
    UCB1BR1 = 0;

    UCB1CTL1 &= ~UCSWRST;
    UCB1IFG &= ~UCRXIFG;

    Dogs102x6_writeCommand(Dogs102x6_initMacro, 13);
}

void startTimerA1(){
    P1IE &= ~BIT7;
    P2IE &= ~BIT2;

    TA1CCR0 = 2000;
    TA1CCTL0 = CCIE;
    TA1CTL = TASSEL_1 | ID_0 | MC__UP | TACLR;
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER1(void) {

    if(CURRENT_BUTTON == 0 && !(P1IN & BIT7)) {
        CURRENT_NUMBER += SUM_NUMBER;
        Dogs102x6_clearScreen();
        printNumber();
    }

    if(CURRENT_BUTTON == 1 && !(P2IN & BIT2)){
        CURRENT_SCROLL_LINE += 24;
        if(CURRENT_SCROLL_LINE > 63) {
            CURRENT_SCROLL_LINE -= 64;
        }
        Dogs102x6_scrollLine(CURRENT_SCROLL_LINE);
    }

    TA1CTL = MC__STOP;

    P2IE |= BIT2;
    P2IFG &= ~BIT2;

    P1IE |= BIT7;
    P1IFG &= ~BIT7;
}

