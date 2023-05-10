# CPE-301-Final-Project-Beck-Whitaker-West
Repository for CPE 301 Swamp Cooler Final Project

Team Members:
- Brandon Beck
- Stephen West
- Aaron Whitaker

Overview:

  We have created a functioning version of a swamp cooler using the provided guidelines and requisites. The design choices that we chose to incorporate into the system include the water sensor for water level monitoring, the stepper motor to act as a vent, a vent control that utilizes a potentiometer rather than a button, the LCD display that displays the temperature and humidity of the system, the temperature and humidity readings that come from the DHT11 sensor, a real time clock that reports each time the fan motor turns on or off, and small fan blades attached to a dc motor that act as the fan for the cooling part of the system. 
	Some constraints to our system include the inability to effectively change the temperature in a drastic manner that would trigger the “running” state that starts the fan. In order to compensate for this, we temporarily changed the value of the minimum operating temperature in a manner that would automatically trigger the “running” state in order to test that it works. We also observed that our system experienced issues with the power distribution of the system. Despite having the additional five volt power source, our DC motor struggles to run when the system enters the running state, and the blue and red LEDs are dimmer than the green and yellow LEDs, but they still function and turn on when appropriate. We could not find a method that would distribute the power equally to all systems.
