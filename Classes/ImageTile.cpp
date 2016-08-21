/*
 * ImageTile.cpp
 *
 *  Created on: 12.07.2016
 *      Author: User
 */

#include "ImageTile.h"

ImageTile::ImageTile() :
type_(0),
tileNum_(1),
tileStatus_(0),
maxScale_(1)
{
	// TODO Auto-generated constructor stub
	//CCLog("ImageTile()");
}



ImageTile::~ImageTile() {
	// TODO Auto-generated destructor stub
	//CCLog("~ImageTile()");
}

ImageTile* ImageTile::create(int tileNum)
{
    ImageTile* pointSprite = new ImageTile();
	if (pointSprite && pointSprite->init())
    {
		pointSprite->autorelease();

		//pointSprite->loadTexture("Images/TileBackground.png", UI_TEX_TYPE_PLIST);
		pointSprite->loadTexture("images/TileBackground.png", UI_TEX_TYPE_LOCAL);
		pointSprite->setColor(ccWHITE);
		pointSprite->setTouchEnabled(true);
		pointSprite->_addTileSprite();

		return pointSprite;
    }

	CC_SAFE_DELETE(pointSprite);
    return NULL;
}


int ImageTile::getTileStatus() {
	return tileStatus_;
}

void ImageTile::setTileStatus(int tileStatus) {
	tileStatus_ = tileStatus;

	if (tileStatus == kTileFaceFixedTag)
		this->setTileFixColor();
}

void ImageTile::changeTileStatus() {
	
	if (tileStatus_ > kTileFaceTag)
		return;

	if (tileStatus_ == kTileFaceTag)
		tileStatus_ = kTileBackTag;
	else
		tileStatus_ = kTileFaceTag;

	this->_addTileSprite();
}

void ImageTile::changeTileStatusDelay(float delay) {
	
	CCSequence *seq = CCSequence::create(CCDelayTime::create(delay), CCCallFuncN::create(this, callfuncN_selector(ImageTile::_callbackTileStatusDelay)), nullptr);
	this->runAction(seq);
}


void ImageTile::_callbackTileStatusDelay(CCNode * Node) {
	this->changeTileStatus();
}


void ImageTile::_addTileSprite() {

	UIImageView *tileTemp = static_cast<UIImageView *>(this->getChildByTag(kTileSpriteTag));
	if (tileTemp)
		tileTemp->removeFromParent();

	UIImageView *tileSprite = UIImageView::create();
	if (!tileStatus_) {
		tileSprite->loadTexture("images/TileBack.png", UI_TEX_TYPE_LOCAL);
	}
	else{
		tileSprite->loadTexture(CCString::createWithFormat("images/%d.png", tileNum_)->getCString(), UI_TEX_TYPE_LOCAL);
	}

	//tileSprite->setAnchorPoint(ccp(0.5f, 0.5f));
	//tileSprite->setPosition(ccp(this->getContentSize().width / 2, this->getContentSize().height / 2));
	this->addChild(tileSprite, 1, kTileSpriteTag);



	float duration = 0.2f;
	tileSprite->setScale(0);
	CCActionInterval *scaleToBig = CCScaleTo::create(duration, 1.0f);
	CCActionInterval *move_ease = CCEaseBackOut::create(scaleToBig);
	//CCSequence* seq = CCSequence::create(move_ease, CCCallFuncN::create(this, callfuncN_selector(Game::_callbackCreateLevelMapAnimation)), NULL);
	tileSprite->runAction(move_ease);


}


void ImageTile::setTileNum(int tileNum) {
	tileNum_ = tileNum;
	if (!tileNum_) {
		tileStatus_ = kTileBackFixedTag;
		this->_addTileSprite();
	}
}

int ImageTile::getTileNum() {
	return tileNum_;
}

void ImageTile::setMaxScale(float maxScale) {
	maxScale_ = maxScale;
}


void ImageTile::setTileFixColor() {
	ccColor3B color;
	color.r = 200;
	color.g = 255;
	color.b = 200;
	this->setColor(color);
}

void ImageTile::setTileErrorColor() {
	ccColor3B color;
	color.r = 255;
	color.g = 200;
	color.b = 200;
	this->setColor(color);
}