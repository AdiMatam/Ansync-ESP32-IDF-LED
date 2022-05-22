# ESP32-LED-Test
 
Ansync Internship Aptitude **Project #1** - Persistent LED control via UART input commands with ESP32 development board  

## Notes
- Used VS Code editor -> ESP-IDF extension for ease of building and flashing  
- File structure based on `examples` directory on IDF repository.

## Implemented Commands
Commands are to be typed into the **serial monitor** (rate set to 115200). Input is processed on-enter (when newline is sent)

- `ledOn` 
- `ledOff` 
- `ledBlink` - pre-defined delay time of 500 milliseconds. 
- `exit`

## How It Works (detailed in code comments)
**`main/main.c`**: Main executable file. Detailed below
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

## Attempted Methods (
### Review `oldCode` directory




## Potential Improvements

## Video Demonstration
- IN PROGRESS...
