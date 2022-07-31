#ifndef __SPRITES_H__
#define __SPRITES_H__

#define SPRITE_BUFFER_SIZE  45

#define SPRITE_DATA \
0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, \
0x15, 0X00, 0x2A, 0X00, 0x54, 0X00, 0x28, 0x01, 0x50, 0x02, 0x20, 0x05, 0x40, 0x0A, 0X00, 0X00, \


void sprite_hgr_to_buffer(uint8_t column, uint8_t row);

void sprite_xor(uint8_t sprite, uint8_t column, uint8_t row, uint8_t shift);

void sprite_buffer_to_hgr(uint8_t column, uint8_t row);

#endif /* __SPRITES_H__ */
