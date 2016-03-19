#include "globals.h"

#include "TileLayer.h"
#include "Tile.h"
#include "World.h"
#include <Windows.h>


void TileLayer::Render() const
{
	SGD::GraphicsManager* gManager = SGD::GraphicsManager::GetInstance();
	int tileNum = 0;
	SGD::Rectangle cameraRect = World::GetInstance()->GetCamera()->GetRect();

	int renderCalls = 0;

	for (int r = 0; r < layerRows; r++)
	{
		for (int c = 0; c < layerColumns; c++)
		{
			float left = tiles[tileNum]->X * tileSize.width;
			float top = tiles[tileNum]->Y * tileSize.height;
			float right = tiles[tileNum]->X * tileSize.width + tileSize.width;
			float bottom = tiles[tileNum]->Y * tileSize.height + tileSize.height;

			if (cameraRect.IsIntersecting	(
				SGD::Rectangle	(	
									c * tileSize.width, 
									r * tileSize.width, 
									c * tileSize.width + tileSize.width, 
									r * tileSize.height + tileSize.height)
								) 
								&& tiles[tileNum]->X != -1)
			{
				gManager->DrawTextureSection(
					tileTexture,
					SGD::Point(c * tileSize.width, r * tileSize.height),
					SGD::Rectangle(left, top, right, bottom)
						);

				renderCalls++;
			}

			tileNum++;
		}
	}
}

Tile* TileLayer::GetTileAt(int x, int y)
{
	if (x >= layerColumns || y >= layerRows || x < 0 || y < 0)
		return nullptr;

	return tiles[x + y * layerColumns];
}

TileLayer::TileLayer(SGD::Size tileSize, int layerColumns, int layerRows, string tilePath, string filePath, int tileRows, int tileColumns)
{
	this->tileSize = tileSize;
	this->layerColumns = layerColumns;
	this->layerRows = layerRows;
	this->tileTexture = SGD::GraphicsManager::GetInstance()->LoadTexture(tilePath.c_str());
	this->texturePath = filePath;
	this->tileRows = tileRows;
	this->tileColumns = tileColumns;
}

TileLayer::~TileLayer()
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(tileTexture);
}

TileType TileLayer::GetTileType(int X, int Y)
{
	return TileType(tiles[X + Y * layerColumns]->X + tiles[X + Y * layerColumns]->Y * tileColumns);
}