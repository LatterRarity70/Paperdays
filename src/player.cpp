#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/UILayer.hpp>
class $modify(UILayerPlayerKeysExt, UILayer) {
	bool init(GJBaseGameLayer * p0) {
		if (!UILayer::init(p0)) return false;
		return true;
	};
	void downButtonUpdate(enumKeyCodes key, bool p1) {
		auto asd = false;
		asd = key == CONTROLLER_LTHUMBSTICK_DOWN ? true : asd;
		asd = key == CONTROLLER_RTHUMBSTICK_DOWN ? true : asd;
		asd = key == CONTROLLER_Down ? true : asd;
		asd = key == KEY_ArrowDown ? true : asd;
		asd = key == KEY_Down ? true : asd;
		asd = key == KEY_S ? true : asd;
		if (!asd) return;
		for (auto p : { m_gameLayer->m_player1, m_gameLayer->m_player2 }) if (p) {
			p->m_holdingButtons[0] = p1;
		}
	}
	void actionButtonUpdate(enumKeyCodes key, bool p1) {
		auto asd = false;
		asd = key == CONTROLLER_A ? true : asd;
		asd = key == KEY_Control ? true : asd;
		asd = key == KEY_Enter ? true : asd;
		asd = key == KEY_Z ? true : asd;
		if (!asd) return;
		for (auto p : { m_gameLayer->m_player1, m_gameLayer->m_player2 }) if (p) {
			p->m_holdingButtons[5] = p1;
			if (p1) {
				if (Ref dialog = CCScene::get()->getChildByType<DialogLayer>(-1)) {
					if (!dialog->m_animating) dialog->handleDialogTap();
				}
				else {
					p->pushButton(PlayerButton::Jump);
					queueInMainThread([=] { p->releaseButton(PlayerButton::Jump); });
				}
			}
		}
	}
	void handleKeypress(cocos2d::enumKeyCodes key, bool p1) {
		UILayer::handleKeypress(key, p1);
		downButtonUpdate(key, p1);
		actionButtonUpdate(key, p1);
		//log::error("{} -> {}", CCKeyboardDispatcher::get()->keyToString(key), p1);
	}
};

#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayerExt, PlayLayer) {
	void startMusic() {
		return CCKeyboardDispatcher::get()->getControlKeyPressed()
			? PlayLayer::startMusic() : void();
	};
};

#include <Geode/modify/GJBaseGameLayer.hpp>
class $modify(GJBaseGameLayerEventsExt, GJBaseGameLayer) {
	void gameEventTriggered(GJGameEvent p0, int p1, int p2) {
		auto eventID = static_cast<int>(p0);
		auto audio = FMODAudioEngine::get();
		for (auto p : { m_player1, m_player2 }) if (p and not p->m_isHidden) {
			if ((eventID == 71 or eventID == 73) and p->m_isOnGround and p->m_isRobot) audio->playEffect("step_jump.ogg", 1.f, 1.f, 0.5f);
			if (eventID >= 2 and eventID <= 5) {//landing
				if (p->m_isRobot) audio->playEffect("step_landing.ogg", 1.f, 1.f / eventID, 0.9f + (eventID / 10));
			}
			if (eventID >= 12 and eventID <= 13) {//jump
				if (p->m_isRobot) {
					if (!p->getActionByTag(5718932)) {
						auto jump_anim = CCSequence::create(
							CCEaseBackOut::create(CCScaleTo::create(
								0.1f, p->getScaleX() - 0.15f, p->getScaleY() + 0.15f
							)),
							CCEaseSineOut::create(CCScaleTo::create(
								0.15f, p->getScale(), p->getScaleX()
							)),
							nullptr
						);
						jump_anim->setTag(5718932);
						p->runAction(jump_anim);
					};
					audio->playEffect("step_jump.ogg", 1.f, 1.f, 1.f);
				}
			}
		}
		GJBaseGameLayer::gameEventTriggered(p0, p1, p2);
	};
};

