#include "hardware.h"
#include "module.h"
#include "robot.h"
#include "registers.h"

#define POS_NB 4

const uint8_t TAIL_ADDR = 21;
const uint8_t BODY_ADDR = 72;
static uint8_t pos_size = 4;
static int8_t mb_pos[POS_NB];

static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data) 
{
  uint8_t i;

  switch (operation)
  {
    case ROP_READ_MB:
      if (address == 2) {
        radio_data->multibyte.size = pos_size;
        for (i = 0; i < pos_size; i++) {
          radio_data->multibyte.data[i] = mb_pos[i];
        }
        return TRUE;
      }
      break;
  }
  return FALSE;
}



int main(void)
{

  hardware_init();

  // Registers the register handler callback function
  radio_add_reg_callback(register_handler);
  
  // Changes the color of the led (first red then green) to show the boot
  // and then say that the module is ready.
  set_color_i(4, 0);
  pause(ONE_SEC);
  set_color_i(2, 0);
  

  // Initialises the body module with the specified address (but do not start
  // the PD controller)
  init_body_module(TAIL_ADDR);
  init_body_module(BODY_ADDR);
  init_limb_module(BODY_ADDR + 1);
  init_limb_module(BODY_ADDR + 2);


  
  // And then... do this
  while (1) {

    mb_pos[0] = bus_get(TAIL_ADDR, MREG_POSITION);
    mb_pos[1] = bus_get(BODY_ADDR, MREG_POSITION);
    mb_pos[2] = bus_get(BODY_ADDR+1, MREG_POSITION);
    mb_pos[3] = bus_get(BODY_ADDR+2, MREG_POSITION);

    // int8_t pos;
    // if (pos > 0) {
    //   set_rgb(pos, 32, 0);
    // } else {
    //   pos = -pos;
    //   set_rgb(0, 32, pos);
    // }

  }

  return 0;
}
