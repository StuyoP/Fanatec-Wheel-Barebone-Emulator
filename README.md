# Fanatec-Wheel-Barebone-Emulator
Berebone Emulator for Fanatec Wheelbase allowing you to create DIY Steering Wheels with full Button, display, LED support.

This project is quite easy to be built with using standard 70x30mm breadboard, JST-XH 2.54 pitch connector and ATmega238P chip. This keeps price, production time and ease of build to minimum. Also the resulcting emulator has pretty low footprint so it perfectly fits in a Podium Hub Shell, for example.

I already build several emulators, each with different periferals and custom code.

# Preface</h3>
This is advanced project, all emulators I build have different code for reason, which will be exlpained later. For connecting different peripherals you will have to decide yourself how to solder the connectors. I will show some examples here, full articke and video will appear at some point on my channel/website. You will need above average soldering and arduino skills to complete your custom project.

# Credits
This project is based on the below ones and would never be possible without them:
<strong>btClubSportWheel by darknao</strong>- ClubSportWheel USB Controller: https://github.com/darknao/btClubSportWheel
<strong>Arduino_Fanatec_Wheel by lshachar</strong>- A do-it-yourself steering wheel to Fanatec's wheel base:https://github.com/lshachar/Arduino_Fanatec_Wheel

Further it was extended by my Discord mates Alexbox364, NetAlf and yours truly, myself.
Alex's implementation relies on DC-DC step-up and Arduino Nano
<strong>F_Interface_AL Project by Alexbox364</strong>- You want to build your own DIY Fanatec base compatible Steering Wheel? You're at the right place :-) : https://github.com/Alexbox364/F_Interface_AL</br>

# History
Building this interface exists on GitHub for long time, but not without some certain limitations. Main one is you need to powerup the Arduion before wheelbase from 5V, ultimately requirin separate USB cable. You cannot power it from wheelbase +5v because of how the Fanatec interface works.

Mine initial idea was to use DC-DC stepup and upscale the 3.3V coming from base to 5V to powerup the Nano. This did not exaclty work well, beacuse Arduino needs some time to load up and wheelbase sees incompatible wheel and enters  safe mode.

This was easy fix - remove bootloader from Arduino so it loads up faster. After a quick test as we hoped - this worked like a charm! This enabled three of us to proceed with extending the interface further.

I will not get into more detail, better check Alex's repository as it hase it all well explained. He developed custom PCB that houses the Nano, DC-DC step up, voltage level converter, shif-registed for extra inputs. His code has full support built into it and is fairly easy for you to implement whatever periopherals you would like to use.

I myself decided to go to another direction - switch to barebone ATmega328p chip as it works natively at 3.3V, has much smaller footprint and gives me extra pins. As you already figured by now, this also worked quite well and thus we have the current project.

# Arduino Considerations
1. Fanatec wheelbase has two grounds, 3.3V and 5V
2. It powers both power supplies separately - firrst you get 3.3V for the logic, then you get the+5V
3. Both GNDs are connectd wheelside - virutally sensing you have wheel attached
4. Fanatec wheelbase powersup quite quickly and it expects SPI response from wheel. If not recived - enters protected mode
5. Power +5V comes after 3.3V logic from wheel is operational - we cannot use it to power the Arduino
6. Arduino Nano having bootloader takes up ome time to boot, longer than Wheelbase likes - BootLoder for Nano shoudl be removed
7. ATmega328p has native 3.3V support, also slightly more pins exposed
8. ATmega328p's native bootloader is much faster than Nano (for chip only you need it)
9. Using barebone chip removes all the universal breakoust of the Nano thus making footprint smaller
10. ATMega328p is rated for 12MHz at 3.3V, which is the frequency of the Fanatec protocol as well
11. Above being said, it does not necessary mean that the chip cannot operate with 16MHz, it is just more prone to errors. As it is a well know fact, ATmega 328p at 3.3V with 16MHz chips is quite stable, we are able to use the common package sold on Aliexpress for cheap - chip+sockt+crystal 16MHz+condensators. For all my steering wheel this proved to be stable and easier to program, so i went with the defautl package
12. This design working natively at 3.3V does not need logic level shifter
13. All extra devices (display, LEDs) can be powered from the Whellbase 5V
14. Design is pretty simple to custom build as we are interfacing directly with the ATmega328p pins, no need of custom PCB or any other breakouts
15. Using custom 3D printed DIY Fanatec Wheel Plug makes it extremely easy to solder a JST cable directly without modifying the original, expensive one
16. Using all of the above it is very easi to build custom wheel with only part needed from Fanatec being a suitable Quick Release
17. Code should be kept lean - ATMega3289p has troubles handling complicated code thus it is needed to remove unneded stuff

# Custom Fanatec Wheel Plug to JST-XH 2.54pitch Cable
Custom plug by Alexbox364 can be found here: [Fanatec Pin Holder](https://www.thingiverse.com/thing:6271297)

I miself remixed it to be easier to print and use the new QR2 design - will upload soon.

Here is a simple diagram how to connect the plug Pins to the JST 8pin cable:

![FWheelEmu_Plug](https://github.com/StuyoP/Fanatec-Wheel-Barebone-Emulator/blob/main/FWheelEmu_Plug.jpg?raw=true)
<img src=“https://github.com/StuyoP/Fanatec-Wheel-Barebone-Emulator/blob/main/FWheelEmu_Plug.jpg?raw=true” alt=“Subject Pronouns” style=“margin-right: 10px;”/>

# Simplified Diagram
This diagram represents the emulator side. It does not have any peripherals - no buttons, displayes, whatsover. You can use all available pins and connect them by your preference:

![SimplifiedDiagram](https://github.com/StuyoP/Fanatec-Wheel-Barebone-Emulator/assets/98518866/f4e8255f-ecf7-46cf-838a-5286edca4f2f)



