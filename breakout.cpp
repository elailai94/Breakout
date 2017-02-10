//==============================================================================
// Breakout!
//
// @description: An implementation of Breakout in C++ and Xlib
// @author: Elisha Lai
// @version: 1.0 24/09/2015
//==============================================================================


// Header files for X functions
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <unistd.h>

using namespace std;

const int BufferSize  = 10;
int FPS               = 30;
double BallSpeed      = 3.5;

enum {White=0, Black, Red, Orange, Yellow,
      Green, DarkGreen, Blue, NavyBlue, DarkMagenta}; // Available colours

// Function declarations
template <typename T>
string toString(const T& t);

// Information to draw on the window.
struct XInfo {
   Display	     *display;
   int		       screen;
   Window	       window;
   GC		         gcList[2];
   unsigned long colourList[10];
   
   Pixmap	       pixmap;    // Double buffer
   int		       width;     // Width of pixmap
   int		       height;    // Height of pixmap

   // Bricks region margin dimensions
   int           sideMargin;
   int           topMargin;
   int           bottomMargin;

   // Bricks region dimensions in terms of number of bricks
   int           rowSize;
   int           colSize;

   int           gapDist;   // Gap distance between bricks
};

// Abstract class representing displayable things.
class Displayable {
public:
   virtual ~Displayable() {} // Destructor
   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) = 0;
   virtual void paint(XInfo &xinfo) = 0;
};    

// Background displayable.
class Background: public Displayable {
public:
   Background() {} // Constructor

   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) {
   } // resize

   virtual void paint(XInfo &xinfo) {
      XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[NavyBlue]);
      XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
   	 0, 0, (xinfo.width / 4), xinfo.height);
   } // paint
};

// Title displayable.
class Title: public Displayable {
   int         x;
   int         y;
   XFontStruct *font;
   string      title;
   Display     *display;

public:
   Title(int x, int y, Display *display, string title)
      : x(x), y(y), display(display), title(title) {
      font = XLoadQueryFont(display, "12x24");
   } // Constructor

   virtual ~Title() {
      XFreeFont(display, font);
   } // Destructor

   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) {
   } // resize

   virtual void paint(XInfo &xinfo) {
      XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[White]);
      XSetFont(xinfo.display, xinfo.gcList[1], font->fid);

      XDrawString(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
       	 x, y, title.c_str(), title.length());
   } // paint
};

// Subtitle displayable.
class Subtitle: public Displayable {
   int         x;
   int         y;
   XFontStruct *font;
   string      title;
   Display     *display;

public:
   Subtitle(int x, int y, Display *display, string title)
      : x(x), y(y), display(display), title(title) {
      font = XLoadQueryFont(display, "10x20");
   } // constructor

   virtual ~Subtitle() {
      XFreeFont(display, font);
   } // Destructor

   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) {
   } // resize

   virtual void paint(XInfo &xinfo) {
      XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[White]);
      XSetFont(xinfo.display, xinfo.gcList[1], font->fid);

      XDrawString(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
       	 x, y, title.c_str(), title.length());
   } // paint
};

// Text displayable.
class Text: public Displayable {
   int         x; 
   int         y;
   XFontStruct *font; 
   string      text;
   Display     *display;

public:
   Text(int x, int y, Display *display, string text)
      : x(x), y(y), display(display), text(text) {
      font = XLoadQueryFont(display, "8x16");
   } // Constructor

   ~Text() {
      XFreeFont(display, font);
   } // Destructor

   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) {
   } // resize

   virtual void paint(XInfo &xinfo) {
      XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[White]);
      XSetFont(xinfo.display, xinfo.gcList[1], font->fid);

      XDrawString(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
         x, y, text.c_str(), text.length());
   } // paint      
};

// Score displayable.
class Score: public Displayable {
   int         x;
   int         y;
   XFontStruct *font;
   int         score;
   int         numBricks;
   Display     *display;

public:
   Score(int x, int y, int numBricks, Display *display)
      : x(x), y(y), numBricks(numBricks), display(display), score(0) {
      font = XLoadQueryFont(display, "10x20");
   } // Constructor

   virtual ~Score() {
      XFreeFont(display, font);
   } // Destructor

   // Returns the score field.
   int getScore() const {
      return score;
   } // getScore

   // Returns the numBricks field.
   int getNumBricks() const {
      return numBricks;
   } // getNumBricks

   // Increments the score by one.
   void incrementScore() {
      score += 1;
   } // incrementScore

   // Decrements the number of bricks by one.
   void decrementNumBricks() {
      numBricks -= 1;
   } // decrementNumBricks

   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) {
   } // resize

   virtual void paint(XInfo &xinfo) {
      XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[White]);
      XSetFont(xinfo.display, xinfo.gcList[1], font->fid);

      string scoreStr = "Score: " + toString(score);
      XDrawString(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
   	 x, y, scoreStr.c_str(), scoreStr.length());
   } // paint
};

