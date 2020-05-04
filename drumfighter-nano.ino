//--------------------------------------------------------------------//
//                  -----  DrumFighter Nano  -----                    //
//             A palm-sized MIDI drum controller using                //
//                 arcade push-buttons in a 4x4 grid.                 //
//                       Written for Teensy 2.0                       //
//            or other Arduino compatible micro-controller            //
//                   with MIDIUSB library v1.0.4                      //
//              Copyright (C) 2019 - Michael Koopman                  //
//           KOOP Instruments (koopinstruments@gmail.com)             //
//--------------------------------------------------------------------//
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//--------------------------------------------------------------------//
//
// GENERAL MIDI PERCUSSION MAP
//------------------------------------------------
// 27 Laser                 58 Vibra Slap
// 28 Whip                  59 Ride Cymbal 2
// 29 Scratch Push          60 High Bongo
// 30 Scratch Pull          61 Low Bongo
// 31 Stick Click           62 Mute High Conga
// 32 Metronome Click       63 Open High Conga
// 34 Metronome Bell        64 Low Conga
// 35 Acoustic Bass Drum    65 High Timbale
// 36 Bass Drum 1           66 Low Timbale
// 37 Side Stick/Rimshot    67 High Agogô
// 38 Acoustic Snare        68 Low Agogô
// 39 Hand Clap             69 Cabasa
// 40 Electric Snare        70 Maracas
// 41 Low Floor Tom         71 Short Whistle
// 42 Closed Hi-hat         72 Long Whistle
// 43 High Floor Tom        73 Short Güiro
// 44 Pedal Hi-hat          74 Long Güiro
// 45 Low Tom               75 Claves
// 46 Open Hi-hat           76 High Wood Block
// 47 Low-Mid Tom           77 Low Wood Block
// 48 Hi-Mid Tom            78 Mute Cuíca
// 49 Crash Cymbal 1        79 Open Cuíca
// 50 High Tom              80 Mute Triangle
// 51 Ride Cymbal 1         81 Open Triangle
// 52 Chinese Cymbal        82 Shaker
// 53 Ride Bell             83 Sleigh Bell
// 54 Tambourine            84 Bell Tree
// 55 Splash Cymbal         85 Castanets
// 56 Cowbell               86 Surdu Dead Stroke
// 57 Crash Cymbal 2        87 Surdu
//
// FluidSynth (FluidR3_GM.sf2) Percussion Program Numbers
// 0 Standard
// 1 Standard 1
// 2 Standard 2
// 3 Standard 3
// 4 Standard 4
// 5 Standard 5
// 6 Standard 6
// 7 Standard 7
// 8 Room
// 9 Room 1
// 10 Room 2
// 11 Room 3
// 12 Room 4
// 13 Room 5
// 14 Room 6
// 15 Room 7
// 16 Power
// 17 Power 1
// 18 Power 2
// 19 Power 3
// 24 Electronic
// 25 TR-808
// 32 Jazz
// 33 Jazz 1
// 34 Jazz 2
// 35 Jazz 3
// 36 Jazz 4
// 40 Brush
// 41 Brush 1
// 42 Brush 2
// 48 Orchestra Kit
//
// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// START OF PROGRAM

#include "MIDIUSB.h"

const byte NUM_TOTAL_BUTTONS  = 16;             // The total number of nodes in the button matrix
const byte NUM_TOTAL_PROGRAMS = 31;             // The total number of MIDI drum programs in the FluidSynth SoundFont

// Note button matrix pins
const byte columns[]   = {0, 1, 2, 3};          // Assign pins to the columns in order of left to right.
const byte rows[]      = {4, 5, 6, 7};          // Assign pins to the rows in order of top to bottom.
const byte columnCount = sizeof(columns);       // Count the number of columns.
const byte rowCount    = sizeof(rows);          // Count the number of rows.

