//
//  ArbotVMA03Platform.h
//  
//
//  Created by Jernej Kaše on 11/20/14.
//
//
#ifndef ____ArbotVMA03Platform__
#define ____ArbotVMA03Platform__


#include "ArbotPlatform.h"

class ArbotVMA03Platform: virtual public ArbotPlatform{
private:
    unsigned char pwm_a;     //PWM control for motor outputs 1 and 2
    unsigned char pwm_b;     //PWM control for motor outputs 3 and 4
    unsigned char dir_a;     //direction control for motor outputs 1 and 2
    unsigned char dir_b;
    byte          blink_pin; //which pin is used for blink
public:
    ArbotVMA03Platform(unsigned char pwm_a=3,unsigned char pwm_b=9,unsigned char dir_a=4,unsigned char dir_b=8, byte blink_pin=13){
      this->pwm_a=pwm_a;
      this->pwm_b=pwm_b;
      this->dir_a=dir_a;
      this->dir_b=dir_b;
    }
  
    virtual void setup(){
      pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
      pinMode(pwm_b, OUTPUT);
      pinMode(dir_a, OUTPUT);
      pinMode(dir_b, OUTPUT);
      pinMode(blink_pin, OUTPUT);
    }
    
    
    virtual void forwards(unsigned char speed){
      speed = normalizeSpeed(speed);
      digitalWrite(dir_a, LOW); 
      digitalWrite(dir_b, LOW);  
      analogWrite(pwm_a, 255*100/speed);
      analogWrite(pwm_b, 255*100/speed);
    }
    virtual void backwards(unsigned char speed){
      speed = normalizeSpeed(speed);
      digitalWrite(dir_a, HIGH); 
      digitalWrite(dir_b, HIGH);  
      analogWrite(pwm_a, 255*100/speed);
      analogWrite(pwm_b, 255*100/speed);
        
    }

    virtual void left(unsigned char speed){
      speed = normalizeSpeed(speed);
      digitalWrite(dir_a, LOW); 
      digitalWrite(dir_b, HIGH);  
      analogWrite(pwm_a, 255*100/speed);
      analogWrite(pwm_b, 255*100/speed);  
    }
    
    virtual void right(unsigned char speed){
      speed = normalizeSpeed(speed);
      digitalWrite(dir_a, HIGH); 
      digitalWrite(dir_b, LOW);  
      analogWrite(pwm_a, 255*100/speed);
      analogWrite(pwm_b, 255*100/speed); 
    }
    
    virtual void fullStop(){
      analogWrite(pwm_a, 0);
      analogWrite(pwm_b, 0);
    }
    
    virtual void blink(int time){
        digitalWrite(13, HIGH);
        delay(time);
    }
};

#endif /* defined(____ArbotVMA03Platform__) */