// Lives displayable.
class Lives: public Displayable {
   int         x;
   int         y;
   XFontStruct *font;
   int         lives;
   Display     *display;

public:
   Lives(int x, int y, Display *display)
      : x(x), y(y), lives(3), display(display) {
      font = XLoadQueryFont(display, "10x20");
   } // Constructor

   virtual ~Lives() {
      XFreeFont(display, font);
   } // Destructor

   // Returns the lives field.
   int getLives() const {
      return lives;
   } // getLives

   // Decrements the number of lives by one.
   void decrement() {
      lives -= 1;
   } // decrement

   // Resets the number of lives to three.
   void reset() {
      lives = 3;
   } // reset

   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) {
      x = xinfo.width - 90;
   } // resize

   virtual void paint(XInfo &xinfo) {
      XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[White]);
      XSetFont(xinfo.display, xinfo.gcList[1], font->fid);

      string livesStr = "Lives: " + toString(lives);
      XDrawString(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
         x, y, livesStr.c_str(), livesStr.length());
   } // paint
};

// Paddle displayable.
class Paddle: public Displayable {
   int x;
   int y;
   int length;
   int thickness;

public:
   Paddle(int x, int y, int length, int thickness)
      : x(x), y(y), length(length), thickness(thickness) {} // Constructor

   // Returns the x field.
   int getX() const {
      return x;
   } // getX

   // Returns the y field.
   int getY() const {
      return y;
   } // getY

   // Returns the length field.
   int getLength() const {
      return length;
   } // getLength

   // Returns the thickness field.
   int getThickness() const {
   	  return thickness;
   } // getThickness

   // Updates the x field to follow mouse motions.
   void follow(XInfo &xinfo, int newx) {
      if (newx < (length / 2)) {
   	 x = (length / 2);
      } else if (newx > (xinfo.width - (length / 2))) {
   	 x = (xinfo.width - (length / 2));
      } else {
         x = newx;
      } // if
   } // follow

   // Resets the paddle to the middle of the screen.
   void reset(XInfo &xinfo) {
      x = (xinfo.width / 2);
   } // reset

   // Makes the paddle twice as long.
   void grow(XInfo &xinfo) {
      const int newLength = 2 * length;
      if (newLength < (xinfo.width / 3)) {
         length = 2 * length;
      } // if
   } // grow

   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) {
      if (x >= xinfo.width) {
         x *= widthResizeFactor;
      } // if

      y = xinfo.height - 20;
      length = length * widthResizeFactor;
   } // resize

   virtual void paint(XInfo &xinfo) {
      XSetForeground(xinfo.display, xinfo.gcList[1], WhitePixel(xinfo.display, xinfo.screen));
      XSetLineAttributes(xinfo.display, xinfo.gcList[1],
   	 thickness, LineSolid, CapRound, JoinRound);
      XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
     	 x, y, x - (length/2), y);
      XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
     	 x, y, x + (length/2), y);
   } // paint
};

// Brick displayable.
class Brick: public Displayable {
   int   row;
   int   col;
   int   x;
   int   y;
   int   width;
   int   height;
   int   colour;
   char  type;
   bool  hit;

public:
   Brick(int row, int col, int x, int y, int width,
         int height, int colour, char type, bool hit)
      : row(row), col(col), x(x), y(y), width(width),
        height(height), colour(colour), type(type), hit(hit) {} // Constructor

   // Returns the row number of the brick.
   int getRow() const {
      return row;
   } // getRow

   // Returns the column number of the brick.
   int getCol() const {
      return col;
   } // getCol

   // Returns the x-coordinate of the brick.
   int getX() const {
      return x;
   } // getX

   // Returns the y-coordinate of the brick.
   int getY() const {
      return y;
   } // getY

   // Returns the width of the brick.
   int getWidth() const {
      return width;
   } // getWidth

   // Returns the height of the brick.
   int getHeight() const {
      return height;
   } // getHeight

   // Returns the number of lives left of the brick.
   bool getHit() const {
      return hit;
   } // getHit

   // Returns the type of the brick.
   char getType() const {
      return type;
   } // getType

   // Updates the brick's status to being hit.
   virtual void destroyed() {
      hit = true;
   } // destroyed

   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) {
      // Region dimensions in terms of pixels
      const int rWidth = (xinfo.width - (2 * xinfo.sideMargin));
      const int rHeight = (xinfo.height - (xinfo.topMargin + xinfo.bottomMargin));
   
      width = ((rWidth - ((xinfo.rowSize - 1) * xinfo.gapDist)) / xinfo.rowSize);
      height = ((rHeight - ((xinfo.colSize - 1) * xinfo.gapDist)) / xinfo.colSize);

      x = ((col * (width + xinfo.gapDist)) + xinfo.sideMargin);
      y = ((row * (height + xinfo.gapDist)) + xinfo.topMargin);
   } // resize

   virtual void paint(XInfo &xinfo) {
      XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[colour]);
    	if (!hit) {
         if (type == 's') {
            XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
               x, y, width, height);
            XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[White]);
            XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
               (x + 9), y, (width - 18), height);
         } else if (type == 'l') {
            XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
               x, y, width, height);
            XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[Black]);
            XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
               (x + 2), (y + 2), (width - 4), (height - 4));
         } else {
            XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
               x, y, width, height);
         } // if
      } //if
   } // paint
};

