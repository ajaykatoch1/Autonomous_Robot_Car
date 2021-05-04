# Autonomous_Robot_Car
This is a autonomous vehicle avoiding robot car I created using a TM4C Cortex M4 based MCU. The firmware is programmed in bare metal register level C and utilizes 28BYJ stepper motors with ULN2003 drivers and uses RCWL-1601 hypersonic sensors. Utilizing input edge capture mode timers and and PWM signals the time and distance of the vehicle from obstacles can be calculated. The firmware uses various registers in order to create these calculations. The hypersonics sensors use UART as well between the tranmitter and recievers. DMM's were used to verify voltages and currents within the motors. This was built using the IAR embedded toolchain with the CMSIS standard.  
