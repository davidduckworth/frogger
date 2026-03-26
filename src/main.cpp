/**
 * @brief Frogger game for waveshare pico display.
 * @details This game was created by Luke Flecker on ProcessingJS
 * 		Ported to C++ desktop by Uncle David,
 * 		Then proted to waveshare by Uncle David.
 * 		The drivers from waveshare's website run at 5fps,
 *		but the original code ran at 60fps.
 * 		A compromise was to have the code run at 5fps, but increase the 
 *		speed to 3x (default), with button options to run at 1x or 2x.
 * 
 */

// Prevent function name magling between C and C++ library inclusion:
#ifdef __cplusplus
extern "C" {
#endif
#include "LCD_Test.h"   //Examples
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include "LCD_1in3.h"
// #include "Debug.h"
#include <stdlib.h> // malloc() free()
#ifdef __cplusplus
}
#endif
#include <algorithm>
#include <vector>
#include "hardware/watchdog.h"

#define SCREEN_HEIGHT		LCD_1IN3_HEIGHT
#define SCREEN_WIDTH		LCD_1IN3_WIDTH
#define JS_SCREEN_HEIGHT	400
#define JS_SCREEN_WIDTH		400

#define DEFAULT_SPEED_FACTOR 3
int speedFactor = DEFAULT_SPEED_FACTOR; //Make objects move faster

// Key codes to help porting:
enum {
	KEY_Up = 0,
	KEY_Down,
	KEY_Right,
	KEY_Left
};

// Waveshare key codes:
#define WAVESHARE_KEY_A 15 
#define WAVESHARE_KEY_B  17
#define WAVESHARE_KEY_X  19
#define WAVESHARE_KEY_Y  21

#define WAVESHARE_KEY_UP  2
#define WAVESHARE_KEY_DOWN  18
#define WAVESHARE_KEY_LEFT  16
#define WAVESHARE_KEY_RIGHT  20
#define WAVESHARE_KEY_CTRL  3

class FroggerObj;
class Frog;

class Color {
    public:
        Color(int r, int g, int b) : r(r), g(g), b(b) {};
        int r, g, b;
};

namespace js2waveshare {
//js functions that will get ported for waveshare drivers:
	void rect(int x, int y, int height, int width);
	void absRect(int x, int y, int height, int width);
	void noStroke();
	void stroke(int r, int g, int b);
	void fill(int r, int g, int b);
//	void textSize(int size);
	void text(const char *textOut, int x, int y);
	void absText(const char *textOut, int x, int y);
	void fill(Color c);
	void background (int r, int g, int b);

// Waveshare data:
	extern "C" UWORD *BlackImage;
// Offsets to account for screens that are smaller than the code to port:
	extern int _offsetX, _offsetY, _offsetXx10; //offset from origin
  //js bridge client functions to be called by client:
    void screenInit();
    void inputInit();
};
//js bridge functions to be supplied by client:
void draw(); 

using namespace js2waveshare;


//Stub added by duckd, not sure why it was necessary:
bool stdio_init_all(void){
    return true;
}

//.cxx declarations:
/**
 * @brief FroggerObj is all of the objects. 
 * @details They have a location and a speed.
 * 			They also have methods to move, draw, and check for death.
 */
class FroggerObj {
    public:
	/**
	 * @brief Construct a new Frogger Obj object
	 * 
	 * @param x : initial coordinate
	 * @param y : initial coordinate
	 * @param speed : optional horizontal speed, if moving 
	 */
        FroggerObj(int x, int y, int speed=0) : x_(x), y_(y), speed_(speed) {};
		virtual void draw() {}; //virtual methods for late binding
		virtual void move();
		virtual void checkForDeath(){};
    protected:
        int x_, y_;
        int speed_;
};

/**
 * @brief moves the oject at its speed one time-tick times its speed
 */
void FroggerObj::move() {
    this->x_ += this->speed_*speedFactor;
    if(this->x_ <= -25) {
        this->x_ = 400;
    } else if(this->x_ >= 400) {
        this->x_ = -25;
    }
};

/**
 * @brief Frog is the green icon that the gamer must get into a langing
 * 
 */
