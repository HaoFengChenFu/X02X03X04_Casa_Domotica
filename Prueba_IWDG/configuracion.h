//DEFINE
#define BUTTONn                                  1
#define USER_BUTTON_EXTI_IRQn                    EXTI15_10_IRQn
#define USER_BUTTON_PIN                          GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT                    GPIOC
//FUNCIONES
void configuracion(void);
void reset_leds(void);
void init_rgb(void);
void init_user_botton(void);
