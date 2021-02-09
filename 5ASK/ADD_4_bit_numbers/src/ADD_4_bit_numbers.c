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
  //best way is to change the values from
  //switches by settings this 2 numbers
  //test values : 2148073472=0x80090000
  //test values :1342439424=0x50040000
  //else change the numbers in the outputline
  //it skips the second_number line
  unsigned int first_number, second_number;
  unsigned int switches, output;

  while (1)
  {
    switches = READ_GPIO(GPIO_SWs); // read value on switches
    first_number = switches >> 16;
    second_number = switches >> 28;
    first_number = first_number & 0x000F;
    second_number = second_number & 0x000F;
    output = first_number + second_number;
    if (output > 0x000F)
    {
      output = 0x0010;
      WRITE_GPIO(GPIO_LEDs, output);
    }
    else
    {
      WRITE_GPIO(GPIO_LEDs, output);
    }
  }

  return (0);
}