class Frog: public FroggerObj {
	public:
	/**
	 * @brief Construct a new Frog object
	 * 
	 * @param x : initial coordinate
	 * @param y : initial coordinate
	 */
		Frog(int x, int y): initialX_(x), initialY_(y), FroggerObj(x, y) {};
		void draw(), move(), checkForDeath(), resetFrog(), die(), keyPressed(uint);
        int getX() {return this->x_;};
        int getY() {return this->y_;};
        int getLives() {
			return this->lives_;
		};
		void setY(int y) {this->y_ = y;};
		void home();
    private:
		int frameOdometer = 0;
		int initialX_, initialY_, lives_ = 3, hopSpeed_ = 25;
};

/**
 * @brief Global variable frog
 * @details Easier to keep global for all the checkForDeath methods.
 */
Frog *frog = new Frog(200, 375);	//change y to 225 for debugging, 375 for normal
//Frog *frog = new Frog(200, 225);	//change y to 225 for debugging, 375 for normal

/**
 * @brief Heart class is the heart objects that signify lives of the gamer.
 * @details The original had an graphic, but we are rendering it for this screen. 
 */
class Heart: public FroggerObj {
	public:
	/**
	 * @brief Construct a new Heart object
	 * 
	 * @param x : coordinate (y is same for all heart obj's)
	 */
		Heart(int x): FroggerObj(x, 10) {
        };
        void checkViability(Frog *fr, int i) { //send to heaven if frog has i lives
			if (fr->getLives()<=i) {
				this->speedY_=1*speedFactor;
			}
		}
        void move() {this->y_-=speedY_;}
        void draw() {
			fill(255,0,0); //red
// Draw a heart, line by line:
			absRect(this->x_+10, this->y_+20, 0, 1);
			absRect(this->x_+9, this->y_+19, 2, 1);
			absRect(this->x_+9, this->y_+18, 2, 1);
			absRect(this->x_+8, this->y_+17, 4, 1);
			absRect(this->x_+7, this->y_+16, 6, 1);
			absRect(this->x_+6, this->y_+15, 8, 1);
			absRect(this->x_+5, this->y_+14, 10, 1);
			absRect(this->x_+3, this->y_+13, 14, 1);
			absRect(this->x_+2, this->y_+12, 16, 1);
			absRect(this->x_+1, this->y_+11, 18, 1);
			absRect(this->x_+0, this->y_+10, 20, 1);
			absRect(this->x_+0, this->y_+9, 9, 1);
			absRect(this->x_+0, this->y_+8, 9, 1);
			absRect(this->x_+1, this->y_+7, 8, 1);
			absRect(this->x_+2, this->y_+6, 5, 1);
			absRect(this->x_+4, this->y_+5, 4, 1);
			absRect(this->x_+10, this->y_+9, 10, 1);
			absRect(this->x_+10, this->y_+8, 9, 1);
			absRect(this->x_+11, this->y_+7, 8, 1);
			absRect(this->x_+12, this->y_+6, 5, 1);
			absRect(this->x_+14, this->y_+5, 4, 1);
        }
    private:
        int speedY_=0;
};

// Set up a vectors that you can add objects to (push) like js:
std::vector<FroggerObj *> movingObjects = {};
std::vector<Heart *> hearts = {};

/**
 * @brief Draws frog (here a green square with a face)
 * 
 */
void Frog::draw() {
    stroke(0, 0, 0);
    fill(0, 255, 0);
    rect(this->x_, this->y_, 25, 24);
    noStroke();
    fill(0, 0, 0);
    rect(this->x_+5, this->y_+5, 5, 5);
    rect(this->x_+15, this->y_+5, 5, 5);
    fill(255, 255, 255);
    rect(this->x_+5, this->y_+15, 10, 5);
}

/**
 * @brief Scroll the screen to get the frog more in the center.
		This is for screens that are smaller than the screen in the original design. 
 * 
 */
