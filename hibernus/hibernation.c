#include "hibernation.h"

#pragma SET_DATA_SECTION(".fram_vars")

unsigned long int *FRAM_write_ptr = (unsigned long int *)FRAM_START; // Pointer that points the FRAM.
unsigned long int *RAM_copy_ptr = (unsigned long int *)RAM_START; // Pointer that points the RAM.
unsigned long int *FLAG_interrupt = (unsigned long int *)INT; // Flag for interrupt.
unsigned long int *CC_Check = (unsigned long int *)CHECK; // Flag for restoring.

// These pointers and variable are used to set the PC.
unsigned long int *FRAM_pc = (unsigned long int *)PC; // Pointer for PC.
unsigned long int *current_SP;

// This array is used to restore the state.
unsigned int Registers[230];

unsigned int *Reg_copy_ptr;
unsigned int counter_check;

int pro;
int t;

#pragma SET_DATA_SECTION()

/// General Purpose registers addresses.
const unsigned int gen[230] = {
    0x100, 0x102, 0x104, 0x120, 0x12a, 0x130, 0x140, 0x144, 0x146, 0x150, 0x152,
    0x154, 0x156, 0x15c, 0x160, 0x162, 0x164, 0x166, 0x168, 0x16a, 0x16c, 0x180,
    0x182, 0x186, 0x188, 0x18a, 0x18c, 0x18e, 0x198, 0x19a, 0x19c, 0x19e, 0x1b0,
    0x200, 0x202, 0x204, 0x206, 0x20a, 0x20c, 0x20e, 0x216, 0x218, 0x21a, 0x21c,
    0x21e, 0x220, 0x222, 0x224, 0x226, 0x22a, 0x22c, 0x22e, 0x236, 0x238, 0x23a,
    0x23c, 0x23e, 0x320, 0x322, 0x324, 0x326, 0x32a, 0x32c, 0x336, 0x340, 0x342,
    0x344, 0x346, 0x350, 0x352, 0x354, 0x356, 0x360, 0x36e, 0x380, 0x382, 0x384,
    0x386, 0x390, 0x392, 0x394, 0x396, 0x3a0, 0x3ae, 0x3c0, 0x3c2, 0x3c4, 0x3c6,
    0x3d0, 0x3d2, 0x3d4, 0x3d6, 0x3e0, 0x3ee, 0x400, 0x402, 0x404, 0x406, 0x410,
    0x412, 0x414, 0x416, 0x420, 0x42e, 0x440, 0x442, 0x444, 0x446, 0x450, 0x452,
    0x454, 0x456, 0x460, 0x46e, 0x4a0, 0x4a2, 0x4a8, 0x4aa, 0x4ac, 0x4ae, 0x4b0,
    0x4b2, 0x4b4, 0x4b6, 0x4b8, 0x4ba, 0x4bc, 0x4be, 0x4c0, 0x4c2, 0x4c4, 0x4c6,
    0x4c8, 0x4ca, 0x4cc, 0x4ce, 0x4d0, 0x4d2, 0x4d4, 0x4d6, 0x4d8, 0x4da, 0x4dc,
    0x4de, 0x4e0, 0x4e2, 0x4e4, 0x4e6, 0x4e8, 0x4ea, 0x4ec, 0x500, 0x502, 0x504,
    0x506, 0x508, 0x50e, 0x510, 0x512, 0x516, 0x51a, 0x520, 0x522, 0x526, 0x52a,
    0x530, 0x532, 0x536, 0x53a, 0X5a0, 0x5a2, 0x5a4, 0x5a6, 0x5c0, 0x5c2, 0x5c6,
    0x5c8, 0x5ca, 0x5cc, 0x5ce, 0x5d0, 0x5d2, 0x5da, 0x5dc, 0x5de, 0x5e0, 0x5e2,
    0x5e6, 0x5e8, 0x5ea, 0x5ec, 0x5ee, 0x5f0, 0x5f2, 0x5fa, 0x5fc, 0x5fe, 0x640,
    0x642, 0x646, 0x648, 0x64a, 0x64c, 0x64e, 0x654, 0x656, 0x658, 0x65a, 0x65c,
    0x65e, 0x660, 0x66a, 0x66c, 0x66e, 0x700, 0x702, 0x704, 0x706, 0x708, 0x70a,
    0x712, 0x71a, 0x71c, 0x71e, 0x8c0, 0x8c2, 0x8c4, 0x8c6, 0x8cc, 0x8ce};

