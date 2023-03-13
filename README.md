# Midi Polyphony Channel Splitter
An arduino program to split polyphonic midi notes into separate channels.

## The Idea  
Many groovebox-type synths, such as the elektron model cycles and elektron model samples, are monophonic. 
This means you can only play or record one note at a time. 
It is possible to create overlapping notes, but it requires playing the notes on two separate tracks simultaneously. 
Unfortunately, this is impossible on most midi keyboards. That's where this project comes in!  
  
Whenever you play a chord on a midi keyboard, it sends each note pressed on a single midi channel. 
However, if we want each of these notes to play simultaneously on a multi-track, monophonic groovebox, 
we need to split these notes up to play on separate midi channels. 
This is what the arduino does. It reads in midi notes from an input midi port, then assigns each note to a different midi channel. 
Once these output channels are determined, the midi notes are sent on their respective channels on an output midi port.  
  
**For example:**

Let's assume that the notes C, E, and G are played from a midi keyboard on midi channel 7. 
If our groovebox is listening for midi notes on channels 1, 2, and 3, then we could play the C note on channel 1, the E note on channel 2, and the G note on channel 3. 
And now, our monophonic groovebox has played a C chord!  

## Hardware
For my implementation, I used a Teensy LC as my microcontroller. 
In addition, you will need 2 midi ports, a 6n138 octocoupler, 4 pushbuttons, a 16x2 LCD, a 10k potentiometer, and an assortment of resistors and capacitors. 
The circuit follows the schematic below:  
  
<img src="https://github.com/shutch42/Midi-Polyphony-Channel-Splitter/blob/main/img/schematic.png" width="50%">
  
I wired up the project on some protoboard, but in the future I might try printing out a more official PCB. 
In the meantime, here's an image of the completed product:  
  
<img src="https://github.com/shutch42/Midi-Polyphony-Channel-Splitter/blob/main/img/completed_project.jpg" width="50%">

## Software
All code for the project is in the [controller_sketch](https://github.com/shutch42/Midi-Polyphony-Channel-Splitter/tree/main/controller_sketch) folder. 
Once you open the file in arduino IDE, it should be ready to upload as long you are using the same pins as the schematic above.  
  
The program works using the PJRC MIDI library for processing midi notes. It also uses LiquidCrystal to handle the LCD, and the PJRC Bounce library to handle button presses. 
The program is surprisingly not super complex. The only difficult part is determining which channels are available to play notes on. 
I did this using an array to track the notes being played on each channel. When a new note is played, I do a linear search on this array for available midi channels. 
If no channel is available, and the note is not currently being played, then the note will just play on the most recent channel.

## Usage
The LCD will display the program settings. 
You can set the number of notes/channels that you wish to use for polyphony, as well as the input midi channel, and output midi channels. 
From there, it should just be plug-and-play. 
Just make sure that your groovebox's tracks match up with the output midi channels, and your midi keyboard is playing on the input midi channel.
