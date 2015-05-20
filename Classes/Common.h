#ifndef __OSCG_COMMON_H__
#define __OSCG_COMMON_H__

#include <array>
#include <algorithm>

#define HIGHSCORE_COUNT 10

EXTERN_C void DbgBreakPoint(void);

#include "cocos2d.h"
namespace OSCG{
	class Common
	{
	public:
		static const cocos2d::Color3B _lcdTextColor;

		static inline cocos2d::Label* getLCDLabel(const std::string& text, float fontSize)
		{
			auto label = cocos2d::Label::createWithTTF(
				text,
				"fonts/LCD.ttf",
				fontSize
				);

			label->setColor(_lcdTextColor);
			return label;
		}

		static inline bool addNewRecord(uint32_t score)
		{
			std::array<uint32_t, HIGHSCORE_COUNT> scores;
			loadHighScores(scores);

			if (scores.back() < score)
			{
				scores.back() = score;
				saveHighScores(scores);
				return true;
			}
			return false;
		}

		template<typename _Ty>
		static inline void loadHighScores(_Ty& scores)
		{
			USING_NS_CC;

			char key[(HIGHSCORE_COUNT / 10) + 2];

			for (int i = 0; i < HIGHSCORE_COUNT; i++)
			{
				scores[i] = UserDefault::getInstance()->getIntegerForKey(_itoa(i, key, 10));
			}
			std::sort(scores.begin(), scores.end(), [](int a1, int a2){return a1 > a2; });
		}

		template<typename _Ty>
		static inline void saveHighScores(const _Ty& scores)
		{
			USING_NS_CC;

			char key[(HIGHSCORE_COUNT / 10) + 2];

			for (int i = 0; i < HIGHSCORE_COUNT; i++)
			{
				UserDefault::getInstance()->setIntegerForKey(_itoa(i, key, 10), scores[i]);
			}
		}

		static void breakOnDebugger()
		{
#ifdef _X86
			_asm int 3;
#else
			*reinterpret_cast<int*>(NULL) = 0;
#endif
		}
	};
}

#endif