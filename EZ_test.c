#include <io.h>
#include <delay.h>
#define F_CPU 1000000UL

#define SET_BIT(VAR,BITNO) (VAR) |=  (1 << (BITNO))
#define CLR_BIT(VAR,BITNO) (VAR) &= ~(1 << (BITNO))
#define TOG_BIT(VAR,BITNO) (VAR) ^=  (1 << (BITNO))
#define GET_BIT(VAR,BITNO) (((VAR) >> (BITNO)) & 0x01)

#define UART_u8_UDR_REG       *((volatile u8*)0x2c)
#define UART_u8_UCSRA_REG       *((volatile u8*)0x2B)
#define UART_u8_UCSRB_REG       *((volatile u8*)0x2A)
#define UART_u8_UCSRC_REG       *((volatile u8*)0x40)
#define UART_u8_UBRRH_REG       *((volatile u8*)0x40)
#define UART_u8_UBRRL_REG       *((volatile u8*)0x29)

typedef unsigned char u8;
typedef unsigned short int u16;

void UART_voidInit(){
    u8 Local_u8UCSRCValue=0x80;
    u16 Local_u16UBRRValue=6;

    /* no double speed */
    CLR_BIT(UART_u8_UCSRA_REG,1);
    /* ENABLE RX CIRCUIT */
    SET_BIT(UART_u8_UCSRB_REG,4);
    /* ENABLE TX CCT */
    SET_BIT(UART_u8_UCSRB_REG,3);
    /* 8 BITS DATA */
    CLR_BIT(UART_u8_UCSRB_REG,2);
    /* set our variable to insert it in the real register  */

    SET_BIT(Local_u8UCSRCValue,2);
    SET_BIT(Local_u8UCSRCValue,1);

    /* ASYNCHRONOUS MODE */

    CLR_BIT(Local_u8UCSRCValue,6);
    /* NO PARITY MODE */
    CLR_BIT(Local_u8UCSRCValue,5);
    CLR_BIT(Local_u8UCSRCValue,4);

    /* STOP BITS =2 */
    SET_BIT(Local_u8UCSRCValue,3);
    /* UPDATE UCSRC REG WUTH THE VALUE OF VARIABLE */
    UART_u8_UCSRC_REG=Local_u8UCSRCValue;
    /* BAUD RATE =9600 */
    
    UART_u8_UBRRL_REG=(u8)Local_u16UBRRValue;
    UART_u8_UBRRH_REG=(u8)(Local_u16UBRRValue>>8);
}

void UART_voidSendByte(u8 Copy_u8TxByte){
    /* wait for tx register to be empty */
    while (!GET_BIT(UART_u8_UCSRA_REG,5));

    /*SEND BYTE*/
    UART_u8_UDR_REG= Copy_u8TxByte;
}

void UART_send_string(char *myString)
{
    char i=0;
    for (i=0; myString[i]!='\0'; i++)
         UART_voidSendByte(myString[i]); 
}

struct pinpin{
    unsigned char IC_pin_num;
    unsigned char Port_pin_num; //counting from 0
};// to ease the code writing.

void set_pin(struct pinpin arg, char val)
{//function that sets/clears a pin based on the IC Pin Number ;
    char t = val;
    if (arg.IC_pin_num>=1 && arg.IC_pin_num !=7 && arg.IC_pin_num <=13){
        
        val=val<<arg.Port_pin_num;
        
        if (arg.IC_pin_num <=9)
        {
            if      (t==1)
                PORTA=PINA|val;
            else if (t==0)
                PORTA=PINA&val;
        }
        else
        {
            if      (t==1)
                PORTC=PINC|val;
            else if (t==0)
                PORTC=PINC&val;            
        }
    }
}

char read_pin(struct pinpin arg)
{
    if (arg.IC_pin_num>=1 && arg.IC_pin_num !=7 && arg.IC_pin_num <=13){
        int i=0;
        unsigned char myRead=0;
        if (arg.IC_pin_num <=9)
            myRead=PINA;
        else
            myRead=PINC;

        for (i=0;i<arg.Port_pin_num; i++)
            myRead = myRead >>1;
        return myRead & 1;
    }
}