#define Y_TRIGGER 25
void Frog::home() {
	int vScootFactor = 5; 	//scoots vertically toward center at a speed proportional to 
							// distance from viewable center and inversely proportional to vScootFactor
	int yTrigger = 25;
	if (SCREEN_HEIGHT < 240) yTrigger = 0;
	frameOdometer++;		//increases every frame drawn that has the active frog.
	if (this->y_ + yTrigger >= (JS_SCREEN_HEIGHT - SCREEN_HEIGHT/2)) {  //frog is out of viewable range
		int diff = SCREEN_HEIGHT - JS_SCREEN_HEIGHT - _offsetY;
//		_offsetY += diff/25 - 1;
		if (_offsetY < SCREEN_HEIGHT - JS_SCREEN_HEIGHT) _offsetY += diff/vScootFactor +1;
		else if (_offsetY > SCREEN_HEIGHT - JS_SCREEN_HEIGHT) _offsetY += diff/vScootFactor - 1;
//		_offsetY = SCREEN_HEIGHT - JS_SCREEN_HEIGHT;
	} else if (this->y_ + yTrigger + _offsetY < SCREEN_HEIGHT/2 - yTrigger) {
		int diff = SCREEN_HEIGHT/2 - yTrigger-(this->y_ + yTrigger + _offsetY);
		_offsetY += diff/vScootFactor +1;
	} else if (this->y_ + yTrigger + _offsetY > SCREEN_HEIGHT/2 + yTrigger) {
		int diff = (this->y_ + yTrigger + _offsetY) - (SCREEN_HEIGHT/2 - yTrigger);
		_offsetY -= diff/vScootFactor +1;
	}
	int hScootFactor = 10;	//scoots horizontally toward center at a speed proportional to 
						//  distance from viewable center and inversely proportional to hScootFactor
	int minOffsetX = SCREEN_WIDTH - JS_SCREEN_WIDTH;  //min allowable value of _offsetX
	int maxOffsetX = 0;									// max allowable value of _offsetX
	/*_offsetXx10 */
	if (this->x_ * 10 + _offsetXx10 > 10*SCREEN_WIDTH/2) {	//frog is right of center; offsetX needs to be more negative
		int diff = (this->x_*10 + _offsetXx10) - 10*SCREEN_WIDTH/2;
		if (false) { 
		} else
		if (diff/hScootFactor > 0) { 
			_offsetXx10 -= (diff/hScootFactor +1)/2;
		} else if (frameOdometer % 2 == 0) _offsetXx10 -= diff/hScootFactor +1;
		if (_offsetXx10 < minOffsetX*10) _offsetXx10 = minOffsetX*10;
		_offsetX = _offsetXx10/10;
	} else if (this->x_*10 + _offsetXx10 < 10*SCREEN_WIDTH/2) { //frog is left of center; offsetX needs to be more positive
		int diff = 10*SCREEN_WIDTH/2 - (this->x_*10 + _offsetXx10);
		if (false) { 
		} else
		if (diff/hScootFactor > 0) { 
			_offsetXx10 += (diff/hScootFactor +1)/2;
		} else if (frameOdometer % 2 == 0) _offsetXx10 += diff/hScootFactor +1;
		if (_offsetXx10 > maxOffsetX*10) _offsetXx10 = maxOffsetX*10;
		_offsetX = _offsetXx10/10;
	}
}

/**
 * @brief moves the frog
 * 
 */
void Frog::move() {
	//If he is on a log or turtle, he moves horizontally:
    switch(this->y_) {
        case 200:
        case 125:
            this->x_-=1*speedFactor;
            break;
        case 175:
        case 100:
            this->x_+=1*speedFactor;
            break;
        case 150:
            this->x_+=2*speedFactor;
            break;
    }
    this->y_ = std::clamp(this->y_, 0, 375);
};

/**
 * @brief Reset frog to beginning of course.
 * 
 */
void Frog::resetFrog() {
    this->x_ = this->initialX_;
    this->y_ = this->initialY_;
	_offsetX=(SCREEN_WIDTH-JS_SCREEN_WIDTH)/2;//-85; 
	_offsetXx10=10*(SCREEN_WIDTH-JS_SCREEN_WIDTH)/2;//-85; 
	_offsetY=(SCREEN_HEIGHT-JS_SCREEN_HEIGHT)/2;//-110; //offset from origin
};

/**
 * @brief Frog gets run over or drowned. Get new frog if lives remain.
 * 
 */
