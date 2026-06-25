#include "hardware.h"
#include "module.h"
#include "can.h"
#include "robot.h"
#include "registers.h"

const uint8_t MOTOR_ADDR[5] = {5,26,24,22,25};
static volatile uint8_t coordinates[2] = {0,0};

/* Register callback function, handles some new registers on the radio.
 * MB register 2 handle x and y coordinates of the robot.
 */
static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
  switch (operation)
  {
    case ROP_WRITE_MB:
      if (address == 2) {
        if (2 == radio_data->multibyte.size){
          coordinates[0] = radio_data->multibyte.data[0];
          coordinates[1] = radio_data->multibyte.data[1];
        }else{
          return FALSE;
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

  uint8_t x_to_led;
  uint8_t y_to_led;

  // Registers the register handler callback function
  radio_add_reg_callback(register_handler);

  // Shuts down all the non head LEDs
  for (int i = 0 ; i < 5 ; i++){
    set_reg_value_dw(MOTOR_ADDR[i], MREG32_LED, 0);
  } 

  // Blinks the led to show the boot
  set_rgb(0, 0, 0);
  pause(ONE_SEC);
  set_rgb(0, 0, 64);
  pause(ONE_SEC);

  // Initialises the body module with the specified address (but do not start the PD controller)
  // init_body_module(MOTOR_ADDR);
  
  // Then update LED's color according to x-y position
  while (1) {
    // green in (0,0) - cyan in (0,2) - yellow in (6,0) - white in (6,2)
    set_rgb(coordinates[0], 64, coordinates[1]);
  }

  return 0;
}
