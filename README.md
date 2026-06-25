Sure — here is the full `README.md` in copy-paste ready Markdown:

````md
# Modular Fish Robot – ME-425 Robotics Practicals

Programming, control, and experimental characterization of a modular lamprey-inspired swimming robot using embedded C/C++, PC-side C++ control software, and vision-based tracking.

## Overview

This project implements the full control stack of a modular fish robot inspired by the Salamandra Robotica II platform. It includes:

- Embedded firmware for motor control and inter-module communication
- PC-side control software for trajectory generation and parameter tuning
- Radio-based register communication between PC and robot
- Vision-based tracking system for trajectory measurement
- Experimental analysis of swimming performance under varying gait parameters

The system enables real-time control of a multi-segment underwater robot and quantitative evaluation of swimming dynamics.

## System Architecture

The system is split into three main components:

**1. Embedded robot firmware (C)**
- Motor control (PID-based position control)
- Register-based communication interface (8/16/32-bit + multibyte buffers)
- CAN bus communication between modules
- LED state feedback system

**2. PC control software (C++)**
- Trajectory generation (sinusoidal gait, traveling wave)
- Register read/write interface via radio
- Experiment orchestration and parameter tuning
- Data logging (CSV export)

**3. Vision tracking system**
- Multi-camera LED-based tracking in aquarium environment
- Position reconstruction (x, y)
- Synchronization with robot telemetry
- Data export for post-processing (Python)

## Key Features

### Embedded Control
- Register-based communication protocol (read/write callbacks)
- Modular body control via CAN bus
- Motor setpoint control using encoded angular commands
- LED feedback driven by state or position

### Motion Generation
- Sinusoidal tail oscillation:
  - Amplitude control (0–60°)
  - Frequency control (0–2 Hz)

- Traveling wave gait:
\[
\theta_i = \frac{i+1}{N} A \sin(2\pi(\nu t + i\phi))
\]

### PC Interface
- Real-time parameter tuning (frequency, amplitude, lag)
- Safe register writing with retry mechanism
- Experiment mode triggering via control registers
- Multibyte coordinate transmission to robot

### Experimental Pipeline
- Automated CSV logging of:
  - Time
  - Position (x, y)
- Speed computation:
\[
v = \frac{\sqrt{dx^2 + dy^2}}{dt}
\]

- Batch analysis across multiple trials

## Project Structure

```text
├── robot/                 # Embedded firmware (C)
│   ├── main.c
│   ├── modes.c
│   ├── bus/
│   └── registers/
│
├── pc/                    # PC control software (C++)
│   ├── main.cpp
│   ├── experiment.cpp
│   ├── tracking/
│   └── utils/
│
├── data/                  # Experimental CSV logs
│
├── plots/                 # Generated figures
│
├── scripts/               # Python data processing
│   └── process_data.py
│
├── README.md
└── requirements.txt
````

## Experiment Modes

* `IMODE_IDLE` – Default safe state
* `IMODE_SINE_DEMO` – Onboard sinusoidal gait generation
* `IMODE_MOTOR_DEMO` – PC-driven motor trajectory execution
* `IMODE_TRACKING` – LED-based position feedback mode

## Communication Protocol

### Registers

* 8-bit registers: parameter control (mode, flags, encoded values)
* 16/32-bit registers: motor commands and system state
* Multibyte registers: coordinate and sensor buffers

### Example PC write

```cpp
regs.set_reg_b(REG8_MODE, 1);
```

### Example robot callback

```c
case ROP_WRITE_8:
    if (address == 2) {
        AMP = DECODE_PARAM_8(radio_data->byte, 0, 60);
    }
```

## Data Processing

Python pipeline used for analysis:

* CSV aggregation across experiments

* Signal smoothing (moving average)

* Speed estimation:
  [
  v = \frac{\sqrt{dx^2 + dy^2}}{dt}
  ]

* Plot generation of:

  * Trajectories
  * Speed evolution
  * Average performance

## Requirements

### System

* C/C++ toolchain (embedded + PC)
* Robot SDK (register + bus API)
* Python 3.x

### Python dependencies

```bash
pip install -r requirements.txt
```

* numpy
* pandas
* matplotlib

## Running the Project

### 1. Compile robot firmware

```bash
make robot
```

### 2. Run PC controller

```bash
./pc_controller
```

### 3. Run experiments

* Set parameters (frequency, amplitude, lag)
* Launch experiment mode
* Data saved in `/data`

### 4. Process results

```bash
python scripts/process_data.py
```

## Safety Notes

* Ensure robot is fully straight before initialization
* Verify radio connection before experiments
* Use `safe_set()` for reliable register writes

## Authors

* Selma Benhassine (SCIPER 300148)
* Victor Labbe (SCIPER 302891)
* Liam Gibbons (SCIPER 300299)

Master in Robotics – EPFL

```

If you want, I can also:
- add **badges (build passing, language, license)**  
- make a **short “GitHub front-page version”**
- or convert it into a **professional research repo README with figures + GIF placeholders**
```
