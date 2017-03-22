// Using the tpic6b595 shift register, 12 in total.
// 

// tpic6b595 has four control pins.
// Electronically these are driven in
// parallel
#define LATCH_PIN  10     //  purple, pin 10
#define CLOCK_PIN  9     //   blue, pin 9
#define DATA_PIN   8     //   green, pin 8
#define ENABLE_PIN 11    //  grey, pin 11

// Enable states (active low)
// Enabling lights LED by sinking
// current.
#define ENABLE_OFF HIGH
#define ENABLE_ON  LOW

// User input to trigger the sequence
// of 90 LED illuminations and camera
// trigers.
#define INPUT_BUTTON A3

// Relay pins, to triger camera
#define RELAY_5VOLT A2
#define RELAY_TOGGLE A1
#define RELAY_GROUND A0

// State flags for operation.
#define STATE_WAITING 1
#define STATE_WORKING 2
int which;
int state;

// Clock to time lighting interval.
long time_elapsed;


// One time setup.
void setup() {

  // Set default state of the relay
  pinMode( RELAY_5VOLT, OUTPUT );
  pinMode( RELAY_TOGGLE, OUTPUT);
  pinMode( RELAY_GROUND, OUTPUT );
  digitalWrite( RELAY_TOGGLE, LOW );
  digitalWrite( RELAY_GROUND, LOW );
  digitalWrite( RELAY_5VOLT, HIGH );

  // Set up input
  pinMode( INPUT_BUTTON, INPUT );

  // Set default state of the shift reg chip pins.
  pinMode( LATCH_PIN,OUTPUT);
  pinMode( CLOCK_PIN,OUTPUT);
  pinMode( DATA_PIN, OUTPUT);
  pinMode( ENABLE_PIN, OUTPUT);

  // Ensure LED circuitry is off
  digitalWrite( ENABLE_PIN, ENABLE_OFF);
  digitalWrite( CLOCK_PIN, LOW );

  // Flush the shift register (all LEDS off)
  clearAll();

  // Activate serial for debug output
  Serial.begin(115200);
  Serial.println("*** RESET ***");

  // Set flags
  which = 0;
  state = STATE_WAITING;
  time_elapsed = millis();
}

// Main loop routine
void loop() {

    
    if( state == STATE_WAITING ) {

        // Read button, if active, transition to 
        // the active program state.
        int button_state = digitalRead( INPUT_BUTTON );
        if( button_state > 0 ) {

          // Flag state change.
          state = STATE_WORKING;

          // Get ready for shift opration
          which = 0;
          clearAll();
          delay(10);
        } else {

           // This routine just illuminates LEDS one
           // by one to give an indication that the device
           // is turned on and active.  
           if( millis() - time_elapsed > 150 ) {
              time_elapsed = millis();

              // A bit lazy, but clear and set a new
              // bit in the shift register.
              // Could just shift by one.
              clearAll();
              set( which );
              which++;
              if( which > 89 ) which = 0;
           }
        
        }
    
    } else if( state == STATE_WORKING ) {

      // If we are in the working state, 
      // We set the LED and take a picture
      set( which );
      delay(250);

      // Auto delays in routine
      takePicture();

      // progress the LED
      clearAll();
      delay(2000);
      which++;

      // If we get to LED 90 we reset
      // the system.
      if( which > 89 ) {
        which = 0;
        state = STATE_WAITING;
        time_elapsed = millis();
      }
    }
    
    delay(5);
}


// To set an LED, we simply clock a 1 across
// the shift register bits by the LED index 
// (which).  Easy!
void set( int which ) {
    
    // First, clear the outputs
    clearAll();
  
    // Disable chip output
    digitalWrite( ENABLE_PIN, ENABLE_OFF );
    
    // Set chip to buffer input
    digitalWrite( LATCH_PIN, LOW );

    // Write a 1 to the data pin
    digitalWrite( DATA_PIN, HIGH );

    // Do 1 clock to load in.
    digitalWrite( CLOCK_PIN, HIGH );
    digitalWrite( CLOCK_PIN, LOW );

    // Now set data to 0
    digitalWrite( DATA_PIN, LOW );

    if( which > 0 ) {

      // Do required clocks to shift the 1
      // along the shift registers
      for( int i = 0; i < which; i++ ) {
        digitalWrite( CLOCK_PIN, HIGH );
        digitalWrite( CLOCK_PIN, LOW );
      }
    }
    // Transfer input to output buffers
    digitalWrite( LATCH_PIN, HIGH);

    // Enable outputs
    digitalWrite( ENABLE_PIN, ENABLE_ON );
}

// To clear, we just ensure that
// a 0 is clocked along all bits
// of the register.
void clearAll() {
    // Disable chip output
    digitalWrite( ENABLE_PIN, ENABLE_OFF );
    
    // Set chip to buffer input
    digitalWrite( LATCH_PIN, LOW );

    // Write a 1 to the data pin
    digitalWrite( DATA_PIN, LOW );

    // Do 90 clocks
    for( int i = 0; i < 97; i++ ) {
      digitalWrite( CLOCK_PIN, HIGH );
      digitalWrite( CLOCK_PIN, LOW );
    }

    // Transfer input to output buffers
    digitalWrite( LATCH_PIN, HIGH);

    // Enable outputs
    digitalWrite( ENABLE_PIN, ENABLE_ON );
  
}

// Short routine to toggle relay, causing DSLR
// to take a photo.  Note delays, timing is not
// critical.
void takePicture() {
  digitalWrite( RELAY_TOGGLE, HIGH );
  delay(250);
  digitalWrite( RELAY_TOGGLE, LOW );
  delay(4000);
}

