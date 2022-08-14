#ifndef __SPRITE_H__
#define __SPRITE_H__

void sprite_init(void);

void sprite_hgr_to_buffer(uint8_t column, uint8_t row);

void sprite_xor(uint8_t sprite, uint8_t column, uint8_t row, uint8_t shift);

void sprite_buffer_to_hgr(uint8_t column, uint8_t row);

#endif /* __SPRITES_H__ */
