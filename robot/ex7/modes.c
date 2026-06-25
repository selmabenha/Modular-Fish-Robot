#include "config.h"
#include "modes.h"
#include "robot.h"
#include "module.h"
#include "registers.h"
#include "hardware.h"

const uint8_t N_body = 5;
const uint8_t MOTOR_ADDR[5] = {25,22,24,26,5};
static float FREQ = 0.0;   // Hz
static float AMP = 0.0;
static float LAG = 0.0;  
 

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
        FREQ = DECODE_PARAM_8(radio_data->byte,0,1.5);
        return TRUE;
      }
      if (address == 4) {
        LAG = DECODE_PARAM_8(radio_data->byte,0.5,1.5);
        return TRUE;
      }
  }
  return FALSE;
}

void sinus_mode()
{
  uint32_t dt, cycletimer;
  float my_time, delta_t, l;
  int8_t l_mot[N_body];

  cycletimer = getSysTICs();
  my_time = 0;

  for(int i = 0; i<N_body; i++)
  {
    //set_reg_value_dw(MOTOR_ADDR[i], MREG32_LED, 0);
    init_body_module(MOTOR_ADDR[i]);
    start_pid(MOTOR_ADDR[i]);
  }
  
  do {
    // Calculates the delta_t in seconds and adds it to the current time
    dt = getElapsedSysTICs(cycletimer);
    cycletimer = getSysTICs();
    delta_t = (float) dt / sysTICSperSEC;
    my_time += delta_t;

    for(int i = 0; i<N_body; i++) {
      // Calculates the sine wave
      l_mot[i] = (int8_t) ((i+1)*AMP/N_body) * sin(M_TWOPI *(FREQ * my_time - i*LAG/N_body) );
      // Outputs the sine wave to the Motors
      bus_set(MOTOR_ADDR[i], MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_mot[i]));
    }
    
    // Make sure there is some delay, so that the timer output is not zero
    pause(ONE_MS);

  } while (reg8_table[REG8_MODE] == IMODE_SINE_DEMO);

  for(int i = 0; i<N_body; i++) {
    bus_set(MOTOR_ADDR[i], MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
    pause(ONE_SEC);
    bus_set(MOTOR_ADDR[i], MREG_MODE, MODE_IDLE);
  }
 

  // Back to the "normal" green
  set_color(2);
}

void main_mode_loop()
{
  // Registers the register handler callback function
  radio_add_reg_callback(register_handler);

  reg8_table[REG8_MODE] = IMODE_IDLE;

  // Shuts down all the non head LEDs
  for (int i = 0 ; i < 5 ; i++){
    set_reg_value_dw(MOTOR_ADDR[i], MREG32_LED, 0);
  }

  while (1)
  {
    switch(reg8_table[REG8_MODE])
    {
      case IMODE_IDLE:
        break;
      case IMODE_SINE_DEMO:
        sinus_mode();
        break;
      default:
        reg8_table[REG8_MODE] = IMODE_IDLE;
    }
  }
}