#include <Geode/modify/PlayerObject.hpp>
class $modify(PlayerObjectExt, PlayerObject) {
	struct Fields {
		PlayerObject* self;
		float m_lastXV = 0.1;
		float m_lastYV = 0.1;
	};
	bool init(int p0, int p1, GJBaseGameLayer* p2, cocos2d::CCLayer* p3, bool p4) {
		if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;
		m_fields->self = this;

		if (!this->m_gameLayer) return true;
		if (this->m_gameLayer != GameManager::get()->m_gameLayer) return true;

		//particle effects
		auto drag = "11a-1a0.68a1.27a-1a90a45a0a20a5a1a0a0a0a0a0a0a30a5a0a0a0.247059a0a0.243137a0a0.243137a0a0.352941a0a11a19a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a182a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(drag, this->m_playerGroundParticles, 0)->setUserObject("ps"_spr, CCStringMake(drag));

		auto land = "11a0.16a0.68a1.27a-1a90a44a0a20a17a1a0a22a0a0a0a0a20a5a0a0a0.247059a0a0.243137a0a0.243137a0a0.352941a0a37a29a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a182a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(land, this->m_landParticles0, 0)->setUserObject("ps"_spr, CCStringMake(land));
		GameToolbox::particleFromString(land, this->m_landParticles1, 0)->setUserObject("ps"_spr, CCStringMake(land));

		
		//add animations
#define anim(name, amount, speed)																								\
		auto name = CCSprite::createWithSpriteFrameName(#name"1.png"_spr);														\
		if (name) {																												\
			if (auto a = name->getTexture()) a->setAliasTexParameters();														\
			name->setID(#name##_spr);																							\
			this->addChild(name);																								\
			auto frames = CCArray::create();																					\
			for (int i = 1; i <= amount; i++) if (auto sprite = CCSprite::createWithSpriteFrameName(							\
				fmt::format("{}/"#name"{}.png", GEODE_MOD_ID, i).data()															\
			)) frames->addObject(sprite->displayFrame());																		\
			else log::warn("there is no {}/"#name"{}.png", GEODE_MOD_ID, i);													\
			name->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimation::createWithSpriteFrames(frames, speed))));	\
		};																														\

		anim(side_susie_idle, 2, 0.5f);
		anim(side_susie_fallingdown, 2, 0.1f);
		anim(side_susie_flyingup, 2, 0.1f);
		anim(side_susie_run, 9, 0.0666f);

		static auto tispeed1 = 0.35f;
		static auto twspeed2 = 0.09f;
		anim(top_susie_dn_idle, 2, tispeed1);
		anim(top_susie_dn_walk, 4, twspeed2);
		anim(top_susie_up_idle, 2, tispeed1);
		anim(top_susie_up_walk, 4, twspeed2);
		anim(top_susie_lr_idle, 2, tispeed1);
		anim(top_susie_lr_walk, 4, twspeed2);

		queueInMainThread(
			[_this = Ref(this)] {
				auto& gameLayer = GameManager::get()->m_gameLayer;
				if (!gameLayer) return;
				if (gameLayer->m_player1 == _this.data()) {
					_this->addAllParticles();
				}
			}
		);

		return true;
	}
	void updateJump(float p0) {
		if (!m_isSpider) PlayerObject::updateJump(p0);
	}
	void ringJump(RingObject* p0, bool p1) {
		if (m_isSpider) m_holdingButtons[5] ? PlayerObject::ringJump(p0, p1) : void();
		else PlayerObject::ringJump(p0, p1);
	};
	void update(float p0) {

		if (this->m_gameLayer != GameManager::get()->m_gameLayer) return PlayerObject::update(p0);

		m_fields->m_lastXV =
			fabs(this->m_platformerXVelocity) > 0.001f ?
			this->m_platformerXVelocity : m_fields->m_lastXV;

		m_fields->m_lastYV =
			fabs(this->m_yVelocity) > 0.001f ?
			this->m_yVelocity : m_fields->m_lastYV;

		m_yVelocity = m_isUpsideDown ? -m_yVelocity : m_yVelocity;

#define in_range(tar, from, to) (tar >= from and tar <= to)
		bool showAnimPlr = (this->m_isRobot);
		bool showTopAnimPlr = (this->m_isSpider);
		bool isOnAir = !this->m_isOnGround or !in_range(m_yVelocity, -5.2f, 5.2f);
		bool isStanding = fabs(this->m_platformerXVelocity) < 0.25f;

		CCSize size = { 15.000f, 30.000f };
		CCSize size2 = { 15.000f, 3.000f };
		CCSize def_size = { 30.000f, 30.000f };
		auto& plrSZ = showAnimPlr or showTopAnimPlr ? 
			(showAnimPlr ? size : size2) : def_size;
		this->m_obContentSize = plrSZ;
		this->m_objectRect.size = plrSZ;
		this->m_width = plrSZ.width;
		this->m_height = plrSZ.height;

		Ref mainLayer = this->querySelector("main-layer");
		Ref spr_player_idle = typeinfo_cast<CCSprite*>(this->querySelector("side_susie_idle"_spr));
		Ref spr_player_run = typeinfo_cast<CCSprite*>(this->querySelector("side_susie_run"_spr));
		Ref spr_player_fall = typeinfo_cast<CCSprite*>(this->querySelector("side_susie_fallingdown"_spr));
		Ref spr_player_jump = typeinfo_cast<CCSprite*>(this->querySelector("side_susie_flyingup"_spr));
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

				CCSpriteFrame* lastFrame = typeinfo_cast<CCSpriteFrame*>(this->getUserObject("anim-last-frame"_spr));
				if (!lastFrame) {
					lastFrame = visible_sprite->displayFrame();
					this->setUserObject("anim-last-frame"_spr, lastFrame);
				}
				if (not visible_sprite->displayFrame()->getRect().equals(lastFrame->getRect())) {
					lastFrame = visible_sprite->displayFrame();
					this->setUserObject("anim-last-frame"_spr, lastFrame);

					bool isMovingOnGround = this->m_isOnGround and std::fabs(this->m_platformerXVelocity) > 0.1f;
					if (isMovingOnGround and spr_player_run->getAnchorPoint().equals(showp)) {
						bool isStepFrame = false;
						auto frameCache = CCSpriteFrameCache::get();
						auto lfr = lastFrame->getRect();
						isStepFrame = lfr.equals(frameCache->spriteFrameByName("side_susie_run4.png"_spr)->getRect()) ? true : isStepFrame;
						isStepFrame = lfr.equals(frameCache->spriteFrameByName("side_susie_run9.png"_spr)->getRect()) ? true : isStepFrame;
						if (isStepFrame) {
							// Play step sound only on frame transition
							std::vector<std::string> stepSounds = { "step_a.ogg", "step_b.ogg" };
							std::string selectedSound = stepSounds[rand() % stepSounds.size()];
							FMODAudioEngine::get()->playEffect(selectedSound, 1.0f, 1.0f, 5.0f);
						};
					};

				}
			};

			this->m_robotBurstParticles->setVisible(0);
		}

