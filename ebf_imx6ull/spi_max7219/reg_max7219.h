#ifndef __MAX7219_H__
#define __MAX7219_H__

// STM32 peripheral

// MAX7219 REG
#define REG_DECODE 0x09
#define REG_INTENSITY 0x0A
#define REG_SCAN_LIMIT 0x0B
#define REG_SHUTDOWN 0x0C
#define REG_DISPLAY_TEST 0x0F

// INTENSITY
#define INTENSITY_MIN 0x00
#define INTENSITY_MAX 0x0F

/*
*********************************************************************************************************
* LED Segments:         a
*                     ----
*                   f|    |b
*                    |  g |
*                     ----
*                   e|    |c
*                    |    |
*                     ----  o dp
*                       d
*   Register bits:
*      bit:  7  6  5  4  3  2  1  0
*           dp  a  b  c  d  e  f  g
*********************************************************************************************************
* Example : The letter 'I' is represented by illuminating LED's 'b' and 'c' (refer above diagram)
*           Therfore the binary representation of 'I' is as follows
*
*           abcdefg
*           0110000
*
*           The table below contains all the binary values for the desired font. New font characters
*           can be added or altered as required.
*
*           The DP bit is used to switch on the decimal place LED. DP is not included in the below table
*           but is added in the register within the libary depending on the content being displayed.
*********************************************************************************************************
*/
/*
static const struct
{
    char ascii;
    char segs;
} MAX7219_Font[] = {
    {'A', 0b1110111},
    {'B', 0b1111111},
    {'C', 0b1001110},
    {'D', 0b1111110},
    {'E', 0b1001111},
    {'F', 0b1000111},
    {'G', 0b1011110},
    {'H', 0b0110111},
    {'I', 0b0110000},
    {'J', 0b0111100},
    {'L', 0b0001110},
    {'N', 0b1110110},
    {'O', 0b1111110},
    {'P', 0b1100111},
    {'R', 0b0000101},
    {'S', 0b1011011},
    {'T', 0b0001111},
    {'U', 0b0111110},
    {'Y', 0b0100111},
    {'[', 0b1001110},
    {']', 0b1111000},
    {'_', 0b0001000},
    {'a', 0b1110111},
    {'b', 0b0011111},
    {'c', 0b0001101},
    {'d', 0b0111101},
    {'e', 0b1001111},
    {'f', 0b1000111},
    {'g', 0b1011110},
    {'h', 0b0010111},
    {'i', 0b0010000},
    {'j', 0b0111100},
    {'l', 0b0001110},
    {'n', 0b0010101},
    {'o', 0b1111110},
    {'p', 0b1100111},
    {'r', 0b0000101},
    {'s', 0b1011011},
    {'t', 0b0001111},
    {'u', 0b0011100},
    {'y', 0b0100111},
    {'-', 0b0000001},
    {' ', 0b0000000},
    {'0', 0b1111110},
    {'1', 0b0110000},
    {'2', 0b1101101},
    {'3', 0b1111001},
    {'4', 0b0110011},
    {'5', 0b1011011},
    {'6', 0b1011111},
    {'7', 0b1110000},
    {'8', 0b1111111},
    {'9', 0b1111011},
    {'/0', 0b0000000},
};
*/

static const struct
{
    char ascii;
    char segs;
} MAX7219_Font[] = {
    {'A', 0x77},
    {'B', 0x7F},
    {'C', 0x4E},
    {'D', 0x7E},
    {'E', 0x4F},
    {'F', 0x47},
    {'G', 0x5E},
    {'H', 0x37},
    {'I', 0x30},
    {'J', 0x3C},
    {'L', 0x0E},
    {'N', 0x76},
    {'O', 0x7E},
    {'P', 0x67},
    {'R', 0x05},
    {'S', 0x5B},
    {'T', 0x0F},
    {'U', 0x3E},
    {'Y', 0x27},
    {'[', 0x4E},
    {']', 0x78},
    {'_', 0x08},
    {'a', 0x77},
    {'b', 0x1F},
    {'c', 0x0D},
    {'d', 0x3D},
    {'e', 0x4F},
    {'f', 0x47},
    {'g', 0x5E},
    {'h', 0x17},
    {'i', 0x10},
    {'j', 0x3C},
    {'l', 0x0E},
    {'n', 0x15},
    {'o', 0x7E},
    {'p', 0x67},
    {'r', 0x05},
    {'s', 0x5B},
    {'t', 0x0F},
    {'u', 0x1C},
    {'y', 0x27},
    {'-', 0x01},
    {' ', 0x00},
    {'0', 0x7E},
    {'1', 0x30},
    {'2', 0x6D},
    {'3', 0x79},
    {'4', 0x33},
    {'5', 0x5B},
    {'6', 0x5F},
    {'7', 0x70},
    {'8', 0x7F},
    {'9', 0x7B},
    {'\0', 0x00},
};

#endif