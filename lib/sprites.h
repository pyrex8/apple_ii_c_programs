#ifndef __SPRITES_H__
#define __SPRITES_H__

#define SPRITE_BUFFER_SIZE  45

void sprite_hgr_to_buffer(uint8_t column, uint8_t row);

void sprite_xor(uint8_t page, uint8_t sprite, uint8_t column, uint8_t row, uint8_t shift);

void sprite_buffer_to_hgr(uint8_t column, uint8_t row);

#endif /* __SPRITES_H__ */
