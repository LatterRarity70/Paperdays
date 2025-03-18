#pragma once

#define in_range(a, m, t) (a >= m and a <= t)

#include <Geode/modify/GJBaseGameLayer.hpp>
class $modify(GJBaseGameLayerSoundEvents, GJBaseGameLayer) {
	void gameEventTriggered(GJGameEvent p0, int p1, int p2) {
		auto eventID = static_cast<int>(p0);
		auto audio = FMODAudioEngine::get();
		if (eventID >= 1 and eventID <= 5) {//landing
			if (m_player1->m_isRobot) audio->playEffect("step_landing.ogg", 1.f, 1.f / eventID, 0.9f + (eventID / 10));
		}
		if (eventID >= 12 and eventID <= 13) {//jump
			if (m_player1->m_isRobot) audio->playEffect("step_jump.ogg", 1.f, 1.f, 1.f);
		}
		if (eventID == 23) {//dash
			auto dashes = {
				"dash1.ogg","dash2.ogg","dash3.ogg","dash4.ogg","dash5.ogg"
			};
			if (m_player1->m_isRobot) audio->playEffect(*select_randomly(dashes.begin(), dashes.end()), 1.f, 1.f, 1.f);
		}
		GJBaseGameLayer::gameEventTriggered(p0, p1, p2);
	};
};