		Ref top_susie_dn_idle = typeinfo_cast<CCSprite*>(this->querySelector("top_susie_dn_idle"_spr));
		Ref top_susie_dn_walk = typeinfo_cast<CCSprite*>(this->querySelector("top_susie_dn_walk"_spr));
		Ref top_susie_up_idle = typeinfo_cast<CCSprite*>(this->querySelector("top_susie_up_idle"_spr));
		Ref top_susie_up_walk = typeinfo_cast<CCSprite*>(this->querySelector("top_susie_up_walk"_spr));
		Ref top_susie_lr_idle = typeinfo_cast<CCSprite*>(this->querySelector("top_susie_lr_idle"_spr));
		Ref top_susie_lr_walk = typeinfo_cast<CCSprite*>(this->querySelector("top_susie_lr_walk"_spr));
		if (showTopAnimPlr and mainLayer) {
			mainLayer->setVisible(!showTopAnimPlr);
			static CCPoint hidep = { 9999999.f, 9999999.f };
			static CCPoint showp = { 0.5f, 0.0f };

			cocos::findFirstChildRecursive<CCSprite>(this,
				[](CCSprite* node) {
					if (string::contains(node->getID(), GEODE_MOD_ID)) node->setAnchorPoint(hidep);
					return false;
				}
			);

			auto& xv = m_platformerXVelocity;
			auto& yv = m_yVelocity;
			auto& lxv = m_fields->m_lastXV;
			auto& lyv = m_fields->m_lastYV;

			bool isMoving = (fabs(xv) > 0.1f || fabs(yv) > 0.1f);
			bool isIdle = !isMoving;

			if (isMoving) {
				lxv = xv;
				lyv = yv;
			}

			bool isHorizontal = fabs(lxv) > fabs(lyv);
			bool isVertical = !isHorizontal;
			bool isUp = lyv > 0;
			bool isDown = lyv < 0;
			bool isLeft = lxv > 0;
			bool isRight = lxv < 0;

			bool isStepFrame = false;

			if (isIdle) {
				if (isHorizontal) {
					if (top_susie_lr_idle) top_susie_lr_idle->setAnchorPoint(showp);
					if (top_susie_lr_idle) top_susie_lr_idle->setFlipX(isLeft);
				}
				else {
					if (isUp) if (top_susie_up_idle) top_susie_up_idle->setAnchorPoint(showp);
					if (isDown) if (top_susie_dn_idle) top_susie_dn_idle->setAnchorPoint(showp);
				}
			}
			else {
				if (isHorizontal) {
					if (top_susie_lr_walk) top_susie_lr_walk->setAnchorPoint(showp);
					if (top_susie_lr_walk) top_susie_lr_walk->setFlipX(isLeft);
				}
				else {
					if (isUp) if (top_susie_up_walk) top_susie_up_walk->setAnchorPoint(showp);
					if (isDown) if (top_susie_dn_walk) top_susie_dn_walk->setAnchorPoint(showp);
				}
			}

			auto visible_sprite = cocos::findFirstChildRecursive<CCSprite>(this,
				[](CCSprite* node) {
					if (!string::contains(node->getID(), GEODE_MOD_ID)) return false;
					return node->getAnchorPoint().equals(showp);
				}
			);
			if (visible_sprite) {

				for (auto p : { m_iconSprite,m_iconSpriteSecondary,m_iconSpriteWhitener,m_iconGlow }) if (p) {
					p->setDisplayFrame(visible_sprite->displayFrame());
					p->setRotation(mainLayer->getRotation());
					p->setScaleX(mainLayer->getScaleX());
					p->setScaleY(mainLayer->getScaleY());
				};

				CCSpriteFrame* lastFrame = typeinfo_cast<CCSpriteFrame*>(this->getUserObject("anim2-last-frame"_spr));
				if (!lastFrame) {
					lastFrame = visible_sprite->displayFrame();
					this->setUserObject("anim2-last-frame"_spr, lastFrame);
				}
				if (not visible_sprite->displayFrame()->getRect().equals(lastFrame->getRect())) {
					lastFrame = visible_sprite->displayFrame();
					this->setUserObject("anim2-last-frame"_spr, lastFrame);
					auto cache = CCSpriteFrameCache::get();
					auto lfr = lastFrame->getRect();
					isStepFrame = lfr.equals(cache->spriteFrameByName("top_susie_dn_walk2.png"_spr)->getRect()) ? true : isStepFrame;
					isStepFrame = lfr.equals(cache->spriteFrameByName("top_susie_dn_walk4.png"_spr)->getRect()) ? true : isStepFrame;
					isStepFrame = lfr.equals(cache->spriteFrameByName("top_susie_lr_walk2.png"_spr)->getRect()) ? true : isStepFrame;
					isStepFrame = lfr.equals(cache->spriteFrameByName("top_susie_lr_walk4.png"_spr)->getRect()) ? true : isStepFrame;
					isStepFrame = lfr.equals(cache->spriteFrameByName("top_susie_up_walk2.png"_spr)->getRect()) ? true : isStepFrame;
					isStepFrame = lfr.equals(cache->spriteFrameByName("top_susie_up_walk4.png"_spr)->getRect()) ? true : isStepFrame;
					if (isStepFrame) {
						// Play step sound only on frame transition
						std::vector<std::string> stepSounds = { "step_a.ogg", "step_b.ogg" };
						std::string selectedSound = stepSounds[rand() % stepSounds.size()];
						FMODAudioEngine::get()->playEffect(selectedSound, 1.0f, 1.0f, 5.0f);
					};
				}
			};

			setRotation(false);
			this->m_vehicleGroundParticles->setVisible(0);
			this->m_shipClickParticles->setVisible(0);
			this->m_robotBurstParticles->setVisible(0);

			m_yVelocityRelated = 0.f;
			m_gravityMod = 0.f;
			m_jumpBuffered = false;
			m_isOnGround = true;

			if (not m_controlsDisabled) {
				auto& btns = m_holdingButtons;

				auto mVel = 0.5f;

				yv = btns[0] or btns[1] ? mVel * m_speedMultiplier : 0.f;
				yv = btns[0] ? -fabs(yv) : fabs(yv);

				xv = btns[2] or btns[3] ? mVel * m_speedMultiplier : 0.f;
				xv = btns[2] ? -fabs(xv) : fabs(xv);
			};

			auto asd = CCArrayExt<CCParticleSystemQuad*>(this->m_particleSystems);
			asd.push_back(this->m_playerGroundParticles);
			asd.push_back(this->m_trailingParticles);
			asd.push_back(this->m_vehicleGroundParticles);
			asd.push_back(this->m_landParticles0);
			asd.push_back(this->m_landParticles1);
			for (auto a : asd) {
				if (a) a->stopSystem();
			}
		}