void Frog::die() {
    this->resetFrog();
    this->lives_--;
};

/**
 * @brief Move the frog based on what key was pressed.
 * 
 * @param keyCode : the key, which is one of the four direction arrows.
 */
void Frog::keyPressed(uint keyCode) {

     if(keyCode==KEY_Up){
        this->y_-=this->hopSpeed_;
    }
    if(keyCode==KEY_Down){
        this->y_+=this->hopSpeed_;
    }
    if(keyCode==KEY_Right){
        this->x_+=this->hopSpeed_;
    }
    if(keyCode==KEY_Left){
        this->x_-=this->hopSpeed_;
    }
}; //keyPressed

/**
 * @brief Check to see if frog made it to all langings
 * 
 */
class Victory {
	private:
		int timer=25;
		int numLandings=5;
    	int landingSpacing=75;
    	int landingThreshhold=15;
    	int firstLanding=25;
		int frogPos[5] = {false, false, false, false, false};
	public: 
		void check(Frog *fr) {
			int allFrogsCrossed = true;
			for (int i=0; i<numLandings; i++) {
				int targetX = firstLanding + i * landingSpacing;
				if (fr->getX() >= targetX - landingThreshhold && fr->getX() <= targetX + landingThreshhold && fr->getY() == 75) {
					frogPos[i] = true;
					movingObjects.push_back(new Frog(targetX, fr->getY()));
					fr->resetFrog();
				}
				allFrogsCrossed = allFrogsCrossed && frogPos[i] ;
			}
			fr->setY(std::clamp(fr->getY(), 100, 375));
			if(allFrogsCrossed) { //all frogs actually crossed
				timer--;
			}
			if(timer <= 0){
				background(0, 255, 0);
				fill(0, 0, 0);
//	            textSize(30);
	            absText("You Win!", SCREEN_WIDTH/2-60, SCREEN_HEIGHT/2-10);
			}
		}
} victory;

/**
 * @brief Turtle object; 
 * @details All turtles travel at same speed (-1), so no speed param in constructor.
 * 
 */
class Turtle: public FroggerObj {
    public:
	/**
	 * @brief Construct a new Turtle object
	 * 
	 * @param x :initial coordinate
	 * @param y :initial coordinate
	 * @param lethalRange: range for death by drowning 
	 */
    	Turtle(int x, int y, int lethalRange): 
			FroggerObj(x, y, -1), lethalRange_(lethalRange) {
				isLethal_ = (lethalRange_ > 0);
        };
		void draw(), checkForDeath();
    protected:
		int lethalRange_;
		bool isLethal_;
};

/**
 * @brief Draw a turtle.
 * 
 */
void Turtle::draw() {
	fill(255, 0, 0);
	rect(this->x_+5, this->y_+7, 15, 11);
	rect(this->x_+3, this->y_+9, 19, 7);
	fill(0, 255, 0);
	rect(this->x_+6, this->y_+5, 3, 2);
	rect(this->x_+15, this->y_+5, 3, 2);
	rect(this->x_+6, this->y_+18, 3, 2);
	rect(this->x_+15, this->y_+18, 3, 2);
	rect(this->x_+21, this->y_+12, 2, 1);
	rect(this->x_+0, this->y_+10, 3, 5);
};//Turtle.draw()

/**
 * @brief Check to see if frog fell into water from a turtle.
 * 
 */
void Turtle::checkForDeath() {
    if(this->isLethal_ && frog->getX() >= this->x_+15 && frog->getX() <= this->x_+this->lethalRange_ && frog->getY() == this->y_) {
        frog->die(); //falls off turtle
    }
};

/**
 * @brief Upper turtle
 * @details Same as lower turtle, but with different checkForDeath
 */
class UpperTurtle: public Turtle {
	using Turtle::Turtle;
    public:
		void checkForDeath();
};

void UpperTurtle::checkForDeath() {
    if(this->isLethal_ && frog->getX() <= this->x_-15 && frog->getX() >= this->x_-this->lethalRange_ && frog->getY() == this->y_) {
        frog->die(); //falls off turtle
    }
};
/**
 * @brief Car object class
 * 
 */