const unsigned long debounceDelayTime = 1;                  // Debounce delay time in milliseconds
unsigned long previousActivationTime[NUM_TOTAL_BUTTONS];    // Previous button activation time for debounce check
unsigned int rawButtons;                                    // Live button states
unsigned int pendingButtons;                                // Pre-debounce candidate states
unsigned int activeButtons;                                 // Post-debounce state of the button deck
unsigned int previousActiveButtons;                         // Previous value for comparison

unsigned int lockButtons;                                   // Button input locking variable

const byte midiChannel = 9;                                 // Default MIDI percussion channel 9 (channel 10 if counting from 1 instead of 0)
byte noteValues[NUM_TOTAL_BUTTONS];                         // User updatable array containing note values of the current layout selection
byte velocityValues[NUM_TOTAL_BUTTONS];                     // User updatable array containing note velocities of the current layout selection

int8_t velocityOffset;                                      // User adjustable velocity offset
int8_t velocityRNG;                                         // Play with introducing a minor amount randomness into velocity levels so it doesn't sound so robotic

byte padLayout;                                             // Current pad layout selection
byte metaState;                                             // Track if meta state is active
byte previousMetaState;                                     // Previous value for comparison
byte midiProgram;                                           // Store MIDI program selection

byte lastVelocityUpState;                                   // State variable for the expression up key
byte lastVelocityDownState;                                 // State variable for the expression down key
byte lastProgramUpState;                                    // State variable for the program up key
byte lastProgramDownState;                                  // State variable for the program down key


// FluidSynth Percussion Program Patch Layout
const byte programMap[NUM_TOTAL_PROGRAMS] = {
 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
24, 25, 32, 33, 34, 35, 36, 40, 41, 42,
48
};


// Pad Base Velocity Levels
    const byte standardVelocity[NUM_TOTAL_BUTTONS] = {
     95, 95, 95, 95,
     95, 95,103, 95,
     95, 95,103,103,
     95, 95,103, 95
    };
    const byte ghostNotesVelocity[NUM_TOTAL_BUTTONS] = {
     95, 95, 95, 95,
     95, 95,103, 95,
     47, 95,103, 55,
     95, 95,103, 95
    };


// Pad Note Layouts
    const byte standardKit[NUM_TOTAL_BUTTONS] = {
     43, 45, 48, 49,
     42, 46, 42, 51,
     37, 38, 38, 37,
     52, 36, 36, 53
    };
    const byte alternateKit[NUM_TOTAL_BUTTONS] = {
     41, 47, 50, 57,
     42, 46, 42, 59,
     37, 40, 40, 37,
     54, 35, 35, 53
    };
    const byte standardGhostNotesKit[NUM_TOTAL_BUTTONS] = {
     43, 45, 48, 49,
     42, 46, 42, 51,
     38, 38, 38, 38,
     52, 36, 36, 53
    };
    const byte alternateGhostNotesKit[NUM_TOTAL_BUTTONS] = {
     41, 47, 50, 57,
     42, 46, 42, 59,
     40, 40, 40, 40,
     54, 35, 35, 53
    };
    const byte jazzKit[NUM_TOTAL_BUTTONS] = {
     43, 45, 40, 49,
     42, 46, 42, 51,
     37, 39, 39, 38,
     44, 36, 36, 53
    };
    const byte alternateJazzKit[NUM_TOTAL_BUTTONS] = {
     41, 47, 40, 57,
     42, 46, 42, 59,
     37, 39, 39, 38,
     44, 35, 35, 53
    };
    const byte drumlineKit[NUM_TOTAL_BUTTONS] = {
     43, 45, 48, 49,
     37, 46, 42, 37,
     38, 38, 38, 38,
     57, 36, 36, 41
    };
    const byte phaseShiftKit[NUM_TOTAL_BUTTONS] = {
     48, 45, 43, 49,
     42, 46, 42, 51,
     38, 38, 38, 38,
     44, 36, 36, 23
    };


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// START SETUP SECTION
void setup()
{
    // Set pinModes for the button matrix.
    for (byte i=0; i<columnCount; i++)                      // For each column pin...
    {
        pinMode(columns[i], INPUT_PULLUP);                  // set the pinMode to INPUT_PULLUP (+5V / HIGH).
    }
    for (byte i=0; i<rowCount; i++)                         // For each row pin...
    {
        pinMode(rows[i], INPUT);                            // Set the pinMode to INPUT (0V / LOW).
    }

    for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)             // Initialize the noteValues array with the Standard kit layout as default
    {
        noteValues[i] = standardKit[i];
    }
    for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)             // Initialize the velocityValues array with the Standard kit layout as default
    {
        velocityValues[i] = standardVelocity[i];
    }

}
// END SETUP SECTION
// ------------------------------------------------------------------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// START LOOP SECTION
void loop()
{
    // Print hardware diagnostic information to the serial console
    // printDiagnostics();

    // Read and store the digital button states
    readDigitalButtons();

    // Run the pad layout selection function
    runPadLayout();

    // Run the velocity adjustment function
    runVelocityAdjustment();

    // Run the program selection function
    runProgramSelect();

    // Send notes to the MIDI bus
    playNotes();

    // Reset input locking variables 
    for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
    {
        if (bitRead(activeButtons, i) == 0)
        {
            bitWrite(lockButtons, i, 0);
        }
    }
}
// END LOOP SECTION
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// START FUNCTIONS SECTION

