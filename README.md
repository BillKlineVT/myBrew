# myBrew
QT-based Beer Brewing Controller and Monitoring

## UI
![alt text](https://github.com/BillKlineVT/myBrew/blob/master/myBrew%20screenshots/myBrew%20UI%20screenshot.png?raw=true)
- The main user interface is intended to be touchscreen friendly, but needs some more work to evaluate button sizes
- There are tabs for displays relevant to each step of the brewing process.

## File Structure
- Base directory of repo includes source code and project files for the QT IDE-based GUI for myBrew
- Subdirectories include helper-function Python apps for database interactions, Raspberry Pi GPIO interaction, PID & PWM Controller libraries
- The QT GUI and Python-based processing apps are linked via an sqlite database to allow for multi-service data access which also enables datalogging for post-brew session data review.
 
## Hardware
- System is designed to run on a Raspberry Pi, should support any version after Rasbperry Pi 1 B
- GPIO control of heating elements is wired to 40A-capable Solid State Relays which are wired to 120VAC residential power circuits.
- Pulse Width Modulation (PWM) is done using Raspberry Pi PWM library.  This enables gradual heating needed to implement PID algorithm.
