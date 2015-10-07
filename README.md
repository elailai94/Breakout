# Breakout
### About
[Breakout](http://en.wikipedia.org/wiki/Breakout_(video_game)) is a simple 2D video game, originally published by Atari in 1976. Breakout is written entirely in C++08 and builts on most UNIX-like operating systems (i.e.: Linux or Mac). The graphical component of the game is implemented using the X11 library. In the game, a layer of bricks lines the top third of the screen. A ball travels across the screen, bouncing off the top and side walls of the screen. When a brick is hit, the ball bounces away and the brick is destroyed. The player loses a turn when the ball touches the bottom of the screen. To prevent this from happening, the player has a movable paddle to bounce the ball upward, keeping it in play.

### Screenshots
#### Splash Screen
![Breakout!_Screenshot1](https://cloud.githubusercontent.com/assets/7763904/10266669/f92b56ec-6a3c-11e5-8610-29d3e0794d0e.png)
#### Gameplay Screen
![Breakout!_Screenshot2](https://cloud.githubusercontent.com/assets/7763904/10266675/29a37944-6a3d-11e5-997c-e8db7661272a.png)

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

> Note: The compilation and execution step can be combined into a single step by running: **make run**.

### Usage
#### Command-Line Options
- **-frame-rate n** is measured in frames per second and controls how often the screen is painted or redrawn. The default frame rate is 30 frames per second.
- **-speed n** is measured in pixels per second and controls how fast the ball moves. The default speed is 3.5 pixels per second.

#### Game Controls
The game can be played using a mouse or a keyboard. At any point in the game, you can switch between the two.

##### Mouse Controls
- **move left** to move the paddle left
- **move right** to move the paddle right
- **click** to release the ball from the paddle

##### Keyboard Controls
- **A** to move the paddle left
- **D** to move the paddle right
- **Space Bar** to release the ball from the paddle
- **Escape** to quit the game

### Bonus Features
- There are two other special types of bricks in addition to the normal bricks (represented by a rectangle with a solid colour fill). The first type of special brick is "elongate" brick (represented by an outline of a rectangle with a smaller black rectangle in it), which makes the paddle twice as long when They are hit by the ball. The second type of special brick is "shorten" brick (represented by a rectangle with a strip of white in the centre of it), which make the ball speed slower by 0.2 pixels per second. However, the "elongate" brick will not make the paddle longer than a third of the screen width. Similarly, the "shorten" brick will not slow the ball slower than 3 pixels per second. There is a 1/50 chance that a "elongate" brick or a "shorten" brick is on the screen.

### Known Issues
- X11 library has memory leaks.

### License
Breakout is licensed under the [MIT license](https://github.com/elailai94/Breakout/blob/master/LICENSE.md).