class Car: public FroggerObj {
    public:
	/**
	 * @brief Construct a new Car object
	 * 
	 * @param x : initial coorinate
	 * @param y : initial coorinate
	 * @param color : color of body
	 * @param speed : horizontal speed
	 */
    	Car(int x, int y, Color color, int speed): FroggerObj(x, y, speed), color_(color) {
            direction_ = speed>0?RIGHT:LEFT;
        };
		void draw(), checkForDeath();
    private:
		Color color_;
        enum {RIGHT, LEFT} direction_;
};

void Car::draw() {
        fill(this->color_);//body
        rect(this->x_+10, this->y_+7, 15, 11); //cabin
        rect(this->x_+(this->direction_==RIGHT?12:8), this->y_+9, 15, 7); //nose
        rect(this->x_+(this->direction_==RIGHT?14:6), this->y_+11, 15, 3);
    fill(255, 217, 0); //bumper and axles
        rect(this->x_+(this->direction_==RIGHT?4:28), this->y_+6, 3, 13);//bumper
        rect(this->x_+(this->direction_==RIGHT?4:24), this->y_+8, 7, 3);
        rect(this->x_+(this->direction_==RIGHT?4:24), this->y_+14, 7, 3);
        rect(this->x_+11, this->y_+5, 3, 2);//axles
        rect(this->x_+21, this->y_+5, 3, 2);
        rect(this->x_+11, this->y_+18, 3, 2);
        rect(this->x_+21, this->y_+18, 3, 2);
    fill(175, 0, 255); //tires
        rect(this->x_+9, this->y_+2, 7, 3);
        rect(this->x_+19, this->y_+2, 7, 3);
        rect(this->x_+9, this->y_+20, 7, 3);
        rect(this->x_+19, this->y_+20, 7, 3);
    fill(0, 0, 0);  //windows
        rect(this->x_+(this->direction_==RIGHT?17:12), this->y_+9, 5, 2);//upper
        rect(this->x_+(this->direction_==RIGHT?17:12), this->y_+14, 5, 2);//lower
        rect(this->x_+(this->direction_==RIGHT?22:10), this->y_+11, 2, 3);//front
};//Car.draw()

void Car::checkForDeath() {
    if(abs(frog->getX() - this->x_) <= 25 && frog->getY() == this->y_) {
        frog->die();//car runs over
    }
};

void Frog::checkForDeath() {
    if(this->x_ <= -25 || this->x_ >= 400){ //frog out of horizontal bounds
        this->die();
    }
};

/**
 * @brief Log object class
 * 
 */
class Log: public FroggerObj {
	public:
	/**
	 * @brief Construct a new Log object
	 * 
	 * @param x : inital coordinate
	 * @param y : inital coordinate
	 * @param length : horizontal length
	 * @param speed : horizontal speed
	 * @param lethalRange : range for which death by drowning occurs
	 */
		Log(int x, int y, int length, int speed, int lethalRange): 
            FroggerObj(x, y, speed), length_(length), lethalRange_(lethalRange) {};
		void draw(), move(), checkForDeath();
    private:
		int length_, lethalRange_;
};

void Log::draw() {
    fill(100, 75, 0);
    rect(this->x_+3, this->y_+3, this->length_, 19);
};

void Log::move() {
    this->x_ += this->speed_*speedFactor;
    if(this->x_ >= 400) {
        this->x_ = -this->length_ - ((this->length_==75)?25:0);
    }
};

void Log::checkForDeath() {
    if(frog->getX() <= this->x_-15 && frog->getX() >= this->x_-this->lethalRange_ && frog->getY() == this->y_) {
        frog->die();
    }
};

class {
	private:
		int timer = 25;
    public:
		void check(Frog *fr) {
			if(fr->getLives()<=-1){ //all frog.lives lost
				this->timer--;
			}
			if(this->timer <= 0){
				background(255, 0, 0);
				fill(0, 0, 0);
//				textSize(30);
				absText("You Lose", SCREEN_WIDTH/2-60, SCREEN_HEIGHT/2-10);
			}
		}
} defeat;

