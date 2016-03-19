
#include "globals.h"
#include "AnimationSystem.h"

/**************************************************************/
// GetInstance
//	- allocate static global instance
//	- return THE instance
/*static*/ AnimationSystem* AnimationSystem::GetInstance(void)
{
	static AnimationSystem s_Instance;	// stored in global memory once
	return &s_Instance;
}

void AnimationSystem::AddAnimation(std::string name, Animation& anim)
{
	animations.emplace(name, anim);
}

void AnimationSystem::Update(AnimationTimeStamp& aniTS, float deltaTime)
{
	aniTS.SetTimeOnFrame(aniTS.GetTimeOnFrame() + deltaTime);
	float dur = animations[aniTS.GetCurrAnimation()].frames[aniTS.GetCurrFrame()].GetDuration();
	if (aniTS.GetTimeOnFrame() > dur)
	{
		aniTS.SetTimeOnFrame(aniTS.GetTimeOnFrame() - dur);
		aniTS.SetCurrFrame(aniTS.GetCurrFrame() + 1);

		int size = animations[aniTS.GetCurrAnimation()].frames.size();
		if (aniTS.GetCurrFrame() >= size)
		{
			if (animations[aniTS.GetCurrAnimation()].GetLooping())
				aniTS.SetCurrFrame(0);
			else
			{
				aniTS.SetCurrFrame(size - 1);
				aniTS.SetEnded(true);
			}
			//TODO:: send message so object can handle it themselves
		}
		else
		{
			std::string eventMess = animations[aniTS.GetCurrAnimation()].frames[aniTS.GetCurrFrame()].GetEvent();
			//if (eventMess != "none")
			//	SGD::Event* frameEve = new SGD::Event(eventMess.c_str(), nullptr, nullptr);
		}
	}
}

void AnimationSystem::ResetAnimation(AnimationTimeStamp& aniTS)
{
	aniTS.SetCurrFrame(0);
	aniTS.SetTimeOnFrame(0);
	aniTS.SetEnded(false);
}

void AnimationSystem::Render(AnimationTimeStamp& aniTS, SGD::Point renderPoint, float rotate, SGD::Color col, SGD::Size scale)
{
	Frame rendFrame = animations[aniTS.GetCurrAnimation()].frames[aniTS.GetCurrFrame()];
	SGD::Vector pointOffset = SGD::Vector(renderPoint - rendFrame.GetAnchorPoint());

	SGD::Rectangle rect = rendFrame.GetRenderRect();

	SGD::GraphicsManager::GetInstance()->DrawTextureSection(animations[aniTS.GetCurrAnimation()].GetImage(),
		SGD::Point(pointOffset.x, pointOffset.y), rect, rotate, SGD::Vector(rendFrame.GetAnchorPoint().x, rendFrame.GetAnchorPoint().y), col, scale);
}

bool AnimationSystem::LoadAnimation(const char* szFilename)
{
	//create the xml doc
	TiXmlDocument doc;

	//attempt to load
	char xPath[258] = "resources\\xml\\";
	strcat_s(xPath, 258, szFilename);
	if (doc.LoadFile(xPath) == false)
		return false;

	//access the root element 
	TiXmlElement* pRoot = doc.RootElement();

	if (pRoot == nullptr)
		return false;

	//iterae through the children elements
	TiXmlElement* anim = pRoot->FirstChildElement("Animation");
	if (anim == nullptr)
		return false;
	while (anim != nullptr)
	{
		int temp;
		bool loopin;

		anim->Attribute("loop", &temp);
		loopin = (temp == 1);

		TiXmlElement* animData = anim->FirstChildElement("ImgPath");
		const char* imgPath = animData->GetText();

		animData = animData->NextSiblingElement("AniName");
		const char* aniName = animData->GetText();

		animData = animData->NextSiblingElement("Frame");
		std::vector<Frame> frameList;
		while (animData != nullptr)
		{
			double frameDur;
			animData->Attribute("duration", &frameDur);

			TiXmlElement* frameData = animData->FirstChildElement("Event");
			const char* eventName = frameData->GetText();

			frameData = frameData->NextSiblingElement("rendRect");
			int x, y, wid, hei;
			frameData->Attribute("x", &x);
			frameData->Attribute("y", &y);
			frameData->Attribute("width", &wid);
			frameData->Attribute("heigth", &hei);
			SGD::Rectangle rendRect = SGD::Rectangle(SGD::Point((float)x, (float)y),
				SGD::Size((float)wid, (float)hei));

			frameData = frameData->NextSiblingElement("collRect");
			frameData->Attribute("x", &x);
			frameData->Attribute("y", &y);
			frameData->Attribute("width", &wid);
			frameData->Attribute("heigth", &hei);
			SGD::Rectangle collRect = SGD::Rectangle(SGD::Point((float)x, (float)y),
				SGD::Size((float)wid, (float)hei));

			frameData = frameData->NextSiblingElement("actRect");
			frameData->Attribute("x", &x);
			frameData->Attribute("y", &y);
			frameData->Attribute("width", &wid);
			frameData->Attribute("heigth", &hei);
			SGD::Rectangle actRect = SGD::Rectangle(SGD::Point((float)x, (float)y),
				SGD::Size((float)wid, (float)hei));

			frameData = frameData->NextSiblingElement("anchor");
			frameData->Attribute("x", &x);
			frameData->Attribute("y", &y);
			SGD::Point anchor = SGD::Point((float)x, (float)y);

			frameData = frameData->NextSiblingElement("weaponPoint");
			frameData->Attribute("x", &x);
			frameData->Attribute("y", &y);
			SGD::Point weapon = SGD::Point((float)x, (float)y);

			frameList.push_back(Frame(rendRect, collRect, actRect, anchor, weapon, eventName, (float)frameDur));

			animData = animData->NextSiblingElement("Frame");
		}

		char relPath[258] = "resources\\graphics\\";
		strcat_s(relPath, 258, imgPath);
		SGD::HTexture texture = SGD::GraphicsManager::GetInstance()->LoadTexture(relPath);
		animations.emplace(aniName, Animation(texture, aniName, loopin, frameList));

		anim = anim->NextSiblingElement("Animation");
	}
	return true;
}