#include <Geode/modify/PlayerObject.hpp>
class $modify(PlayerObjectExt, PlayerObject) {
	struct Fields {
		float m_lastPlatformerXVelocity = 0.1;
	};
	bool isCube() {
		auto player = this;
		if (!player->m_isShip && !player->m_isBall && !player->m_isBird && !player->m_isDart && !player->m_isRobot && !player->m_isSpider && !player->m_isSwing)
			return true;
		return false;
	}
	bool isStanding() {
		return fabs(this->m_platformerXVelocity) < 0.25f and this->m_isPlatformer;
	}
	bool isOnAir() {
		return !this->m_isOnGround or !in_range(m_yVelocity, -0.2f, 0.2f);
	}
	bool isTurnedLeft() {
		return m_fields->m_lastPlatformerXVelocity < 0.f and this->m_isPlatformer;
	}
	bool showAnimPlr() {
		return (this->m_isRobot) and !this->m_isDead;
	}
	void mySch(float) {
		if (not this) return;

		m_fields->m_lastPlatformerXVelocity =
			fabs(this->m_platformerXVelocity) > 0.001f ?
			this->m_platformerXVelocity : m_fields->m_lastPlatformerXVelocity;

		m_yVelocity = m_isUpsideDown ? -m_yVelocity : m_yVelocity;

		auto mainLayer = this->getChildByIDRecursive("main-layer");
		auto spr_player_idle = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("spr_player_idle"_spr));
		auto spr_player_run = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("spr_player_run"_spr));
		auto spr_player_fall = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("spr_player_fall"_spr));
		auto spr_player_jump = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("spr_player_jump"_spr));
		if (spr_player_idle and spr_player_run and mainLayer) {
			mainLayer->setVisible(!showAnimPlr());
			;; spr_player_run->setVisible(showAnimPlr() and !isOnAir() and !isStanding());
			; spr_player_idle->setVisible(showAnimPlr() and !isOnAir() and isStanding());
			; spr_player_fall->setVisible(showAnimPlr() and isOnAir() and m_yVelocity <= -0.0f);
			; spr_player_jump->setVisible(showAnimPlr() and isOnAir() and m_yVelocity >= 0.0f);
			auto visible_sprite = cocos::findFirstChildRecursive<CCSprite>(this,
				[](CCSprite* node) {
					if (!string::contains(node->getID(), GEODE_MOD_ID"/spr_player")) return false;
					node->pauseSchedulerAndActions();
					return cocos::nodeIsVisible(node);
				}
			);
			if (visible_sprite) {
				auto name = getFrameName(visible_sprite);

				visible_sprite->resumeSchedulerAndActions();
				visible_sprite->setRotation(mainLayer->getRotation());
				visible_sprite->setScaleX(mainLayer->getScaleX());
				visible_sprite->setScaleY(mainLayer->getScaleY());

				if (m_iconSprite) m_iconSprite->setDisplayFrame(visible_sprite->displayFrame());
				if (m_iconSpriteSecondary) m_iconSpriteSecondary->setDisplayFrame(visible_sprite->displayFrame());
				if (m_iconSpriteWhitener) m_iconSpriteWhitener->setDisplayFrame(visible_sprite->displayFrame());
				if (m_iconGlow) m_iconGlow->setDisplayFrame(visible_sprite->displayFrame());

				//step sound
				auto waitForStepB = this->getChildByID("waitForStepB"_spr);
				if (string::contains(name, "spr_player_run")) {
					if (!waitForStepB) {
						waitForStepB = createDataNode("waitForStepB"_spr);
						this->addChild(waitForStepB);
					}

					if (string::contains(name, "spr_player_run1") and !waitForStepB->isVisible()) {
						FMODAudioEngine::sharedEngine()->playEffect("step_a.ogg", 1.f, 1.f, 1.f);
						waitForStepB->setVisible(true);
					}
					if (string::contains(name, "spr_player_run6") and waitForStepB->isVisible()) {
						FMODAudioEngine::sharedEngine()->playEffect("step_b.ogg", 1.f, 1.f, 1.f);
						waitForStepB->setVisible(false);
					}

					if (fabs(m_fields->m_lastPlatformerXVelocity) == 0.45f) FMODAudioEngine::sharedEngine
					()->playEffect("step_a.ogg", 1.f, 1.f, 1.f
					);

				}
				else if (waitForStepB) waitForStepB->setVisible(false);
			};

			this->m_robotBurstParticles->setVisible(0);
		}

		if (m_holdingLeft or m_holdingRight) {
			m_isPlatformer = 1;
			if (m_gameLayer) m_gameLayer->m_isPlatformer = 1;
		}

		m_yVelocity = m_isUpsideDown ? -m_yVelocity : m_yVelocity;
	}
	bool init(int p0, int p1, GJBaseGameLayer * p2, cocos2d::CCLayer * p3, bool p4) {
		if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;

		//add animations
#define add(name, amount, speed) \
		auto name = CCSprite::create(#name"1.png"_spr); \
		if (name) { \
			name->setID(#name""_spr); \
			name->setVisible(0); \
			name->setScale(0.f); \
			this->addChild(name); \
			auto frames = CCArray::create(); \
			for (int i = 1; i <= amount; i++) if (auto sprite = CCSprite::create( \
				fmt::format("{}/"#name"{}.png", GEODE_MOD_ID, i).data() \
			)) frames->addObject(sprite->displayFrame()); \
			else log::warn("there is no {}/"#name"{}.png", GEODE_MOD_ID, i); \
			log::debug("{}", frames); \
			name->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimation::createWithSpriteFrames(frames, speed)))); \
		};
		add(spr_player_idle, 5, 0.25f);
		add(spr_player_run, 9, 0.07f);
		add(spr_player_fall, 3, 0.035f);
		add(spr_player_jump, 3, 0.035f);
#undef add

		this->schedule(schedule_selector(PlayerObjectExt::mySch));

		auto drag = "4a-1a0.9a0.35a20a90a45a75a20a5a1a0a-300a0a0a0a0a1a2a0a0a1a0.1a1a0.1a1a0.1a0.352941a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(drag, this->m_playerGroundParticles, 0);

		auto land = "8a0.02a0.84a0.35a-1a90a45a75a20a8a1a0a-300a0a0a0a0a1a2a0a0a1a0.1a1a0.1a1a0.1a0.352941a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(land, this->m_landParticles0, 0);
		GameToolbox::particleFromString(land, this->m_landParticles1, 0);

		return true;
	}
};