int main(void)
{
// Construct all objects, pushing them onto stack movingObjects:
	movingObjects.push_back(new Car(0, 350, *new Color(255, 50, 50), 2));
	movingObjects.push_back(new Car(200, 350, *new Color(0, 115, 255), 2));
	movingObjects.push_back(new Car(50, 300, *new Color(255, 175, 0), 3));//3
	movingObjects.push_back(new Car(350, 300, *new Color(255, 0, 200), 3));
	movingObjects.push_back(new Car(25, 275, *new Color(0, 255, 50), 1));
	movingObjects.push_back(new Car(250, 275, *new Color(0, 115, 255), 1));//6
	movingObjects.push_back(new Car(350, 325, *new Color(255, 100, 0), -2));
	movingObjects.push_back(new Car(150, 325, *new Color(0, 255, 174), -2));
	movingObjects.push_back(new Car(100, 250, *new Color(255, 0, 0), -3));//9
	movingObjects.push_back(new Car(300, 250, *new Color(170, 255, 0), -3));
    movingObjects.push_back(new Log(0, 175, 75, 1, 50));
    movingObjects.push_back(new Log(125, 175, 75, 1, 50));
    movingObjects.push_back(new Log(250, 175, 75, 1, 50));
    movingObjects.push_back(new Log(375, 175, 75, 1, 50));
    movingObjects.push_back(new Log(0, 150, 125, 2, 50));
    movingObjects.push_back(new Log(175, 150, 125, 2, 50));
    movingObjects.push_back(new Log(350, 150, 125, 2, 50));
    movingObjects.push_back(new Log(25, 100, 100, 1, 25));
    movingObjects.push_back(new Log(150, 100, 100, 1, 25));
    movingObjects.push_back(new Log(275, 100, 100, 1, 25));
    movingObjects.push_back(new Log(400, 100, 100, 1, 25));
	movingObjects.push_back(new Turtle(375, 200, 60));
	movingObjects.push_back(new Turtle(350, 200, 0));
	movingObjects.push_back(new Turtle(325, 200, 0));
	movingObjects.push_back(new Turtle(275, 200, 35));
	movingObjects.push_back(new Turtle(250, 200, 0));
	movingObjects.push_back(new Turtle(225, 200, 0));
	movingObjects.push_back(new Turtle(175, 200, 35));
	movingObjects.push_back(new Turtle(150, 200, 0));
	movingObjects.push_back(new Turtle(125, 200, 0));
	movingObjects.push_back(new Turtle(75, 200, 35));
	movingObjects.push_back(new Turtle(50, 200, 0));
	movingObjects.push_back(new Turtle(25, 200, 0));//12
	movingObjects.push_back(new UpperTurtle(375, 125, 0));
	movingObjects.push_back(new UpperTurtle(350, 125, 85));
	movingObjects.push_back(new UpperTurtle(250, 125, 0));
	movingObjects.push_back(new UpperTurtle(225, 125, 85));
	movingObjects.push_back(new UpperTurtle(125, 125, 0));
	movingObjects.push_back(new UpperTurtle(100, 125, 135));
	movingObjects.push_back(frog);
// Hearts, which should be on top, are pushed last:
    hearts.push_back(new Heart(SCREEN_WIDTH-80));
    hearts.push_back(new Heart(SCREEN_WIDTH-55));
    hearts.push_back(new Heart(SCREEN_WIDTH-30));

	screenInit();
	inputInit();
// Print credits splash screen if a reboot was caused by a button:
    if (watchdog_enable_caused_reboot()) {
	// Use waveshare driver funtions:
		Paint_Clear(BLUE);
//Frogger:
		Paint_DrawString_EN(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-90, "  Frogger", &Font24, BRRED, BLUE);
//Luke:
		Paint_DrawString_EN(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-50,  "   ProcessingJS", &Font16, BRRED, BLUE);
		Paint_DrawString_EN(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-30, "  by Luke Flecker", &Font16, BRRED, BLUE);
		Paint_DrawString_EN(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-10, "     SEAS '29", &Font16, BRRED, BLUE);
//Chad:
		Paint_DrawString_EN(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2+25, "   technical assistance", &Font12, BRRED, BLUE);
		Paint_DrawString_EN(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2+41, "     by Chad Duckworth", &Font12, BRRED, BLUE);
//Uncle David:
		Paint_DrawString_EN(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2+65, "         C++ port", &Font12, BRRED, BLUE);
		Paint_DrawString_EN(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2+81, "       by Uncle David", &Font12, BRRED, BLUE);
		Paint_DrawString_EN(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2+97, "           SEAS '87", &Font12, BRRED, BLUE);
		LCD_1IN3_Display(BlackImage);
		DEV_Delay_ms(2000);
	}
// Loop though checking keys, drawing and moving objects:
	while (1) {
        if(DEV_Digital_Read(WAVESHARE_KEY_UP ) == 0){
			frog->keyPressed(KEY_Left);
		} 
		if (DEV_Digital_Read(WAVESHARE_KEY_DOWN ) == 0){
			frog->keyPressed(KEY_Right);
		} 
		if (DEV_Digital_Read(WAVESHARE_KEY_LEFT) == 0){
			frog->keyPressed(KEY_Down);
		} 
		if (DEV_Digital_Read(WAVESHARE_KEY_RIGHT) == 0){
			frog->keyPressed(KEY_Up);
		}
        if(DEV_Digital_Read(WAVESHARE_KEY_A ) == 0) {
			Paint_Clear(BLUE);
		    Paint_DrawString_EN(SCREEN_WIDTH/2-80, SCREEN_HEIGHT/2-60, "    1X", &Font24, BRRED, BLUE);
			speedFactor = 1;
			LCD_1IN3_Display(BlackImage);
		    DEV_Delay_ms(500);
		}
        if(DEV_Digital_Read(WAVESHARE_KEY_B ) == 0) {
			Paint_Clear(BLUE);
		    Paint_DrawString_EN(SCREEN_WIDTH/2-80, SCREEN_HEIGHT/2-60, "    2X", &Font24, BRRED, BLUE);
			speedFactor = 2;
			LCD_1IN3_Display(BlackImage);
		    DEV_Delay_ms(500);
		}
        if(DEV_Digital_Read(WAVESHARE_KEY_X ) == 0) {
			Paint_Clear(BLUE);
		    Paint_DrawString_EN(SCREEN_WIDTH/2-80, SCREEN_HEIGHT/2-60, "    3X", &Font24, BRRED, BLUE);
		    Paint_DrawString_EN(SCREEN_WIDTH/2-80, SCREEN_HEIGHT/2-30, "(default)", &Font24, BRRED, BLUE);
			speedFactor = 3;
			LCD_1IN3_Display(BlackImage);
		    DEV_Delay_ms(500);
		}
        if(DEV_Digital_Read(WAVESHARE_KEY_Y ) == 0) {
			Paint_Clear(BLUE);
		    Paint_DrawString_EN(SCREEN_WIDTH/2-58, SCREEN_HEIGHT/2-60, "REBOOT", &Font24, BRRED, BLUE);
			LCD_1IN3_Display(BlackImage);
		    DEV_Delay_ms(500);
			// Arm Watchdog to reboot if not pet w/in 1ms:
			watchdog_enable(1, 1);  
			while(1); // Reboots w/in 1ms
		}

		draw();
		LCD_1IN3_Display(BlackImage);
	}
	return 0;
} //main()

