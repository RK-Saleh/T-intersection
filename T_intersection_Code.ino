//Master Pedestrian Light System

#include <Wire.h>

void setup() { 
    Wire.begin(); // Start I2C as master
    pinMode(8, INPUT);      // Button
    pinMode(6, OUTPUT);     // Green 
    pinMode(9, OUTPUT);     // Red 

    // Start with pedestrian red
    digitalWrite(6, LOW); 
    digitalWrite(9, HIGH); 
}

void loop() { 
    if (digitalRead(8) == HIGH) { 
        // Pedestrian button pressed
        digitalWrite(6, LOW); //Green OFF
        digitalWrite(9, HIGH); //Red ON

        // Send button press signal to Slave
        Wire.beginTransmission(8); // Address of the slave Arduino
        Wire.write('B');           // Send 'B' for button press
        Wire.endTransmission();

        delay(5000); 
        digitalWrite(6, HIGH);    // Green ON
        digitalWrite(9, LOW);     // Red OFF
        delay(6000); 
      
        // Reset to initial state
        digitalWrite(6, LOW);    //Green OFF 
        digitalWrite(9, HIGH);    //Red ON
    }
}


// Slave Traffic Light Control System

#include <Wire.h>

enum TrafficState { GREEN, YELLOW, RED_WITH_PEDESTRIAN_GREEN, YELLOW_AFTER_RED };

// Initialize the traffic light state
TrafficState currentState = GREEN;

void setup() { 
    Wire.begin(8); // Start I2C as slave with address 8
    Wire.onReceive(Receive); // Register Receive Signal

    // Initialize pins for traffic light control
    pinMode(7, OUTPUT); // D1_Green
    pinMode(4, OUTPUT); // D1_Yellow
    pinMode(2, OUTPUT); // D1_Red
    pinMode(12, OUTPUT); // D2_Green
    pinMode(11, OUTPUT); // D2_Yellow
    pinMode(10, OUTPUT); // D2_Red
    pinMode(5, OUTPUT); // D3_Red
    pinMode(6, OUTPUT); // D3_Yellow
    pinMode(8, OUTPUT); // D3_Green

    // Start initial state with traffic lights green
    digitalWrite(2, LOW); 
    digitalWrite(4, LOW); 
    digitalWrite(7, HIGH);
    digitalWrite(10, LOW); 
    digitalWrite(11, LOW); 
    digitalWrite(12, HIGH);
    digitalWrite(8, LOW); 
    digitalWrite(6, LOW); 
    digitalWrite(5, HIGH);
}

void loop() { 
    switch (currentState) {
        case GREEN:
            // Remains green until a signal is received from the master
            break;

        case YELLOW:
            // Transition to yellow light
            digitalWrite(7, LOW);    
            digitalWrite(4, HIGH);
            digitalWrite(12, LOW);    
            digitalWrite(11, HIGH);    
            digitalWrite(5, HIGH);

            delay(2000);  // Wait for yellow light duration

            // Transition to red light
            digitalWrite(2, HIGH);    
            digitalWrite(4, LOW);
            digitalWrite(10, HIGH);    
            digitalWrite(11, LOW);    
            digitalWrite(5, LOW);
            digitalWrite(6, HIGH); // Turn on pedestrian red light

            delay(3000); // Wait for red light duration

            currentState = RED_WITH_PEDESTRIAN_GREEN;
            break;

        case RED_WITH_PEDESTRIAN_GREEN:
            // Pedestrian green light and traffic red light active
            digitalWrite(4, LOW);   
            digitalWrite(2, HIGH);  
            digitalWrite(11, LOW);   
            digitalWrite(10, HIGH);
            digitalWrite(8, HIGH); // Pedestrian green light
            digitalWrite(6, LOW);

            delay(3000);  // Wait for pedestrian crossing duration

            // Prepare to switch back to traffic control
            digitalWrite(2, HIGH);    
            digitalWrite(4, LOW);
            digitalWrite(10, HIGH);    
            digitalWrite(11, LOW);    
            digitalWrite(8, LOW);
            digitalWrite(6, HIGH);

            delay(2000);  // Buffer duration

            // Reset pedestrian lights
            digitalWrite(2, HIGH);    
            digitalWrite(4, LOW);
            digitalWrite(10, HIGH);    
            digitalWrite(11, LOW);    
            digitalWrite(6, LOW);
            digitalWrite(5, HIGH); // Pedestrian red light back on

            delay(1000);              

            currentState = YELLOW_AFTER_RED; 
            break;

        case YELLOW_AFTER_RED:
            // Transition from red to yellow
            digitalWrite(2, LOW);    
            digitalWrite(4, HIGH);  
            digitalWrite(10, LOW);    
            digitalWrite(11, HIGH);

            delay(2000);             // Wait for yellow light duration

            // Switch back to green light
            digitalWrite(4, LOW);    
            digitalWrite(7, HIGH);  
            digitalWrite(11, LOW);    
            digitalWrite(12, HIGH); 

            currentState = GREEN;     
            break;
    }
}

// Function to handle data received from master
void Receive(int bytes) { 
    while (Wire.available()) { 
        char c = Wire.read(); 
        if (c == 'B') { 
            // Button pressed signal
            currentState = YELLOW; // Transition to yellow when button is pressed
        }
    }
}
