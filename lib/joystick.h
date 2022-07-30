#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

void joystick_run(void);

uint8_t joystick_fire_get(void);
uint8_t joystick_up_get(void);
uint8_t joystick_down_get(void);
uint8_t joystick_left_get(void);
uint8_t joystick_right_get(void);

#endif /* __JOYSTICK_H__ */
