#include "GameScene.h"

USING_NS_CC;

CCScene* Game::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    Game *layer = Game::create();

    // add layer as a child to scene
    scene->addChild(layer);
	layer->setTag(0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Game::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "scene/GUI/CloseNormal.png",
                                        "scene/GUI/CloseSelected.png",
                                        this,
                                        menu_selector(Game::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
								origin.y + +visibleSize.height - pCloseItem->getContentSize().height / 2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...
	srand(time(NULL));
	curTile_ = nullptr;
	lives_ = MAX_LIVES;
	openTilesLeft_ = 0;
	level_ = 0;
	timer_ = 0;

	// загрузка сцены
	CCNode * pNode = SceneReader::sharedSceneReader()->createNodeWithSceneFile("scene/SceneGame.json"); //cocos2dx 2.2
	this->addChild(pNode, 0, 0);

	// установка маштаба
	pNode->setScaleY(visibleSize.height / TOP_EDGE_DESCTOP);
	pNode->setScaleX(visibleSize.width / RIGHT_EDGE_DESCTOP);

	lockTouch_ = true;

	CCNode *bgNode = pNode->getChildByTag(10003);
	UILayer * bgLayer = static_cast<UILayer *>(bgNode->getChildren()->objectAtIndex(0));
	UIImageView *background = static_cast<ImageView *>(bgLayer->getWidgetByName("ImageViewBackground"));
	this->_generateMap(background, level_+1);

    return true;
}


// функция генерирует случайный уровень
void Game::_generateMap(UIImageView *background, int mapSize) {

	background->removeAllChildren();

	lockTouch_ = true;
	lives_ = MAX_LIVES;
	if (mapSize > MAX_TILES)
		mapSize = MAX_TILES;

	int rest = (mapSize * mapSize) % 2;

	int pictureCount = (mapSize * mapSize) / 2;
	openTilesLeft_ = pictureCount * 2;
	std::vector<int> mapNum;

	// создать картинки для области карты
	for (int i = 0; i < pictureCount; i++) {
		mapNum.push_back(i+1);
		mapNum.push_back(i+1);
	}
	// перемешать их
	for (int i = 0; i < (pictureCount * 2); i++) {
		std::swap(mapNum[i], mapNum[rand() % pictureCount]);
	}

	CCArray *arraySeq = CCArray::create();
	int nTag = 0;
	int nMap = 0;

	for (int i = 0; i < mapSize; ++i) {
		for (int j = 0; j < mapSize; ++j) {

			ImageTile *tile = ImageTile::create(0);
			float maxScale = (RIGHT_EDGE_DESCTOP -100) / mapSize / tile->getContentSize().width;
			
			CCPoint leftDownConer = ccp(50 - RIGHT_EDGE_DESCTOP / 2 + tile->getContentSize().width / 2 * maxScale, 50 - TOP_EDGE_DESCTOP / 2 + tile->getContentSize().height / 2 * maxScale);
			tile->setPosition(ccp(leftDownConer.x + tile->getContentSize().width * maxScale * j, leftDownConer.y + tile->getContentSize().height * maxScale * (mapSize - 1 - i)));
			tile->addTouchEventListener(this, toucheventselector(Game::_onTouchTileMap));
			tile->setMaxScale(maxScale);
			background->addChild(tile, 1, nTag);

			if (rest && i + 1 == mapSize && j + 1 == mapSize) {
				tile->setTileNum(0);
			}
			else {
				tile->setTileNum(mapNum[nMap]);
				++nMap;
				tile->changeTileStatus();
				tile->changeTileStatusDelay(2.0f);
			}
			++nTag;

			float duration = 0.2f * (rand() % 4 + 1);
			tile->setScale(0);
			CCActionInterval *scaleToBig = CCScaleTo::create(duration, maxScale);
			CCActionInterval *move_ease = CCEaseBackOut::create(scaleToBig);
			arraySeq->addObject(CCTargetedAction::create(tile, move_ease));
		}
	}

	if (!level_) {
		CCSequence* seq = CCSequence::create(CCSpawn::create(arraySeq), CCCallFuncN::create(this, callfuncN_selector(Game::_callbackCreateLevelMapAnimation)), NULL);
		this->runAction(seq);
		this->_addGameStartText(background);
	}
	else {
		CCSequence* seq = CCSequence::create(CCSpawn::create(arraySeq), CCDelayTime::create(1.5f), CCCallFuncN::create(this, callfuncN_selector(Game::_callbackCreateLevelMapAnimation)), NULL);
		this->runAction(seq);
		this->_addTimerText(background);
	}

}


void Game::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}


// функция обработки обратного вызова анимации создания уровня
// функция добавляет точки А и Б в поле
void Game::_callbackCreateLevelMapAnimation(CCNode * node) {
	lockTouch_ = false;
}