unsigned int i;

void Hibernus(void)
{
    // Restore procedure.
    // Waiting until the system reaches the restore threshold.
    t = COMP_ON;
    if (t == 0) {
        *FLAG_interrupt = 1;
        int_rising();

        power_off();
        // LPM4, External Comparator will force exit.
        __bis_SR_register(LPM4_bits + GIE);
        __no_operation();
    }

    // It is not the first time but the state has not been saved properly.
    if (*CC_Check == 0) {
        *CC_Check = 0;

        *FLAG_interrupt = 2;
        int_falling();

        __bis_SR_register(GIE); // Set interrupt.
        __no_operation();
    }

    // The first time the system does not need a restore.
    if ((*CC_Check != 0) && (*CC_Check != 1)) {
        *CC_Check = 0;

        *FLAG_interrupt = 2;
        int_falling();

        __bis_SR_register(GIE); // Set interrupt.
        __no_operation();
    }

    // If you have to restore a previous state.
    if (*CC_Check == 1) {
        Restore();
    }
}

void Hibernate(void)
{
    *CC_Check = 0;

    // Copy core registers to FRAM.
    asm(" MOVA R1,&0xEDF4");
    asm(" MOVA R2,&0xEDF8");
    asm(" MOVA R3,&0xEDFC");
    asm(" MOVA R4,&0xEE00");
    asm(" MOVA R5,&0xEE04");
    asm(" MOVA R6,&0xEE08");
    asm(" MOVA R7,&0xEE0C");
    asm(" MOVA R8,&0xEE10");
    asm(" MOVA R9,&0xEE14");
    asm(" MOVA R10,&0xEE18");
    asm(" MOVA R11,&0xEE1C");
    asm(" MOVA R12,&0xEE20");
    asm(" MOVA R13,&0xEE24");
    asm(" MOVA R14,&0xEE28");
    asm(" MOVA R15,&0xEE2C");

    // PC
    current_SP = (void *)_get_SP_register();
    *FRAM_pc = *current_SP;
    // Copy all the RAM into FRAM.
    Save_RAM();
    // Copy all the general registers onto the FRAM.
    pro = 0;
    Save_Register();
    *CC_Check = 1;
}

void Restore(void)
{
    // Restore all Register and RAM memory.
    Restore_Register();

    // Restore RAM.
    FRAM_write_ptr = (unsigned long int *)FRAM_START;

    // Leave the space for the core registers.
    for (i = 0; i < 16; i++) {
        *FRAM_write_ptr++;
    }

    RAM_copy_ptr = (unsigned long int *)RAM_START;

    while (RAM_copy_ptr < (unsigned long int *)RAM_END) {
        *RAM_copy_ptr++ = *FRAM_write_ptr++;
    }

    asm(" MOVA &0xEDF4,R1");
    asm(" MOVA &0xEDF8,R2");
    asm(" MOVA &0xEDFC,R3");
    asm(" MOVA &0xEE00,R4");
    asm(" MOVA &0xEE04,R5");
    asm(" MOVA &0xEE08,R6");
    asm(" MOVA &0xEE0C,R7");
    asm(" MOVA &0xEE10,R8");
    asm(" MOVA &0xEE14,R9");
    asm(" MOVA &0xEE18,R10");
    asm(" MOVA &0xEE1C,R11");
    asm(" MOVA &0xEE20,R12");
    asm(" MOVA &0xEE24,R13");
    asm(" MOVA &0xEE28,R14");
    asm(" MOVA &0xEE2C,R15");

    *current_SP = *FRAM_pc;

    *FLAG_interrupt = 2;
    int_falling();

    __bis_SR_register(GIE);
    __no_operation();

    *CC_Check = 0;
    pro = 1;
}