void AnimationSystem::UnloadAnimation()
{
	for (auto iter = animations.begin(); iter != animations.end(); ++iter)
		SGD::GraphicsManager::GetInstance()->UnloadTexture(iter->second.GetImage());

	animations.clear();
}

bool AnimationSystem::UnloadSingleAnimation(AnimationTimeStamp& aniTS)
{
	return SGD::GraphicsManager::GetInstance()->UnloadTexture(animations[aniTS.GetCurrAnimation()].GetImage());
}


SGD::Rectangle AnimationSystem::CollisionRect(AnimationTimeStamp& aniTS, SGD::Point position, float rotate)
{
	Frame collFrame = animations[aniTS.GetCurrAnimation()].frames[aniTS.GetCurrFrame()];
	SGD::Vector offset = SGD::Vector(position - collFrame.GetAnchorPoint());

	SGD::Rectangle collide = collFrame.GetCollideRect();
	offset.y += collide.top;
	offset.x += collide.left;

	offset.x = ((position.x - offset.x)*cos(rotate)) - ((offset.y - position.y)*sin(rotate)) + position.x;
	offset.y = ((offset.y - position.y)*cos(rotate)) - ((position.x - offset.x)*sin(rotate)) + position.y;

	return SGD::Rectangle(SGD::Point(offset.x, offset.y), SGD::Size(collide.ComputeSize()));
}

SGD::Rectangle AnimationSystem::ActiveRect(AnimationTimeStamp& aniTS, SGD::Point position, float rotate)
{
	Frame actFrame = animations[aniTS.GetCurrAnimation()].frames[aniTS.GetCurrFrame()];
	SGD::Vector offset = SGD::Vector(position.x + actFrame.GetActiveRect().left, position.y + actFrame.GetActiveRect().top);
	SGD::Rectangle attack = actFrame.GetActiveRect();
	//
	//offset.y += attack.top;
	//offset.x += attack.left;

	//offset.x = ((position.x - offset.x)*cos(rotate)) - ((offset.y - position.y)*sin(rotate)) + position.x;
	//offset.y = ((offset.y - position.y)*cos(rotate)) - ((position.x - offset.x)*sin(rotate)) + position.y;

	return SGD::Rectangle(SGD::Point(offset.x, offset.y), SGD::Size(attack.ComputeSize()));
}

SGD::Point AnimationSystem::GetWeaponPoint(AnimationTimeStamp& aniTS, SGD::Point position, float rotate)
{
	Frame weaponFrame = animations[aniTS.GetCurrAnimation()].frames[aniTS.GetCurrFrame()];
	SGD::Vector offset = SGD::Vector(position.x + weaponFrame.GetWeapon().x, position.y + weaponFrame.GetWeapon().y);

	SGD::Point weapon = weaponFrame.GetWeapon();

	return SGD::Point(offset.x, offset.y);
}

SGD::Point AnimationSystem::GetAnchorPoint(AnimationTimeStamp& aniTS, SGD::Point position, float rotate)
{
	Frame weaponFrame = animations[aniTS.GetCurrAnimation()].frames[aniTS.GetCurrFrame()];
	SGD::Vector offset = SGD::Vector(position.x + weaponFrame.GetAnchorPoint().x, position.y + weaponFrame.GetAnchorPoint().y);
	return SGD::Point(offset.x, offset.y);
}