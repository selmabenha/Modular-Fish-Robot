#include "config.h"
#include "modes.h"
#include "robot.h"
#include "module.h"
#include "registers.h"
#include "hardware.h"

const uint8_t MOTOR_ADDR = 21;
static float FREQ = 1.0;   // Hz
static float AMP = 40.0;   

/* Register callback function, handles some new registers on the radio.
 * All these registers are of course completely useless, but it demonstrates how
 * to implement a register callback function, and what it can do.
 */
static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
  switch (operation)
  {
    case ROP_WRITE_8:
      if (address == 2) {
        AMP = DECODE_PARAM_8(radio_data->byte,0,60);
        return TRUE;
      }
      if (address == 3) {
        FREQ = DECODE_PARAM_8(radio_data->byte,0,2);
        return TRUE;
      }
  }
  return FALSE;
}

void sine_demo_mode()
{
  uint32_t dt, cycletimer;
  float my_time, delta_t, l;
  int8_t l_mot;

  cycletimer = getSysTICs();
  my_time = 0;

  init_body_module(MOTOR_ADDR);
  start_pid(MOTOR_ADDR);

  do {
    // Calculates the delta_t in seconds and adds it to the current time
    dt = getElapsedSysTICs(cycletimer);
    cycletimer = getSysTICs();
    delta_t = (float) dt / sysTICSperSEC;
    my_time += delta_t;

    // Calculates the sine wave
    l = 32 * sin(M_TWOPI * FREQ * my_time);

    l_mot = (int8_t) AMP * sin(M_TWOPI * FREQ * my_time);

    // Outputs the sine wave to the LED
    if (l >= 0) {
      set_rgb(0, l, 32);
    } else {
      set_rgb(-l, 0, 32);
    }

    // Outputs the sine wave to the Motors
    bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_mot));

    // Make sure there is some delay, so that the timer output is not zero
    pause(ONE_MS);

  } while (reg8_table[REG8_MODE] == IMODE_SINE_DEMO);

  bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
  pause(ONE_SEC);
  bus_set(MOTOR_ADDR, MREG_MODE, MODE_IDLE);

  // Back to the "normal" green
  set_color(2);
}

void main_mode_loop()
{
  // Registers the register handler callback function
  radio_add_reg_callback(register_handler);

  reg8_table[REG8_MODE] = IMODE_IDLE;

  while (1)
  {
    switch(reg8_table[REG8_MODE])
    {
      case IMODE_IDLE:
        break;
      case IMODE_SINE_DEMO:
        sine_demo_mode();
        break;
      default:
        reg8_table[REG8_MODE] = IMODE_IDLE;
    }
  }
}
