//--------------------------------------------------------------------//
//                  -----  DrumFighter Nano  -----                    //
//             A palm-sized MIDI drum controller using                //
//                 arcade push-buttons in a 4x4 grid.                 //
//          Written for Adafruit ItsyBitsy 32u4 - (5V 16MHz)          //
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
// 27 Laser
// 28 Whip
// 29 Scratch Push
// 30 Scratch Pull
// 31 Stick Click
// 32 Metronome Click
// 34 Metronome Bell
// 35 Acoustic Bass Drum
// 36 Bass Drum 1
// 37 Side Stick/Rimshot
// 38 Acoustic Snare
// 39 Hand Clap
// 40 Electric Snare
// 41 Low Floor Tom
// 42 Closed Hi-hat
// 43 High Floor Tom
// 44 Pedal Hi-hat
// 45 Low Tom
// 46 Open Hi-hat
// 47 Low-Mid Tom
// 48 Hi-Mid Tom
// 49 Crash Cymbal 1
// 50 High Tom
// 51 Ride Cymbal 1
// 52 Chinese Cymbal
// 53 Ride Bell
// 54 Tambourine
// 55 Splash Cymbal
// 56 Cowbell
// 57 Crash Cymbal 2
// 58 Vibra Slap
// 59 Ride Cymbal 2
// 60 High Bongo
// 61 Low Bongo
// 62 Mute High Conga
// 63 Open High Conga
// 64 Low Conga
// 65 High Timbale
// 66 Low Timbale
// 67 High Agogô
// 68 Low Agogô
// 69 Cabasa
// 70 Maracas
// 71 Short Whistle
// 72 Long Whistle
// 73 Short Güiro
// 74 Long Güiro
// 75 Claves
// 76 High Wood Block
// 77 Low Wood Block
// 78 Mute Cuíca
// 79 Open Cuíca
// 80 Mute Triangle
// 81 Open Triangle
// 82 Shaker
// 83 Sleigh Bell
// 84 Bell Tree
// 85 Castanets
// 86 Surdu Dead Stroke
// 87 Surdu
//
// GarageBand iOS Drum Notes
// 31 +Pedal Hi-Hat
// 32 +Electric Snare
// 33 +Pedal Hi-Hat
// 34 +Acoustic Snare
// 35 Acoustic Bass Drum
// 36 Bass Drum 1
// 37 Side Stick
// 38 Acoustic Snare
// 39 Hand Clap
// 40 Electric Snare
// 41 Lo Floor Tom
// 42 Closed Hi Hat
// 43 Hi Floor Tom
// 44 Pedal Hi Hat
// 45 Lo Tom Tom
// 46 Open Hi Hat
// 47 Low -Mid Tom Tom
// 48 Hi Mid Tom Tom
// 49 Crash Cymbal 1
// 50 Hi Tom Tom
// 51 Ride Cymbal 1
// 52 +Ride Cymbal 1 | Chinese Cymbal
// 53 Ride Bell
// 54 Tambourine
// 55
// 56 N/A
// 57 Crash Cymbal 2
// 58 N/A
// 59 +Ride Cymbal 1
// 62 Mute Hi Conga
// 63 Open Hi Conga
// 64 Low Conga
// 65 Hi Timbale
// 66 Lo Timbale
// 70 Maracas
//
// FLUIDSYNTH FluidR3_GM.sf2 PERCUSSION PROGRAMS
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
// Brush Kit is usually the same as the Standard Drum Kit with the following three exceptions:
// Note #38 is a Brush Tap instead of a Snare Drum.
// Note #39 is a Brush Slap instead of a Hand Clap.
// Note #40 is a Brush Swirl instead of an Electric Snare Drum.
//
// PAD BUTTON BINARY VALUES
//     1      2      4      8
//    16     32     64    128
//   256    512   1024   2048
//  4096   8192  16384  32768

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// START OF PROGRAM


#include "MIDIUSB.h"

const byte NUM_TOTAL_BUTTONS = 16;      // Total number of buttons in the matrix
const byte NUM_TOTAL_PROGRAMS = 31;     // Total number of MIDI programs

// Note button matrix pins
const byte columns[] =   {11, 10, 9, 7};    // Assign pins to the columns in order of left to right.
const byte rows[] =      {A1, A2, A3, A4};  // Assign pins to the rows in order of top to bottom.
const byte columnCount = sizeof(columns);   // Count the number of columns.
const byte rowCount =    sizeof(rows);      // Count the number of rows.