// Ball displayable.
class Ball: public Displayable {
   double x;
   double y;
   int    diameter;
   double xVelocity;
   double yVelocity;

   // Checks if the ball hits the side walls.
   bool hitSideWalls(XInfo &xinfo) {
      if (x <= 0 || x >= (xinfo.width - getDiameter())) {
         return true;
      } else {
      	 return false;
      } // if
   } // isHitSideWalls

   // Checks if the ball hits the top wall.
   bool hitTopWall(XInfo &xinfo) {
      if (y <= 0) {
   	 return true;
      } else {
   	 return false;
      } // if
   } // isHitTopWall

   // Checks if the ball hits the bottom wall.
   bool hitBottomWall(XInfo &xinfo) {
      if (y >= (xinfo.height - getDiameter())) {
         return true;
      } else {
   	 return false;
      } // if
   } // isHitBottomWall

   // Checks if the ball hits a brick.
   bool hitBrick(vector<Brick *> &bVector, Paddle &paddle, Score &score,
   	         XInfo &xinfo, int &ballBrickCollisionType) {
      vector<Brick *>::const_iterator begin = bVector.begin();
      vector<Brick *>::const_iterator end = bVector.end();

      while (begin != end) {
         Brick *brick = *begin;
         
         if (!brick->getHit()) {
            // X-coordinates of right end of brick surface
            int brickRightEndX = (brick->getX() + brick->getWidth());
            // Y-coordinates of bottom end of brick surface
            int brickBottomEndY = (brick->getY() + brick->getHeight());
            // X-coordinates of the centre of ball
            int ballCentreX = (x + (diameter / 2));
            // Y-coordinates of the centre of ball
            int ballCentreY = (y + (diameter / 2));
         
            if (brick->getCol() <= (xinfo.colSize - 1)) {
               brickRightEndX += xinfo.gapDist;
            } // if

            if (brick->getRow() <= (xinfo.rowSize - 1)) {
               brickBottomEndY += xinfo.gapDist;
            } // if

            if (((y + diameter) >= brick->getY()) &&
               (y <= brickBottomEndY) &&
               (ballCentreX > brick->getX()) &&
               (ballCentreX <= brickRightEndX)) {  // Bottom of ball hits top of brick?
               brick->destroyed();
               score.incrementScore();
               score.decrementNumBricks();
               if (brick->getType() == 'l') {
                  paddle.grow(xinfo);
               } else if (brick->getType() == 's') {
                  decreaseSpeed();
               } // if
               ballBrickCollisionType = 1;
               return true;
            } else if ((y <= brickBottomEndY) &&
               (y >= brick->getY()) &&
               (ballCentreX > brick->getX()) &&
               (ballCentreX <= brickRightEndX)) {  // Top of ball hits bottom of brick?
               brick->destroyed();
               score.incrementScore();
               score.decrementNumBricks();
               if (brick->getType() == 'l') {
                  paddle.grow(xinfo);
               } else if (brick->getType() == 's') {
                  decreaseSpeed();
               } // if
         	   ballBrickCollisionType = 2;
         	   return true;
            } else if (((x + diameter) >= brick->getX()) &&
               (x <= brickRightEndX) &&
               (ballCentreY > brick->getY()) &&
               (ballCentreY <= brickBottomEndY)) { // Right of ball hits left of brick?
               brick->destroyed();
               score.incrementScore();
               score.decrementNumBricks();
               if (brick->getType() == 'l') {
                  paddle.grow(xinfo);
               } else if (brick->getType() == 's') {
                  decreaseSpeed();
               } // if
               ballBrickCollisionType = 3;
               return true;
            } else if ((x <= brickRightEndX) &&
               (x >= brick->getX()) &&
               (ballCentreY > brick->getY()) && 
               (ballCentreY <= brickBottomEndY)) { // Left of ball hits right of brick?
               brick->destroyed();
               score.incrementScore();
               score.decrementNumBricks();
               if (brick->getType() == 'l') {
                  paddle.grow(xinfo);
               } else if (brick->getType() == 's') {
                  decreaseSpeed();
               } // if
               ballBrickCollisionType = 4;
               return true;
            } // if

         } // if
         begin++;
      } // while

      return false;
   } // isHitBrick

