# kbsw

tool for switching keyboard layouts in windows via single left shift or control press (first and second layout respectively)

The usual approach of layout switching via ctrl+shift or another key combination has an inconvenience that you have to know the current layout, that is you have to look at tray area and find that tiny little icon with current layout. You may set key combinations for concrete layouts but signle key pres is better that a key combination. Just realize that shift and control keys are never used in isolation. Precious keyboard space is wasted!

Punto switcher has that option but it's not perfect. When you begin to press some key combination (ctrl+t for instance to open a new tab), but in the middle of the process you change your mind and release control without pressing t, the switching happens anyway. That annoys very much and this program was written to fix that very thing. The program activates layout if key up followed key down in a short period of time with no other events in between. Left shift activates first layout, left control activates second layout.

I wrote the program for myself, so it's extremely simple, not tested on others but my machine, under 300 lines of code, no configuration, no ui, does only one task.

## Change Log

### 1.1.0
* keep layout between processes

### 1.0.0
* basic switching functionality

