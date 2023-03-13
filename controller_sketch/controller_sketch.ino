#include <LiquidCrystal.h>
#include <MIDI.h>
#include <Bounce.h>

#define BUTTON_L 9
#define BUTTON_R 10
#define BUTTON_MINUS 11
#define BUTTON_PLUS 12

#define SET_NUM_NOTES -2
#define SET_INPUT_CHANNEL -1

Bounce button_l = Bounce(BUTTON_L, 10);
Bounce button_r = Bounce(BUTTON_R, 10);
Bounce button_minus = Bounce(BUTTON_MINUS, 10);
Bounce button_plus = Bounce(BUTTON_PLUS, 10);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

const int rs=3, en=4, d4=5, d5=6, d6=7, d7=8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int selection = SET_NUM_NOTES;
int channels[6] = {1, 2, 3, 4, 5, 6};
int ch_notes[6] = {-1, -1, -1, -1, -1, -1};
int num_notes = 3;
int input_channel = 7;
int prev_note_index = 0;

void print_channels() {
  lcd.setCursor(0, 1);
  lcd.print("Out:             ");
  lcd.setCursor(5, 1);
  for(int i = 0; i < num_notes; i++) {
    lcd.print(channels[i]); 
    lcd.print(" ");
  }
}

int get_channel(int note) {
  for(int i = 0; i < num_notes; i++) {
    if(ch_notes[i] == -1) {
      ch_notes[i] = note;
      prev_note_index = i;
      return channels[i];
    }
  }
  ch_notes[prev_note_index] = note;
  return channels[prev_note_index];
}

int free_channel(int note) {
  for(int i = 0; i < num_notes; i++) {
    if(ch_notes[i] == note) {
      ch_notes[i] = -1;
      return channels[i];
    }
  }
  return 0;
}

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  lcd.print("Notes: ");
  lcd.print(num_notes);
  lcd.print(" In: ");
  lcd.print(input_channel);
  print_channels();
  lcd.cursor();
  lcd.setCursor(7, 0);
  pinMode(BUTTON_L, INPUT);
  pinMode(BUTTON_R, INPUT);
  pinMode(BUTTON_MINUS, INPUT);
  pinMode(BUTTON_PLUS, INPUT);

  Serial.begin(9600);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  
  // Handle key presses / settings
  button_l.update();
  button_r.update();
  button_minus.update();
  button_plus.update();

  if(selection == SET_NUM_NOTES) {
    if(button_l.risingEdge()) {
      selection = num_notes-1;
    }
    if(button_r.risingEdge()) {
      selection = SET_INPUT_CHANNEL;
    }
    if(button_minus.risingEdge() && num_notes > 1) {
      num_notes--;
      lcd.print(num_notes);
      print_channels();
    }
    if(button_plus.risingEdge() && num_notes < 6) {
      num_notes++;
      lcd.print(num_notes);
      print_channels();
    }
    lcd.setCursor(7, 0);
  }
  else if(selection == SET_INPUT_CHANNEL) {
    if(button_l.risingEdge()) {
      selection = SET_NUM_NOTES;
    }
    if(button_r.risingEdge()) {
      selection = 0;
    }
    if(button_minus.risingEdge() && input_channel > 1) {
      input_channel--;
      lcd.print(input_channel);
      lcd.print(" ");
    }
    if(button_plus.risingEdge() && input_channel < 16) {
      input_channel++;
      lcd.print(input_channel);
    }
    lcd.setCursor(13, 0);
  }
  else if(selection >= 0) {
    if(button_l.risingEdge()) {
      selection--;
    }
    if(button_r.risingEdge()) {
      if(selection == (num_notes - 1)) {
        selection = SET_NUM_NOTES;
      }
      else {
        selection++;
      }
    }
    if(button_minus.risingEdge() && channels[selection] > 0) {
      channels[selection]--;
      print_channels();
    }
    if(button_plus.risingEdge() && channels[selection] < 9) {
      channels[selection]++;
      print_channels();
    }
    lcd.setCursor(selection * 2 + 5, 1);
  }


  // Handle midi button presses
  if(MIDI.read() && (MIDI.getChannel() == input_channel)) {
    byte type = MIDI.getType();
    int note = MIDI.getData1();
    int velocity = MIDI.getData2();
    int channel = 0;
    
    switch(type) {
      case midi::NoteOn:
        channel = get_channel(note);
        MIDI.sendNoteOn(note, velocity, channel);
        break;
      case midi::NoteOff:
        channel = free_channel(note);
        Serial.print("Ending note ");
        Serial.print(note);
        Serial.print(", ");
        Serial.print(velocity);
        Serial.print(", ");
        Serial.println(channel);
        if(channel) {
          MIDI.sendNoteOff(note, velocity, channels[0]);
        }
        break;
    }
    
  }
}