//PORTA and PORTC will be used
void AND3_NAND3_IO_CONFIG(struct pinpin *inp, struct pinpin *out){
//This will be used for _7410, _7412, _7411 and _7415 functions.
    /*
        Those Devices have the same IO Configurations, So instead of writing
        those lines in every function, I wrote a function to do it to save
        memory, Plus a pinout diagram :D
        
        [7, 6, 5, 4, 3, 2, 1]
        =====================
        ====================C
        =====================
        [8, 9,10,11,12,13,14]
        POWER:
            14: Vcc
            7 : GND
        I/O:
            1, 2,13: IN_1
                12 : O__1
            
            3, 4, 5: IN_2
                6  : O__2
            
            9,10,11: IN_3
                8  : O__3  

        These pins will be connected first to PORTA then PORTC
    */
 //IC_PINS: 98654321    //7 is discluded because it's GND.
    DDRA =0b10011111;
    PORTA=0b00000000;

 //IC_PINS: XXXXDCBA       //X: Don't Care
    DDRC =0b00101011;
    PORTC=0b00000000; //No OUTPUT nor Pull_Up enabled.

    out[0].IC_pin_num=12;
    out[0].Port_pin_num=2;
    inp[0].IC_pin_num=1;
    inp[0].Port_pin_num=0;
    inp[1].IC_pin_num=2;
    inp[1].Port_pin_num=1;
    inp[2].IC_pin_num=13;
    inp[2].Port_pin_num=3;

    out[1].IC_pin_num=6;
    out[1].Port_pin_num=5;
    inp[3].IC_pin_num=3;
    inp[3].Port_pin_num=2;
    inp[4].IC_pin_num=4;
    inp[4].Port_pin_num=3;
    inp[5].IC_pin_num=5;
    inp[5].Port_pin_num=4;

    out[2].IC_pin_num=8;
    out[2].Port_pin_num=6;
    inp[6].IC_pin_num=9;
    inp[6].Port_pin_num=7;
    inp[7].IC_pin_num=10;
    inp[7].Port_pin_num=0;
    inp[8].IC_pin_num=11;
    inp[8].Port_pin_num=1;
}

