/*
 * ImageTile.h
 *
 *  Created on: 12.07.2016
 *      Author: User
 */

#ifndef __ImageTile_H__
#define __ImageTile_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Constant.h"


USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;
using namespace std;


class ImageTile : public UIImageView {
	
public:

	enum enumImageTilePub
	{
		kTileBackTag = 0,
		kTileFaceTag,
		kTileFaceFixedTag,
		kTileBackFixedTag,
	};

	ImageTile();

	static ImageTile *create(int tileNum);

	virtual ~ImageTile();


	int getTileStatus();
	void setTileStatus(int tileStatus);
	void changeTileStatus();

	void setTileNum(int tileNum);
	int getTileNum();

	void setMaxScale(float maxScale);
	void changeTileStatusDelay(float delay);
	void setTileFixColor();
	void setTileErrorColor();

protected:

	void _addTileSprite();
	void _callbackTileStatusDelay(CCNode * Node);

	int tileStatus_;			// статус тайла 0 - рубашка, 1 - картинка, 2 - фиксированная картинка
	int tileNum_;				// условный номер картинки
	float maxScale_;
	int type_;
	
	enum enumImageTile
	{
		kTileSpriteTag,
	};

};

#endif // __ImageTile_H__