/**
 * @brief Draw/move objects, check for death/victory, and home the screen
 * 
 */
void draw() {
    background(0, 0, 100); //water
    fill(175, 0, 255); //banks
    rect(-1, 225, 401, 25);
    rect(-1, 375, 401, 25);
    fill(0, 0, 0); //road
    rect(-1, 250, 401, 125);
    fill(0, 255, 0);  //landing
	rect(-1, 50, 401, 50);
    fill(0, 0, 100);
    rect(25, 75, 25, 25);
    rect(100, 75, 25, 25);
    rect(175, 75, 25, 25);
    rect(250, 75, 25, 25);
    rect(325, 75, 25, 25);

	for (FroggerObj * objPtr: movingObjects) {
		objPtr->draw();
		objPtr->move();
		objPtr->checkForDeath();
	}
	for (int i=0; i<hearts.size(); i++) {
		hearts[i]->checkViability(frog, i);//send to heaven if frog has i lives
		hearts[i]->draw();
		hearts[i]->move();
	}
    victory.check(frog);
	defeat.check(frog);
	frog->home();
} // draw()

//.cxx:
namespace js2waveshare {
//generic js hidden variables:
	extern int _useStroke;

//more js variables:
	int _useStroke = false;
//	int _fontSize = 12;
	UWORD _fillColor, _strokeColor, _backGroundColor;
	int _offsetX=-80, _offsetY=-160; //offset from origin; used by text() and rect()
	int _offsetXx10= -800, _offsetYx10= -800;
    extern "C" { UWORD *BlackImage;}
};