// FluidSynth Percussion Program Patch Layout
const byte programMap[NUM_TOTAL_PROGRAMS] = {   // FluidSynth SoundFont percussion channel program numbers are not totally linear, and jump around a bit
 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 24, 25,
32, 33, 34, 35, 36, 40, 41, 42, 48
};

// Pad Layouts
// Standard Drum Kit Notes
const byte Standard[NUM_TOTAL_BUTTONS] = {
 43, 45, 48, 49,
 42, 46, 42, 51,
 37, 38, 38, 37,
 52, 36, 36, 53
};
// Alternate Drum Kit Notes
const byte Alternate[NUM_TOTAL_BUTTONS] = {
 41, 47, 50, 57,
 42, 46, 42, 59,
 37, 40, 40, 37,
 54, 35, 35, 53

};
// Jazz Kit Notes
const byte Jazz[NUM_TOTAL_BUTTONS] = {
 43, 45, 40, 49,
 42, 46, 42, 51,
 37, 39, 39, 38,
 44, 36, 36, 53
};


// Standard w/ Ghost Notes
const byte StandardGhostNotes[NUM_TOTAL_BUTTONS] = {
 43, 45, 48, 49,
 42, 46, 42, 51,
 38, 38, 38, 38,
 52, 36, 36, 53
};
// Alternate w/ Ghost Notes
const byte AlternateGhostNotes[NUM_TOTAL_BUTTONS] = {
 41, 47, 50, 57,
 42, 46, 42, 59,
 40, 40, 40, 40,
 54, 35, 35, 53
};
// Etnhic Drum Notes
const byte Ethnic[NUM_TOTAL_BUTTONS] = {
 64, 62, 63, 79,
 70, 82, 54, 78,
 77, 61, 60, 76,
 71, 66, 65, 56
};


// Inverted Cymbals/Snares Standard Drum Kit Notes
const byte InvertedStandard[NUM_TOTAL_BUTTONS] = {
 41, 47, 50, 57,
 42, 46, 42, 59,
 37, 38, 38, 37,
 52, 36, 36, 53
};
// Inverted Cynbals/Snares Alternate Drum Kit Notes
const byte InvertedAlternate[NUM_TOTAL_BUTTONS] = {
 43, 45, 48, 49,
 42, 46, 42, 51,
 37, 40, 40, 37,
 54, 35, 35, 53
};
// Alternate Jazz Kit Notes
const byte AlternateJazz[NUM_TOTAL_BUTTONS] = {
 41, 47, 40, 57,
 42, 46, 42, 59,
 37, 39, 39, 38,
 44, 35, 35, 53
};


// Inverted Cymbals/Snares Standard Drum Kit w/ Ghost Notes
const byte InvertedStandardGhostNotes[NUM_TOTAL_BUTTONS] = {
 41, 47, 50, 57,
 42, 46, 42, 59,
 38, 38, 38, 38,
 52, 36, 36, 53
};
// Inverted Cynbals/Snares Alternate Drum Kit w/ Ghost Notes
const byte InvertedAlternateGhostNotes[NUM_TOTAL_BUTTONS] = {
 43, 45, 48, 49,
 42, 46, 42, 51,
 40, 40, 40, 40,
 54, 35, 35, 53
};
// Orchestra Kit Notes
const byte Orchestra[NUM_TOTAL_BUTTONS] = {
 43, 45, 48, 49,
 42, 46, 42, 51,
 25, 38, 38, 25,
 55, 36, 36, 53
};

