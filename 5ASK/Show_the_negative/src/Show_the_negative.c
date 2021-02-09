// memory-mapped I/O addresses
#define GPIO_SWs 0x80001400
#define GPIO_LEDs 0x80001404
#define GPIO_INOUT 0x80001408

#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_GPIO(dir, value)             \
  {                                        \
    (*(volatile unsigned *)dir) = (value); \
  }

int main(void)
{
  int En_Value = 0xFFFF;
  WRITE_GPIO(GPIO_INOUT, En_Value);
  unsigned int first_number, switches, output;
  unsigned int prev_temp;
  unsigned int second = 0x0000;
  //change the value in output line
  //test values:0b0101010101010101=21845=0x5555 output:0xAAAA
  //test values:0b000000000101001=41=0x29 output :0xffd6
  //best practice to change the value in the first number line
  //by changing the switches value
  //0x55550000 = 1431633920 output:0xAAAA
  //0x00290000=2686976 output :0xffd6
  switches = READ_GPIO(GPIO_SWs); // read value on switches
  first_number = switches >> 16;

  output = first_number & 0xFFFF ^ 0xFFFF;
  prev_temp = first_number >> 15 & 0x0001;
  int i;
  for (i = 0; i < 8; i++)
  {
    WRITE_GPIO(GPIO_LEDs, output);
    //delay
    WRITE_GPIO(GPIO_LEDs, second);
    //delay
  }

  while (1)
  {

    //change the value in output line
    //test values:0b0101010101010101=21845=0x5555 output:0xAAAA
    //test values:0b000000000101001=41=0x29 output :0xffd6

    //best practice to change the value in the first number line
  //by changing the switches value
  //0x55550000 = 1431633920 output:0xAAAA (this has 0 in the msb)
  //0x00290000=2686976 output :0xffd6
  //to check what we need to change the msb to change the values
  //0xd5550000= 3579117568 (this has 1 in the msb)
    unsigned int current_temp;
    switches = READ_GPIO(GPIO_SWs); // read value on switches
    first_number = switches >> 16;
    output = first_number & 0xFFFF ^ 0xFFFF;
    current_temp = first_number >> 15 & 0x0001;
    if (prev_temp != current_temp)
    {
      int i;
      for (i = 0; i < 8; i++)
      {
        WRITE_GPIO(GPIO_LEDs, output);
        //delay
        WRITE_GPIO(GPIO_LEDs, second);
        //delay
      }
      prev_temp = current_temp;
    }
  }
  return (0);
}
