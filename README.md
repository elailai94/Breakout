# Breakout
### About
[Breakout](http://en.wikipedia.org/wiki/Breakout_(video_game)) is a simple 2D video game, originally published by Atari in 1976. Breakout is written entirely in C++08 and builts on most UNIX-like operating systems (i.e.: Linux or Mac). The graphical component of the game is implemented using the X11 library. In the game, a layer of bricks lines the top third of the screen. A ball travels across the screen, bouncing off the top and side walls of the screen. When a brick is hit, the ball bounces away and the brick is destroyed. The player loses a turn when the ball touches the bottom of the screen. To prevent this from happening, the player has a movable paddle to bounce the ball upward, keeping it in play.

### Compilation
```Bash
make
```

### Clean Build
```Bash
make clean
```

### Execution
```Bash
./breakout
```

### Usage
#### Command-Line Options
- **-frame-rate n** is measured in frames per second and controls how often the screen is painted or redrawn. The default frame rate is 30 frames per second.
- **-speed n** is measured in pixels per second and controls how fast the ball moves. The default speed is 4 pixels per second.

#### Game Controls
The game can be played using a mouse or a keyboard. At any point in the game, you can switch between the two.
##### Mouse Controls
- **move left** to move the paddle left
- **move right** to move the paddle right

### Known Issues
- X11 library has memory leaks.

### License
Breakout is licensed under the [MIT license](https://github.com/elailai94/Breakout/blob/master/LICENSE.md).