unsigned long currentTime;                              // Current milliseconds since power on for this program loop
unsigned long previousButtonsTime[NUM_TOTAL_BUTTONS];   // Previous button activation time for debounce check
const byte debounceDelayTime = 50;                      // Debounce delay time in milliseconds
const byte drumsChannel = 9;                            // Default MIDI percussion channel 9 (channel 10 if counting from 1 instead of 0)
byte noteNumbers[NUM_TOTAL_BUTTONS];                    // User updatable array containing note values of the current layout selection
word pressedButtons;                                    // Current binary state of the entire button deck
word previousPressedButtons;                            // Comparison value for pressedButtons
byte padLayout;                                         // Current pad layout selection value
byte metaKeyState;                                      // State variable for the meta key button on the case rear
byte velocity = 95;                                     // Default velocity level
int8_t velocityRNG;                                     // Playing with introducing small randomness into velocity levels so it doesn't sound so robotic
byte expressionLevel = 95;                              // Default MIDI expression level
byte midiProgram;                                       // Store MIDI program selection
byte lastExpressionUpState;                             // State variable for the expression up key (meta + pressedButtons == 8)
byte lastExpressionDownState;                           // State variable for the expression down key (meta + pressedButtons == 128)
byte lastProgramUpState;                                // State variable for the program up key (meta + pressedButtons == 2048)
byte lastProgramDownState;                              // State variable for the program down key (meta + pressedButtons == 32768)

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// START SETUP SECTION
void setup()
{
    // Serial.begin(31250);

    // Meta key on case rear
    pinMode(13, INPUT_PULLUP);

    // Set pinModes for the button matrix.
    for (byte i=0; i<columnCount; i++)      // For each column pin...
    {
        pinMode(columns[i], INPUT_PULLUP);  // set the pinMode to INPUT_PULLUP (+5V / HIGH).
    }
    for (byte i=0; i<rowCount; i++)         // For each row pin...
    {
        pinMode(rows[i], INPUT);            // Set the pinMode to INPUT (0V / LOW).
    }

    for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)     // Initialize the noteNumbers array with the Standard kit layout as default
    {
        noteNumbers[i] = Standard[i];
    }
}
// END SETUP SECTION
// ------------------------------------------------------------------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// START LOOP SECTION
void loop()
{
    currentTime = millis();             // Set current time to a reference variable for this program loop
    metaKeyState = !digitalRead(13);    // Read the button state of the meta key on case rear, and invert it (due to INPUT_PULLUP)
    velocityRNG = random(-3, 3);        // Set a random value range from -3 to +3 to be applied to note velocity
    readButtons();                      // Read the state of the button matrix

    // Pad layouts
    // Row 1
    if (metaKeyState == HIGH && pressedButtons == 1) // Standard
    {
        padLayout = 0;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = Standard[i];
        }
    }
    if (metaKeyState == HIGH && pressedButtons == 2) // Alternate
    {
        padLayout = 1;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = Alternate[i];
        }
    }
    if (metaKeyState == HIGH && pressedButtons == 4) // Jazz
    {
        padLayout = 2;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = Jazz[i];
        }
    }

    // Row 2
    if (metaKeyState == HIGH && pressedButtons == 16) // Standard w/ Ghost Notes
    {
        padLayout = 3;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = StandardGhostNotes[i];
        }
    }
    if (metaKeyState == HIGH && pressedButtons == 32) // Alternate w/ Ghost Notes
    {
        padLayout = 4;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = AlternateGhostNotes[i];
        }
    }
    if (metaKeyState == HIGH && pressedButtons == 64) // Ethnic
    {
        padLayout = 5;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = Ethnic[i];
        }
    }

    // Row 3
    if (metaKeyState == HIGH && pressedButtons == 256) // Inverted Standard
    {
        padLayout = 6;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = InvertedStandard[i];
        }
    }
    if (metaKeyState == HIGH && pressedButtons == 512) // Inverted Alternate
    {
        padLayout = 7;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = InvertedAlternate[i];
        }
    }
    if (metaKeyState == HIGH && pressedButtons == 1024) // Alternate Jazz
    {
        padLayout = 8;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = AlternateJazz[i];
        }
    }

    // Row 4
    if (metaKeyState == HIGH && pressedButtons == 4096) // Inverted Standard w/ Ghost Notes
    {
        padLayout = 9;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = InvertedStandardGhostNotes[i];
        }
    }
    if (metaKeyState == HIGH && pressedButtons == 8192) // Inverted Alternate w/ Ghost Notes
    {
        padLayout = 10;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = InvertedAlternateGhostNotes[i];
        }
    }
    if (metaKeyState == HIGH && pressedButtons == 16384) // Orchestra
    {
        padLayout = 11;
        for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)
        {
            noteNumbers[i] = Orchestra[i];
        }
    }

    //  Expression Level Up/Down/Reset (Red Buttons)
    if (metaKeyState == HIGH && pressedButtons == 8 && expressionLevel <= 119 && lastExpressionUpState == LOW) // Top Red Button
    {
        lastExpressionUpState = HIGH;
        expressionLevel = expressionLevel + 8;
        controlChange(drumsChannel, 11, expressionLevel);
    }
    if (metaKeyState == HIGH && pressedButtons == 128 && expressionLevel >= 8 && lastExpressionDownState == LOW) // Bottom Red Button
    {
        lastExpressionDownState = HIGH;
        expressionLevel = expressionLevel - 8;
        controlChange(drumsChannel, 11, expressionLevel);
    }
    if (metaKeyState == HIGH && pressedButtons == 136) // Both Red Buttons
    {
        expressionLevel = 95;
        controlChange(drumsChannel, 11, expressionLevel);
        delay(1000);
    }

    //  Program Selection Up/Down/Reset
    if (metaKeyState == HIGH && pressedButtons == 2048 && midiProgram < NUM_TOTAL_PROGRAMS && lastProgramUpState == LOW) // Top Green Button
    {
        lastProgramUpState = HIGH;
        midiProgram = midiProgram + 1;
        programChange(drumsChannel, programMap[midiProgram]);
    }
    if (metaKeyState == HIGH && pressedButtons == 32768 && midiProgram > 0 && lastProgramDownState == LOW) // Bottom Green Button
    {
        lastProgramDownState = HIGH;
        midiProgram = midiProgram - 1;
        programChange(drumsChannel, programMap[midiProgram]);
    }
    if (metaKeyState == HIGH && pressedButtons == 34816) // Both Green Buttons
    {
        midiProgram = 0;
        programChange(drumsChannel, programMap[midiProgram]);
        delay(1000);
    }

    //  Alter button velocities based on pad layout for ghost notes
    if (padLayout == 0 || padLayout == 1 || padLayout == 2 || padLayout == 5 || padLayout == 6 || padLayout == 7 || padLayout == 8 || padLayout == 11)
    {
        playButtonsNormal();
    }
    if (padLayout == 3 || padLayout == 4 || padLayout == 9 || padLayout == 10)
    {
        playButtonsGhostNotes();
    }

    //  Reset function button states
    if (pressedButtons !=     8 && pressedButtons !=   136) { lastExpressionUpState =   LOW; }
    if (pressedButtons !=   128 && pressedButtons !=   136) { lastExpressionDownState = LOW; }
    if (pressedButtons !=  2048 && pressedButtons != 34816) { lastProgramUpState =      LOW; }
    if (pressedButtons != 32768 && pressedButtons != 34816) { lastProgramDownState =    LOW; }

}
// END LOOP SECTION
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------------------------------------------
// START FUNCTIONS SECTION