		m_yVelocity = m_isUpsideDown ? -m_yVelocity : m_yVelocity;

		m_gravityMod = m_gravityMod == 0.f ? 1.f : m_gravityMod;

		PlayerObject::update(p0);

		if (m_ghostTrail and (showAnimPlr or showTopAnimPlr)) m_ghostTrail->m_position = CCPointZero;

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



// https://github.com/Kingminer7/PlatformerJoystick/blob/main/src/JoystickNode.hpp
// why reinvent the wheel ya? anyways i just getting away from dependencies
class PD_Kingminer7sJoystickNode : public CCMenu {
public:
	static PD_Kingminer7sJoystickNode* create() {
		auto ret = new PD_Kingminer7sJoystickNode;
		if (!ret->init()) {
			delete ret;
			return nullptr;
		}
		ret->autorelease();
		return ret;
	};
	bool init() override {
		if (!CCMenu::init()) return false;
		setContentSize({ 100, 100 });

		m_bg = CCSprite::createWithSpriteFrameName("sliderthumb.png");
		m_bg->setScale(getContentSize().width / m_bg->getContentSize().width);
		if (auto a = m_bg->getTexture()) a->setAliasTexParameters();
		addChildAtPosition(m_bg, Anchor::Center);

		m_center = CCSprite::createWithSpriteFrameName("smallDot.png");
		m_center->setScale(getContentSize().width / m_center->getContentSize().width / 3);
		m_center->setZOrder(1);
		if (auto a = m_center->getTexture()) a->setAliasTexParameters();
		addChildAtPosition(m_center, Anchor::Center);

		registerWithTouchDispatcher();

		return true;
	};
	void draw() override { CCMenu::draw(); };
	void registerWithTouchDispatcher() override {
		CCTouchDispatcher::get()->addTargetedDelegate(this, -512, true);
	};
	bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
		if (!isTouchEnabled() || !nodeIsVisible(this)) return false;
		if (ccpDistance(getPosition(), touch->getLocation()) <= getScaledContentSize().width / 2) {
			ccTouchMoved(touch, event);
			return true;
		}
		return false;
	};
	void ccTouchEnded(CCTouch* touch, CCEvent* event) override {
		if (auto uil = UILayer::get(); uil && uil->m_gameLayer) {
			handleInput(uil->m_gameLayer, { 0, 0 }, m_currentInput);
		}
		m_currentInput = CCPoint{ 0, 0 };
		m_center->setPosition(getScaledContentSize() / 2);
	};
	void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
		auto pos = convertToNodeSpace(touch->getLocation());
		auto fromCenter = pos - getScaledContentSize() / 2;

