#pragma once

#include "../SGD Wrappers/SGD_Geometry.h"
#include <d3dx9.h>
#include "Game.h"

//public width and height of the window 800x600
#define SCREENWIDTH (float)Game::GetInstance()->GetScreenWidth()
#define SCREENHEIGHT (float)Game::GetInstance()->GetScreenHeight()

class Camera
{
public:
	//camera will take in position
	Camera(void);
	~Camera() = default;
	Camera(const Camera &c) = delete;
	Camera& operator=(const Camera &c) = delete;

	//getRect function to get the size and position of the camera
	SGD::Rectangle GetRect();

	void Update(float dt);

	//transform matrix is created in GetMatrix
	D3DXMATRIX GetMatrix();
	void	   SetPosition(SGD::Point newPosition);
	SGD::Point GetPosition(void) const { return cameraPosition; }
	//function to be called to shake the camera when player is shot or when there are large explosions
	//parameter is to define the intensity of the shake you are looking for. Duration represents the 
	// ammount of time the effect will be applied
	void CameraShake(int shakeMagnitude, float duration);
	void UpdateCameraShake();
private:
	//position.x, y, scale, DirectX transform matrix?
	SGD::Point cameraPosition;
	SGD::Vector cameraShake_offset;
	D3DXMATRIX transformMatrix;
	D3DXMATRIX ScalingMatrix;
	float shake_timer = 0.f;
	int shake_magnitude = 0;

	// annon struct to store values for camera easing
	struct
	{
		// the location the easing is currently at
		SGD::Vector cameraLook_easing = SGD::Vector::Zero;
		// the location the camera is currently easing towards
		SGD::Vector cameraLook_current = SGD::Vector::Zero;

		// the ammount to incrament/decrament 'applied'
		float easing_bias;
		// the max value for 'applied'
		float easing_threshld;
		// the maximum distance the camera can be offset by when 
		// using easing
		float easing_maxValue;

		float applied = 0.f;


	} cameraOffsetInfo;
	//start render call, set matrix, whenever we do things in camera space
	//1 0 0 x
	//0 1 0 y
	//0 0 1 z
	//0 0 0 1

	//camera shake, 
};