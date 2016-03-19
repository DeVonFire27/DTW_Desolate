#pragma once
#include "../SGD Wrappers/SGD_AudioManager.h"
#include "../SGD Wrappers/SGD_GraphicsManager.h"
#include "../SGD Wrappers/SGD_InputManager.h"
#include "../SGD Wrappers/SGD_String.h"
#include "../SGD Wrappers/SGD_Key.h"

#include "../SGD Wrappers/SGD_EventManager.h"
#include "../SGD Wrappers/SGD_Event.h"
#include "../SGD Wrappers/SGD_MessageManager.h"
#include "../SGD Wrappers/SGD_Message.h"

#include "CharacterTypes\Medic.h"
#include "CharacterTypes\Cyborg.h"
#include "CharacterTypes\Gunslinger.h"
#include "CharacterTypes\Sniper.h"
#include "CharacterTypes\Brawler.h"

#include "Agents\Player.h"
#include "Agents\NPC.h"
#include "Agents\Character.h"

#include "MessageID.h"
#include "Messages/CreateBulletMsg.h"
#include "Messages/CreateEnemyMsg.h"
#include "Messages/CreateNPCMsg.h"
#include "Messages/CreateHealthPickupMsg.h"
#include "Messages/CreatePlayerMsg.h"
#include "Messages/CreateWeaponMsg.h"
#include "Messages/DestroyEntityMsg.h"
#include "Messages/CreateAbility.h"
#include "Messages\CreateLaserBullet.h"
#include "Messages\CreateHitMarkerMsg.h"

#include "Tile System\World.h"
#include "Tile System\TileLayer.h"
#include "Tile System\Tile.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define MENU_BACKIMAGE_WIDTH 1720.0f
#define MENU_BACKIMAGE_HEIGHT 1080.0f

#define ARCADEBUILD 0 //set to 1 if on arcade, 0 if standard build