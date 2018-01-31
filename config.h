// Configuration:
// PLL Prescaler Selection bits:
#pragma config  PLLDIV = 1
//  No prescale (4 MHz oscillator input drives PLL directly)  
// #pragma config PLLDIV = 2
//  Divide by 2 (8 MHz oscillator input)  
// #pragma config PLLDIV = 3
//  Divide by 3 (12 MHz oscillator input)  
// #pragma config PLLDIV = 4
//  Divide by 4 (16 MHz oscillator input)  
//#pragma config PLLDIV = 5
// Divide by 5 (20 MHz oscillator input)  
// #pragma config PLLDIV = 6
//  Divide by 6 (24 MHz oscillator input)  
// #pragma config PLLDIV = 10
//  Divide by 10 (40 MHz oscillator input)  
// #pragma config PLLDIV = 12
//  Divide by 12 (48 MHz oscillator input)  
// 
// System Clock Postscaler Selection bits:
 #pragma config  CPUDIV = OSC1_PLL2
//  [Primary Oscillator Src: /1][96 MHz PLL Src: /2]  
//#pragma config CPUDIV = OSC2_PLL3
// [Primary Oscillator Src: /2][96 MHz PLL Src: /3]  
// #pragma config CPUDIV = OSC3_PLL4
//  [Primary Oscillator Src: /3][96 MHz PLL Src: /4]  
// #pragma config CPUDIV = OSC4_PLL6
//  [Primary Oscillator Src: /4][96 MHz PLL Src: /6]  
// 
// USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1):
#pragma config  USBDIV = 1
//  USB clock source comes directly from the primary oscillator block with no postscale  
// #pragma config USBDIV = 2
//  USB clock source comes from the 96 MHz PLL divided by 2  
// 
// Oscillator Selection bits:
// #pragma config  FOSC = XT_XT
//  XT oscillator (XT)  
// #pragma config FOSC = XTPLL_XT
//  XT oscillator, PLL enabled (XTPLL)  
#pragma config FOSC = ECIO_EC
//  EC oscillator, port function on RA6 (ECIO)  
// #pragma config FOSC = EC_EC
//  EC oscillator, CLKO function on RA6 (EC)  
// #pragma config FOSC = ECPLLIO_EC
//  EC oscillator, PLL enabled, port function on RA6 (ECPIO)  
// #pragma config FOSC = ECPLL_EC
//  EC oscillator, PLL enabled, CLKO function on RA6 (ECPLL)  
// #pragma config FOSC = INTOSCIO_EC
// Internal oscillator, port function on RA6, EC used by USB (INTIO)  
// #pragma config FOSC = INTOSC_EC
//  Internal oscillator, CLKO function on RA6, EC used by USB (INTCKO)  
// #pragma config FOSC = INTOSC_XT
//  Internal oscillator, XT used by USB (INTXT)  
// #pragma config FOSC = INTOSC_HS
//  Internal oscillator, HS oscillator used by USB (INTHS)  
// #pragma config FOSC = HS
//  HS oscillator (HS)  
// #pragma config FOSC = HSPLL_HS
//  HS oscillator, PLL enabled (HSPLL)  
// 
// Fail-Safe Clock Monitor Enable bit:
#pragma config  FCMEN = OFF
//  Fail-Safe Clock Monitor disabled  
// #pragma config FCMEN = ON
//  Fail-Safe Clock Monitor enabled  
// 
// Internal/External Oscillator Switchover bit:
#pragma config  IESO = OFF
//  Oscillator Switchover mode disabled  
// #pragma config IESO = ON
//  Oscillator Switchover mode enabled  
// 
// Power-up Timer Enable bit:
// #pragma config  PWRT = ON
//  PWRT enabled  
#pragma config PWRT = OFF
//  PWRT disabled  
// 
// Brown-out Reset Enable bits:
#pragma config  BOR = OFF
//  Brown-out Reset disabled in hardware and software  
// #pragma config BOR = SOFT
//  Brown-out Reset enabled and controlled by software (SBOREN is enabled)  
// #pragma config BOR = ON_ACTIVE
//  Brown-out Reset enabled in hardware only and disabled in Sleep mode (SBOREN is disabled)  
// #pragma config BOR = ON
//  Brown-out Reset enabled in hardware only (SBOREN is disabled)  
// 
// Brown-out Reset Voltage bits:
// #pragma config  BORV = 0
//  Maximum setting  
// #pragma config BORV = 1
//    
// #pragma config BORV = 2
//    
#pragma config BORV = 3
//  Minimum setting  
// 
// USB Voltage Regulator Enable bit:
#pragma config  VREGEN = OFF
//  USB voltage regulator disabled  
// #pragma config VREGEN = ON
//  USB voltage regulator enabled  
// 
// Watchdog Timer Enable bit:
#pragma config  WDT = OFF
//  WDT disabled (control is placed on the SWDTEN bit)  
// #pragma config WDT = ON
//  WDT enabled  
// 
// Watchdog Timer Postscale Select bits:
// #pragma config  WDTPS = 1
//  1:1  
// #pragma config WDTPS = 2
//  1:2  
// #pragma config WDTPS = 4
//  1:4  
// #pragma config WDTPS = 8
//  1:8  
// #pragma config WDTPS = 16
//  1:16  
// #pragma config WDTPS = 32
//  1:32  
// #pragma config WDTPS = 64
//  1:64  
// #pragma config WDTPS = 128
//  1:128  
// #pragma config WDTPS = 256
//  1:256  
// #pragma config WDTPS = 512
//  1:512  
// #pragma config WDTPS = 1024
//  1:1024  
// #pragma config WDTPS = 2048
//  1:2048  
// #pragma config WDTPS = 4096
//  1:4096  
// #pragma config WDTPS = 8192
//  1:8192  
// #pragma config WDTPS = 16384
//  1:16384  
#pragma config WDTPS = 32768
//  1:32768  
// 
// CCP2 MUX bit:
// #pragma config  CCP2MX = OFF
//  CCP2 input/output is multiplexed with RB3  
#pragma config CCP2MX = ON
//  CCP2 input/output is multiplexed with RC1  
// 
// PORTB A/D Enable bit:
// #pragma config  PBADEN = OFF
//  PORTB<4:0> pins are configured as digital I/O on Reset  
#pragma config PBADEN = ON
//  PORTB<4:0> pins are configured as analog input channels on Reset  
// 
// Low-Power Timer 1 Oscillator Enable bit:
#pragma config  LPT1OSC = OFF
//  Timer1 configured for higher power operation  
// #pragma config LPT1OSC = ON
//  Timer1 configured for low-power operation  
// 
// MCLR Pin Enable bit:
// #pragma config  MCLRE = OFF
//  RE3 input pin enabled; MCLR pin disabled  
#pragma config MCLRE = ON
//  MCLR pin enabled; RE3 input pin disabled  
// 
// Stack Full/Underflow Reset Enable bit:
// #pragma config  STVREN = OFF
//  Stack full/underflow will not cause Reset  
#pragma config STVREN = ON
//  Stack full/underflow will cause Reset  
// 
// Single-Supply ICSP Enable bit:
#pragma config  LVP = OFF
//  Single-Supply ICSP disabled  
// #pragma config LVP = ON
//  Single-Supply ICSP enabled  
// 
// Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit:
#pragma config  ICPRT = OFF
//  ICPORT disabled  
// #pragma config ICPRT = ON
//  ICPORT enabled  
// 
// Extended Instruction Set Enable bit:
#pragma config  XINST = OFF
//  Instruction set extension and Indexed Addressing mode disabled (Legacy mode)  
// #pragma config XINST = ON
//  Instruction set extension and Indexed Addressing mode enabled  
// 
// Code Protection bit:
// #pragma config  CP0 = ON
//  Block 0 (000800-001FFFh) is code-protected  
#pragma config CP0 = OFF
//  Block 0 (000800-001FFFh) is not code-protected  
// 
// Code Protection bit:
// #pragma config  CP1 = ON
//  Block 1 (002000-003FFFh) is code-protected  
#pragma config CP1 = OFF
//  Block 1 (002000-003FFFh) is not code-protected  
// 
// Code Protection bit:
// #pragma config  CP2 = ON
//  Block 2 (004000-005FFFh) is code-protected  
#pragma config CP2 = OFF
//  Block 2 (004000-005FFFh) is not code-protected  
// 
// Code Protection bit:
// #pragma config  CP3 = ON
//  Block 3 (006000-007FFFh) is code-protected  
#pragma config CP3 = OFF
//  Block 3 (006000-007FFFh) is not code-protected  
// 
// Boot Block Code Protection bit:
// #pragma config  CPB = ON
//  Boot block (000000-0007FFh) is code-protected  
#pragma config CPB = OFF
//  Boot block (000000-0007FFh) is not code-protected  
// 
// Data EEPROM Code Protection bit:
// #pragma config  CPD = ON
//  Data EEPROM is code-protected  
#pragma config CPD = OFF
//  Data EEPROM is not code-protected  
// 
// Write Protection bit:
// #pragma config  WRT0 = ON
//  Block 0 (000800-001FFFh) is write-protected  
#pragma config WRT0 = OFF
//  Block 0 (000800-001FFFh) is not write-protected  
// 
// Write Protection bit:
// #pragma config  WRT1 = ON
//  Block 1 (002000-003FFFh) is write-protected  
#pragma config WRT1 = OFF
//  Block 1 (002000-003FFFh) is not write-protected  
// 
// Write Protection bit:
// #pragma config  WRT2 = ON
//  Block 2 (004000-005FFFh) is write-protected  
#pragma config WRT2 = OFF
//  Block 2 (004000-005FFFh) is not write-protected  
// 
// Write Protection bit:
// #pragma config  WRT3 = ON
//  Block 3 (006000-007FFFh) is write-protected  
#pragma config WRT3 = OFF
//  Block 3 (006000-007FFFh) is not write-protected  
// 
// Configuration Register Write Protection bit:
// #pragma config  WRTC = ON
//  Configuration registers (300000-3000FFh) are write-protected  
#pragma config WRTC = OFF
//  Configuration registers (300000-3000FFh) are not write-protected  
// 
// Boot Block Write Protection bit:
// #pragma config  WRTB = ON
//  Boot block (000000-0007FFh) is write-protected  
#pragma config WRTB = OFF
//  Boot block (000000-0007FFh) is not write-protected  
// 
// Data EEPROM Write Protection bit:
// #pragma config  WRTD = ON
//  Data EEPROM is write-protected  
#pragma config WRTD = OFF
//  Data EEPROM is not write-protected  
// 
// Table Read Protection bit:
// #pragma config  EBTR0 = ON
//  Block 0 (000800-001FFFh) is protected from table reads executed in other blocks  
#pragma config EBTR0 = OFF
//  Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks  
// 
// Table Read Protection bit:
// #pragma config  EBTR1 = ON
//  Block 1 (002000-003FFFh) is protected from table reads executed in other blocks  
#pragma config EBTR1 = OFF
//  Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks  
// 
// Table Read Protection bit:
// #pragma config  EBTR2 = ON
//  Block 2 (004000-005FFFh) is protected from table reads executed in other blocks  
#pragma config EBTR2 = OFF
//  Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks  
// 
// Table Read Protection bit:
// #pragma config  EBTR3 = ON
//  Block 3 (006000-007FFFh) is protected from table reads executed in other blocks  
#pragma config EBTR3 = OFF
//  Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks  
// 
// Boot Block Table Read Protection bit:
// #pragma config  EBTRB = ON
//  Boot block (000000-0007FFh) is protected from table reads executed in other blocks  
#pragma config EBTRB = OFF
//  Boot block (000000-0007FFh) is not protected from table reads executed in other blocks  