		if (fromCenter.getLength() > getScaledContentWidth() / 2 - m_center->getScaledContentWidth() / 2) {
			fromCenter = fromCenter.normalize() * (getScaledContentWidth() / 2 - m_center->getScaledContentWidth() / 2);
			pos = fromCenter + getScaledContentSize() / 2;
		}

		CCPoint inp = { 0, 0 };

		auto angle = atan2(fromCenter.y, fromCenter.x);

		if (std::abs(fromCenter.x) > std::abs(fromCenter.normalize().x) * 15) {
			if (angle > 5 * -M_PI / 12 && angle < 5 * M_PI / 12) inp.x = 1;
			else if (angle > 7 * M_PI / 12 || angle < 7 * -M_PI / 12) inp.x = -1;
		}

		if (std::abs(fromCenter.y) > std::abs(fromCenter.normalize().y) * 15) {
			if (angle > M_PI / 12 && angle < 11 * M_PI / 12) inp.y = 1;
			else if (angle < -M_PI / 12 && angle > -11 * M_PI / 12) inp.y = -1;
		}

		if (inp != m_currentInput) {
			if (auto uil = UILayer::get(); uil && uil->m_gameLayer) {
				handleInput(uil->m_gameLayer, inp, m_currentInput);
			}
		}
		m_currentInput = inp;