void printDiagnostics()
{
    Serial.print("Buttons: ");
    for (int myButton = 0; myButton < NUM_TOTAL_BUTTONS; myButton++)
    {
        int myValue = bitRead(activeButtons, myButton);
        Serial.print(myValue);Serial.print(" ");
    }
    Serial.println();
    delay(10);              // Delay a bit to stabalize the serial monitor stream
}


void readDigitalButtons()
{
    for (byte columnIndex=0; columnIndex < columnCount; columnIndex++)  // Iterate through each of the column pins.
    {
        byte currentColumn = columns[columnIndex];                      // Hold the currently selected column pin in a variable.
        pinMode(currentColumn, OUTPUT);                                 // Set that column pin to OUTPUT mode and...
        digitalWrite(currentColumn, LOW);                               // set the pin state to LOW turning it into a temporary ground.
        for (byte rowIndex=0; rowIndex < rowCount; rowIndex++)          // Now iterate through each of the row pins that are connected to the current column pin.
        {
            byte currentRow = rows[rowIndex];                           // Hold the currently selected row pin in a variable.
            pinMode(currentRow, INPUT_PULLUP);                          // Set that row pin to INPUT_PULLUP mode (+5V / HIGH).
            int buttonNumber = columnIndex + (rowIndex * columnCount);  // Assign this location in the matrix a unique number.
            delayMicroseconds(50);                                      // Slow down to give the pin modes time to change state, and eliminate ghost inputs

            // Button Matrix
            //   0-----1-----2-----3
            //   |     |     |     |
            //   4-----5-----6-----7
            //   |     |     |     |
            //   8-----9----10----11
            //   |     |     |     |
            //  12----13----14----15

            if ((!digitalRead(currentRow)) == HIGH)                     // If the button is active
            {
                bitWrite(rawButtons, buttonNumber, 1);                  // Set the bit in the bit position of the button number to a 1.
            }
            else
            {
                bitWrite(rawButtons, buttonNumber, 0);                  // Set the bit to a 0.
            }

            if (bitRead(rawButtons, buttonNumber) != bitRead(pendingButtons, buttonNumber))     // If a difference between raw and pending button is discovered
            {
                previousActivationTime[buttonNumber] = millis();                                // Reset the debounce clock for this button
                bitWrite(pendingButtons, buttonNumber, bitRead(rawButtons, buttonNumber));      // Write the button state to the pendingButtons variable
            }

            if ((millis() - previousActivationTime[buttonNumber]) > debounceDelayTime && bitRead(activeButtons, buttonNumber) != bitRead(pendingButtons, buttonNumber)) // If button passes debounce
            {
                bitWrite(activeButtons, buttonNumber, bitRead(pendingButtons, buttonNumber));   // Write the button state to the activeButtons variable to be sent to the note hopper
            }
            pinMode(currentRow, INPUT);                                 // Set the selected row pin back to INPUT mode (0V / LOW).
        }
        pinMode(currentColumn, INPUT);                                  // Set the selected column pin back to INPUT mode (0V / LOW) and move onto the next column pin.
    }

    if (bitRead(activeButtons, 0) == HIGH && bitRead(activeButtons, 4) == HIGH && bitRead(activeButtons, 8) == HIGH && bitRead(activeButtons, 12) == HIGH)  // If the left-most 4 buttons are held
    {
        metaState = HIGH;                                               // Enable the meta key state
    }
    else
    {
        metaState = LOW;                                                // Otherwise, disable it
    }
}


