#include "HelloWorldScene.h"
#include "HighScoresScene.h"
#include "PlayScene.h"
#include "Common.h"

USING_NS_CC;

using OSCG::Common;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

	auto exitItem = MenuItemLabel::create(Common::getLCDLabel("exit", 24),
		CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
	auto playItem = MenuItemLabel::create(Common::getLCDLabel("play", 24),
		CC_CALLBACK_1(HelloWorld::menuPlayCallback, this));
	auto highscoresItem = MenuItemLabel::create(Common::getLCDLabel("highscores", 24),
		CC_CALLBACK_1(HelloWorld::menuHighscoresCallback, this));

    // create menu, it's an autorelease object
	auto menu = Menu::create(playItem, highscoresItem, exitItem, NULL);
	menu->alignItemsVerticallyWithPadding(50);
	menu->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height * 0.45));
	this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
	auto title = Common::getLCDLabel("Old School Car Game", 32);

    // position the label on the center of the screen
    title->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height * 0.75));
    // add the label as a child to this layer
    this->addChild(title, 1);

    // add LCD background
	auto rect = Rect::ZERO;
	auto screenSize = Director::getInstance()->getWinSize();
    auto sprite = Sprite::create("LCD.png");
	
	sprite->setScale(visibleSize.width / sprite->getContentSize().width,visibleSize.height / sprite->getContentSize().height);

	sprite->setAnchorPoint(Vec2(0, 0));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)

	//MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    //return;
#endif

    Director::getInstance()->end();
	exit(0);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::menuHighscoresCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->pushScene(HighScores::createScene());
	//Director::getInstance()->pushScene(HighScores::createScene());
}

void HelloWorld::menuPlayCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->pushScene(Play::createScene());
}
