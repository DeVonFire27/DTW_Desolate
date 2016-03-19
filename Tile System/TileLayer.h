#pragma once

#include <vector>
using namespace std;

#include "../Camera.h"

#include "../../SGD Wrappers/SGD_GraphicsManager.h"

enum TileType {Empty, Sand, Save, Water, Mountain, MusicTown1, Rock, Building, MusicExploration1};

struct Tile;

class TileLayer
{
	SGD::Size tileSize;
	SGD::HTexture tileTexture;
	string texturePath;
	int tileRows;
	int tileColumns;
public:
	vector<Tile*> tiles;

	int layerColumns;
	int layerRows;	

	SGD::Size GetTileSize() { return tileSize; }
	string GetFilePath() { return texturePath; }
	int GetTileColumns() { return tileColumns; }
	int GetTileRows() { return tileRows; }

	TileType GetTileType(int X, int Y);
	Tile* GetTileAt(int x, int y);

	TileLayer(SGD::Size tileSize, int layerColumns, int layerRows, string tilePath, string texturePath, int tileRows, int tileColumns);
	~TileLayer();
	void Render() const;
	void Exit();
};