void runPadLayout()
{
    if (metaState == HIGH)
    {
        // Standard Drum Kit
        // ● ● ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        if (bitRead(activeButtons, 1) == HIGH && bitRead(lockButtons, 1) == 0 )
        {
            bitWrite(lockButtons, 1, 1);
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                noteValues[i] = standardKit[i];
            }
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                velocityValues[i] = standardVelocity[i];
            }
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
        // Alternate Drum Kit Notes
        // ● ○ ● ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        if (bitRead(activeButtons, 2) == HIGH && bitRead(lockButtons, 2) == 0 )
        {
            bitWrite(lockButtons, 2, 1);
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                noteValues[i] = alternateKit[i];
            }
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                velocityValues[i] = standardVelocity[i];
            }
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
        // Standard w/ Ghost Notes
        // ● ○ ○ ○
        // ● ● ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        if (bitRead(activeButtons, 5) == HIGH && bitRead(lockButtons, 5) == 0 )
        {
            bitWrite(lockButtons, 5, 1);
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                noteValues[i] = standardGhostNotesKit[i];
            }
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                velocityValues[i] = ghostNotesVelocity[i];
            }
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
        // Alternate w/ Ghost Notes
        // ● ○ ○ ○
        // ● ○ ● ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        if (bitRead(activeButtons, 6) == HIGH && bitRead(lockButtons, 6) == 0 )
        {
            bitWrite(lockButtons, 6, 1);
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                noteValues[i] = alternateGhostNotesKit[i];
            }
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                velocityValues[i] = ghostNotesVelocity[i];
            }
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
        // Jazz Kit Notes
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ● ○ ○
        // ● ○ ○ ○
        if (bitRead(activeButtons, 9) == HIGH && bitRead(lockButtons, 9) == 0 )
        {
            bitWrite(lockButtons, 9, 1);
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                noteValues[i] = jazzKit[i];
            }
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                velocityValues[i] = standardVelocity[i];
            }
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
        // Alternate Jazz Kit Notes
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ● ○
        // ● ○ ○ ○
        if (bitRead(activeButtons, 10) == HIGH && bitRead(lockButtons, 10) == 0 )
        {
            bitWrite(lockButtons, 10, 1);
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                noteValues[i] = alternateJazzKit[i];
            }
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                velocityValues[i] = standardVelocity[i];
            }
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
        // Drumline Kit Notes
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ● ○ ○
        if (bitRead(activeButtons, 13) == HIGH && bitRead(lockButtons, 13) == 0 )
        {
            bitWrite(lockButtons, 13, 1);
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                noteValues[i] = drumlineKit[i];
            }
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                velocityValues[i] = ghostNotesVelocity[i];
            }
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
        // PhaseShift Kit Notes
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ● ○
        if (bitRead(activeButtons, 14) == HIGH && bitRead(lockButtons, 14) == 0 )
        {
            bitWrite(lockButtons, 14, 1);
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                noteValues[i] = phaseShiftKit[i];
            }
            for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
            {
                velocityValues[i] = ghostNotesVelocity[i];
            }
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
    }
}

