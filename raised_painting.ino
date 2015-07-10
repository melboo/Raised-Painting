
// raised painting - metropolitan collab - parsons the new school for design Fall13//
// modification of adafruit wave shield example


#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"


SdReader card;    
FatVolume vol;    
FatReader root;   
FatReader f;      

WaveHC wave;     

boolean Intro = true;
boolean finish = false;
boolean f_played = false;
boolean f_1 = false;
boolean f_2 = false;
boolean f_3 = false;

// Switches
byte switches[] = {A0, A1, A2, A3, A4, A5};

#define NUMSWITCHS sizeof(switches)
volatile byte sw_connect[NUMSWITCHS], sw_justchanged[NUMSWITCHS];

int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 

void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  putstring(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}

void setup() {
  byte i;

  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
 
  pinMode(13, OUTPUT);
 
  
  
  if (!card.init()) {      
    putstring_nl("Card init. failed!");  
    sdErrorCheck();
    while(1);                         
  }
  
  card.partialBlockRead(true);
 
  uint8_t part;
  for (part = 0; part < 5; part++) {  
    if (vol.init(card, part)) 
      break;                           
  }
  if (part == 5) {                       
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();     
    while(1);                        
  }
  

  putstring("Using partition ");
  putstring(", type is FAT");
  
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); 
    while(1);                            
  }
  
  putstring_nl("Ready!");
  
  TCCR2A = 0;
  TCCR2B = 1<<CS22 | 1<<CS21 | 1<<CS20;

  TIMSK2 |= 1<<TOIE2;

}

SIGNAL(TIMER2_OVF_vect) {
  check_switches();
}

void check_switches()
{
  static byte previousstate[NUMSWITCHS];
  static byte currentstate[NUMSWITCHS];
  byte index;

  for (index = 0; index < NUMSWITCHS; index++) {
    
    int TempValue = digitalRead(switches[index]); 
    if(TempValue == 0){
      currentstate[index] = LOW;
    }else {
      currentstate[index] = HIGH;
    }
    
    
    if (currentstate[index] == previousstate[index]) { 
      if((index == 3) || (index == 4) || (index == 5)){
        
        if ((sw_connect[index] == LOW) && (currentstate[index] == LOW)) { 
            sw_justchanged[index] = 1;
        }
        sw_connect[index] = !currentstate[index]; 
      }
       if((index == 0) || (index == 1) || (index == 2)){
         if ((sw_connect[index] == LOW) && (currentstate[index] == HIGH)) { 
            
            sw_justchanged[index] = 1;
          }
        sw_connect[index] = currentstate[index];  
       } 
    }
    previousstate[index] = currentstate[index];   
  }
}

void loop() {
  if (sw_justchanged[0]) {
    
    sw_justchanged[0] = 0;
    playfile("PU_03.WAV");
    while (wave.isplaying && sw_connect[0]) {
    }
    wave.stop();    
  }
  if (sw_justchanged[1]) {
    sw_justchanged[1] = 0;
    playfile("PU_02.WAV");
    while (wave.isplaying && sw_connect[1]) {
    }
    wave.stop();    
  }
  if (sw_justchanged[2]) {
    sw_justchanged[2] = 0;
    playfile("PU_01.WAV");
    while (wave.isplaying && sw_connect[2]) {
    }
    wave.stop();    
  }
  if (sw_justchanged[3]) {
    sw_justchanged[3] = 0;
    playfile("PD_03.WAV");
    while (wave.isplaying && sw_connect[3]) {
    }
    f_1 = true;
    wave.stop();    
  }
  if (sw_justchanged[4]) {
    sw_justchanged[4] = 0;
    playfile("PD_02.WAV");
    while (wave.isplaying && sw_connect[4]) {
    }
    f_2 = true;
    wave.stop();    
  }
  
  if (sw_justchanged[5]) {
    sw_justchanged[5] = 0;
    playfile("PD_01.WAV");
    while (wave.isplaying && sw_connect[5]) {
    }
    f_3 = true;
    wave.stop();    
  }
  
  if(f_1 && f_2 && f_3){
      f_played = true;
      playfile("FIN.WAV");
      while (wave.isplaying && f_played) {       
      }
      wave.stop();    
      f_1 = false; 
      f_2 = false;
      f_3 = false;  
  }
  
  
  if(Intro) {
    playfile("INTRO.WAV");
    while (wave.isplaying && Intro) {
    }
    wave.stop();  
    Intro = false;  
  }
  
}


void playcomplete(char *name) {
  playfile(name);
  while (wave.isplaying) {
  }
}

void playfile(char *name) {
  if (wave.isplaying) {
    wave.stop(); 
  }
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.print(name); return;
  }

  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
  
  wave.play();
}
