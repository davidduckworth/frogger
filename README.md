# frogger
Frogger video game for a Raspberry Pico and a matching WaveShare HAT Screen
<img width="413" height="299" alt="game-in-play-cropped" src="https://github.com/user-attachments/assets/dc8f2adb-7e61-4f19-a756-3b4784d07df2" />

Hardware Requirements:
1. Raspberry Pi Pico (rp2040) or equivalent (e.g., rp2350 also works, but you change the .ini file).
2. Waveshare HAT display with joystick and buttons (e.g., https://www.amazon.com/waveshare-1-3-inch-LCD-Pico/dp/B092VVCBQP).
Software Requirements:
VSCode, PlatformIO, c++
 
Notes:
This game was first developed by my then-15yo nephew on Kahn Academy Processing JS (https://www.khanacademy.org/computer-programming/uncle-davids-frogger-oop/5254551307862016), which sports a 400x400 canvas. The HAT is only 240x240, so the screen drifts into focus as the frog moves. The library was downloaded from WaveShare's website, and placed into the project as a local project library, to make it simpler for others to develop. The original code ran too slow, so I made the movements skip ahead two for every one move, tripling the speed. 

Future work might include getting it up to speed without jerking, and including more (Kahn Academy) games (especially for the rp2350, which has more memory).


