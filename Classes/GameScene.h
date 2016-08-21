#ifndef __Game_SCENE_H__
#define __Game_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "ImageTile.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;
using namespace std;


class Game : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(Game);

protected:

	void _callbackCreateLevelMapAnimation(CCNode * pNode);
	void _callbackGameOver(CCNode * pNode);
	void _onTouchTileMap(CCObject* pSender, TouchEventType type);

	UIImageView *_getBackgroundImage();

	void _generateMap(UIImageView *background, int mapSize);

	void _addGameStartText(UIImageView *background);
	void _addGameOverText();
	void _addTimerText(UIImageView *background);
	void _updateTimer(float dt);
	void _updateLives();


	void _playPressSound();
	void _playUnpressSound();

	enum enumGame
	{
		kTimerLabeTag = 1000
	};

	bool lockTouch_;

	int level_;										// чем выше уровень, тем больше клеток на карте
	int lives_;										// количество жизней (ошибок)
	int openTilesLeft_;								// сколько клеток еще осталось открыть
	int timer_;
	ImageTile *curTile_;

};

#endif // __Game_SCENE_H__
