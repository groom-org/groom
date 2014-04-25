#ifndef GROOM_BUTTON_H_
#define GROOM_BUTTON_H_

/* initialize the button */
void button_init(void);

/*
 * find out if the button has been pressed since the
 * last time button_was_pressed was called
 */
uint8_t button_was_pressed(void);

/* get the current value of the button */
uint8_t button_val(void);

#endif /* GROOM_BUTTON_H_ */