// функция обрабатывает нажатия на клетки уровня
void Game::_onTouchTileMap(CCObject* pSender, TouchEventType type) {

	//if (lockTouch_)
	//	return;

	ImageTile *tile = static_cast<ImageTile *>(pSender);

	if (type == TOUCH_EVENT_BEGAN) {
		this->_playPressSound();
	}

	if (type == TOUCH_EVENT_ENDED && !lockTouch_) {

		this->_playUnpressSound();

		if (curTile_ != tile && lives_ && tile->getTileNum()) {
			tile->changeTileStatus();

			if (!curTile_) {

				if (tile->getTileStatus() != ImageTile::kTileFaceFixedTag) {
					curTile_ = tile;
					tile->setColor(ccWHITE);
				}
			}
			else if (curTile_->getTileNum() == tile->getTileNum()) {
				curTile_->setTileStatus(ImageTile::kTileFaceFixedTag);
				tile->setTileStatus(ImageTile::kTileFaceFixedTag);
				openTilesLeft_ -= 2;
				curTile_ = nullptr;
			}
			else if (tile->getTileStatus() != ImageTile::kTileFaceFixedTag) {
				tile->setTileErrorColor();
				tile->changeTileStatusDelay(0.2f);
				--lives_;
				this->_updateLives();
			}
		}

		if (!openTilesLeft_) {
			++level_;
			this->_generateMap(this->_getBackgroundImage(), level_+1);
		}

		if (!lives_) {
			CCSequence* seq = CCSequence::create(CCDelayTime::create(0.5f), CCCallFuncN::create(this, callfuncN_selector(Game::_callbackGameOver)), NULL);
			this->runAction(seq);
		}

	}

	if (type == TOUCH_EVENT_CANCELED) {
	}
}


// получить фоновый рисунок
// т.е. родителя всех элементов уровня
UIImageView *Game::_getBackgroundImage() {
	
	CCNode *pNode = CCDirector::sharedDirector()->getRunningScene()->getChildByTag(0);
	CCNode *bgNode = pNode->getChildByTag(0)->getChildByTag(10003); // top
	UILayer * bgLayer = static_cast<UILayer *>(bgNode->getChildren()->objectAtIndex(0));
	UIImageView *background = static_cast<ImageView *>(bgLayer->getWidgetByName("ImageViewBackground"));

	return background;
}


void Game::_addGameStartText(UIImageView *background) {
	// добавление текста инициативы
	LabelBMFont	*textLabel = LabelBMFont::create();
	textLabel->setFntFile("fonts/play_100_0.fnt");
	textLabel->setPosition(ccp(0, -120));
	textLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	textLabel->setText("Touch to Start");
	textLabel->setScaleY(1.5f);
	textLabel->setColor(ccBLACK);
	background->addChild(textLabel, 5);

	float duration = 0.5f;
	// анимация мигания текста
	CCAction *actionText = CCRepeatForever::create(
		CCSequence::create(
		CCFadeOut::create(duration),
		CCFadeIn::create(duration),
		0
		));
	//actionText->retain();
	textLabel->runAction(actionText);
}


void Game::_callbackGameOver(CCNode * pNode) {
	this->_addGameOverText();
}

void Game::_addGameOverText() {
	// добавление текста инициативы
	level_ = 0;
	curTile_ = nullptr;
	this->_generateMap(this->_getBackgroundImage(), level_ + 1);

	LabelBMFont	*textLabel = LabelBMFont::create();
	textLabel->setFntFile("fonts/play_100_0.fnt");
	textLabel->setPosition(ccp(0, 400));
	textLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	textLabel->setText("Game Over");
	textLabel->setScaleY(1.5f);
	textLabel->setColor(ccYELLOW);
	this->_getBackgroundImage()->addChild(textLabel, 5);

	float duration = 0.9f;
	// анимация мигания текста
	CCAction *actionText = CCRepeatForever::create(
		CCSequence::create(
		CCTintTo::create(duration, 255, 0, 0),
		//CCDelayTime::create(duration / 2),
		CCTintTo::create(duration, 255, 255, 0),
		0
		));
	textLabel->runAction(actionText);
}


void Game::_playPressSound() {
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/event/eventpress.wav");
}

void Game::_playUnpressSound() {
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/event/eventunpress.wav");
}


void Game::_addTimerText(UIImageView *background) {
	LabelBMFont	*textLabel = LabelBMFont::create();
	textLabel->setFntFile("fonts/play_100_0.fnt");
	textLabel->setPosition(ccp(0, 400));
	textLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	textLabel->setText("3");
	textLabel->setScaleY(1.5f);
	textLabel->setColor(ccBLACK);
	background->addChild(textLabel, 5, kTimerLabeTag);
	timer_ = 2;
	this->schedule(schedule_selector(Game::_updateTimer), 0.5f);
}

void Game::_updateTimer(float dt) {
	
	LabelBMFont	*textLabel = dynamic_cast<LabelBMFont *>(this->_getBackgroundImage()->getChildByTag(kTimerLabeTag));
	if (!textLabel)
		return;

	if (timer_ < 0 ) {
		this->unschedule(schedule_selector(Game::_updateTimer));
		//textLabel->removeFromParent();
		this->_updateLives();
	}
	else {
		textLabel->setText(CCString::createWithFormat("%d", timer_)->getCString());
		--timer_;
	}
}

void Game::_updateLives() {
	LabelBMFont	*textLabel = dynamic_cast<LabelBMFont *>(this->_getBackgroundImage()->getChildByTag(kTimerLabeTag));
	if (!textLabel)
		return;

	textLabel->setText(CCString::createWithFormat("Lives - %d", lives_)->getCString());

}