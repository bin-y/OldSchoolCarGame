#include "PlayScene.h"
#include "NewRecordScene.h"
#include "HighScoresScene.h"
#include "Common.h"
#include <string>
#include <algorithm>

//x will not display but treated as part of car when checking spawnable and passable positions
const char Play::_car[CAR_HEIGHT][CAR_WIDTH] = {
	{ ' ', '*', ' ' },
	{ '*', '+', '*' },
	{ 'x', '*', 'x' },
	{ '*', '*', '*' }
};

USING_NS_CC;

using OSCG::Common;
using std::string;
using namespace std::placeholders;

Scene* Play::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Play::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool Play::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	std::srand(time(NULL));

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	_screenCenterX = Director::getInstance()->getWinSize().width / 2;
	
	{
		_scoreLabel = Common::getLCDLabel("00000000", 32);
		auto staticScoreLabel = Common::getLCDLabel("score:", 32);
		staticScoreLabel->setPosition(
			Vec2(origin.x, origin.y + visibleSize.height)
			+ Vec2(
			staticScoreLabel->getContentSize().width / 2,
			-staticScoreLabel->getContentSize().height
			)
			);

		_scoreLabel->setPosition(
			origin 
			+ visibleSize
			- Vec2(
			_scoreLabel->getContentSize().width / 2,
			_scoreLabel->getContentSize().height
			)
			);
		this->addChild(staticScoreLabel, 1);
		this->addChild(_scoreLabel, 1);
	}

	memset(_road, ' ', sizeof(_road));
	_roadLabel = Common::getLCDLabel("", 16);
	_carLabel = Common::getLCDLabel("", 16);
	_carLabel->setString(
		" * \n"\
		"*#*\n"\
		" # \n"\
		"***");

	_charSize.width = _carLabel->getContentSize().width / CAR_WIDTH;
	_charSize.height = _carLabel->getContentSize().height / CAR_HEIGHT;

	_score = 0;
	_speed = 0;
	_carPositionX = (ROAD_WIDTH / 2) - (CAR_WIDTH / 2);
	moveForward(10);
	

	_roadLabel->setPosition(
		Vec2(
		origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2 - _scoreLabel->getContentSize().height
		)
		);
	
	_carLabel->setPosition(
		_roadLabel->getPosition()
		- (_roadLabel->getContentSize() / 2)
		+ (_carLabel->getContentSize() / 2)
		);
	_carLabel->setPositionX(_carLabel->getPositionX() + _charSize.width * (_carPositionX + 1));
	// add the label as a child to this layer
	this->addChild(_roadLabel, 1);
	this->addChild(_carLabel, 1);

	// add LCD background
	{
		auto rect = Rect::ZERO;
		auto screenSize = Director::getInstance()->getWinSize();
		auto sprite = Sprite::create("LCD.png");

		sprite->setScale(visibleSize.width / sprite->getContentSize().width, visibleSize.height / sprite->getContentSize().height);

		// position the sprite on the center of the screen
		sprite->setAnchorPoint(Vec2(0, 0));

		// add the sprite as a child to this layer
		this->addChild(sprite, 0);
	}


	// Create a "one by one" touch event listener (processes one touch at a time)
	auto listener1 = EventListenerTouchOneByOne::create();
	// When "swallow touches" is true, then returning 'true' from the onTouchBegan method will "swallow" the touch event, preventing other listeners from using it.
	listener1->setSwallowTouches(true);

	// Example of using a lambda expression to implement onTouchBegan event callback function
	listener1->onTouchBegan = [this](Touch* touch, Event* event){

		if (touch->getLocationInView().x < _screenCenterX)
		{ 
			_carPositionX--;
			_carLabel->setPositionX(_carLabel->getPositionX() - _charSize.width);
		}
		else
		{
			_carPositionX++;
			_carLabel->setPositionX(_carLabel->getPositionX() + _charSize.width);
		}

		if (_carPositionX > (ROAD_WIDTH - CAR_WIDTH) || !isEnoughtSpaceForCar(_carPositionX, ROAD_HEIGHT - CAR_HEIGHT))
		{
			gameOver();
		}

		return true;
	};

	// Trigger when moving touch
	listener1->onTouchMoved = [](Touch* touch, Event* event){
	};

	// Process the touch end event
	listener1->onTouchEnded = [=](Touch* touch, Event* event){
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	
	return true;
}

void Play::moveForward(float delayTime)
{
	memmove(&_road[ROAD_POSITION(0, 1)], &_road[ROAD_POSITION(0, 0)], sizeof(_road) - ROAD_WIDTH);
	memset(&_road[ROAD_POSITION(0, 0)], ' ', ROAD_WIDTH);

#ifdef _DEBUG
	char debugRoad[] = {
		""
	};
	if (sizeof(debugRoad) > 1)
	{
		memcpy(&_road[ROAD_POSITION(0, 2)], debugRoad, sizeof(debugRoad) - 1);
	}
#endif // _DEBUG
	
	// collision detection 
	if (!isEnoughtSpaceForCar(_carPositionX, ROAD_HEIGHT - CAR_HEIGHT))
	{
		gameOver();
	}

	// update difficulty and spawn car 
	{
		_difficulty = getScoreDifficulty();

		auto spawnCount = cocos2d::random<uint32_t>(
			0,
			std::max<uint32_t>(1, ((ROAD_WIDTH / CAR_WIDTH) - 1) * _difficulty / 100));

		while (spawnCount != 0)
		{
			if (!spawnCar()) break;;
			spawnCount--;
		}
	}

	// update road
	{
		string roadLabelText;


		for (int y = CAR_HEIGHT; y < ROAD_HEIGHT; y++)
		{
			roadLabelText += '|';
			roadLabelText.append(&_road[ROAD_POSITION(0, y)], ROAD_WIDTH);
			roadLabelText += "|\n";
		}
		roadLabelText.pop_back();
		_roadLabel->setString(std::move(roadLabelText));
	}

	// update score
	_score++;
	{
		string scoreText(9, 0);
		sprintf(&scoreText[0], "%08d", _score);
		_scoreLabel->setString(scoreText);
	}

	// update speed
	if (_speed != getScoreSpeed())
	{
		_speed = getScoreSpeed();
		this->unschedule("moveForward");
		float newInterval = 0.50f -(static_cast<float>(_speed) / 100);
		this->schedule(std::bind(&Play::moveForward, this, _1), newInterval, "moveForward");
	}
}

void Play::gameOver()
{
	this->unschedule("moveForward");


	if (Common::addNewRecord(_score))
	{
		Director::getInstance()->pushScene(NewRecord::createScene(_score));
	}
	else Director::getInstance()->pushScene(HighScores::createScene());
}
