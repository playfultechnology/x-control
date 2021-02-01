# x-control
Open Source USB-HID custom control interfaces

## X-Control-1
The x-control-1 is a primary controller interface based on the design of the Thrustmaster FCS Mark I.
The FCS features 2 analog axes, 4 digital buttons, and a 6-position POV hat switch (analog). The original design featured a 15-pin DSUB gameport interface.
X-Control-1 retains all functionality of the original design, exposed as a modern USB HID device.
![x-control-1 hat](xcontrol1/interior_cutaway.jpg)

### Schematic
![x-control-1 schematic](xcontrol1/xcontrol1_schem.jpg)

### Hardware Implementation
The design approach is to try to ensure any modifications made to the original hardware are non-destructive.
#### Axes
One necessary adjustment is to the way the linear pots are wired to the vertical and horizontal axis. In the original design, VCC is connected to the wiper, and the analog signal line is connected to one side of the pot. The other side of the pot is left unconnected. 
The value of the pot was read using a monostable multivibrator circuit, which measured the length of time to charge and discharge a capacitor. The bigger the potentiometer resistance value is, the longer it takes for the capacitor to reach a certain threshold voltage, which in turn can be used to derive the position of the stick (since the potentiometer is directly adjusted by the angle of the stick). This simple method for measuring the resistance value is cheap, but not very accurate.
(read more at https://www.epanorama.net/documents/joystick/pc_joystick.html)

In modern circuits, it is more common to wire either end of the pot to GND and VCC, and to connect the middle wiper output to a DAC to read the resistance value, and this is the approach taken here.

#### Hat
The original wiring of the hat switch may seem fairly complicated. Indeed, it was the subject of a [U.S. Patent 5389950](https://uspto.report/patent/grant/5389950). Each directional switch in the hat corresponds to a different resistor value. These are wired in parallel with a resistor connected to the centre switch, meaning that a number of discrete digital inputs can be multiplexed together into a single analog value. By reading that value via an ADC and applying appropriate logic in code, we can work out what combination of resistors must have been connected.
![x-control-1 hat](xcontrol1/hat_wiring.jpg)

| Hat Position | Resistor | 10-bit ADC value (split over 47kΩ divider) |
| ------------ | -------- | ------------------------------------ |
| Left | 240kΩ | 443 |
| Up | 200Ω | 1019 |
| Centre | 82.5kΩ | 371
| Down| 82kΩ | 545 |
| Right| 27kΩ | 714 |
| Down-Left| 82kΩ + 240kΩ (in parallel) | 586 |
| Down-Right| 82kΩ + 27kΩ (in parallel) | 759 |

Note all resistors listed are wired in parallel with the 82.5kΩ central resistor (the blue resistor in the above image)

### Software
See x-control.ino for code implementation, which should be uploaded to an ATMega32u4-based board such as an Arduino Pro Micro.
The code makes use of the excellent HID library by NicoHood - https://github.com/NicoHood/HID

To edit the description of the hardware (e.g. as displayed by Windows when the device is connected), it is necessary to follow these additional steps:
 - Edit the boards.txt file. This can either be found at *C:\Users\%USERNAME%\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.3* or *%ARDUINO_INSTALL_DIR%\arduino-1.8.9\hardware\arduino\avr*
 - Add the following lines to the bottom of the file. This will create a new menu option in the Arduino IDE allowing you to override the build options of the "Micro" target.
menu.custom=Build Overrides
micro.menu.custom.micro=Arduino/Genuino Micro
micro.menu.custom.micro.build.usb_product="Arduino Micro"
micro.menu.custom.micro_1=xcontrol
micro.menu.custom.micro_1.build.usb_product="X-Control-1"
micro.menu.custom.micro_1.build.vid=0x1209
micro.menu.custom.micro_1.build.pid=0x0C01
- Note that Windows caches metadata for each USB PID/VID. If it becomes necessary to update the metadata for this device, you may need to use regedit to delete the following entries for the VIDxxxx and PIDyyyy values used:
HKEY_USERS\S-1-5-21-4161308201-3297097293-619256985-1002\System\CurrentControlSet\Control\MediaProperties\PrivateProperties\Joystick\OEM\VID_xxxx&PID_yyyy
HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\USB\VID_xxxx&PID_xxxx
