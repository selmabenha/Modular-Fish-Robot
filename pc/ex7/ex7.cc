#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <ctime>
#include <windows.h>
#include "trkcli.h"
#include "utils.h"
#include "remregs.h"
#include "robot.h"
#include "regdefs.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const char* TRACKING_PC_NAME = "biorobpc6";   ///< host name of the tracking PC
const uint16_t TRACKING_PORT = 10502;          ///< port number of the tracking PC

const uint8_t RADIO_CHANNEL = 126;         ///< robot radio channel
const char* INTERFACE = "COM3";            ///< robot radio interface

static float freq = 1.0;
static int amp = 55;
static float lag = 0.5;

bool safe_set(CRemoteRegs& regs, uint8_t reg, uint8_t c)
{
  for (int i(0); i<10; i++) {
    if (regs.set_reg_b(reg, c)) {
      return true;
    }
  }
  cerr << "ERROR writing register " << (int) reg << endl;
  return false;
}


void write_freq(CRemoteRegs& regs, float f)
{  
  safe_set(regs, 3, ENCODE_PARAM_8(f,0,1.5));
}

void write_amp(CRemoteRegs& regs, int A)
{  
  safe_set(regs, 2, ENCODE_PARAM_8(A,0,60));
}

void write_lag(CRemoteRegs& regs, float l)
{  
  safe_set(regs, 4, ENCODE_PARAM_8(l,0.5,1.5));
}

void experiment(ofstream& file, CTrackingClient& trk)
{
  float start = time_d();


  while (!kbhit()) {
    uint32_t frame_time;
    // Gets the current position
    if (!trk.update(frame_time)) {
      break;
    }

    double x, y;
    cout.precision(2);
    
    // Gets the ID of the first spot (the tracking system supports multiple ones)
    int id = trk.get_first_id();
    
    // Reads its coordinates (if (id == -1), then no spot is detected)
    if (id != -1 && trk.get_pos(id, x, y)) {
      cout << "(" << fixed << x << ", " << y << ")" << " m      \r";
      // Save data 
      file << time_d() - start << "," << x << "," << y << endl;
    } else {
      cout << "(not detected)" << '\r';
    }
    // Waits 10 ms before getting the info next time (anyway the tracking runs at 15 fps)
    Sleep(10);
  }

  // Clears the console input buffer (as kbhit() doesn't)
  FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}


int main()
{
  CTrackingClient trk;
  CRemoteRegs regs;

  // Connects to the tracking server
  if (!trk.connect(TRACKING_PC_NAME, TRACKING_PORT)) {
    return 1;
  }  

  // Connects to the radio
  if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
    return 1;
  }

  // Reboots the head microcontroller to make sure it is always in the same state
  reboot_head(regs);

  // Loop until all experiments are done 
  while(1){

    // Display actual params
    cout << "( freq = " << freq << ", amp = " << amp << ", lag = " << lag << ")" << endl;

    // Ask for changes 
    cout << " Change (Y/N/Q)? ";
    char change = (LOWORD(ext_key()));
    if (change == 'y'){
      cout << " Freq =  ";
      cin >> freq;
      cout << "Amp = ";
      cin >> amp;
      cout << "Lag = ";
      cin >> lag;
    } else if (change == 'q') {
      break;
    } else {
      cout << "No change..." << endl;
    }

    cout << "Press a key once robot is straight... " << flush;
    ext_key();
    cout << endl;

    // Get Robot rigid 
    write_amp(regs,0);  // set ampl = 0
    safe_set(regs, REG8_MODE, 2);
    


    // Init data file
    ostringstream fn;
    fn << "data/data_" << freq << "_" << amp << "_" << lag << "_" << time(NULL) << ".csv";
    ofstream file(fn.str());
    file << "Time,X,Y" << endl;

    cout << "Press a key once robot is ready to swim... " << flush;
    ext_key();
    cout << endl;

    // Sends parameters to the robot
    write_freq(regs,freq);
    write_amp(regs,amp);
    write_lag(regs,lag);


    // Run experiment 
    regs.set_reg_dw(REG32_LED, 0xffffff);
    experiment(file, trk);
    regs.set_reg_dw(REG32_LED, 0x000200);
    safe_set(regs, REG8_MODE, 0);
  }

  regs.close();
}

  

  
