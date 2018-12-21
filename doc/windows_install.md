# Using serial2ti83 / ArduGrayLink under Windows 

## Installation
1.  Download "GTK+" from [SourceForge](http://downloads.sourceforge.net/gladewin32/gtk-2.12.9-win32-2.exe)
2.  Download "TILP2" from [TiCalc.org](https://www.ticalc.org/pub/win/link/tilp.zip)
3.  **Install GTK+ first**, then TILP2 or else TILP2 is'nt going to install (if you have'nt got GTK+).
4.  Upload ArduGrayLink to your Arduino and wire everything up.
5.  Go into the Device Manager and search for your ArduGrayLink / Arduino (Under COM & LPT)
6.  Make an right-click on the device and click on Properties, then port settings and check that "Bits per secound" is set to "9600"
7.  Click on Advanced and there change **"COM Port Number"** to **COM1 to 4** (remember the number!) and **click on OK**
8.  Start TILP click on **File** then **Change Device**
9.  Select **"GrayLink"** as cable, set port to your **"COM Port Number"** and select your calc in the calc dropdown.
10.  Click OK and you can finally send some programs to your calculator! :D
