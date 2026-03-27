# frogger
Frogger video game for a Raspberry Pico and a matching WaveShare HAT Screen

<img width="244" height="213" alt="game-in-play2" src="https://github.com/user-attachments/assets/9845d7cd-d3cd-4766-9991-a51aee8f20d7" /><img width="240" height="240" alt="Screenshot_20260325_220011-large" src="https://github.com/user-attachments/assets/2c5acc62-b1bd-41d7-a129-0cc7555b4802" />


Hardware Requirements:
1. Raspberry Pi Pico (rp2040) or equivalent (e.g., rp2350 also works, but you change the .ini file).
2. Waveshare 1.3inch/65k-color HAT display with joystick and buttons (e.g., https://www.amazon.com/waveshare-1-3-inch-LCD-Pico/dp/B092VVCBQP).

Software Requirements:
VSCode, PlatformIO, c++
 
Notes:
This game was first developed by my then-15yo nephew on Kahn Academy Processing JS (https://www.khanacademy.org/computer-programming/uncle-davids-frogger-oop/5254551307862016), which sports a 400x400 canvas. The HAT is only 240x240, so the screen drifts into focus as the frog moves. The library was downloaded from WaveShare's website, and placed into the project as a local project library, to simplify development. The ported code ran too slow, so I made the movements skip ahead two for every one move, tripling the speed and adding some jerkiness. 

Future work might include getting it up to speed without jerking, and including even more (Kahn Academy) games (especially for the rp2350, which has more memory).
