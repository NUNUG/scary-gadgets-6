# Scary Gadgets 6 (October 2018)
This is the repository for all the gadgets presented for the Oct. 2018 meeting of the Northern Utah .NET User's group.
Please consider joining us at [http://nunug.org](http://nunug.org) or on [our NUNUG Slack](http://nunug.slack.com).

## Gadgets:
### Jump Scare
There were two jointly created jump scare machines. Both are powered by pneumatics and microcontrollers, but they are triggered differently. Dan's machine uses an arduino and a distance sensor to detect when someone walks by. Nate's uses a remote control that he can trigger from inside his house as people approach. Both could be combined, or you can come up with a unique way to trigger the prop.

The frames for each were made of steel and the props ride on aluminum attached to the steel frame. A pneumatic cylinder then provides the thrust for the prop. A 25mm bore cylinder at 20psi was used. With the cylinder attached 3.75 times as far from the fulcrum as the payload (in our build the cylinder was 3.75" away from the fulcrum and the payload was 2') should provide about 15 lbs of force at the payload side of the arm. That's 30 ft*lbs of torque! Our first run was at 50 psi, and it delivered almost 250 lbs at the payload arm. Make sure you're careful with who's standing in front of the machine, and don't overpower it. We added a speed reducer to the line after testing to make sure the prop doesn't rip itself apart, and also to protect people.

#### Dan's
This jump scare machine used an arduino and a VL53L1X distance sensor to determine the subject's distance from the machine. The distance is calculated in mm, and returned every time through the main loop. A threshold is set (1.5m), and the two numbers compared. If the read number is less than the threshold but greater than a safe distance, the prop triggers. After it has been triggered, it waits 3 seconds and then resets. There is then a 60 second cooldown before it will fire again. The cooldown is there to make sure it doesn't accidentally jump twice for the same group of trick-or-treaters.

#### Nate's
This jump scare machine uses a remote control to activate the machine.

### Tron Costume
The tron costume is made of two pieces -- the disc, and the suit. The disc was 3D-printed using a Prusa i3 MK3 and PETG plastic. The PETG is good because it should offer more outside durability along with some flexibility. PLA or ABS could also be used, just pick the plastic that works with your printer and situation. The 3D model can be downloaded [on Thingiverse](https://www.thingiverse.com/thing:1032216).

The interior of the disc was then lined with two strips of [NeoPixel LEDs](https://www.adafruit.com/category/168). The LEDs are connected to a 9v battery through a voltage regulator (they're 5V devices) and controlled with an ATTiny85 in the form of the Adafruit Trinket 5V. A small switch was soldered between the battery and voltage regulator to allow the user to turn it on and off at will.

The suit was purchased from Walmart, but it was augmented with more LEDs. In this case several strips of 5050 SMD LEDs were sewed onto a simple white shirt and pants in the same pattern as the Tron costume suit, and then worn under the suit. It was then attached to the costume (to line it up) using fabric tape. The 4 9V batteries (one for each limb) were attached at each hip. Since the batteries are easily accessible, no switches were used.

### Prop Dropper
This is Phil's overnight creation.  A monster on a string skulks approaching trick-or treaters.  When they close the gap and reach the target distance of 40 cm, the ultrasonic range finder ("ping sensor") triggers the Prop Dropper to turn its drum upside down, dumping the spool of string and its spooky appendage (a spider or bat maybe) in the victim's path.  As the screams hopefully abate, the drum is again turned upward and begins to wind up the string for the next cycle.  

There are two 3D-printed parts and two motors.  One is a servo motor and the other is a continuous rotation servo motor.  They are powered by a standard Arduino of any kind and the sensor is a 4-pin (not 3-pin) ultrasonic range finder, such as an HC-SR04.  An Arduino nano can be purchased for $2.50 (free shipping) on Ebay, the sensor for $1 (free shipping) on ebay, the motors for about $4 (for the MG996 servo) and $10 (for the MG995 continuous rotation servo).  I also use a battery box to power the motors.  You could use batteries or a wall wart.  The 3D printed parts are all optional if you want to use some cardboard, a wire nut and some hot glue.  I just wanted mine to be a little nicer.
