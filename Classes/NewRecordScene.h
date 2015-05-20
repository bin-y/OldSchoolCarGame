#ifndef __NEWRECORD_SCENE_H__
#define __NEWRECORD_SCENE_H__

#include "cocos2d.h"

class NewRecord : public cocos2d::Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene(uint32_t score);

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	void setScore(uint32_t score);
	// implement the "static create()" method manually
	CREATE_FUNC(NewRecord);
private:
	cocos2d::Label* _scoreLabel;
};

#endif