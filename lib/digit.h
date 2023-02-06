#ifndef __DIGIT_H__
#define __DIGIT_H__

void digit_init(void);

void digit_set(uint8_t column, uint8_t row, uint8_t digit);
uint8_t digit_ones_get(uint8_t value);
uint8_t digit_tens_get(uint8_t value);
uint8_t digit_hundreds_get(uint8_t value);

#endif /* __DIGIT_H__ */
