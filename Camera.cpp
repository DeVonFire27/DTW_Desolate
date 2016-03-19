#include "globals.h"
#include "Camera.h"
#include "Math.h"
#include <ctime>
#include <stdlib.h>

Camera::Camera()
{
	cameraOffsetInfo.easing_bias = 0.2f;
	cameraOffsetInfo.easing_threshld = 5.f;
	cameraOffsetInfo.easing_maxValue = 40.f;
}

SGD::Rectangle Camera::GetRect()
{
	SGD::Rectangle tempRect;
	tempRect.left = -cameraPosition.x + cameraOffsetInfo.cameraLook_easing.x;
	tempRect.top = -cameraPosition.y + cameraOffsetInfo.cameraLook_easing.y;
	tempRect.right = tempRect.left + SCREENWIDTH;
	tempRect.bottom = tempRect.top + SCREENHEIGHT;

	return tempRect;
}


void Camera::Update(float dt)
{
	// store a pointer to the player
	Player* player = GameplayState::GetInstance()->player;
	// null check the player
	if (player)
	{
		// find the velocity of the player
		cameraOffsetInfo.cameraLook_current = player->GetCharacter()->GetVelocity();
		// find the distance between the current camera position, and it's easing position
		float distance = (cameraOffsetInfo.cameraLook_current - cameraOffsetInfo.cameraLook_easing).ComputeLength();
		// if distance is greater than one, increace 'applied
		if (distance > 1)
			cameraOffsetInfo.applied += distance / 100000.f;
		else
			// otherwise, decreace applied until it's zero
			cameraOffsetInfo.applied = Math::Floor(cameraOffsetInfo.applied - 0.1f, 0.f);
		// clamp applied between zero and easing_threshold
		cameraOffsetInfo.applied = Math::Clamp(cameraOffsetInfo.applied, 0.f, cameraOffsetInfo.easing_threshld);
		// trim the velocity down to easing_maxValue
		cameraOffsetInfo.cameraLook_current = cameraOffsetInfo.cameraLook_current.Trim(cameraOffsetInfo.easing_maxValue);
		// use linear interpolation to find where the easing should put the camera at this frame
		float easing_x = Math::lerp(cameraOffsetInfo.cameraLook_easing.x, cameraOffsetInfo.cameraLook_current.x, cameraOffsetInfo.applied / cameraOffsetInfo.easing_threshld);
		float easing_y = Math::lerp(cameraOffsetInfo.cameraLook_easing.y, cameraOffsetInfo.cameraLook_current.y, cameraOffsetInfo.applied / cameraOffsetInfo.easing_threshld);
		// store current easing into camera info
		cameraOffsetInfo.cameraLook_easing = { easing_x, easing_y };
	}

	// decrament shake timer by time elapsed
	if (shake_timer > 0.f)
	{
		shake_timer -= dt;
		UpdateCameraShake();
	}
	else // reset camera shake if the timer reaches zero
		cameraShake_offset = { 0.f, 0.f };
}

D3DXMATRIX Camera::GetMatrix()
{
	//generate matrix from position
	D3DXMatrixTranslation(&transformMatrix, cameraPosition.x + cameraShake_offset.x - cameraOffsetInfo.cameraLook_easing.x, cameraPosition.y + cameraShake_offset.y - cameraOffsetInfo.cameraLook_easing.y, 0);

	return transformMatrix;
}

void Camera::CameraShake(int shakeMagnitude, float duration)
{
	// store magnitude
	shake_magnitude = shakeMagnitude;
	// store the duration
	shake_timer = duration;
	// update camera shake
	UpdateCameraShake();
}

void Camera::UpdateCameraShake()
{
	// create a random angle
	float rand_angle = Math::to_radians(float(rand() % 360));
	// create a random length based on 'shakeMagnitude parameter
	float rand_length = float(rand() % shake_magnitude);

	// set the shake offset relitive to the shake value
	cameraShake_offset = SGD::Vector(cos(rand_angle), sin(rand_angle)) * rand_length;

}

void Camera::SetPosition(SGD::Point newPosition)
{
	newPosition.x = (float)(int)newPosition.x;
	newPosition.y = (float)(int)newPosition.y;
	cameraPosition = newPosition;
}
