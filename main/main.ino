#include <Arduino.h>
#include <new.h>

#include "event.h"
#include "a_memory.h"
#include "button.h"
#include "timer.h"
#include "manager.h"
#include "rotery_encoder.h"

// #include <SPI.h>
// #include <SD.h>
#include <TM1637Display.h>

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
    } pin_1{2,3}, pin_2{4, 5};

  constexpr const uint8_t reset_btn_pin = 6;
  constexpr const uint8_t brighness = 0x0f;
};

struct Player {
    TM1637Display display;
    Timer timer_  = {1000, &Player::timerCb, reinterpret_cast<void*>(this)};
    Manager &manager_;

    uint32_t timer_count_ = 0;

    static void timerCb(void* v_self) {
        auto self = reinterpret_cast<Player*>(v_self);
        self->display.showNumberDec(self->timer_count_);
        ++self->timer_count_;
    }

    Player(const CFG::DisplayCfg &disp, Manager &manager) 
     : display(disp.CLK, disp.DIO)
     , manager_(manager)
    {
        display.setBrightness(CFG::brighness);
        display.clear();
        manager_.addItem(timer_);
        timerCb(this);
    }

    void pause_timer() {
        timer_.pause();
    }

    void resume_timer() {
        timer_.resume();
    }

    ~Player() {
        manager_.remoteItem(timer_);
    }
};


Manager manager;

Player display_1(CFG::pin_1, manager);
Player display_2(CFG::pin_2, manager);

Player *cur_player = &display_1;

Button b(CFG::reset_btn_pin, [](bool state){
    Serial.println("reset pressed");
    if (state) {
        cur_player->pause_timer();
        cur_player = cur_player == &display_1 ? &display_2 : &display_1;
        cur_player->resume_timer();
    }
});

struct {
    int counter = 0;
    int aState;
    int aLastState;
} rot;



RoteryEncoder encoder{11, 12};

void setup() {
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.println("running chess timer");

    display_1.resume_timer();

    manager.addItem(b);

    manager.init();

}

void loop() {
    Manager cfgManager;
    cfgManager.addItem(encoder);
    while(1) {
        cfgManager.poll();
    }

    manager.poll();


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