void runVelocityAdjustment()
{
    if (metaState == HIGH)
    {
        // Velocity Up
        // ● ○ ○ ●
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        if (bitRead(activeButtons, 3) == 1 && bitRead(lockButtons, 3) == 0 && velocityOffset < 16)
        {
            bitWrite(lockButtons, 3, 1);
            velocityOffset = velocityOffset + 8;
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
        // Velocity Down
        // ● ○ ○ ○
        // ● ○ ○ ●
        // ● ○ ○ ○
        // ● ○ ○ ○
        if (bitRead(activeButtons, 7) == HIGH && bitRead(lockButtons, 7) == 0 && velocityOffset > -40)
        {
            bitWrite(lockButtons, 7, 1);
            velocityOffset = velocityOffset - 8;
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
    }
}

void runProgramSelect()
{
    if (metaState == HIGH)
    {
        // Program Up
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ●
        // ● ○ ○ ○
        if (bitRead(activeButtons, 11) == HIGH && bitRead(lockButtons, 11) == 0 && midiProgram < NUM_TOTAL_PROGRAMS)
        {
            bitWrite(lockButtons, 11, 1);
            midiProgram = midiProgram + 1;
            programChange(midiChannel, programMap[midiProgram]);
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
        // Program Down
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ○
        // ● ○ ○ ●
        if (bitRead(activeButtons, 15) == HIGH && bitRead(lockButtons, 15) == 0 && midiProgram > 0)
        {
            bitWrite(lockButtons, 15, 1);
            midiProgram = midiProgram - 1;
            programChange(midiChannel, programMap[midiProgram]);
            noteOn(midiChannel, 38, 95 + velocityOffset + velocityRNG);
            noteOff(midiChannel, 38, 0);
        }
    }
}

//  Play Notes
//  Read the states for each bit in the value, compare them to the previous
//  values, and send note on/off commands for each button where appropriate.
void playNotes()
{
    velocityRNG = random(-3, 3);                                                            // Set a random value range from -3 to +3 to be applied to note velocity

    if (metaState == LOW)
    {
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)                                         // For all buttons in the matrix
        {
            if (bitRead(activeButtons, i) != bitRead(previousActiveButtons, i))             // If the button has changed since the last program loop
            {
                if (bitRead(activeButtons, i))                                              // If the button is active
                {
                    noteOn(midiChannel, noteValues[i], velocityValues[i] + velocityOffset + velocityRNG);   // Send a noteOn for this note
                    bitWrite(previousActiveButtons, i , 1);                                 // Write to the "previous" value for comparison on next loop
                }
                else                                                                        // If the button is inactive
                {
                    noteOff(midiChannel, noteValues[i], 0);                                 // Send a noteOff for this note
                    bitWrite(previousActiveButtons, i , 0);                                 // Write to the "previous" value for comparison on next loop
                }
            }
        }
    }
}


// MIDI PACKET FUNCTIONS
//----------------------

// Send MIDI Note On
// 1st byte = Event type (0x09 = note on, 0x08 = note off).
// 2nd byte = Event type bitwise ORed with MIDI channel.
// 3rd byte = MIDI note number.
// 4th byte = Velocity (7-bit range 0-127)
void noteOn(byte channel, byte pitch, byte velocity)
{
    channel = 0x90 | channel;                                                   // Bitwise OR outside of the struct to prevent compiler warnings
    midiEventPacket_t noteOn = {0x09, channel, pitch, velocity};                // Build a struct containing all of our information in a single packet
    MidiUSB.sendMIDI(noteOn);                                                   // Send packet to the MIDI USB bus
    Serial1.write(0x90 | channel);                                              // Send event type/channel to the MIDI serial bus
    Serial1.write(pitch);                                                       // Send note number to the MIDI serial bus
    Serial1.write(velocity);                                                    // Send velocity value to the MIDI serial bus
}

// Send MIDI Note Off
// 1st byte = Event type (0x09 = note on, 0x08 = note off).
// 2nd byte = Event type bitwise ORed with MIDI channel.
// 3rd byte = MIDI note number.
// 4th byte = Velocity (7-bit range 0-127)
void noteOff(byte channel, byte pitch, byte velocity)
{
    channel = 0x80 | channel;                                                   // Bitwise OR outside of the struct to prevent compiler warnings
    midiEventPacket_t noteOff = {0x08, channel, pitch, velocity};               // Build a struct containing all of our information in a single packet
    MidiUSB.sendMIDI(noteOff);                                                  // Send packet to the MIDI USB bus
    Serial1.write(0x80 | channel);                                              // Send event type/channel to the MIDI serial bus
    Serial1.write(pitch);                                                       // Send note number to the MIDI serial bus
    Serial1.write(velocity);                                                    // Send velocity value to the MIDI serial bus
}

// Control Change
// 1st byte = Event type (0x0B = Control Change).
// 2nd byte = Event type bitwise ORed with MIDI channel.
// 3rd byte = MIDI CC number (7-bit range 0-127).
// 4th byte = Control value (7-bit range 0-127).
void controlChange(byte channel, byte control, byte value)
{
    channel = 0xB0 | channel;                                                   // Bitwise OR outside of the struct to prevent compiler warnings
    midiEventPacket_t event = {0x0B, channel, control, value};                  // Build a struct containing all of our information in a single packet
    MidiUSB.sendMIDI(event);                                                    // Send packet to the MIDI USB bus
    Serial1.write(0xB0 | channel);                                              // Send event type/channel to the MIDI serial bus
    Serial1.write(control);                                                     // Send control change number to the MIDI serial bus
    Serial1.write(value);                                                       // Send control chnage value to the MIDI serial bus
}

// Program Change
// 1st byte = Event type (0x0C = Program Change).
// 2nd byte = Event type bitwise ORed with MIDI channel.
// 3rd byte = Program value (7-bit range 0-127).
void programChange(byte channel, byte value)
{
    channel = 0xC0 | channel;                                                   // Bitwise OR outside of the struct to prevent compiler warnings
    midiEventPacket_t event = {0x0C, channel, value};                           // Build a struct containing all of our information in a single packet
    MidiUSB.sendMIDI(event);                                                    // Send packet to the MIDI USB bus
    Serial1.write(0xC0 | channel);                                              // Send event type/channel to the MIDI serial bus
    Serial1.write(value);                                                       // Send program change value to the MIDI serial bus
}

// Pitch Bend
// (14 bit value 0-16363, neutral position = 8192)
// 1st byte = Event type (0x0E = Pitch bend change).
// 2nd byte = Event type bitwise ORed with MIDI channel.
// 3rd byte = The 7 least significant bits of the value.
// 4th byte = The 7 most significant bits of the value.
void pitchBendChange(byte channel, int value)   //byte lowValue, byte highValue)
{
    channel = 0xE0 | channel;                                                   // Bitwise OR outside of the struct to prevent compiler warnings
    byte lowValue = value & 0x7F;                                               // Capture the 7 least significant bits of the value
    byte highValue = value >> 7;                                                // Capture the 7 most significant bits of the value
    midiEventPacket_t bendEvent = {0x0E, channel, lowValue, highValue};         // Build a struct containing all of our information in a single packet
    MidiUSB.sendMIDI(bendEvent);                                                // Send packet to the MIDI USB bus
    Serial1.write(0xE0 | channel);                                              // Send event type/channel to the MIDI serial bus
    Serial1.write(lowValue);                                                    // Send pitch bend low byte to the MIDI serial bus
    Serial1.write(highValue);                                                   // Send pitch bend high byte to the MIDI serial bus
}

// END FUNCTIONS SECTION
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

// END OF PROGRAM
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