   // Checks if the ball hits the paddle.
   bool hitPaddle(Paddle &paddle, int &ballPaddleCollisionType) {
      // X-coordinates of left end of paddle
      const int paddleLeftX = (paddle.getX() - (paddle.getLength() / 2));
      // X-coordinates of right end of paddle
      const int paddleRightX = (paddle.getX() + (paddle.getLength() / 2));
      // X-coordinates of the centre of ball
      const int ballCentreX = (x + (diameter / 2));
      // Y-coordinates of the centre of ball
      const int ballCentreY = (y + (diameter / 2));

      if (((y + diameter) >= (paddle.getY() - (paddle.getThickness() / 2))) &&
      	 (y <= (paddle.getY() + (paddle.getThickness() / 2))) &&
         (ballCentreX >= paddleLeftX) &&
         (ballCentreX <= paddleRightX)) {                                  // Bottom of ball hits top of paddle? 
      	 ballPaddleCollisionType = 1;
      	 return true;
      } else if ((y <= (paddle.getY() + (paddle.getThickness() / 2))) &&
         (y >= (paddle.getY() - (paddle.getThickness() / 2))) &&
         (ballCentreX >= paddleLeftX) &&
         (ballCentreX <= paddleRightX)) {                                  // Top of ball hits bottom of paddle?      
         ballPaddleCollisionType = 2;
         return true;
      } else if (((x + diameter) >= paddleLeftX) &&
         (x <= paddleRightX) &&
         (ballCentreY >= (paddle.getY() - (paddle.getThickness() / 2))) &&
         (ballCentreY <= (paddle.getY() + (paddle.getThickness() / 2)))) { // Right of ball hits left of paddle?
         ballPaddleCollisionType = 3;
         return true;
      } else if ((x <= paddleRightX) &&
         (x >= paddleLeftX) &&
         (ballCentreY >= (paddle.getY() - (paddle.getThickness() / 2))) && 
         (ballCentreY <= (paddle.getY() + (paddle.getThickness() / 2)))) { // Left of ball hits right of paddle?
         ballPaddleCollisionType = 4;
         return true;
      } else {
      	 return false;
      } // if
   } // isHitPaddle

   // Resets the ball to the middle of the screen and
   // on the paddle.
   void reset(Paddle &paddle, XInfo &xinfo) {
      x = (xinfo.width/2) - (paddle.getThickness()/2);
      y = (xinfo.height - 25 - diameter);
      xVelocity = (BallSpeed/FPS) * cos(M_PI / 4.0);
      yVelocity = (BallSpeed/FPS) * sin(M_PI / 4.0);
   } // reset

public:
   Ball(double x, double y, int diameter)
      : x(x), y(y), diameter(diameter) {
      xVelocity = (BallSpeed/FPS) * cos(M_PI / 4.0);
      yVelocity = (BallSpeed/FPS) * sin(M_PI / 4.0);
   } // Constructor

   // Returns the x field.
   int getX() const {
      return x;
   } // getX

   // Returns the y field.
   int getY() const {
      return y;
   } // getY

   // Returns the radius of the ball.
   int getDiameter() const {
      return diameter;
   } // getDiameter

   // Moves the ball in the right direction.
   void move() {
      x += xVelocity;
      y -= yVelocity;
   } // move

   // Checks whether the ball has collide with bricks and paddle.
   void checkCollision(vector<Brick *> &bVector, Paddle &paddle, Score &score,
                       Lives &lives, XInfo &xinfo, bool &release, bool &quit) {	  
      int ballBrickCollisionType = 0;
      int ballPaddleCollisionType = 0;

      if (score.getNumBricks() == 0) {
         quit = true;
      } else if (hitSideWalls(xinfo)) {
         xVelocity = -xVelocity;
      } else if (hitTopWall(xinfo)) {
         yVelocity = -yVelocity;
      } else if (hitBottomWall(xinfo)) {
         yVelocity = -yVelocity;
         lives.decrement();
         
         if (lives.getLives() == -1) {
            quit = true;
         } else {
            paddle.reset(xinfo);
            reset(paddle, xinfo);
            release = false;
         } // if

      } else if (hitBrick(bVector, paddle, score, xinfo, ballBrickCollisionType)) {
         
         if ((ballBrickCollisionType == 1) ||
            (ballBrickCollisionType == 2)) {
            yVelocity = -yVelocity;
         } else {
            xVelocity = -xVelocity;
         } // if
      
      } else if (hitPaddle(paddle, ballPaddleCollisionType)) {
          
         if ((ballPaddleCollisionType == 1) ||
            (ballPaddleCollisionType == 2)) {
            yVelocity = -yVelocity;
         } else {
            xVelocity = -xVelocity;
         } // if

      } // if
   } // checkCollision

   // Updates the x field to follow mouse motions
   // if the ball isn't release from the paddle.
   void follow(XInfo &xinfo, int newx) {
      x = newx;
   } // follow

   // Decreases the speed of the ball by 0.1.
   void decreaseSpeed() {
      const double newBallSpeed = BallSpeed - 0.2;
      if (BallSpeed > 3) {
         BallSpeed = newBallSpeed;
         xVelocity = (BallSpeed/FPS) * cos(M_PI / 4.0);
         yVelocity = (BallSpeed/FPS) * sin(M_PI / 4.0);
      } // if
   } // decreaseSpeed

   virtual void resize(double widthResizeFactor, double heightResizeFactor, XInfo &xinfo, bool release) {
      const double resizeFactor = min(widthResizeFactor, heightResizeFactor);
      double resizedDiameter = diameter * resizeFactor;

      if (resizedDiameter < 10) {
         resizedDiameter = 10;
      } // if
      diameter = resizedDiameter;

      if (x >= xinfo.width) {
         x *= widthResizeFactor;
      } // if

      if (!release) {
         y = (xinfo.height - 25 - diameter);
      } else {
         if (y >= xinfo.height) {
            y *= heightResizeFactor;
         } // if
      } // if
   } // resize

