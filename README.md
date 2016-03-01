# Introduction
Watch Your Tone is a speech simulator that displays a "talking" smiley face when speech/sound is detected through a microphone. 
Th color and type of face will depend on the tone at which the person is speaking at. Higher pitchdsound will display a talking green smile, medium pitched sound will display an orange smile and lower pitched sound will display a red smile. The tone is determined by the volume at which the sound is at. This is made with the assumption that the higher the volume the higher the tone. 
#Specifications
This project was developed on Atemel Studio 6.2 and programed onto the ATMega1284 microcontroller using an Olimex AVR-ISP-MK2 programmer.
Software
I used the io.h and interrupt.h files provided in lab. FOur different state machines were created for each of the different faces which were displayed on the 8x8 LED matrix using shift registers. 
Hardware
Microcontroller used was the ATMega1284. This microcontroller along with all the hardware were placed on the breadboard.
#Difficulties/Bugs
- Using a component I've never programmed with before(microphone), created some complications with the production of this project.
- Couldn't read in frequency which was the intended method for detecting tone. 
- Microphone would either pick up my voice easily along with too much background noise or not pick up background noise, but i would have to whistle or talk loud for it to detect sound properly.  
These problems would've been solved if I had more experience with using an electret microphone. I would've changed tone detection to be based off of frequency, and added the ability to click a button which saves where you want your neutral frequency/tone to be at. 

# Bugs
- When doing a long whistle or sound for a long time, sometimes the LED matrix will fluctuate between states then settle back to the state it should be in. This can be fixed by adjusting the volume at which each color is triggered at. 
- Difficult to get to orange face(medium tone) sometimes. By spreading out the volume required for each color more, it should be easier to get to the neutral face. 
