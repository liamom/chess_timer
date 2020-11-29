#include <Arduino.h>
#include <new.h>

#include "a_memory.h"
#include "button.h"
#include "timer.h"
#include "manager.h"
#include "title_card.h"

// #include <SPI.h>
// #include <SD.h>
#include <TM1637Display.h>

#include <Encoder.h>

const uint8_t SEG_DONE[] = {
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
    SEG_C | SEG_E | SEG_G,                           // n
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
    };

// Module connection pins (Digital Pins)
namespace CFG {
    struct DisplayCfg{
        uint8_t CLK;
        uint8_t DIO;
    } pin_1{4,5}, pin_2{6, 7};

  constexpr const uint8_t reset_btn_pin = 8;
  constexpr const uint8_t brighness = 0x0f;

  constexpr const uint8_t clock_button_left = 10;
  constexpr const uint8_t clock_button_right = 11;
  constexpr const uint8_t buzzer = 12;
  
  constexpr const uint8_t knob_1 = 3;
  constexpr const uint8_t knob_2 = 2;

  constexpr const uint8_t encoder_button = 9;
};


void setup() {
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

    pinMode(CFG::reset_btn_pin, INPUT_PULLUP);
    pinMode(CFG::buzzer, OUTPUT); // Set buzzer - pin 9 as an output

    Serial.println("running chess timer");
}

struct TimeControl {
    uint32_t minutes = 1;
    uint32_t seconds = 0;
    uint32_t bonus = 0;
} timeControl;

void setMinutes(Encoder &knob, ReleaseButton &encoderButton, TM1637Display &display) {
    Serial.println("setMinutes");

    knob.write(timeControl.minutes * 4);
    while(1) {
        int read = knob.read() ;
        read /= 4;
        read %= 100;
        read = read  >= 0 ? read : 100 + read;
        timeControl.minutes = read;
        display.showNumberDecEx(timeControl.minutes, 0b01000000, false, 2, 0);

        if(encoderButton.processPress()) {
            break;
        }
    } 

    Serial.print("Setting minutes to: ");
    Serial.println(timeControl.minutes);
}

void setSeconds(Encoder &knob, ReleaseButton &encoderButton, TM1637Display &display) {
    Serial.println("setSeconds");

    knob.write(timeControl.seconds * 4);

    int length = timeControl.minutes == 0 ? 4 : 2;
    int pos = timeControl.minutes == 0 ? 0 : 2;

    while(1) {
        int read = knob.read() ;
        read /= 4;
        read %= 60;
        read = read >= 0 ? read : 60 + read;
        timeControl.seconds = read;
        
        display.showNumberDecEx(timeControl.seconds, 0b01000000, true, length, pos);

        if(encoderButton.processPress()) {
            break;
        }
    } 

    Serial.print("Setting seconds to: ");
    Serial.println(timeControl.seconds);
}

void setBonus(Encoder &knob, ReleaseButton &encoderButton, TM1637Display &display) {
    Serial.println("setBonus");

    knob.write(timeControl.bonus * 4);

    uint8_t b_dash[] = {
        SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,
        SEG_G,
        0,
        0
    };
    display.setSegments(b_dash);

    while(1) {
        int read = knob.read() ;
        read /= 4;
        read %= 60;
        read = read >= 0 ? read : 60 + read;
        timeControl.bonus = read;
        display.showNumberDecEx(timeControl.bonus, 0b01000000, false, 2, 2);

        if(encoderButton.processPress()) {
            break;
        }
    } 

    Serial.print("Setting bonus to: ");
    Serial.println(timeControl.bonus);
}


struct Player {    
    TM1637Display &display;
    CountdownTimer timer;
    PushButton btn;
};


void displayTimeFormatted(TM1637Display &display, uint32_t time_left_ms) {
    uint32_t rount_up = (time_left_ms % 1000) == 0 ? 0 : 1;
    uint32_t time_left_rouded = time_left_ms / 1000;
    time_left_rouded += rount_up;

    uint32_t minutes = time_left_rouded / 60;
    uint32_t seconds = time_left_rouded % 60;

    Serial.print("setting timer to: ");
    Serial.println(time_left_rouded);
    if (minutes != 0) {
        display.showNumberDecEx(minutes, 0b01000000, false, 2, 0);
        display.showNumberDecEx(seconds, 0b01000000, true, 2, 2);
    } else {
        display.showNumberDecEx(seconds, 0b01000000, false, 4, 0);
    }
}