char _7410(){//3*3-1 NAND GATE
    unsigned char i=0;
    unsigned char myRead, expectedRead;
    char a0, a1, a2;
    struct pinpin I[9],O[3];//A is INPUT, Y is OUTPUT
    AND3_NAND3_IO_CONFIG(I,O);
    delay_ms(100);
    for (i=0; i<=0b111; i++)
    {    
        a0=(i)%2;
        a1=(i>>1)%2;
        a2=(i>>2)%2;

        set_pin(I[0], a0);
        set_pin(I[1], a1);
        set_pin(I[2], a2);
        delay_ms(1);
        myRead=read_pin(O[0]);
        expectedRead=!(a0&a1&a2) & 0b00000001;
        delay_ms(100);
        if (expectedRead!=myRead) return 0b11111111;

        set_pin(I[3], a0);
        set_pin(I[4], a1);
        set_pin(I[5], a2);
        delay_ms(1);
        myRead=read_pin(O[1]);

        if (expectedRead!=myRead) return 0b11111111;
        set_pin(I[6], a0);
        set_pin(I[7], a1);
        set_pin(I[8], a2);
        delay_ms(1);
        myRead=read_pin(O[2]);

        if (expectedRead!=myRead) return 0b11111111;

    }
    return 0b00000001;
}
char _7411(){//3*3-1 AND GATE
    unsigned char i=0;
    unsigned char myRead, expectedRead;
    char a0, a1, a2;
    struct pinpin I[9],O[3];//A is INPUT, Y is OUTPUT
    AND3_NAND3_IO_CONFIG(I,O);
    delay_ms(1000);
    for (i=0; i<=0b111; i++)
    {     
        a0=(i>>0)%2;
        a1=(i>>1)%2;
        a2=(i>>2)%2;

        set_pin(I[0], a0);
        set_pin(I[1], a1);
        set_pin(I[2], a2);
        delay_ms(1);
        myRead=read_pin(O[0]);
        expectedRead=(a0&&a1&&a2); //& 0b00000001;
        
        if (expectedRead!=myRead) return 0b11111111;
        delay_ms(10);
        set_pin(I[3], a0);
        set_pin(I[4], a1);
        set_pin(I[5], a2);
        delay_ms(1);
        myRead=read_pin(O[1]);

        if (expectedRead!=myRead) return 0b11111111;
        delay_ms(10);
        set_pin(I[6], a0);
        set_pin(I[7], a1);
        set_pin(I[8], a2);
        delay_ms(1);
        myRead=read_pin(O[2]);
        
        if (expectedRead!=myRead) return 0b11111111;
        delay_ms(1);
    }
    return 0b00000010;
}
char _7412(){//3*3-1 NAND GATE OC
    unsigned char i=0;
    unsigned char myRead, expectedRead,ret=0b00000100;
    char a0, a1, a2;
    struct pinpin I[9],O[3];//A is INPUT, Y is OUTPUT
    AND3_NAND3_IO_CONFIG(I,O);
    
    for (i=0; i<=0b111; i++)
    {
        a0=(i>>0)%2;
        a1=(i>>1)%2;
        a2=(i>>2)%2;
        expectedRead=!(a0&&a1&&a2) & 1;

        set_pin(I[0], a0);
        set_pin(I[1], a1);
        set_pin(I[2], a2);
        set_pin(O[0], 1);
        delay_ms(1);
        myRead=read_pin(O[0]);

        if (expectedRead!=myRead) {ret= 0b11111111;break;}
        
        set_pin(I[3], a0);
        set_pin(I[4], a1);
        set_pin(I[5], a2);
        set_pin(O[1], 1);
        delay_ms(1);
        myRead=read_pin(O[1]);

        if (expectedRead!=myRead) {ret= 0b11111111;break;}

        set_pin(I[6], a0);
        set_pin(I[7], a1);
        set_pin(I[8], a2);
        set_pin(O[2], 1);
        delay_ms(1);
        myRead=read_pin(O[2]);
        
        if (expectedRead!=myRead) {ret= 0b11111111;break;}
    }
    return ret;
}
char _7415(){//3*3-1 AND GATE OC
    unsigned char i=0;
    unsigned char myRead, expectedRead, ret=0b00100000;
    char a0, a1, a2;
    struct pinpin I[9],O[3];//A is INPUT, Y is OUTPUT
    AND3_NAND3_IO_CONFIG(I,O);
    for (i=0; i<=0b111; i++)
    {
        //delay_ms(500);     
        a0=(i>>0)%2;
        a1=(i>>1)%2;
        a2=(i>>2)%2;

        set_pin(I[0], a0);
        set_pin(I[1], a1);
        set_pin(I[2], a2);
        set_pin(O[0], 1);
        delay_ms(1);
        myRead=read_pin(O[0]);
        expectedRead=(a0&&a1&&a2);
        
        if (expectedRead!=myRead) {ret=0b11111111;break;}
        
        set_pin(I[3], a0);
        set_pin(I[4], a1);
        set_pin(I[5], a2);
        set_pin(O[1], 1);
        delay_ms(1);
        myRead=read_pin(O[1]);

        if (expectedRead!=myRead) {ret=0b11111111;break;}

        set_pin(I[6], a0);
        set_pin(I[7], a1);
        set_pin(I[8], a2);
        set_pin(O[2], 1);
        delay_ms(1);
        myRead=read_pin(O[2]);
        
        if (expectedRead!=myRead) {ret=0b11111111;break;}
    }
    return ret;
}
char _7414(){
    //Hex Inverter
    struct pinpin I[6];
    struct pinpin O[6];
    char myRead,i=0;

    I[0].IC_pin_num=1;
    I[0].Port_pin_num=0;
    I[1].IC_pin_num=3;
    I[1].Port_pin_num=2;
    I[2].IC_pin_num=5;
    I[2].Port_pin_num=4;
    I[3].IC_pin_num=9;
    I[3].Port_pin_num=7;
    I[4].IC_pin_num=11;
    I[4].Port_pin_num=1;
    I[5].IC_pin_num=13;
    I[5].Port_pin_num=3;
    
    O[0].IC_pin_num=2;
    O[0].Port_pin_num=1;
    O[1].IC_pin_num=4;
    O[1].Port_pin_num=3;
    O[2].IC_pin_num=6;
    O[2].Port_pin_num=5;
    O[3].IC_pin_num=8;
    O[3].Port_pin_num=6;
    O[4].IC_pin_num=10;
    O[4].Port_pin_num=0;
    O[5].IC_pin_num=12;
    O[5].Port_pin_num=2;
//IC_PINS: 98654321 
    DDRA=0b10010101;
//IC_PINS: XXXXDCBA
    DDRC=0b00001010;

    PORTA=0;
    PORTC=0;

    for (i=0;i<6;i++)
    {
        //delay_ms(1000);   
        myRead = read_pin(O[i]);
        if (myRead!=1) return 0b11111111;
        delay_ms(1);
        set_pin(I[i], 1);
        
        myRead = read_pin(O[i]);
        if (myRead!=0) return 0b11111111;
        
    }
    
    return 0b00010000;
}
char _7413(){ //2*4-1 NAND GATE  
    struct pinpin I[8];
    struct pinpin O[2];
    char i;
    char a0, a1, a2, a3;
    char myRead,expectedRead;

    O[0].IC_pin_num=6;
    O[0].Port_pin_num=5;
    I[0].IC_pin_num=1;
    I[0].Port_pin_num=0;
    I[1].IC_pin_num=2;
    I[1].Port_pin_num=1;
    I[2].IC_pin_num=4;
    I[2].Port_pin_num=3;
    I[3].IC_pin_num=5;
    I[3].Port_pin_num=4;

    O[1].IC_pin_num=8;
    O[1].Port_pin_num=6;
    I[4].IC_pin_num=9;
    I[4].Port_pin_num=7;
    I[5].IC_pin_num=10;
    I[5].Port_pin_num=0;
    I[6].IC_pin_num=12;
    I[6].Port_pin_num=2;
    I[7].IC_pin_num=13;
    I[7].Port_pin_num=3;
//IC_PINS: 98654321
    DDRA=0b10011011;
//IC_PINS: XXXXDCBA
    DDRC=0b00001101;
    
    PORTA=0;
    PORTC=0;

    for (i=0; i<=0b1111;i++){
        a0=(i>>0)%2;
        a1=(i>>1)%2;
        a2=(i>>2)%2;
        a3=(i>>3)%2;
        expectedRead=!(a0&&a1&&a2&&a3) &1;
        
        set_pin(I[0],a0);
        set_pin(I[1],a1);
        set_pin(I[2],a2);
        set_pin(I[3],a3);
        delay_ms(1);
        myRead=read_pin(O[0]);

        if (myRead!=expectedRead) return 0b11111111; 
                
        set_pin(I[4],a0);
        set_pin(I[5],a1);
        set_pin(I[6],a2);
        set_pin(I[7],a3);
        delay_ms(1);
        myRead=read_pin(O[1]);
        
        if (myRead!=expectedRead) return 0b11111111;
    }
    return 0b00001000;
}
void main(void)
{ 
    char flagReg=0;
    char b_prev=0, b_cur=0;
    DDRA=0; DDRB=0xFF;DDRC=0;   DDRD=0xFF;     
    PORTA=0;PORTB=0  ;PORTC=0  ;PORTD=0;
    DDRC.4=0; //Input for a Push button
    PORTC.4=0;
    UART_voidInit();
    while(1)
    {
        b_cur=PINC.4;
        if (b_cur==1 && b_prev==0)
        {
            b_prev=1;
            PORTA=0;PORTB=0  ;PORTC=0  ;PORTD=0;
            DDRC.4=0; 
            //Testing starts
            flagReg=_7410();
            if (flagReg==0b11111111){   flagReg=_7411();PORTB=2+1;
            if (flagReg==0b11111111){   flagReg=_7412();PORTB=4+2+1;
            if (flagReg==0b11111111){   flagReg=_7413();PORTB=8+4+2+1;
            if (flagReg==0b11111111){   flagReg=_7414();PORTB=16+8+4+2+1;
            if (flagReg==0b11111111){   flagReg=_7415();PORTB=32+16+8+4+2+1;
            }}}}}
            switch (flagReg)
            {
                case 0b00000001:
                    UART_send_string("7410 ");
                    break;
                case 0b00000010:
                    UART_send_string("7411 ");
                    break;
                case 0b00000100:
                    UART_send_string("7412 ");
                    break;
                case 0b00001000:
                    UART_send_string("7413 ");
                    break;
                case 0b00010000:
                    UART_send_string("7414 ");
                    break;
                case 0b00100000:
                    UART_send_string("7415 ");
                    break;
                default:
                    UART_send_string("fail. ");
                    break;  
            }
        }
        else if (b_cur==0 && b_prev==1)
            b_prev=0;
    }
}