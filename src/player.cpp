#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/PlayerObject.hpp>
class $modify(PlayerObjectExt, PlayerObject) {
	struct Fields : DialogDelegate {
		PlayerObject* self;
		DialogLayer* dialogLayer = nullptr;
		virtual void dialogClosed(DialogLayer* p0) {
			self->m_controlsDisabled = (true);
			self->enablePlayerControls();
			self->releaseAllButtons();
			dialogLayer = nullptr;
		};
		float m_lastPlatformerXVelocity = 0.1;
	};
	bool init(int p0, int p1, GJBaseGameLayer* p2, cocos2d::CCLayer* p3, bool p4) {
		if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;
		m_fields->self = this;

		//particle effects
		auto drag = "11a-1a0.68a1.27a-1a90a45a0a20a5a1a0a0a0a0a0a0a30a5a0a0a0.247059a0a0.243137a0a0.243137a0a0.352941a0a11a19a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a182a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(drag, this->m_playerGroundParticles, 0)->setUserObject("ps"_spr, CCStringMake(drag));

		auto land = "11a0.16a0.68a1.27a-1a90a44a0a20a17a1a0a22a0a0a0a0a20a5a0a0a0.247059a0a0.243137a0a0.243137a0a0.352941a0a37a29a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a182a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(land, this->m_landParticles0, 0)->setUserObject("ps"_spr, CCStringMake(land));
		GameToolbox::particleFromString(land, this->m_landParticles1, 0)->setUserObject("ps"_spr, CCStringMake(land));

		
		//add animations
#define add(name, amount, speed)																								\
		auto name = CCSprite::createWithSpriteFrameName(#name"1.png"_spr);																			\
		if (name) {																												\
			name->setID(#name""_spr);																							\
			this->addChild(name);																								\
			auto frames = CCArray::create();																					\
			for (int i = 1; i <= amount; i++) if (auto sprite = CCSprite::createWithSpriteFrameName(												\
				fmt::format("{}/"#name"{}.png", GEODE_MOD_ID, i).data()															\
			)) frames->addObject(sprite->displayFrame());																		\
			else log::warn("there is no {}/"#name"{}.png", GEODE_MOD_ID, i);													\
			name->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimation::createWithSpriteFrames(frames, speed))));	\
		};
		add(side_susie_idle, 2, 0.5f);
		add(side_susie_run, 10, 0.05f);
		add(side_susie_fallingdown, 2, 0.05f);
		add(side_susie_flyingup, 2, 0.05f);

		return true;
	}
	virtual void update(float p0) {

		m_fields->m_lastPlatformerXVelocity =
			fabs(this->m_platformerXVelocity) > 0.001f ?
			this->m_platformerXVelocity : m_fields->m_lastPlatformerXVelocity;

		m_yVelocity = m_isUpsideDown ? -m_yVelocity : m_yVelocity;

		CCSize size = { 15.000f, 30.000f };
		this->m_obContentSize = size;
		this->m_objectRect.size = size;
		this->m_width = size.width;
		this->m_height = size.height;

#define in_range(tar, from, to) (tar >= from and tar <= to)
		bool showAnimPlr = (this->m_isRobot) and !this->m_isDead;
		bool isOnAir = !this->m_isOnGround or !in_range(m_yVelocity, -5.2f, 5.2f);
		bool isStanding = fabs(this->m_platformerXVelocity) < 0.25f;

		auto mainLayer = this->querySelector("main-layer");
		auto spr_player_idle = typeinfo_cast<CCSprite*>(this->querySelector("side_susie_idle"_spr));
		auto spr_player_run = typeinfo_cast<CCSprite*>(this->querySelector("side_susie_run"_spr));
		auto spr_player_fall = typeinfo_cast<CCSprite*>(this->querySelector("side_susie_fallingdown"_spr));
		auto spr_player_jump = typeinfo_cast<CCSprite*>(this->querySelector("side_susie_flyingup"_spr));
		if (spr_player_idle and spr_player_run and mainLayer) {
			mainLayer->setVisible(!showAnimPlr);
			static CCPoint hidep = { 9999999.f, 9999999.f };
			static CCPoint showp = { 0.5f, 0.5f };
			//hide all animates
			cocos::findFirstChildRecursive<CCSprite>(this,
				[](CCSprite* node) {
					if (string::contains(node->getID(), GEODE_MOD_ID)) node->setAnchorPoint(hidep);
					return false;
				}
			);
			//show current state anim
			;; spr_player_run->setAnchorPoint((showAnimPlr and !isOnAir and !isStanding) ? showp : hidep);
			; spr_player_idle->setAnchorPoint((showAnimPlr and !isOnAir and isStanding) ? showp : hidep);
			; spr_player_fall->setAnchorPoint((showAnimPlr and isOnAir and m_yVelocity <= -0.0f) ? showp : hidep);
			; spr_player_jump->setAnchorPoint((showAnimPlr and isOnAir and m_yVelocity >= 0.0f) ? showp : hidep);
			auto visible_sprite = cocos::findFirstChildRecursive<CCSprite>(this,
				[](CCSprite* node) {
					if (!string::contains(node->getID(), GEODE_MOD_ID)) return false;
					return node->getAnchorPoint().equals(showp);
				}
			);
			if (visible_sprite) {
				//auto name = getFrameName(visible_sprite);

				visible_sprite->setRotation(mainLayer->getRotation());
				visible_sprite->setScaleX(mainLayer->getScaleX());
				visible_sprite->setScaleY(mainLayer->getScaleY());

				for (auto p : { m_iconSprite,m_iconSpriteSecondary,m_iconSpriteWhitener,m_iconGlow }) if (p) {
					p->setDisplayFrame(visible_sprite->displayFrame());
					p->setRotation(mainLayer->getRotation());
					p->setScaleX(mainLayer->getScaleX());
					p->setScaleY(mainLayer->getScaleY());
				};
			};

			this->m_robotBurstParticles->setVisible(0);
		}

		m_yVelocity = m_isUpsideDown ? -m_yVelocity : m_yVelocity;

		PlayerObject::update(p0);

		if (m_ghostTrail and showAnimPlr) m_ghostTrail->m_position = CCPointZero;

		cocos2d::ParticleStruct aaahhh;
		for (auto ptr : { m_playerGroundParticles, m_landParticles0, m_landParticles1 }) if (ptr) {
			if (auto str = typeinfo_cast<CCString*>(ptr->getUserObject("ps"_spr))) {
				GameToolbox::particleStringToStruct(str->getCString(), aaahhh);
				ptr->setGravity(CCPointMake(aaahhh.GravityX, aaahhh.GravityY));
				ptr->setStartColor({ aaahhh.StartColorR, aaahhh.StartColorG, aaahhh.StartColorB, aaahhh.StartColorA });
				ptr->setEndColor({ aaahhh.EndColorR, aaahhh.EndColorG, aaahhh.EndColorB, aaahhh.EndColorA });
			};
			ptr->setVisible(true);
		};
	};
};