void js2waveshare::screenInit() {
    DEV_Delay_ms(50);
    if(DEV_Module_Init()!=0){
//        return -1; Can't return a value from void function.
    }
    DEV_SET_PWM(50);
    /* LCD Init */
    LCD_1IN3_Init(HORIZONTAL);
    LCD_1IN3_Clear(WHITE);
    LCD_1IN3_Clear(BLACK);
    
//duckd changed LCD_1IN3_HEIGHT to LCD_1IN3_HEIGHT+1:
    UDOUBLE Imagesize = (LCD_1IN3_HEIGHT+1)*LCD_1IN3_WIDTH*2;
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    // /*1.Create a new image cache named IMAGE_RGB and fill it with black*/
    Paint_NewImage((UBYTE *)BlackImage,LCD_1IN3.WIDTH,LCD_1IN3.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_Clear(BLACK);
    Paint_SetRotate(ROTATE_90);
}

void js2waveshare::inputInit() {
    SET_Infrared_PIN(WAVESHARE_KEY_A);    
    SET_Infrared_PIN(WAVESHARE_KEY_B);
    SET_Infrared_PIN(WAVESHARE_KEY_X);
    SET_Infrared_PIN(WAVESHARE_KEY_Y);
		 
	SET_Infrared_PIN(WAVESHARE_KEY_UP);
    SET_Infrared_PIN(WAVESHARE_KEY_DOWN);
    SET_Infrared_PIN(WAVESHARE_KEY_LEFT);
    SET_Infrared_PIN(WAVESHARE_KEY_RIGHT);
    SET_Infrared_PIN(WAVESHARE_KEY_CTRL);
}

void js2waveshare::fill(int r, int g, int b) {
// Convert from 24 bit RGB (8-8-8-) color to 16 bit RGB (5-6-5) color:
	_fillColor = ((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3);
}

void js2waveshare::fill(Color c) {
	_fillColor = ((c.r&0xF8)<<8)|((c.g&0xFC)<<3)|((c.b&0xF8)>>3);
}

/*
void js2waveshare::textSize(int size) {
	_fontSize = size;
}
/**/

void js2waveshare::text(const char *textOut, int x, int y) {
}

void js2waveshare::absText(const char *textOut, int x, int y) {
    Paint_DrawString_EN(x, y, textOut, &Font24, _fillColor, _backGroundColor);
}

void js2waveshare::stroke(int r, int g, int b) {
	_strokeColor = ((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3);

	_useStroke = true;
}

void js2waveshare::noStroke() {
	_useStroke = false;
}

void js2waveshare::rect(int x, int y, int width, int height) {
    absRect(x+_offsetX, y+_offsetY, width, height);
}

void js2waveshare::absRect(int x, int y, int width, int height) {
	int xStart = x;
	int xEnd = x+width;
	int yStart = y;
	int yEnd = y+height;
	if (xStart>SCREEN_WIDTH||xEnd<1||yStart>SCREEN_HEIGHT||yEnd<1) return;
	if (xStart<1) xStart = 1;
	if (yStart<1) yStart = 1;
	if (xEnd>SCREEN_WIDTH) xEnd = SCREEN_WIDTH;
	if (yEnd>SCREEN_HEIGHT) yEnd = SCREEN_HEIGHT;
	Paint_DrawRectangle(xStart, yStart, xEnd, yEnd, _fillColor, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

void js2waveshare::background (int r, int g, int b) {
	_backGroundColor = ((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3);
    Paint_Clear(_backGroundColor);
}