   virtual void paint(XInfo &xinfo) {
      XSetForeground(xinfo.display, xinfo.gcList[1], xinfo.colourList[White]);
      XFillArc(xinfo.display, xinfo.pixmap, xinfo.gcList[1],
         x, y, diameter, diameter, 0, 360*64);
   } // paint
};

// Function to put out a message on error exits.
void error(string str) {
  cerr << str << endl;
  exit(0);
} // error

// Template function to convert from some Type to a string
// Primarily used for integer to string conversions
template <typename T>
string toString(const T& t) {
   ostringstream oss;
   if (oss << t) {
      return oss.str();
   } else {
      error("String not convertible from Type.");
   } // if
} // toString

// Template function to convert from a string to some Type
// Primarily used for string to integer conversions
template <typename T>
T fromString(const string& s) {
   istringstream iss(s);
   T n;
   if (iss >> n) {
      return n;
   } else {
      error("Type not convertible from string.");
   } // if
} // fromString

// Initializes the graphics context list.
void initGCList(XInfo &xinfo) {
   // Create graphics contexts
   // Graphics context for pixmap
   int i = 0;
   xinfo.gcList[i] = XCreateGC(xinfo.display, xinfo.window, 0, 0);
   XSetBackground(xinfo.display, xinfo.gcList[i], WhitePixel(xinfo.display, xinfo.screen));
   XSetForeground(xinfo.display, xinfo.gcList[i], BlackPixel(xinfo.display, xinfo.screen));
   XSetFillStyle(xinfo.display, xinfo.gcList[i], FillSolid);
   XSetLineAttributes(xinfo.display, xinfo.gcList[i],
      1, LineSolid, CapButt, JoinRound);

   // Graphics context for text, paddle, ball and bricks
   i = 1;
   xinfo.gcList[i] = XCreateGC(xinfo.display, xinfo.window, 0, 0);
   XSetBackground(xinfo.display, xinfo.gcList[i], BlackPixel(xinfo.display, xinfo.screen));
} // initGCList

// Creates a window and initializes X.
void initX(int argc, char *argv[], XInfo &xinfo) {
   XSizeHints hints;
   unsigned long white, black;

   if (argc > 1) {                                  // Optional arguments are specified at the command line?
      // Processes all optional argument specified at the command line
      for (int i = 1; i < argc; ++i) {
         string optionalArgument = argv[i];         // Stores the optional argument read in from command line

         if (optionalArgument == "-frame-rate") {   // Control how often the screen is painted?
            
            FPS = fromString<int>(argv[i + 1]);
            i += 1;

         } else if (optionalArgument == "-speed") { // Control how fast the ball moves?

            BallSpeed = fromString<double>(argv[i + 1]);
            i += 1;

         } // if
      } // for
   } // if

   // Display opening uses the DISPLAY	environment variable.
   // It can go wrong if DISPLAY isn't set, or you don't have permission.	
   xinfo.display = XOpenDisplay("");
   if (!xinfo.display)	{
      error("Can't open display.");
   } // if
	
   // Find out some things about the display you're using.
   xinfo.screen = DefaultScreen(xinfo.display);

   white = XWhitePixel(xinfo.display, xinfo.screen);
   black = XBlackPixel(xinfo.display, xinfo.screen);

   hints.x = 100;
   hints.y = 100;
   hints.min_width = 600;
   hints.min_height = 400;
   hints.width = hints.min_width;
   hints.height = hints.min_height;
   hints.flags = PPosition | PSize | PMinSize;

   xinfo.window = XCreateSimpleWindow( 
      xinfo.display,	                // Display where window appears
      DefaultRootWindow(xinfo.display), // Window's parent in window tree
      hints.x, hints.y,			// Upper left corner location
      hints.width, hints.height,	// Size of the window
      5,			        // Width of window's border
      black,			        // Window border colour
      white);			        // Window background colour

   // Initializes the graphics context list.		
   XSetStandardProperties(
      xinfo.display,		        // Display containing the window
      xinfo.window,		        // Window whose properties are set
      "Breakout!",		        // Window's title
      "Breakout!",			// Icon's title
      None,				// Pixmap for the icon
      argv, argc,			// Applications command line args
      &hints);			        // Size hints for the window

   initGCList(xinfo);

   // Set up colours.
   XColor xcolour;
   Colormap cmap;
   char color_vals[256][256] = 
      {"white", "black", "red", "orange", "yellow",
       "green", "darkgreen", "blue", "navyblue", "darkmagenta"};

   cmap = DefaultColormap(xinfo.display, xinfo.screen);
   for (int i = 0; i < 10; ++i) {
      if (!XParseColor(xinfo.display, cmap, color_vals[i], &xcolour)) {
         cerr << "Bad colour: " << color_vals[i] << endl;
      } // if
      if (!XAllocColor(xinfo.display, cmap, &xcolour)) {
         cerr << "Bad colour: " << color_vals[i] << endl;
      } // if
      xinfo.colourList[i]=xcolour.pixel;
   } // for

   int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
   xinfo.pixmap = XCreatePixmap(xinfo.display, xinfo.window, hints.width, hints.height, depth);
   xinfo.width = hints.width;
   xinfo.height = hints.height;

   // Sets up dimensions for bricks region.
   xinfo.sideMargin = (xinfo.width / 10);
   xinfo.topMargin = xinfo.sideMargin;
   xinfo.bottomMargin = ((xinfo.height / 5) * 3);
   xinfo.rowSize = 15;
   xinfo.colSize = 8;
   xinfo.gapDist = 2;

   // Tells the window manager what input events you want.
   XSelectInput(xinfo.display, xinfo.window, 
      KeyPressMask| ButtonPressMask |
      PointerMotionMask | EnterWindowMask |
      LeaveWindowMask | StructureNotifyMask |
      ExposureMask);

   // Reduces flickering by not painting the background.
   XSetWindowBackgroundPixmap(xinfo.display, xinfo.window, None);

   //Puts the window on the screen.
   XMapRaised(xinfo.display, xinfo.window);
	
   XFlush(xinfo.display);
} // initX