void Save_RAM(void)
{
    FRAM_write_ptr = (unsigned long int *)FRAM_START;

    for (i = 0; i < 16; i++) {
        *FRAM_write_ptr++;
    }

    // Copy all RAM onto the FRAM.
    RAM_copy_ptr = (unsigned long int *)RAM_START;

    while (RAM_copy_ptr <= (unsigned long int *)RAM_END) {
        *FRAM_write_ptr++ = *RAM_copy_ptr++;
    }
}

void Save_Register(void)
{
    for (i = 0; i < 230; i++) {
        Reg_copy_ptr = (unsigned int *)gen[i];
        Registers[i] = *Reg_copy_ptr;
    }
}

void Restore_Register(void)
{
    // Unlock registers.
    MPUCTL0_H = 0xA5;
    PMMCTL0_H = 0xA5;
    FRCTL0_H = 0xA5;
    CSCTL0_H = 0xA5;

    // Restore registers.
    for (i = 0; i < 3; i++) {
        Reg_copy_ptr = (unsigned int *)gen[i];
        *Reg_copy_ptr = Registers[i];
    }

    for (i = 4; i < 6; i++) {
        Reg_copy_ptr = (unsigned int *)gen[i];
        *Reg_copy_ptr = Registers[i];
    }

    for (i = 7; i < 13; i++) {
        Reg_copy_ptr = (unsigned int *)gen[i];
        *Reg_copy_ptr = Registers[i];
    }

    for (i = 15; i < 34; i++) {
        Reg_copy_ptr = (unsigned int *)gen[i];
        *Reg_copy_ptr = Registers[i];
    }

    for (i = 36; i < 47; i++) {
        Reg_copy_ptr = (unsigned int *)gen[i];
        *Reg_copy_ptr = Registers[i];
    }

    for (i = 48; i < 169; i++) {
        Reg_copy_ptr = (unsigned int *)gen[i];
        *Reg_copy_ptr = Registers[i];
    }

    for (i = 170; i < 230; i++) {
        Reg_copy_ptr = (unsigned int *)gen[i];
        *Reg_copy_ptr = Registers[i];
    }

    // Lock registers.
    MPUCTL0_H = 0x01;
    PMMCTL0_H = 0x01;
    FRCTL0_H = 0x01;
    CSCTL0_H = 0x01;
}

inline void int_falling(void)
{
    P1IFG = 0; // Clear P1 interrupt flag.
    P1IES |= BIT0; // Trigger P1.0 interrupt on falling edge.
    P1IE |= BIT0; // P1.0 interrupt enabled.
    P1IFG = 0; // Clear P1 interrupt flag.
}

inline void int_rising(void)
{
    P1IFG = 0; // Clear P1 interrupt flag.
    P1IES &= ~(BIT0); // Trigger P1.0 interrupt on rising edge.
    P1IE |= BIT0; // P1.0 interrupt enabled.
    P1IFG = 0; // Clear P1 interrupt flag.
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    switch (__even_in_range(P1IV, P1IV_P1IFG0)) {
    case P1IV_P1IFG0:
        // Disable interrupts.
        P1IFG = 0;
        P1IE &= ~BIT0;

        // Hibernate the processor.
        if (*FLAG_interrupt == 2) {
            Hibernate();

            if (pro == 0) {
                t = COMP_ON;
                if (t == 0) {
                    *FLAG_interrupt = 4;
                    int_rising();
                    // Enter sleep state.
                    power_off();
                    __bis_SR_register(LPM4_bits + GIE); // LPM4, External Comparator will force exit
                    __no_operation();
                } else {
                    *FLAG_interrupt = 2;
                    int_falling();
                    // Return to active mode with interrupts enabled.
                    __bis_SR_register(GIE);
                    __no_operation();
                }
            }
        }

        if (*FLAG_interrupt == 4) {
            *FLAG_interrupt = 2;
            int_falling();
            // Return to active mode with interrupts enabled.
            __bis_SR_register(GIE);
            __no_operation();
        }

        pro = 0;
        // Exit sleep state, processor is active.
        __bic_SR_register_on_exit(LPM4_bits);
        break;
    default:
        break;
    }
}
