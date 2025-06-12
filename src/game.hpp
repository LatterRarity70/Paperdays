#pragma once

#define in_range(tar, from, to) (tar >= from and tar <= to)

enum ua_addition {
	none = 10
	, set_to_player_susie_haunted
	, set_to_player_susie_normal
	, push_jump_button
	, release_jump_button
	, push_left_button
	, release_left_button
	, push_right_button
	, release_right_button
	, enable_platformer
	, disable_platformer
	, playDeathEffect
	, animatePlatformerJump
	, playSpawnEffect
	, tryPlaceCheckpoint
	, playExitDualEffect
};
static inline std::map<const char*, int> UA_ADDITIONS = {
	{ "set to player_susie_haunted", ua_addition::set_to_player_susie_haunted },
	{ "set to player_susie_normal", ua_addition::set_to_player_susie_normal },
	{ "push jump button", ua_addition::push_jump_button },
	{ "release jump button", ua_addition::release_jump_button },
	{ "push left button", ua_addition::push_left_button },
	{ "release left button", ua_addition::release_left_button },
	{ "push right button", ua_addition::push_right_button },
	{ "release right button", ua_addition::release_right_button },
	{ "enable platformer", ua_addition::enable_platformer },
	{ "disable platformer", ua_addition::disable_platformer },
	{ "play death effect", ua_addition::playDeathEffect },
	{ "animate platformer jump (uses trigger object scale!)", ua_addition::animatePlatformerJump },
	{ "play spawn effect", ua_addition::playSpawnEffect },
	{ "try place checkpoint", ua_addition::tryPlaceCheckpoint },
	{ "play exit dual effect", ua_addition::playExitDualEffect }
};

#include <Geode/modify/SetupEventLinkPopup.hpp>
#include <MDInfoAlertButton.hpp>
class $modify(SetupEventLinkPopupExt, SetupEventLinkPopup) {
	bool init(EventLinkTrigger * p0, cocos2d::CCArray * p1) {
		if(!SetupEventLinkPopup::init(p0, p1)) return false;
		
		auto str = std::string();
		for (auto i : UA_ADDITIONS) str += fmt::format(
			"- <c-{}f>{}</c> - {}\n", 
			i.second, i.second, i.first
		);

		auto infLabel = MDInfoAlertButton::create("Umbral Abyss Additions", str.c_str(), 0.575f);
		infLabel->setPosition(CCPointMake(130.f, 105.f));
		this->m_buttonMenu->addChild(infLabel);

		return true;
	}
};

#include <Geode/modify/GJBaseGameLayer.hpp>
class $modify(GJBaseGameLayerEventsExt, GJBaseGameLayer) {
	void activateEventTrigger(EventLinkTrigger * p0, gd::vector<int> const& p1) {

		typedef ua_addition a;
		auto eID = p0->m_extraID2;
		for (auto player : { m_player1, m_player2 }) if (player) {

			if (eID == a::enable_platformer) {
				player->m_isPlatformer = true;
				m_isPlatformer = true;
			}
			if (eID == a::disable_platformer) {
				player->m_isPlatformer = false;
				m_isPlatformer = false;
			}

			if (eID == a::set_to_player_susie_haunted) {
				auto strOBJ = typeinfo_cast<CCString*>(player->getUserObject("player_model_type"));
				if (strOBJ) strOBJ->initWithFormat("%s", "player_susie_haunted");
			};
			if (eID == a::set_to_player_susie_normal) {
				auto strOBJ = typeinfo_cast<CCString*>(player->getUserObject("player_model_type"));
				if (strOBJ) strOBJ->initWithFormat("%s", "player_susie_normal");
			};

			if (eID == a::push_jump_button) player->pushButton(PlayerButton::Jump);
			if (eID == a::release_jump_button) player->releaseButton(PlayerButton::Jump);
			if (eID == a::push_left_button) player->pushButton(PlayerButton::Left);
			if (eID == a::release_left_button) player->releaseButton(PlayerButton::Left);
			if (eID == a::push_right_button) player->pushButton(PlayerButton::Right);
			if (eID == a::release_right_button) player->releaseButton(PlayerButton::Right);

			if (eID == a::playDeathEffect) {
				player->playDeathEffect();
				player->resetObject();
			}
			if (eID == a::animatePlatformerJump) player->animatePlatformerJump(p0->getScale());
			if (eID == a::playSpawnEffect) player->playSpawnEffect();
			if (eID == a::tryPlaceCheckpoint) player->tryPlaceCheckpoint();
			if (eID == a::playExitDualEffect) playExitDualEffect(player);
		};

		GJBaseGameLayer::activateEventTrigger(p0, p1);
	};
	void gameEventTriggered(GJGameEvent p0, int p1, int p2) {
		auto eventID = static_cast<int>(p0);
		auto audio = FMODAudioEngine::get();
		for (auto player : { m_player1, m_player2 }) if (player) {
			if (eventID >= 2 and eventID <= 5) {//landing
				if (player->m_isRobot) audio->playEffect("step_landing.ogg", 1.f, 1.f / eventID, 0.9f + (eventID / 10));
			}
			if (eventID >= 12 and eventID <= 13) {//jump
				if (player->m_isRobot) {

					if (!player->getActionByTag(5718932)) {
						auto jump_anim = CCSequence::create(
							CCEaseBackOut::create(CCScaleTo::create(
								0.1f, player->getScaleX() - 0.15f, player->getScaleY() + 0.15f
							)),
							CCEaseSineOut::create(CCScaleTo::create(
								0.15f, player->getScale(), player->getScaleX()
							)),
							nullptr
						);
						jump_anim->setTag(5718932);
						player->runAction(jump_anim);
					};

					audio->playEffect("step_jump.ogg", 1.f, 1.f, 1.f);
				}
			}
			if (eventID == 23) {//dash
				auto dashes = {
					"dash1.ogg","dash2.ogg","dash3.ogg","dash4.ogg","dash5.ogg"
				};
				if (player->m_isRobot) audio->playEffect(*select_randomly(dashes.begin(), dashes.end()), 1.f, 1.f, 1.f);
			}
		}
		GJBaseGameLayer::gameEventTriggered(p0, p1, p2);
	};
};