// Sets up the splash screen to be displayed on screen.
void setUpSplashScreenDVector(vector<Displayable *> &dVector, XInfo &xinfo) {
   const int leftColumnTextMargin = 15;
   const int rightColumnTextMargin = (leftColumnTextMargin + (xinfo.width / 4));

   dVector.push_back(new Background());
   dVector.push_back(new Title(leftColumnTextMargin, 190, xinfo.display,
      "Breakout!"));
   dVector.push_back(new Text(rightColumnTextMargin, 25, xinfo.display,
      "Objective:"));
   dVector.push_back(new Text(rightColumnTextMargin, 45, xinfo.display,
      "Destroy all the bricks without letting the ball"));
   dVector.push_back(new Text(rightColumnTextMargin, 65, xinfo.display,
      "touch the bottom of the screen more than three times"));
   dVector.push_back(new Text(rightColumnTextMargin, 95, xinfo.display,
      "How To Play:"));
   dVector.push_back(new Text(rightColumnTextMargin, 115, xinfo.display,
      "Move the paddle left or right using a mouse or keys"));
   dVector.push_back(new Text(rightColumnTextMargin, 135, xinfo.display,
      "on a keyboard to bounce the ball upward"));
   dVector.push_back(new Text(rightColumnTextMargin, 165, xinfo.display,
      "Game Controls:"));
   dVector.push_back(new Text(rightColumnTextMargin, 185, xinfo.display,
      "- Mouse Controls:"));
   dVector.push_back(new Text(rightColumnTextMargin, 205, xinfo.display,
      "Move left - move paddle left"));
   dVector.push_back(new Text(rightColumnTextMargin, 225, xinfo.display,
      "Move right - move paddle right"));
   dVector.push_back(new Text(rightColumnTextMargin, 245, xinfo.display,
      "Click - release ball from paddle"));
   dVector.push_back(new Text(rightColumnTextMargin, 270, xinfo.display,
      "- Keyboard Controls:"));
   dVector.push_back(new Text(rightColumnTextMargin, 290, xinfo.display,
      "A - move paddle left"));
   dVector.push_back(new Text(rightColumnTextMargin, 310, xinfo.display,
      "D - move paddle right"));
   dVector.push_back(new Text(rightColumnTextMargin, 330, xinfo.display,
      "Space Bar - release ball from paddle"));
   dVector.push_back(new Text(rightColumnTextMargin, 350, xinfo.display,
      "Escape - quit game"));
   dVector.push_back(new Text(rightColumnTextMargin, (xinfo.height - 15), xinfo.display,
      "Click anywhere on the screen to start the game"));
} // setUpSplashScreenDVector