void readButtons() {
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

            if (digitalRead(currentRow) == LOW && (currentTime - previousButtonsTime[buttonNumber]) > debounceDelayTime) // If the state is LOW (meaning active, due to INPUT_PULLUP) and the button passes debounce check...
            {
                bitWrite(pressedButtons, buttonNumber, 1);              // in the binary value pressedNotesUpper, set the bit in the bit position of the button number to a 1.
                previousButtonsTime[buttonNumber] = currentTime;        // Reset the debounce timer.
            }
            if (digitalRead(currentRow) == HIGH)                        // If the value is HIGH (meaning inactive due to INPUT_PULLUP)
            {
                bitWrite(pressedButtons, buttonNumber, 0);              // Set the bit to a 0.
            }
            pinMode(currentRow, INPUT);                                 // Set the selected row pin back to INPUT mode (0V / LOW).
        }
        pinMode(currentColumn, INPUT);                                  // Set the selected column pin back to INPUT mode (0V / LOW) and move onto the next column pin.
    }
}

//  Play Buttons
//  Read the states for each bit in the value, compare them to the previous
//  values, and send note on/off commands for each button where appropriate.
void playButtonsNormal()
{
    for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)                                         // For all buttons in the matrix
    {
        if (i != 6 && i != 10 && i != 14)                                               // If the button is NOT a right hand hi-hat, snare, or kick drum
        {
            if (bitRead(pressedButtons, i) != bitRead(previousPressedButtons, i))       // If the button has changed since the last program loop
            {
                if (bitRead(pressedButtons, i))                                         // If the button is active
                {
                    noteOn(drumsChannel, noteNumbers[i], velocity + velocityRNG);       // Send a noteOn for this note at the default velocity level
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 1);                            // Write to the "previous" value for comparison on next loop
                }
                else                                                                    // If the button is inactive
                {
                    noteOff(drumsChannel, noteNumbers[i], 0);                           // Send a noteOff for this note
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 0);                            // Write to the "previous" value for comparison on next loop
                }
            }
        }
        if (i == 6 || i == 10 || i == 14)                                               // If the button IS a right hand hi-hat, snare, or kick drum
        {
            if (bitRead(pressedButtons, i) != bitRead(previousPressedButtons, i))       // If the button has changed since the last program loop
            {
                if (bitRead(pressedButtons, i))                                         // If the button is active
                {
                    noteOn(drumsChannel, noteNumbers[i], velocity + velocityRNG + 24);  // Send a noteOn for this note at a slightly higher velocity level for added dynamics
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 1);                            // Write to the "previous" value for comparison on next loop
                }
                else                                                                    // If the button is inactive
                {
                    noteOff(drumsChannel, noteNumbers[i], 0);                           // Send a noteOff for this note
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 0);                            // Write to the "previous" value for comparison on next loop
                }
            }
        }
    }
}
void playButtonsGhostNotes()
{
    for (int i = 0; i < NUM_TOTAL_BUTTONS; i++)                                         // For all buttons in the matrix
    {
        if (i != 6 && i != 8 && i != 10 && i != 11 && i!= 14)                           // If the button is NOT a right hand hi-hat, snare, or kick drum, or a sidestick
        {
            if (bitRead(pressedButtons, i) != bitRead(previousPressedButtons, i))       // If the button has changed since the last program loop
            {
                if (bitRead(pressedButtons, i))                                         // If the button is active
                {
                    noteOn(drumsChannel, noteNumbers[i], velocity + velocityRNG);       // Send a noteOn for this note at the default velocity level
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 1);                            // Write to the "previous" value for comparison on next loop
                }
                else                                                                    // If the button is inactive
                {
                    noteOff(drumsChannel, noteNumbers[i], 0);                           // Send a noteOff for this note
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 0);                            // Write to the "previous" value for comparison on next loop
                }
            }
        }
        if (i == 6 || i == 10 || i == 14)                                               // If the button IS a right hand hi-hat, snare, or kick drum
        {
            if (bitRead(pressedButtons, i) != bitRead(previousPressedButtons, i))       // If the button has changed since the last program loop
            {
                if (bitRead(pressedButtons, i))                                         // If the button is active
                {
                    noteOn(drumsChannel, noteNumbers[i], velocity + velocityRNG + 24);  // Send a noteOn for this note at a slightly higher velocity level for added dynamics
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 1);                            // Write to the "previous" value for comparison on next loop
                }
                else                                                                    // If the button is inactive
                {
                    noteOff(drumsChannel, noteNumbers[i], 0);                           // Send a noteOff for this note
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 0);                            // Write to the "previous" value for comparison on next loop
                }
            }
        }
        if (i == 8 || i == 11)                                                          // If the button IS a sidestick
        {
            if (bitRead(pressedButtons, i) != bitRead(previousPressedButtons, i))       // If the button has changed since the last program loop
            {
                if (bitRead(pressedButtons, i))                                         // If the button is active
                {
                    noteOn(drumsChannel, noteNumbers[i], velocity + velocityRNG - 48);  // Send a noteOn for this note at a much low velocity level for ghost note playing
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 1);                            // Write to the "previous" value for comparison on next loop
                }
                else                                                                    // If the button is inactive
                {
                    noteOff(drumsChannel, noteNumbers[i], 0);                           // Send a noteOff for this note
                    MidiUSB.flush();                                                    // Flush the buffer to send the command immediately
                    bitWrite(previousPressedButtons, i , 0);                            // Write to the "previous" value for comparison on next loop
                }
            }
        }
    }
}

