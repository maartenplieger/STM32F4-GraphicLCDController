GPIO_InitTypeDef GPIO_InitStructure;

/* Initialize the GPIOD port.
 * See also the comments beginning stm32f4xx_gpio.c
 * (found in the library)
 */
static void setup_ports(void) {
  /* Enable the GPIOD peripheral clock before we
   * actually setup GPIOD.
   * This function is declared in stm32f4xx_rcc.h
   * and implemented in stm32f4xx_rcc.c
   */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  /* which pins we select to setup
   * every pin number is mapped to a bit number */
  GPIO_InitStructure.GPIO_Pin   = 65535;
  /* pins in output mode */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  /* high clock speed for the selected pins
   * see stm32f4xx_gpio.h for different speeds
   * (enum GPIOSpeed_TypeDef) */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  /* operating output type for the selected pins
   * see the enum GPIOOType_TypeDef in stm32f4xx_gpio.h
   * for different values.
   * PP stands for "push/pull", OD stands for "open drain"
   * google for "push pull vs open drain" */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  /* operating Pull-up/Pull down for the selected pins */
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  
  /* Write this data into memory at the address
   * mapped to GPIO device port D, where the led pins
   * are connected */
  
  /* This call resolves in
   * GPIO_Init((GPIO_TypeDef *) 0X40020C00, &GPIO_InitStructure)
   * where 0X40020C00 is the memory address mapped to
   * the GPIOD port. Without the library we would have to know all
   * these memory addresses. */
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}