  #include <avr/interrupt.h>
  #include <avr/sleep.h>

  /*

  =====================================================
  |     ATTINY13 watchdog low-power LED blinker.      |
  | For use in the TinySat circuit sculpture project. |
  |       ** Harrison Broadbent, July 2021 **         |
  =====================================================

  This program flashes an led connected to pin 3 of an ATTINY13 chip every ~10seconds. 
  A number of small tweaks have been added to lower the power consumption of this chip. 
  With a standard red 3mm led connected with a 200ohm resistor, power consumption is approx - 

    off-mode: 30 μA
    on-mode : 5 mA
  
  Code adapted from   - https://forum.arduino.cc/t/watchdog-on-attiny13-every-2-minute/570158
  Project inspiration - https://www.bhoite.com/sculptures/tiny-cube-sat/ 
  
  */

  int count = 0;
  int darkness_threshold = 220;
  int panel_voltage_reading;

  // led on PB4, pin 3
  // Panel voltage measure on PB2, pin 7, A1
  int led_pin = 4;

  // Runs every time the watchdog wakes
  ISR(WDT_vect) {
    digitalWrite(led_pin, LOW);
    count++;

    // LED routine runs after an off-cycle.
    if (count > 2) {

      // only flash the LED when it's dark
      panel_voltage_reading = analogRead(A1);
      if (panel_voltage_reading < darkness_threshold) {
        digitalWrite(led_pin, HIGH);
        delay(30);
        digitalWrite(led_pin, LOW);
        delay(40);
        digitalWrite(led_pin, HIGH);
        delay(50);
        digitalWrite(led_pin, LOW);
        delay(60);
      }
      
      count = 0;
    }
  }
  
  void setup() {          
    pinMode(A1, INPUT);   
    pinMode(led_pin, OUTPUT);   
    (ADCSRA &= ~(1<<ADEN)); // disable ADC; save ~320μA
  
    // quickly flash LED to indicate startup
    digitalWrite(led_pin, HIGH);   
    delay(200);
    digitalWrite(led_pin, LOW);

    // hold LED on if it's dark
    panel_voltage_reading = analogRead(A1);
    if (panel_voltage_reading < darkness_threshold) {
      delay(500);
      digitalWrite(led_pin, HIGH);   
      delay(100);
      digitalWrite(led_pin, LOW);
      delay(100);
    }
    
    count = 0;
  
    // Set countdown timer for ~8s
    WDTCR |= (1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0); // 8s
  
    // Enable watchdog & global interrupts
    WDTCR |= (1<<WDTIE);
    sei();
  
    // Use the power down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
    for (;;) {
      // sleep and wait for an interrupt
      sleep_mode();
    }
  }