// MIDI Packet Functions

// Send MIDI Note On
// 1st byte = Event type (0x09 = note on, 0x08 = note off).
// 2nd byte = Event type combined with MIDI channel.
// 3rd byte = MIDI note number.
// 4th byte = Velocity (7-bit range 0-127)
void noteOn(byte channel, byte pitch, byte velocity)
{
    midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOn);
}

// Send MIDI Note Off
// 1st byte = Event type (0x09 = note on, 0x08 = note off).
// 2nd byte = Event type combined with MIDI channel.
// 3rd byte = MIDI note number.
// 4th byte = Velocity (7-bit range 0-127)
void noteOff(byte channel, byte pitch, byte velocityLow)
{
    midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOff);
}

// Control Change
// 1st byte = Event type (0x0B = Control Change).
// 2nd byte = Event type combined with MIDI channel.
// 3rd byte = MIDI CC number (7-bit range 0-127).
// 4th byte = Control value (7-bit range 0-127).
void controlChange(byte channel, byte control, byte value)
{
    midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
    MidiUSB.sendMIDI(event);
}

// Program Change
// 1st byte = Event type (0x0C = Program Change).
// 2nd byte = Event type combined with MIDI channel.
// 3rd byte = Program value (7-bit range 0-127).
void programChange(byte channel, byte value)
{
    midiEventPacket_t event = {0x0C, 0xC0 | channel, value};
    MidiUSB.sendMIDI(event);
}

// END FUNCTIONS SECTION
// ------------------------------------------------------------------------------------------------------------------------------------------------------------

// END OF PROGRAM
// ------------------------------------------------------------------------------------------------------------------------------------------------------------