void play_ending() {
    auto play = [](unsigned long freq, uint32_t time) {
        tone(CFG::buzzer, freq); // Send 1KHz sound signal...
        delay(time);        // ...for 1 sec
    };

    auto delay_no_sound = [](uint32_t time) {
        noTone(CFG::buzzer);     // Stop sound...
        delay(time);        // ...for 1sec
    };

    play(987, 53);
    delay_no_sound(53);
    play(987, 53);
    delay_no_sound(53);
    play(987, 53);
    delay_no_sound(53);
    play(987, 428);
    play(784, 428);
    play(880, 428);
    play(987, 107);
    delay_no_sound(214);
    play(880, 107);
    play(987, 857);

    noTone(CFG::buzzer);
    return;
}

void loop() {

    Serial.println("Running loop");
    TM1637Display leftDisplay{CFG::pin_1.CLK, CFG::pin_1.DIO};
    TM1637Display rightDisplay{CFG::pin_2.CLK, CFG::pin_2.DIO};
    leftDisplay.setBrightness(0x0f);
    rightDisplay.setBrightness(0x0f);
    leftDisplay.clear();
    rightDisplay.clear();

    // show title card
    showTitleCard(leftDisplay, rightDisplay);

    Encoder knob(CFG::knob_1, CFG::knob_2);
    ReleaseButton encoderButton(CFG::encoder_button);
    setMinutes(knob, encoderButton, leftDisplay);
    setSeconds(knob, encoderButton, leftDisplay);
    setBonus(knob, encoderButton, rightDisplay);

    uint32_t total_time_in_ms = ((timeControl.minutes * 60) + timeControl.seconds) * 1000;
    Serial.print("Game time: ");
    Serial.print(total_time_in_ms);
    Serial.print(" +");
    Serial.println(timeControl.bonus);

    Player left_player  = {leftDisplay, {total_time_in_ms, 1000}, CFG::clock_button_left};
    Player right_player = {rightDisplay, {total_time_in_ms, 1000}, CFG::clock_button_right};

    displayTimeFormatted(left_player.display, total_time_in_ms);
    displayTimeFormatted(right_player.display, total_time_in_ms);

    Player *current_player = nullptr;

    {
        ReleaseButton left_btn {CFG::clock_button_left};
        ReleaseButton right_btn {CFG::clock_button_right};

        for (;;) {
            if(left_btn.processPress()){
                Serial.print("left_btn");
                current_player = &left_player;
                break;
            } else if (right_btn.processPress()) {
                Serial.print("right_btn");
                current_player = &right_player;
                break;
            }
        }
    }

    Serial.println("Starting timer");
    current_player->timer.resume();
    for (;;) {
        uint32_t time_left_ms;
        if (current_player->timer.processUpdate(&time_left_ms)) {
            displayTimeFormatted(current_player->display, time_left_ms);

            if (time_left_ms == 0) {
                break;
            }
        }

        if(current_player->btn.processPress()) {
            // switch current player to the other player
            Serial.print("switching player, adding bonus=");
            Serial.println(timeControl.bonus);
            {
                CountdownTimer &cur_timer = current_player->timer;
                cur_timer.pause();
                cur_timer.addBonus(timeControl.bonus * 1000);
                displayTimeFormatted(current_player->display, cur_timer.getCurrentTime());
            }

            current_player = current_player == &left_player ? &right_player : &left_player;
            current_player->timer.resume();
        }
        
    }

    Serial.println("game over");
    play_ending();


  //delay(2000);

  // // put your main code here, to run repeatedly:
  // uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  // uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
  // display.setBrightness(0x0f);

  // display.setSegments(SEG_DONE);
  // delay(2000);


  // // All segments on
  // display.setSegments(data);
  // delay(2000);

  // // Selectively set different digits
  // data[0] = display.encodeDigit(0);
  // data[1] = display.encodeDigit(1);
  // data[2] = display.encodeDigit(2);
  // data[3] = display.encodeDigit(3);
  // display.setSegments(data);
  // delay(2000);

  // display.clear();
  // delay(2000);


}
