#ifndef _APP_H_
#define _APP_H_

	extern void init_app_lorawan(void);
	extern void do_app_lorawan(void);
	extern void isr_app_lorawan_1ms(void);

	extern void init_app(void);
	extern void do_app(void);

	extern void isr_app_1ms(void);
	extern void isr_app_100us(void);
	extern void isr_app_custom(void);
	extern void isr_t4(void);

#endif