// Sets up the bricks region to be displayed on screen.
void setUpBricksRegion(vector<Displayable *> &dVector,
	               vector<Brick *> &bVector, XInfo &xinfo) {
   // Region dimensions in terms of pixels
   const int rWidth = (xinfo.width - (2 * xinfo.sideMargin));
   const int rHeight = (xinfo.height - (xinfo.topMargin + xinfo.bottomMargin));
   
   // Brick dimensions
   const int bWidth = ((rWidth - ((xinfo.rowSize - 1) * xinfo.gapDist)) / xinfo.rowSize);
   const int bHeight = ((rHeight - ((xinfo.colSize - 1) * xinfo.gapDist)) / xinfo.colSize);

   // Initial the pseudo-random number generator seed
   srand(time(0));

   // Adds all the bricks to display vector and brick vector
   for (int i = 0; i < xinfo.colSize; ++i) {
      for (int j = 0; j < xinfo.rowSize; ++j) {
   	 const int x = ((j * (bWidth + xinfo.gapDist)) + xinfo.sideMargin);
   	 const int y = ((i * (bHeight + xinfo.gapDist)) + xinfo.topMargin);
   	 const int brickIndex = ((i * xinfo.rowSize) + j);
         const int randNum = rand() % 50;
         char brickType;
         int brickColour;

         if (randNum == 7) {
            brickType = 'l';
         } else if (randNum == 19) {
            brickType = 's';
         } else {
            brickType = 'n';
         } // if

   	 if (brickIndex <= ((0 * xinfo.rowSize) + 14)) {
   	    brickColour = Red;
   	 } else if (brickIndex <= ((1 * xinfo.rowSize) + 14)) {
   	    brickColour = Orange;
   	 } else if (brickIndex <= ((2 * xinfo.rowSize) + 14)) {
   	    brickColour = Yellow;
   	 } else if (brickIndex <= ((3 * xinfo.rowSize) + 14)) {
   	    brickColour = Green;
   	 } else if (brickIndex <= ((4 * xinfo.rowSize) + 14)) {
   	    brickColour = DarkGreen;
   	 } else if (brickIndex <= ((5 * xinfo.rowSize) + 14)) {
   	    brickColour = Blue;
   	 } else if (brickIndex <= ((6 * xinfo.rowSize) + 14)) {
            brickColour = NavyBlue;
   	 } else {
   	    brickColour = DarkMagenta;
   	 } // if

         Brick *newBrick = new Brick(i, j, x, y, bWidth, bHeight, brickColour, brickType, false);
         dVector.push_back(newBrick);
         bVector.push_back(newBrick);
      } // for
   } // for
} // setUpBricksRegion

// Gets current microseconds.
unsigned long now() {
   timeval tv;
   gettimeofday(&tv, 0);
   return tv.tv_sec * 1000000 + tv.tv_usec;
} // now

// Handles key press events.
void handleKeyPress(Paddle &paddle, Ball &ball, XInfo &xinfo, XEvent &event,
                    bool &release, bool &quit, bool splashScreen) {
   KeySym key;
   char text[BufferSize];

   int i = XLookupString( 
      (XKeyEvent *)&event, // The keyboard event
      text, 		   // Buffer when text will be written
      BufferSize, 	   // Size of the text buffer
      &key, 		   // Workstation-independent key symbol
      0);		   // Pointer to a composeStatus structure (unused)

   if (i == 1) {
      if (key == XK_Escape) {
         quit = true;
      } else if (key == XK_a) {
         if (!splashScreen) {
	    paddle.follow(xinfo, paddle.getX() - (paddle.getLength() / 3));
	    if (!release) {
	       ball.follow(xinfo, paddle.getX() - (ball.getDiameter() / 2));
	    } // if
         } // if
      } else if (key == XK_d) {
         if (!splashScreen) {
            paddle.follow(xinfo, paddle.getX() + (paddle.getLength() / 3));
            if (!release) {
               ball.follow(xinfo, paddle.getX() - (ball.getDiameter() / 2));
            } // if
         } // if
      } else if (key == XK_space) {
	 if ((!release) && (!splashScreen)) {
            release = true;
	 } // if
      } // if
   } // if
} // handleKeyPress

// Handles mouse motion events.
void handleMotion(Paddle &paddle, Ball &ball, XInfo &xinfo,
                  XEvent &event, bool inside, bool release) {
   if (inside) {
      paddle.follow(xinfo, event.xmotion.x);
      if (!release) {
	  ball.follow(xinfo, paddle.getX()-(ball.getDiameter() / 2));
      } // if
   } // if
} // handleMotion

// Handles window resize events.
void handleResize(vector<Displayable *> &dVector, XInfo &xinfo, XEvent &event,
	          bool release) {
   XConfigureEvent xce = event.xconfigure;
   
   if (xce.width != xinfo.width || xce.height != xinfo.height) {
      const double widthResizeFactor = xce.width / double(xinfo.width);
      const double heightResizeFactor = xce.height / double(xinfo.height);
      double resizedGapDist = xinfo.gapDist * widthResizeFactor;
      
      XFreePixmap(xinfo.display, xinfo.pixmap);
      int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
      xinfo.pixmap = XCreatePixmap(xinfo.display, xinfo.window,
	 xce.width, xce.height, depth);
      xinfo.width = xce.width;
      xinfo.height = xce.height;
      xinfo.sideMargin = (xinfo.width / 10);
      xinfo.topMargin = xinfo.sideMargin;
      xinfo.bottomMargin = ((xinfo.height / 5) * 3);

      if (resizedGapDist < 2) {
      	 resizedGapDist = 2;
      } // if
      xinfo.gapDist = resizedGapDist;

      vector<Displayable *>::const_iterator begin = dVector.begin();
      vector<Displayable *>::const_iterator end = dVector.end();

      while (begin != end) {
         Displayable *d = *begin;
         d->resize(widthResizeFactor, heightResizeFactor, xinfo, release);
         begin++;
      } // while
   } // if
} // handleResize

// Handles animation.
void handleAnimation(vector<Brick *> &bVector, Paddle &paddle, Ball &ball,
	             Score &score, Lives &lives, XInfo &xinfo, bool &release,
                     bool &quit) {
   if (release) {
      ball.move();
      ball.checkCollision(bVector, paddle, score, lives, xinfo, release, quit);
   } // if
} // handleAnimation

