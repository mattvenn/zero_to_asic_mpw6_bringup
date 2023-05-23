int incomingByte = 0; // for incoming serial data

void setup() {
  Serial.begin(9600); // usb serial
  Serial1.begin(9600); // asic serial
}

void loop() {
  // from asic via arduino to host
  if (Serial1.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial1.read();

    // say what you got:
    Serial.write(incomingByte);
    }
/*
    if(incomingByte == 10)
   {
        Serial.println("got newline  writing a char");
        Serial1.write('b');
    }
  }
  */
  // from host to arduino and on to asic
  if (Serial.available() > 0) {
    incomingByte = Serial.read();

    // say what you got:
    Serial1.write(incomingByte);
    }
    
}