		m_center->setPosition(pos);
	};
	void ccTouchCancelled(CCTouch* touch, CCEvent* event) override {
		ccTouchEnded(touch, event);
	};
	void handleInput(GJBaseGameLayer* layer, CCPoint input, CCPoint old) {
		if (!layer->m_uiLayer) return;
		Ref ui = layer->m_uiLayer;
		//x
		if (old.x == 1) layer->queueButton(3, false, false);
		else if (old.x == -1) layer->queueButton(2, false, false);
		if (input.x == 1) layer->queueButton(3, true, false);
		else if (input.x == -1) layer->queueButton(2, true, false);
		//y
		if (old.y == 1) layer->queueButton(1, false, false);
		else if (old.y == -1) ui->handleKeypress(KEY_S, false);
		if (input.y == 1) layer->queueButton(1, true, false);
		else if (input.y == -1) ui->handleKeypress(KEY_S, true);
		//action
		if (old.equals(input)) {
			ui->handleKeypress(KEY_Z, 1);
			queueInMainThread([=] { if (ui) ui->handleKeypress(KEY_Z, 0); });
		}
	};
	void fakePosition() {
		CCPoint pos = getContentSize() / 2 + m_currentInput * getContentWidth() / 2;

		auto fromCenter = pos - getScaledContentSize() / 2;

		if (fromCenter.getLength() > getScaledContentWidth() / 2 - m_center->getScaledContentWidth() / 2) {
			fromCenter = fromCenter.normalize() * (getScaledContentWidth() / 2 - m_center->getScaledContentWidth() / 2);
			pos = fromCenter + getScaledContentSize() / 2;
		}

		CCPoint inp = { 0, 0 };
		auto angle = atan2(fromCenter.y, fromCenter.x);
		if (std::abs(fromCenter.x) > std::abs(fromCenter.normalize().x) * 15) {
			if (angle > 5 * -M_PI / 12 && angle < 5 * M_PI / 12) inp.x = 1;
			else if (angle > 7 * M_PI / 12 || angle < 7 * -M_PI / 12) inp.x = -1;
		}
		if (std::abs(fromCenter.y) > std::abs(fromCenter.normalize().y) * 15) {
			if (angle > M_PI / 12 && angle < 11 * M_PI / 12) inp.y = 1;
			else if (angle < -M_PI / 12 && angle > -11 * M_PI / 12) inp.y = -1;
		}

		m_center->setPosition(pos);
	};
	CCSprite* m_bg = nullptr;
	CCSprite* m_center = nullptr;
	bool m_twoPlayer = false;
	CCPoint m_currentInput = { 0, 0 };
};
class $modify(JSUILayer, UILayer) {
	struct Fields {
		PD_Kingminer7sJoystickNode* m_joystickNode;
		bool m_left = false;
		bool m_right = false;
	};
	bool init(GJBaseGameLayer * gjbgl) {
		if (!UILayer::init(gjbgl)) return false;
		queueInMainThread(
			[_this = Ref(this), gjbgl]() {
				_this->m_fields->m_joystickNode = PD_Kingminer7sJoystickNode::create();
				_this->m_fields->m_joystickNode->m_twoPlayer = gjbgl->m_level->m_twoPlayerMode;
				_this->addChildAtPosition(_this->m_fields->m_joystickNode, Anchor::BottomLeft, { 75, 75 }, false);
				_this->schedule(schedule_selector(JSUILayer::fixVisibility), 0.0f);//fixVisibility
			}
		);
		return true;
	}
	void fixVisibility(float asd = 0.f) {
		Ref dialog = CCScene::get()->getChildByType<DialogLayer>(-1);
		if (!m_fields->m_joystickNode) return;
		if (!m_inPlatformer or (dialog and dialog->isRunning())) {
			m_fields->m_joystickNode->setVisible(false);
			if (m_fields->m_joystickNode->isTouchEnabled()) m_fields->m_joystickNode->setTouchEnabled(false);
			return;
		}
		if (!m_fields->m_joystickNode->isTouchEnabled()) m_fields->m_joystickNode->setTouchEnabled(true);
		if (auto p1move = getChildByType<GJUINode>(0)) {
			p1move->setPosition({ 10000, 10000 });
			m_fields->m_joystickNode->setVisible(p1move->isVisible());
			if (auto a = p1move->m_firstSprite) m_fields->m_joystickNode->setOpacity(a->getOpacity());
		}
	}
};