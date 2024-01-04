# Fanatec-Wheel-Barebone-Emulator
Berebone Emulator for Fanatec Wheelbase allowing you to create DIY Steering Wheels with full Button, display, LED support.<br />
This project is quite easy to be built with using standard 70x30mm breadboard, JST-XH 2.54 pitch connector and ATmega238P chip. This keeps price, production time and ease of build to minimum. Also the resulcting emulator has pretty low footprint so it perfectly fits in a Podium Hub Shell, for example.<br />
I already build several emulators, each with different periferals and custom code.<br />

<h3>Preface</h3>
This is advanced project, all emulators I build have different code for reason, which will be exlpained later. For connecting different peripherals you will have to decide yourself how to solder the connectors. I will show some examples here, full articke and video will appear at some point on my channel/website. You will need above average soldering and arduino skills to complete your custom project.

<h3>Credits</h3>
This project is based on the below ones and would never be possible without them:<br />
<strong>btClubSportWheel by darknao</strong>- ClubSportWheel USB Controller: https://github.com/darknao/btClubSportWheel</b>
<strong>Arduino_Fanatec_Wheel by lshachar</strong>- A do-it-yourself steering wheel to Fanatec's wheel base:https://github.com/lshachar/Arduino_Fanatec_Wheel</b>
</b>
Further it was extended by my Discord mates Alexbox364, NetAlf and yours truly, myself. </b>
Alex's implementation relies on DC-DC step-up and Arduino Nano:</b>
<strong>F_Interface_AL Project by Alexbox364</strong>- You want to build your own DIY Fanatec base compatible Steering Wheel? You're at the right place :-) : https://github.com/Alexbox364/F_Interface_AL</b>

<h3>History</h3>
Building this interface exists on GitHub for long time, but not without some certain limitations. Main one is you need to powerup the Arduion before wheelbase from 5V, ultimately requirin separate USB cable. You cannot power it from wheelbase +5v because of how the Fanatec interface works. </b>
Mine initial idea was to use DC-DC stepup and upscale the 3.3V coming from base to 5V to powerup the Nano. This did not exaclty work well, beacuse Arduino needs some time to load up and wheelbase sees incompatible wheel and enters  safe mode.</br>
This was easy fix - remove bootloader from Arduino so it loads up faster. After a quick test as we hoped - this worked like a charm! This enabled three of us to proceed with extending the interface further.</b>
I will not get into more detail, better check Alex's repository as it hase it all well explained. He developed custom PCB that houses the Nano, DC-DC step up, voltage level converter, shif-registed for extra inputs. His code has full support built into it and is fairly easy for you to implement whatever periopherals you would like to use.</br>
I myself decided to go to another direction - switch to barebone ATmega328p chip as it works natively at 3.3V, has much smaller footprint and gives me extra pins. As you already figured by now, this also worked quite well and thus we have the current project.<br>