#include <Geode/modify/GameObject.hpp>
class $modify(TextGameObjectExt, GameObject) {
	void textSetsupExt(float = 0.f) {
		CCSize size = this->getContentSize();
		this->m_width = size.width;
		this->m_height = size.height;
		this->m_objectType = GameObjectType::Solid;
		this->m_isPassable = true;
		this->m_isTrigger = true;
	}
	void customSetup() {
		GameObject::customSetup();
		if (auto textobject = typeinfo_cast<TextGameObject*>(this)) {
			textSetsupExt();
			this->schedule(schedule_selector(TextGameObjectExt::textSetsupExt));
		}
	}
};

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
	bool isCube() {
		auto player = this;
		if (!player->m_isShip && !player->m_isBall && !player->m_isBird && !player->m_isDart && !player->m_isRobot && !player->m_isSpider && !player->m_isSwing)
			return true;
		return false;
	}
	bool isStanding() {
		return fabs(this->m_platformerXVelocity) < 0.25f;
	}
	bool isOnAir() {
		return !this->m_isOnGround or !in_range(m_yVelocity, -5.2f, 5.2f);
	}
	bool isTurnedLeft() {
		return m_fields->m_lastPlatformerXVelocity < 0.f and this->m_isPlatformer;
	}
	bool showAnimPlr() {
		return (this->m_isRobot) and !this->m_isDead;
	}
	bool init(int p0, int p1, GJBaseGameLayer * p2, cocos2d::CCLayer * p3, bool p4) {
		if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;
		m_fields->self = this;

		auto player_model_type = CCString::create("player_susie_haunted");
		setUserObject("player_model_type", player_model_type);

		//add animations
#define add(name, amount, speed)																								\
		auto name = CCSprite::create(#name"1.png"_spr);																			\
		if (name) {																												\
			name->setID(#name""_spr);																							\
			this->addChild(name);																								\
			auto frames = CCArray::create();																					\
			for (int i = 1; i <= amount; i++) if (auto sprite = CCSprite::create(												\
				fmt::format("{}/"#name"{}.png", GEODE_MOD_ID, i).data()															\
			)) frames->addObject(sprite->displayFrame());																		\
			else log::warn("there is no {}/"#name"{}.png", GEODE_MOD_ID, i);													\
			name->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimation::createWithSpriteFrames(frames, speed))));	\
		};

		add(player_susie_haunted_idle, 5, 0.25f);
		add(player_susie_haunted_run, 9, 0.07f);
		add(player_susie_haunted_fall, 3, 0.035f);
		add(player_susie_haunted_jump, 3, 0.035f);

		add(player_susie_normal_idle, 5, 0.25f);
		add(player_susie_normal_run, 9, 0.07f);
		add(player_susie_normal_fall, 3, 0.035f);
		add(player_susie_normal_jump, 3, 0.035f);

#undef add

		//particle effects
		auto drag = "11a-1a0.68a1.27a-1a90a45a0a20a5a1a0a0a0a0a0a0a30a5a0a0a0.247059a0a0.243137a0a0.243137a0a0.352941a0a11a19a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a182a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(drag, this->m_playerGroundParticles, 0)->setUserObject("ps"_spr, CCStringMake(drag));

		auto land = "11a0.16a0.68a1.27a-1a90a44a0a20a17a1a0a22a0a0a0a0a20a5a0a0a0.247059a0a0.243137a0a0.243137a0a0.352941a0a37a29a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a182a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(land, this->m_landParticles0, 0)->setUserObject("ps"_spr, CCStringMake(land));
		GameToolbox::particleFromString(land, this->m_landParticles1, 0)->setUserObject("ps"_spr, CCStringMake(land));

		return true;
	}
	virtual void update(float p0) {

		m_fields->m_lastPlatformerXVelocity =
			fabs(this->m_platformerXVelocity) > 0.001f ?
			this->m_platformerXVelocity : m_fields->m_lastPlatformerXVelocity;

		m_yVelocity = m_isUpsideDown ? -m_yVelocity : m_yVelocity;

		CCSize size = { 18.000f, 30.000f };
		this->m_obContentSize = size;
		this->m_objectRect.size = size;
		this->m_width = size.width;
		this->m_height = size.height;
		//this->setRScale(size.width);
		//this->setRScaleY(size.width);

		auto player_model_type_str_obj = typeinfo_cast<CCString*>(getUserObject("player_model_type"));
		//log::debug("{}", player_model_type_str_obj->getCString());
		std::string player_model_type = player_model_type_str_obj ? player_model_type_str_obj->getCString() : "player_susie_normal";

		//log::debug("{}", player_model_type);

		auto mainLayer = this->querySelector("main-layer");
		auto spr_player_idle = typeinfo_cast<CCSprite*>(this->querySelector(""_spr + player_model_type + "_idle"));
		auto spr_player_run = typeinfo_cast<CCSprite*>(this->querySelector(""_spr + player_model_type + "_run"));
		auto spr_player_fall = typeinfo_cast<CCSprite*>(this->querySelector(""_spr + player_model_type + "_fall"));
		auto spr_player_jump = typeinfo_cast<CCSprite*>(this->querySelector(""_spr + player_model_type + "_jump"));
		if (spr_player_idle and spr_player_run and mainLayer) {
			mainLayer->setVisible(!showAnimPlr());
			static CCPoint hidep = { 9999999.f, 9999999.f };
			static CCPoint showp = { 0.5f, 0.5f };
			//hide all animates
			cocos::findFirstChildRecursive<CCSprite>(this,
				[](CCSprite* node) {
					if (string::contains(node->getID(), GEODE_MOD_ID"/player_")) node->setAnchorPoint(hidep);
					return false;
				}
			);
			//show current state anim
			;; spr_player_run->setAnchorPoint((showAnimPlr() and !isOnAir() and !isStanding()) ? showp : hidep);
			; spr_player_idle->setAnchorPoint((showAnimPlr() and !isOnAir() and isStanding()) ? showp : hidep);
			; spr_player_fall->setAnchorPoint((showAnimPlr() and isOnAir() and m_yVelocity <= -0.0f) ? showp : hidep);
			; spr_player_jump->setAnchorPoint((showAnimPlr() and isOnAir() and m_yVelocity >= 0.0f) ? showp : hidep);
			auto visible_sprite = cocos::findFirstChildRecursive<CCSprite>(this,
				[](CCSprite* node) {
					if (!string::contains(node->getID(), GEODE_MOD_ID"/player_")) return false;
					return node->getAnchorPoint().equals(showp);
				}
			);
			if (visible_sprite) {
				auto name = getFrameName(visible_sprite);

				visible_sprite->setRotation(mainLayer->getRotation());
				visible_sprite->setScaleX(mainLayer->getScaleX());
				visible_sprite->setScaleY(mainLayer->getScaleY());

				for (auto p : { m_iconSprite,m_iconSpriteSecondary,m_iconSpriteWhitener,m_iconGlow }) if (p) {
					p->setDisplayFrame(visible_sprite->displayFrame());
					p->setRotation(mainLayer->getRotation());
					p->setScaleX(mainLayer->getScaleX());
					p->setScaleY(mainLayer->getScaleY());
				};

				//step sound
				auto waitForStepB = this->getUserObject("waitForStepB"_spr);
				if (string::contains(name, "run")) {
					if (!waitForStepB) {
						waitForStepB = new CCObject(); 
						waitForStepB->autorelease();
						this->setUserObject("waitForStepB"_spr, waitForStepB);
					}
					if (string::contains(name, "run1") and !waitForStepB->m_nTag) {
						FMODAudioEngine::sharedEngine()->playEffect("step_a.ogg", 1.f, 1.f, 1.f);
						waitForStepB->m_nTag = (true);
					}
					if (string::contains(name, "run6") and waitForStepB->m_nTag) {
						FMODAudioEngine::sharedEngine()->playEffect("step_b.ogg", 1.f, 1.f, 1.f);
						waitForStepB->m_nTag = (false);
					}
					if (fabs(m_fields->m_lastPlatformerXVelocity) == 0.45f) {
						FMODAudioEngine::sharedEngine()->playEffect("step_a.ogg", 1.f, 1.f, 1.f);
					}
				}
				else if (waitForStepB) waitForStepB->m_nTag = (false);
			};

			this->m_robotBurstParticles->setVisible(0);
		}

		m_yVelocity = m_isUpsideDown ? -m_yVelocity : m_yVelocity;

		PlayerObject::update(p0);

		if (m_ghostTrail and showAnimPlr()) m_ghostTrail->m_position = CCPointZero;

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
	void collidedWithObject(float p0, GameObject* p1, cocos2d::CCRect p2, bool p3) {
		if (auto textObj = typeinfo_cast<TextGameObject*>(p1)) {

			std::string str = textObj->m_text.c_str();
			if (m_fields->dialogLayer) {
				auto anyIsHeld = false;
				for (auto btn : m_holdingButtons) anyIsHeld = btn.second ? btn.second : anyIsHeld;
				if (anyIsHeld) {
					m_fields->dialogLayer->handleDialogTap();
				};
			};
			if (m_controlsDisabled) this->m_holdingButtons.clear();
			if (string::startsWith(str, "dialog:") and this->m_holdingButtons[1] and isStanding()) {
				this->m_controlsDisabled = (true);
				this->disablePlayerControls();
				this->releaseAllButtons();

				auto raw_data = string::replace(str, "dialog:", "[ ") + "]";
				auto data = matjson::parse(raw_data).unwrapOrDefault();

				/*
					example text:
					```
					dialog:1,"char:the noone","<cr>hi!</c> why ar u here ever?","!","its feels bad you here"
					```
				*/

				auto not_skippable = true;
				auto character = std::string("???");
				auto characterFrame = 0;

				auto dialogObjectsArr = CCArrayExt<DialogObject>();

				for (auto& val : data) {
					if (val.isNumber()) characterFrame = val.asInt().unwrapOrDefault();
					if (val.isString()) {
						auto text = val.asString().unwrapOrDefault();
						if (string::startsWith(text, "!")) {
							not_skippable = false;
							continue;
						}
						if (string::startsWith(text, "char:")) {
							character = string::replace(text, "char:", "");
							continue;
						}
						//(character, text, characterFrame, skippable, cocos2d::ccColor3B color);
						dialogObjectsArr.push_back(DialogObject::create(
							character, text, characterFrame, 1.f, not not_skippable, ccWHITE
						));
					}
				}
				if (m_fields->dialogLayer) m_fields->dialogLayer->removeFromParent();
				m_fields->dialogLayer = DialogLayer::createDialogLayer(
					dialogObjectsArr[0], dialogObjectsArr.inner(), 1
				);
				m_fields->dialogLayer->m_delegate = m_fields.self();
				m_fields->dialogLayer->updateChatPlacement(DialogChatPlacement::Bottom);
				m_gameLayer->m_uiLayer->addChild(m_fields->dialogLayer);
				m_fields->dialogLayer->animateInRandomSide();
			}

			return;
		}
		PlayerObject::collidedWithObject(p0, p1, p2, p3);
	};
};

#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayerExt, PlayLayer) {
	void prepareCreateObjectsFromSetup(gd::string & p0) {
		PlayLayer::prepareCreateObjectsFromSetup(p0);

		if (std::string(m_level->m_levelDesc.c_str()) == "generated_plat") {

			this->runAction(CCRepeatForever::create(CCSpawn::create(CallFuncExt::create([&]{}), nullptr)));
		}
	}
};