// Repaints all displayables in the display vector.
void repaint(vector<Displayable *> &dVector, XInfo &xinfo) {
   vector<Displayable *>::const_iterator begin = dVector.begin();
   vector<Displayable *>::const_iterator end = dVector.end();

   // Draws into the buffer.
   XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gcList[0], 
      0, 0, xinfo.width, xinfo.height);

   while (begin != end) {
      Displayable *d = *begin;
      d->paint(xinfo);
      begin++;
   } // while
   
   // Copies buffer to window
   XCopyArea(xinfo.display, xinfo.pixmap, xinfo.window, xinfo.gcList[0],
      0, 0, xinfo.width, xinfo.height,  // Region of pixmap to copy
      0, 0);                            // Position to put top left corner of pixmap in window

   XFlush(xinfo.display);
} // repaint

// Returns heap-allocated memory for display vector.
void cleanUp(vector<Displayable *> dVector) {
   vector<Displayable *>::const_iterator begin = dVector.begin();
   vector<Displayable *>::const_iterator end = dVector.end();

   while (begin != end) {
      Displayable *d = *begin;
      delete d;
      begin++;
   } // while
} // cleanUp

// Responds to events from the user.
void eventLoop(XInfo &xinfo) {
   vector<Displayable *> splashScreenDVector; // Vector of displayables for splash screen

   // Add stuff to paint to the splash screen display vector
   setUpSplashScreenDVector(splashScreenDVector, xinfo);

   vector<Displayable *> gameScreenDVector;   // Vector of displayables for game screen
   vector<Brick *> bVector;                   // Vector of bricks
   
   // Add stuff to paint to the game screen display vector
   Paddle *paddle = new Paddle(xinfo.width/2, xinfo.height - 20, xinfo.width/10, 10);
   Ball *ball = new Ball((xinfo.width/2) - (paddle->getThickness()/2),
      (xinfo.height - 35), paddle->getThickness());
   Score *score = new Score(10, 25, (xinfo.rowSize * xinfo.colSize), xinfo.display);
   Lives *lives = new Lives(xinfo.width - 90, 25, xinfo.display);
   gameScreenDVector.push_back(paddle);
   gameScreenDVector.push_back(ball);
   gameScreenDVector.push_back(score);
   gameScreenDVector.push_back(lives);
   setUpBricksRegion(gameScreenDVector, bVector, xinfo);

   XEvent event;
   bool inside = false;
   bool release = false;
   bool splashScreen = true;
   bool quit = false;

   while(!quit) {
      unsigned long startTime = now();

      if (XPending(xinfo.display) > 0) {
         XNextEvent(xinfo.display, &event);
         switch(event.type) {
	    case KeyPress:
	       handleKeyPress(*paddle, *ball, xinfo, event, release, quit, splashScreen);
	       break;
	    case ButtonPress:
	       if (splashScreen) {
	          splashScreen = false;
	       } else {
		  release = true;
	       } // if
	       break;
	    case EnterNotify:
	       inside = true;
	       break;
	    case LeaveNotify:
	       inside = false;
	       break;
	    case MotionNotify:
	       if (!splashScreen) {
		  handleMotion(*paddle, *ball, xinfo, event, inside, release);
	       } // if
	       break;
	    case ConfigureNotify:
	       if (splashScreen) {
	          handleResize(splashScreenDVector, xinfo, event, release);
	       } else {
	          handleResize(gameScreenDVector, xinfo, event, release);
	       } // if
               break;
            case Expose:
               if (event.xexpose.count == 0) {
               	  if (splashScreen) {
               	     repaint(splashScreenDVector, xinfo);
               	  } else {
               	     repaint(gameScreenDVector, xinfo);
               	  } // if
               } // if
               break;
         } // switch
      } // if

      if (splashScreen) {
         repaint(splashScreenDVector, xinfo);
      } else {
         handleAnimation(bVector, *paddle, *ball, *score, *lives, xinfo, release, quit);
         repaint(gameScreenDVector, xinfo);
      } // if
	  
      if (XPending(xinfo.display) == 0) {
         usleep(startTime + (1000000/FPS) - now());
      } // if
   } // while

   cleanUp(splashScreenDVector);
   cleanUp(gameScreenDVector);
} // eventLoop

// Start executing here.
// First initialize window.
// Next loop responding to events.
// Exit forcing window manager to clean up - cheesy, but easy.
int main (int argc, char *argv[]) {
   XInfo xinfo;
   initX(argc, argv, xinfo);
   eventLoop(xinfo);
   XFreeGC(xinfo.display, xinfo.gcList[0]);
   XFreeGC(xinfo.display, xinfo.gcList[1]);
   XFreePixmap(xinfo.display, xinfo.pixmap);
   XUnmapWindow(xinfo.display, xinfo.window);
   XDestroyWindow(xinfo.display, xinfo.window);
   XCloseDisplay(xinfo.display);
} // main
