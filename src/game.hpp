#pragma once

#define in_range(tar, from, to) (tar >= from and tar <= to)

static inline std::map<int, const char*> UA_ADDITIONS = {
	{ 0, "no special action" },
	{ 1742001, "switch player model" },
	{ 1742002, "toggle platformer" },
};

#include <Geode/modify/ArtTriggerGameObject.hpp>
class $modify(ArtTriggerGameObjectExt, ArtTriggerGameObject) {
	virtual void triggerObject(GJBaseGameLayer * p0, int p1, gd::vector<int> const* p2) {
		//log::debug("{}", __FUNCTION__);
		if (string::startsWith(fmt::format("{}", this->m_artIndex), "174200")) {

			if (this->m_artIndex == 1742002) {
				p0->m_player1->m_isPlatformer = !p0->m_player1->m_isPlatformer;
				p0->m_isPlatformer = !p0->m_isPlatformer;
			}

			if (this->m_artIndex == 1742001) {
				auto player_model_types_str_obj = typeinfo_cast<CCString*>(p0->m_player1->getUserObject("player_model_types"));
				auto player_model_types = player_model_types_str_obj ? string::split(player_model_types_str_obj->getCString(), ",") : std::vector<std::string>{};
				auto new_set = std::vector<std::string>();
				auto sel_next = false;
				for (auto type : player_model_types) {
					if (string::startsWith(type, "^")) {
						new_set.push_back(std::string(type.begin() + 1, type.end()));
						sel_next = true;
					}
					else {
						new_set.push_back(sel_next ? ("^" + type) : type);
						sel_next = false;
					}
				}
				auto new_set_str = string::join(new_set, ",");
				player_model_types_str_obj->m_sString = (string::contains(new_set_str, "^") ? new_set_str : "^" + new_set_str).c_str();
			};

		}
		else ArtTriggerGameObject::triggerObject(p0, p1, p2);
	};
};

#include <Geode/modify/EditorUI.hpp>
class $modify(EditorUIext, EditorUI) {
	void editObject(cocos2d::CCObject * p0) {
		EditorUI::editObject(p0);

		log::debug("asd");

		if (!this->m_editorLayer) return log::error("m_editorLayer = {}", m_editorLayer);
		if (!this->m_editorLayer->getParent()) return log::error("m_editorLayer->getParent() = {}", m_editorLayer->getParent());
		auto scene = this->m_editorLayer->getParent();

		log::debug("scene = {}", scene);

		log::debug("m_selectedObjectIndex = {}", m_selectedObjectIndex);

		auto art_trigger = typeinfo_cast<ArtTriggerGameObject*>(m_selectedObject);
		if (art_trigger) if (art_trigger->m_objectID == 3031) {
			findFirstChildRecursive<SetupArtSwitchPopup>(scene, [art_trigger](SetupArtSwitchPopup* popup) {

				auto label = SimpleTextArea::create("UA ADDITION:    [0000000]", "bigFont.fnt");
				label->setPosition(CCPointMake(-8.000f, -36.000f));
				label->setScale(0.5f);
				label->runAction(CCRepeatForever::create(CCSpawn::create(CallFuncExt::create([label, art_trigger] {
					if (art_trigger and label) label->setText(fmt::format(
						"UA ADDITION:    [{}]", art_trigger->m_artIndex
					).c_str());
					}), nullptr)));
				label->setID("label"_spr);
				popup->m_buttonMenu->addChild(label);

				auto image = ButtonSprite::create(" no spetial action ");
				image->setScale(0.8f);

				auto btn = CCMenuItemExt::createSpriteExtra(image, [image, art_trigger](auto) {

					for (auto one : UA_ADDITIONS) {
						if (art_trigger->m_artIndex == one.first) continue;
						art_trigger->m_artIndex = one.first;
						break;
					}
					image->m_label->setString(UA_ADDITIONS.at(art_trigger->m_artIndex));
					limitNodeSize(image->m_label, image->getContentSize() * 0.8, 1337.f, 0.1f);

					});
				image->m_label->setString(UA_ADDITIONS.at(art_trigger->m_artIndex));
				limitNodeSize(image->m_label, image->getContentSize() * 0.8, 1337.f, 0.1f);

				btn->setID("btn"_spr);
				btn->setPosition(CCPointMake(90.000f, -42.000f));

				popup->m_buttonMenu->addChild(btn);

				return true;

				});
		}
	};
};

