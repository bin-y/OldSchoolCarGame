#include "HighScoresScene.h"
#include "Common.h"
#include <array>
#include <algorithm>

USING_NS_CC;

using OSCG::Common;
using std::array;

Scene* HighScores::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HighScores::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HighScores::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	array<int, HIGHSCORE_COUNT> scores;
	Common::loadHighScores(scores);
	auto menu = Menu::create();

	for (int i = 0; i < HIGHSCORE_COUNT; i++)
	{
		char scoreTitle[16];
		sprintf(scoreTitle, "%02d. %10u", i + 1, scores[i]);
		menu->addChild(MenuItemLabel::create(Common::getLCDLabel(scoreTitle, 24)));
	}
	// create menu, it's an autorelease object
	menu->alignItemsVerticallyWithPadding(15);
	menu->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height * 0.45));
	menu->setEnabled(FALSE);
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	auto title = Common::getLCDLabel("Highscores", 32);

	// position the label on the center of the screen
	title->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height * 0.8));
	// add the label as a child to this layer
	this->addChild(title, 1);

	// add LCD screen background image
	auto rect = Rect::ZERO;
	auto screenSize = Director::getInstance()->getWinSize();
	auto sprite = Sprite::create("LCD.png");

	sprite->setScale(visibleSize.width / sprite->getContentSize().width, visibleSize.height / sprite->getContentSize().height);

	// position the sprite on the center of the screen
	sprite->setAnchorPoint(Vec2(0, 0));

	// add the sprite as a child to this layer
	this->addChild(sprite, 0);


	//Create a "one by one" touch event listener (processes one touch at a time)
	auto listener1 = EventListenerTouchOneByOne::create();
	// When "swallow touches" is true, then returning 'true' from the onTouchBegan method will "swallow" the touch event, preventing other listeners from using it.
	listener1->setSwallowTouches(true);

	// Example of using a lambda expression to implement onTouchBegan event callback function
	listener1->onTouchBegan = [](Touch* touch, Event* event){
		return true;
	};

	//Trigger when moving touch
	listener1->onTouchMoved = [](Touch* touch, Event* event){
	};

	//Process the touch end event
	listener1->onTouchEnded = [=](Touch* touch, Event* event){
		Director::getInstance()->popToRootScene();
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	return true;
}
