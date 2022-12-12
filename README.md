# ESP32-LED-Test
 
Ansync Internship Aptitude **Project #1** - Persistent LED control via UART input commands with ESP32 development board  

## Notes
- Used VS Code editor -> ESP-IDF extension for ease of building and flashing  
- File structure based on *`examples`* directory on IDF repository.

## Implemented Commands
Commands are to be typed into the **serial monitor** (rate set to 115200). Input is processed on-enter (when newline is sent)

- `ledOn` 
- `ledOff` 
- `ledBlink` - pre-defined delay time of 500 milliseconds. 
- `exit`

## How It Works (detailed in code comments)
### *`main/main.c`*: Main executable file. Detailed below ###
- One i32 allocated on **non-volatile storage** to store/retrieve current state of LED. 

Possible LED states are defined by an enum  
```c
- enum State {
    ON = 1, OFF, BLINK
};
```

- Upon running, an attempt is made to retrieve the previous state of the LED. If no state is stored (very first time ever running the program), LED state is set to `off` (`setupNVS()`) 

- In main loop, program performs current LED state WHILE awaiting Serial input (UART). After sanitizing the command (ignoring newline):  
    -  Associated LED task is executed - `processCommand(string)`  
    -  Associated LED state is stored in NVS - `updateNVS()`

## Attempted Methods
### Review *`main/attempts`* directory

- *`commandRepl.c - almost entirely functional`*:
    - Overview: Uses `console` submodule of ESP-IDF. Creates a CLI in which command strings are essentially bound to functions. 

    - **Shortcoming:** Inputs cannot be handled WHILE led task is ongoing. IE. While LED is blinking away, no new commands can be processed...so blinks must be iterative instead of perpetual.
- *`uart_interrupt.c`*
    - Overview: Using event-based interrupts to send commands and trigger LED state change (more efficient than current solution)

    - **Shortcoming:** Undiagnosed error when registering the 'interrupt handle' function. Was unable to find information on setting "interruptBits" which determine "what triggers an interrupt"
- *`viaArduino.c`*
    - Overview: Linked Arduino UNO transmit pin to ESP32 receive pin. Attempted to send commands from UNO...was initially unable to send directly from PC -> ESP32.

    - **Shortcoming:** Though messages appear to be received by ESP32, the characters are "garbled" -> random unicode values. Might be due to disagreeing receive/transmit speeds?

