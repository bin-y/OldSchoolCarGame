#include "HighScoresScene.h"
#include "NewRecordScene.h"
#include "Common.h"
#include <array>
#include <algorithm>

USING_NS_CC;

using OSCG::Common;
using std::array;

Scene* NewRecord::createScene(uint32_t score)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = NewRecord::create();

	layer->setScore(score);
	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool NewRecord::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	_scoreLabel = Common::getLCDLabel("0000000000", 50);
	_scoreLabel->setAlignment(TextHAlignment::CENTER);
	_scoreLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height * 0.45));
	this->addChild(_scoreLabel, 1);

	auto title = Common::getLCDLabel("New Record!", 32);

	// position the label on the center of the screen
	title->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height * 0.6));
	// add the label as a child to this layer
	this->addChild(title, 1);

	// add "HelloWorld" splash screen"
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
		Director::getInstance()->pushScene(HighScores::createScene());
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	return true;
}

void NewRecord::setScore(uint32_t score)
{
	char scoreValue[16];
	sprintf(scoreValue, "%u", score);
	_scoreLabel->setString(scoreValue);
}
