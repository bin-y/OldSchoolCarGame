#ifndef __PLAY_SCENE_H__
#define __PLAY_SCENE_H__

#include "cocos2d.h"
#include "Common.h"
#include <set>
#include <vector>

#define CAR_HEIGHT 4
#define CAR_WIDTH 3

#define VISIBLE_ROAD_HEIGHT 45
#define VISIBLE_ROAD_WIDTH (VISIBLE_ROAD_HEIGHT * 8 / 10)

#define ROAD_HEIGHT (VISIBLE_ROAD_HEIGHT + CAR_HEIGHT - 1)
#define ROAD_WIDTH VISIBLE_ROAD_WIDTH

#define ROAD_POSITION(_x, _y) (((_y) * ROAD_WIDTH) + (_x))

class Play : public cocos2d::Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(Play);
private:
	char _road[ROAD_HEIGHT * ROAD_WIDTH];
	static const char _car[CAR_HEIGHT][CAR_WIDTH];
	cocos2d::Label *_roadLabel;
	cocos2d::Label *_carLabel;
	cocos2d::Label *_scoreLabel;
	uint32_t _score;
	uint32_t _carPositionX;
	cocos2d::Size _charSize;
	uint32_t _screenCenterX;
	uint32_t _difficulty;
	uint32_t _speed;

	inline bool isEnoughtSpaceForCar(uint32_t x, uint32_t y)
	{
		for (uint32_t offsetY = 0; offsetY < CAR_HEIGHT; offsetY++)
		{
			for (uint32_t offsetX = 0; offsetX < CAR_WIDTH; offsetX++)
			{
				if (_road[ROAD_POSITION(x + offsetX, y + offsetY)] != ' '
					&& _car[offsetY][offsetX] != ' ')
				{
					return false;
				}
			}
		}
		return true;
	}

	inline bool isPositionsContinus(const std::vector<uint32_t>& positions)
	{
		int continusEnd = 0;
		for (; continusEnd < positions.size(); continusEnd++)
		{
			if (positions[continusEnd] != positions[0] + continusEnd)
			{
				return false;
			}
		}

		return true;
	}

	inline void getSpawnablePositions(std::vector<uint32_t>& positions)
	{
		std::vector<uint32_t> linePassablePositions[ROAD_HEIGHT];

		{
			uint32_t y = ROAD_HEIGHT - CAR_HEIGHT;
			for (uint32_t x = 0; x < ROAD_WIDTH - CAR_WIDTH + 1; x++)
			{
				x = getLineContinusPassablePositions(x, y, linePassablePositions[y]);
				if (_carPositionX > x)
				{
					linePassablePositions[y].clear();
				}
				else break;
			}
			for (y = ROAD_HEIGHT - 1; y > ROAD_HEIGHT - CAR_HEIGHT; y--)
			{
				linePassablePositions[y] = linePassablePositions[ROAD_HEIGHT - CAR_HEIGHT];
			}

		}

		uint32_t y = ROAD_HEIGHT - CAR_HEIGHT;


		while (TRUE)
		{
			uint32_t i = 0;

			for (uint32_t x = 0;
				(x < ROAD_WIDTH - CAR_WIDTH + 1) && (i < linePassablePositions[y + 1].size());
				x++)
			{
				std::vector<uint32_t> continusPassablePositions;
				auto xEnd = getLineContinusPassablePositions(x, y, continusPassablePositions);

				if (x <= linePassablePositions[y + 1][i] && linePassablePositions[y + 1][i] <= xEnd)
				{
					auto oldSize = linePassablePositions[y].size();
					linePassablePositions[y].resize(oldSize + continusPassablePositions.size());
					std::copy(continusPassablePositions.begin(), continusPassablePositions.end(), linePassablePositions[y].begin() + oldSize);

					do
					{
						i++;
					} while (i < linePassablePositions[y + 1].size() && linePassablePositions[y + 1][i] <= xEnd);
				}
				x = xEnd;
			}
			if (linePassablePositions[y].size() == 0)
			{
				positions.clear();
				return;
			}

			if (y == 0)break;
			y--;
		}

		std::vector<uint32_t> spawnablePositions;

		for (uint32_t x = 0; x < ROAD_WIDTH - CAR_WIDTH + 1; x++)
		{
			x = getLineContinusPassablePositions(x, 0, spawnablePositions);
		}

		for (y = 0; y < CAR_HEIGHT * 2; y++)
		{
			if (linePassablePositions[y].size() < CAR_WIDTH * 2)
			{
				if (isPositionsContinus(linePassablePositions[y]))
				{
					// exculde these positions from spawnable positions
					if (linePassablePositions[y].size() >= (CAR_WIDTH + 1))
					{
						linePassablePositions[y].erase(linePassablePositions[y].begin());
						linePassablePositions[y].erase(linePassablePositions[y].end() - 1);
					}
					else
					{
						for (int i = 1; i < CAR_WIDTH; i++)
						{
							linePassablePositions[y].push_back(linePassablePositions[y].back() + 1);
						}

						if (linePassablePositions[y].front() != 0)
						{
							for (int i = 1; i < CAR_WIDTH; i++)
							{
								linePassablePositions[y].push_back(linePassablePositions[y].front() - i);
								if (linePassablePositions[y].back() == 0)
									break;
							}
							std::sort(linePassablePositions[y].begin(), linePassablePositions[y].end());
						}
					}

					std::vector<uint32_t> positionDifference;

					positionDifference.resize(spawnablePositions.size());
					auto newEnd = std::set_difference(
						spawnablePositions.begin(),
						spawnablePositions.end(),
						linePassablePositions[y].begin(),
						linePassablePositions[y].end(),
						positionDifference.begin()
						);
					positionDifference.resize(newEnd - positionDifference.begin());
					spawnablePositions = std::move(positionDifference);

					if (spawnablePositions.empty())
					{
						break;
					}
				}
			}
		}
		positions = std::move(spawnablePositions);
	}

	inline decltype(_speed) getScoreSpeed()
	{
		return std::min<decltype(_speed)>(_score / 10, 48) + 1;
	}

	inline decltype(_difficulty) getScoreDifficulty()
	{
		return std::min<decltype(_difficulty)>(_score / 20, 90) + 5;
	}

	inline bool spawnCar()
	{
		std::vector<uint32_t> spawnablePositions;
		getSpawnablePositions(spawnablePositions);
		if (spawnablePositions.size() == 0)
		{
			return false;
		}

		auto spawnPosIndex = cocos2d::random<uint32_t>(0, spawnablePositions.size() - 1);
		auto positionToSpawn = spawnablePositions[spawnPosIndex];

		for (uint32_t y = 0; y < CAR_HEIGHT; y++)
		{
			for (uint32_t x = 0; x < CAR_WIDTH; x++)
			{
				if (_car[y][x] != ' ' && _car[y][x] != 'x')
				{
					_road[ROAD_POSITION(positionToSpawn + x, y)] = _car[y][x];
				}
			}
		}
		return true;
	}

	inline uint32_t getLineContinusPassablePositions(uint32_t x, uint32_t y, std::vector<uint32_t>& positions)
	{
		for (; x < ROAD_WIDTH - CAR_WIDTH + 1; x++)
		{
			if (isEnoughtSpaceForCar(x, y))
				positions.push_back(x);
			else break;
		}
		return x;
	}

	void moveForward(float delayTime);
	void gameOver();
};

#endif