#include <Geode/modify/GJBaseGameLayer.hpp>
class $modify(GJBaseGameLayerSoundEvents, GJBaseGameLayer) {
	void gameEventTriggered(GJGameEvent p0, int p1, int p2) {
		auto eventID = static_cast<int>(p0);
		auto audio = FMODAudioEngine::get();
		if (eventID >= 2 and eventID <= 5) {//landing
			if (m_player1->m_isRobot) audio->playEffect("step_landing.ogg", 1.f, 1.f / eventID, 0.9f + (eventID / 10));
		}
		if (eventID >= 12 and eventID <= 13) {//jump
			if (m_player1->m_isRobot) {

				if (!m_player1->getActionByTag(5718932)) {
					auto jump_anim = CCSequence::create(
						CCEaseBackOut::create(CCScaleTo::create(
							0.1f, m_player1->getScaleX() - 0.15f, m_player1->getScaleY() + 0.15f
						)),
						CCEaseSineOut::create(CCScaleTo::create(
							0.15f, m_player1->getScale(), m_player1->getScaleX()
						)),
						nullptr
					);
					jump_anim->setTag(5718932);
					m_player1->runAction(jump_anim);
				};

				audio->playEffect("step_jump.ogg", 1.f, 1.f, 1.f);
			}
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
		return !this->m_isOnGround or !in_range(m_yVelocity, -5.2f, 5.2f);
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

		CCSize size = { 18.000f, 30.000f };
		this->m_obContentSize = size;
		this->m_objectRect.size = size;
		this->m_width = size.width;
		this->m_height = size.height;
		//this->setRScale(size.width);
		//this->setRScaleY(size.width);

		auto player_model_types_str_obj = typeinfo_cast<CCString*>(getUserObject("player_model_types"));
		//log::debug("{}", player_model_types_str_obj->getCString());
		auto player_model_types = player_model_types_str_obj ? string::split(player_model_types_str_obj->getCString(), ",") : std::vector<std::string>{};
		auto player_model_type = std::string();
		for (auto type : player_model_types) if (string::startsWith(type, "^")) player_model_type = std::string(type.begin() + 1, type.end());

		//log::debug("{}", player_model_type);

		auto mainLayer = this->getChildByIDRecursive("main-layer");
		auto spr_player_idle = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive(""_spr + player_model_type + "_idle"));
		auto spr_player_run = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive(""_spr + player_model_type + "_run"));
		auto spr_player_fall = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive(""_spr + player_model_type + "_fall"));
		auto spr_player_jump = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive(""_spr + player_model_type + "_jump"));
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

				if (m_iconSprite) m_iconSprite->setDisplayFrame(visible_sprite->displayFrame());
				if (m_iconSpriteSecondary) m_iconSpriteSecondary->setDisplayFrame(visible_sprite->displayFrame());
				if (m_iconSpriteWhitener) m_iconSpriteWhitener->setDisplayFrame(visible_sprite->displayFrame());
				if (m_iconGlow) m_iconGlow->setDisplayFrame(visible_sprite->displayFrame());

				//step sound
				auto waitForStepB = this->getChildByID("waitForStepB"_spr);
				if (string::contains(name, "run")) {
					if (!waitForStepB) {
						waitForStepB = createDataNode("waitForStepB"_spr);
						this->addChild(waitForStepB);
					}

					if (string::contains(name, "run1") and !waitForStepB->isVisible()) {
						FMODAudioEngine::sharedEngine()->playEffect("step_a.ogg", 1.f, 1.f, 1.f);
						waitForStepB->setVisible(true);
					}
					if (string::contains(name, "run6") and waitForStepB->isVisible()) {
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

		m_yVelocity = m_isUpsideDown ? -m_yVelocity : m_yVelocity;
	}
	bool init(int p0, int p1, GJBaseGameLayer * p2, cocos2d::CCLayer * p3, bool p4) {
		if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;

		auto player_model_types = CCString::create("^player_susie_haunted,player_susie_normal");
		setUserObject("player_model_types", player_model_types);

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
			log::debug("{} {}", ""#name"", frames);																				\
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

		this->schedule(schedule_selector(PlayerObjectExt::mySch));
		
        //particle effects
		auto drag = "4a-1a0.9a0.35a20a90a45a75a20a5a1a0a-300a0a0a0a0a1a2a0a0a1a0.1a1a0.1a1a0.1a0.352941a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(drag, this->m_playerGroundParticles, 0);

		auto land = "8a0.02a0.84a0.35a-1a90a45a75a20a8a1a0a-300a0a0a0a0a1a2a0a0a1a0.1a1a0.1a1a0.1a0.352941a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a1a1a0a1a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
		GameToolbox::particleFromString(land, this->m_landParticles0, 0);
		GameToolbox::particleFromString(land, this->m_landParticles1, 0);

		return true;
	}
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
