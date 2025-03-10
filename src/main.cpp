#include <_main.hpp>
#include <fs.hpp>

#include <SahderLayer.hpp>
#include <ViewportLayer.hpp>

#define in_range(a, m, t) (a >= m and a <= t)

#include <Geode/modify/GJBaseGameLayer.hpp>
class $modify(GJBaseGameLayerSoundEvents, GJBaseGameLayer) {
	void gameEventTriggered(GJGameEvent p0, int p1, int p2) {
		auto eventID = static_cast<int>(p0);
		auto audio = FMODAudioEngine::get();
		if (eventID >= 1 and eventID <= 5) {//landing
			audio->playEffect("step_landing.ogg", 1.f, 1.f / eventID, 0.9f + (eventID / 10));
		}
		if (eventID >= 12 and eventID <= 13) {//jump
			audio->playEffect("step_jump.ogg", 1.f, 1.f, 1.f);
		}
		if (eventID == 23) {//dash
			auto dashes = {
				"dash1.ogg","dash2.ogg","dash3.ogg","dash4.ogg","dash5.ogg"
			};
			audio->playEffect(*select_randomly(dashes.begin(), dashes.end()), 1.f, 1.f, 1.f);
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
		return (this->m_isRobot or isCube()) and !this->m_isDead;
	}
	void mySch(float) {
		if (not this) return;
		m_fields->m_lastPlatformerXVelocity =
			fabs(this->m_platformerXVelocity) > 0.001f ?
			this->m_platformerXVelocity : m_fields->m_lastPlatformerXVelocity;
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
					if (!string::contains(node->getID(), GEODE_MOD_ID)) return false;
					return cocos::nodeIsVisible(node);
				}
			);
			if (visible_sprite) {
				auto name = getFrameName(visible_sprite);

				visible_sprite->setFlipX(isTurnedLeft());
				visible_sprite->setFlipY(this->m_isUpsideDown);

				if (m_iconSprite) m_iconSprite->setDisplayFrame(visible_sprite->displayFrame());
				if (m_iconSpriteSecondary) m_iconSpriteSecondary->setDisplayFrame(visible_sprite->displayFrame());
				if (m_iconSpriteWhitener) m_iconSpriteWhitener->setDisplayFrame(visible_sprite->displayFrame());
				if (m_iconGlow)m_iconGlow->setDisplayFrame(visible_sprite->displayFrame());

				//step sound
				auto waitForStepB = FMODAudioEngine::sharedEngine()->getChildByID("waitForStepB"_spr);
				if (string::contains(name, "spr_player_run")) {
					if (!waitForStepB) {
						waitForStepB = createDataNode("waitForStepB"_spr);
						FMODAudioEngine::sharedEngine()->addChild(waitForStepB);
					}
					if (string::contains(name, "spr_player_run1") and !waitForStepB->isVisible()) {
						FMODAudioEngine::sharedEngine()->playEffect("step_a.ogg", 1.f, 1.f, 1.f);
						waitForStepB->setVisible(true);
					}
					if (string::contains(name, "spr_player_run6") and waitForStepB->isVisible()) {
						FMODAudioEngine::sharedEngine()->playEffect("step_b.ogg", 1.f, 1.f, 1.f);
						waitForStepB->setVisible(false);
					}
				}
				else if (waitForStepB) waitForStepB->setVisible(false);
			};

			this->m_robotBurstParticles->setVisible(0);
		}

		if (m_holdingLeft or m_holdingRight) {
			m_isPlatformer = 1;
			if (m_gameLayer) m_gameLayer->m_isPlatformer = 1;
		}
	}
	void updateRotation(float p0) {
		if (this->isCube()) {
			auto isPlatformer = m_isPlatformer;
			this->m_isPlatformer = 0;
			this->m_isRobot = 1;
			PlayerObject::updateRotation(isOnAir() ? (p0 * 0.05f) : (p0 * 2.0f));
			this->m_isRobot = 0;
			this->m_isPlatformer = isPlatformer;
		}
		else PlayerObject::updateRotation(p0);
	}
	void updateJump(float p0) {
		if (this->isCube()) {
			this->m_isRobot = 1;
			PlayerObject::updateJump(p0);
			this->m_isRobot = 0;
		}
		else PlayerObject::updateJump(p0);
	}
	bool init(int p0, int p1, GJBaseGameLayer * p2, cocos2d::CCLayer * p3, bool p4) {
		if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;

		//add animations
#define add(name, amount, speed) \
		auto name = CCSprite::create(#name"1.png"_spr); \
		if (name) { \
			name->setID(#name""_spr); \
			name->setVisible(0); \
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

#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
class $modify(CCMenuItemSpriteExtraExt, CCMenuItemSpriteExtra) {
	$override void selected() {
		if (m_selectSound.empty()) this->m_selectSound = "btnClick.ogg";
		if (auto spr = typeinfo_cast<CCNodeRGBA*>(this->getNormalImage())) {
			spr->setCascadeColorEnabled(1);
			spr->setCascadeOpacityEnabled(1);
			if (spr->getColor() == ccWHITE) {
				this->m_animationEnabled = this->m_animationType == MenuAnimationType::Move;
				this->m_colorEnabled = this->m_animationType == MenuAnimationType::Scale;
			};
		}
		return CCMenuItemSpriteExtra::selected();
	}
};

#include <Geode/modify/GManager.hpp>
class $modify(ResourcesLoader, GManager) {
	$override void setup() {

		//for ui.blah.json to ui/blah.json as example
		for (auto& p : fs::glob::glob(getMod()->getResourcesDir().string() + "/*.*.*")) {
			auto name = p.filename().string();
			std::reverse(name.begin(), name.end());
			auto should_replace = false;
			for (auto& ch : name) {
				ch = (ch == '.' and should_replace) ? '/' : ch;
				should_replace = (ch == '.') ? true : should_replace;
			}
			std::reverse(name.begin(), name.end());
			auto todvde = std::filesystem::path(name);
			auto newp = p.parent_path() / todvde.parent_path();
			std::filesystem::create_directories(newp, fs::last_err_code);
			std::filesystem::rename(p, newp / todvde.filename(), fs::last_err_code);
		}

		CCFileUtils::sharedFileUtils()->addPriorityPath(
			getMod()->getResourcesDir().string().c_str()
		);

		GManager::setup();
	}
};

#include <Geode/modify/CCSpriteFrameCache.hpp>
class $modify(CCSpriteFrameCacheExt, CCSpriteFrameCache) {
	CCSpriteFrame* spriteFrameByName(const char* pszName) {
		//log::debug("{}({})", __FUNCTION__, pszName);
		auto frameAtSprExtName = (Mod::get()->getID() + "/" + pszName);
		auto frameAtSprExt = CCSpriteFrameCache::get()->m_pSpriteFrames->objectForKey(frameAtSprExtName);
		auto rtn = CCSpriteFrameCache::spriteFrameByName(
			frameAtSprExt ? frameAtSprExtName.data() : pszName
		);
		return rtn;
	};
};

#include <Geode/modify/GameManager.hpp>
class $modify(GameManagerExt, GameManager) {
	$override int countForType(IconType p0) {
		return 1;
	}
	$override ccColor3B colorForIdx(int p0) {
		return ccWHITE;
	}
};

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayerExt, LoadingLayer) {
	void upd(float) {
		if (not this) return;

		if (auto loading_bg = this->getChildByID("loading_bg")) {
			loading_bg->setScaleX(
				CCDirector::get()->getWinSize().width / loading_bg->getContentWidth()
			);
			loading_bg->setScaleY(
				CCDirector::get()->getWinSize().height / loading_bg->getContentHeight()
			);
		}

		if (auto nodeToSetup = typeinfo_cast<CCLabelBMFont*>(this->getChildByID("loading-text"))) {
			nodeToSetup->setVisible(
				!string::contains(nodeToSetup->getString(), "Loading resources")
			);
		};

	};
	bool init(bool p0) {
		if (!LoadingLayer::init(p0)) return false;
		
		static bool volume_preloaded = 0;
		if (volume_preloaded) void();
		else FMODAudioEngine::get()->setBackgroundMusicVolume(GameManager::get()->m_bgVolume);//xd
		volume_preloaded = 1;

		GameManager::get()->fadeInMusic("WaitingTheme.mp3");
		
		findFirstChildRecursive<CCLabelBMFont>(
			this, [this](CCLabelBMFont* node) {
				node->setFntFile("gjFont30.fnt");
				node->setScale(node->getScale() * 0.82f);
				return false;
			}
		);

		if (auto a = this->getChildByID("bg-texture")) a->setVisible(0);

		if (auto nodeToSetup = this->getChildByID("progress-slider")) {
			nodeToSetup->setPosition(CCPointMake(26.f, 32.f));
			nodeToSetup->setAnchorPoint(CCPointMake(0.f, 0.5f));
			nodeToSetup->setZOrder(3);
		};
		if (auto nodeToSetup = typeinfo_cast<CCLabelBMFont*>(this->getChildByID("loading-text"))) {
			nodeToSetup->setVisible(
				!string::contains(nodeToSetup->getString(), "Loading resources")
			);
			nodeToSetup->setAnchorPoint(CCPointMake(-0.2f, 1.5f));
			nodeToSetup->setZOrder(3);
		};
		if (auto nodeToSetup = this->getChildByID("geode-small-label")) {
			nodeToSetup->setPosition(CCPointMake(26.f, 52.f));
			nodeToSetup->setAnchorPoint(CCPointMake(0.f, 0.5f));
			nodeToSetup->setZOrder(3);
		};
		if (auto nodeToSetup = this->getChildByID("geode-small-label-2")) {
			nodeToSetup->setPosition(CCPointMake(242.f, 32.f));
			nodeToSetup->setAnchorPoint(CCPointMake(0.f, 0.5f));
			nodeToSetup->setZOrder(3);
		};

		auto loading_bg = CCSprite::create("loading_bg.png");
		if (loading_bg) {
			loading_bg->setID("loading_bg");
			loading_bg->setAnchorPoint(CCPointMake(0.f, 0.f));
			loading_bg->setScaleX(getContentWidth() / loading_bg->getContentWidth());
			loading_bg->setScaleY(getContentHeight() / loading_bg->getContentHeight());
			this->addChild(loading_bg);
		}

		{
			auto verLabel = CCLabelBMFont::create(
				fmt::format(
					"SDK {} at {} Platform, Release {}",
					Mod::get()->getVersion().toVString(),
					GEODE_PLATFORM_NAME,
					Mod::get()->getMetadata().getGeodeVersion().toVString()
				).c_str(),
				"gjFont30.fnt",
				kCCTextAlignmentLeft
			);
			verLabel->limitLabelWidth(92.f, 0.5f, 0.1f);
			verLabel->setPositionY(this->getContentHeight()); 
			verLabel->setScale(0.337f);
			verLabel->setAnchorPoint(CCPointMake(-0.12f, 2.1f));
			verLabel->setOpacity(77);
			this->addChild(verLabel);
		};

		this->schedule(schedule_selector(LoadingLayerExt::upd));
		return !false;
	};
};

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerExt, MenuLayer) {
	void upd(float) {
		if (not this) return; 
	}
	bool init() {
		if (!MenuLayer::init()) return false;

		if (1) findFirstChildRecursive<CCNode>(
			this, [this](CCNode* node) {
				if (node == this) return false;
				node->setZOrder(-9999);
				node->setVisible(0);
				return false;
			}
		);

		//gjFont30.fnt
		
		auto menu = CCMenu::create();
		menu->setID("menu"_spr);

		menu->addChild(SimpleTextArea::create(getMod()->getName(), "gjFont30.fnt", 1.0f), 0, 56829);
		menu->getChildByTag(56829)->addChild(SahderLayer::create("basic.vsh", "menuListTitle.fsh"));

#define menu_item_to_link_label(name, id) {															\
auto TAR_NAME = std::string(name); auto TAR_ID = std::string(id);									\
auto item = CCMenuItemExt::createSpriteExtra(														\
	SimpleTextArea::create(TAR_NAME, "bigFont.fnt", 0.6f)->getLines()[0],							\
	[this, TAR_NAME, TAR_ID](CCNode*) {																\
		auto button_node = (this ? this : CCNode::create())->getChildByIDRecursive(TAR_ID);			\
		if (button_node) if (auto cast = typeinfo_cast<CCMenuItem*>(button_node)) cast->activate();	\
	}																								\
);																									\
item->setCascadeColorEnabled(1);																	\
item->setAnchorPoint({0.f, 0.5f});																	\
menu->addChild(item);																				\
}

#define menu_item_label(text, func, ...) {											\
auto TAR_TEXT = std::string(text); auto item = CCMenuItemExt::createSpriteExtra(	\
	SimpleTextArea::create(TAR_TEXT, "bigFont.fnt", 0.6f)->getLines()[0], func		\
);																					\
item->setCascadeColorEnabled(1);													\
item->setAnchorPoint({0.f, 0.5f});													\
menu->addChild(item); __VA_ARGS__													\
}
		menu_item_to_link_label("play", "play-button");
		menu_item_to_link_label("settings", "settings-button");
		menu_item_to_link_label("geode", "geode.loader/geode-button");

		menu->addChild(SimpleTextArea::create("   ", "bigFont.fnt", 1.0f));

		{ 
			auto item = CCMenuItemExt::createSpriteExtra(
				SimpleTextArea::create("more", "bigFont.fnt", 0.6f)->getLines()[0],
				[this](CCNode*) {
					if (auto a = this->getChildByID("submenu"_spr)) {
						if (a->getChildByID("more-submenu"_spr)) return a->removeFromParentAndCleanup(0);
						a->removeFromParentAndCleanup(0);
					}
					auto menu = CCMenu::create();
					menu->setID("menu"_spr);

					findFirstChildRecursive<CCMenuItem>(
						this, [this, menu](CCMenuItem* founded_item) {
							menu_item_to_link_label(nodeName(founded_item), founded_item->getID());
							return false;
						}
					);

					menu->setLayout(ColumnLayout::create()
						->setAxisReverse(1)
						->setGap(-5.000f)
						->setCrossAxisLineAlignment(AxisAlignment::End)
					);

					auto submenu_scroll = ScrollLayer::create({ 860.f, 235.f });
					submenu_scroll->m_cutContent = 0;
					submenu_scroll->m_contentLayer->addChild(menu);
					submenu_scroll->m_contentLayer->setContentHeight(menu->getContentHeight());
					submenu_scroll->scrollToTop();
					submenu_scroll->setID("submenu"_spr);
					submenu_scroll->setZOrder(-1);
					submenu_scroll->addChild(createDataNode("more-submenu"_spr));
					submenu_scroll->runAction(CCEaseExponentialOut::create(CCMoveBy::create(0.5f, ccp(6, 0))));
					this->addChildAtPosition(submenu_scroll, Anchor::BottomLeft, { -108.f - 6, 0.f }, 0);
				}
			); 
			item->setCascadeColorEnabled(1); 
			item->setAnchorPoint({ 0.f, 0.5f }); 
			menu->addChild(item); 
		};

		menu_item_to_link_label("quit", "close-button");

		menu->setLayout(ColumnLayout::create()->setAxisReverse(1)->setCrossAxisLineAlignment(AxisAlignment::Start));
		menu->setAnchorPoint({ 0.f, 0.5f });
		this->addChildAtPosition(menu, Anchor::Left, {66.6f,0}, 0);

		//menu->addChild(SahderLayer::create("basic.vsh", "menu.fsh"));

		//bg
		auto bg = CCLayer::create();
		bg->setID("bg"_spr);
		this->addChild(bg, -3);

		auto shaderedBG = SahderLayer::create("basic.vsh", "menuBG.fsh");
		shaderedBG->addValueContainer("center")->setAnchorPoint({ 0.650f, 0.500f});
		shaderedBG->addValueContainer("radius")->setScale(0.625f);
		shaderedBG->addValueContainer("strength")->setScale(-0.675f);
		shaderedBG->addValueContainer("blurSize")->setScale(0.100f);
		shaderedBG->m_onDrawEnd = [shaderedBG]
			{
				auto shader = shaderedBG->m_shaderProgram;

				auto center = shaderedBG->getValueContainer("center");
				GLint centerLoc = glGetUniformLocation(shader->getProgram(), "center");
				glUniform2f(centerLoc, center->getAnchorPoint().x, center->getAnchorPoint().y);

				auto radius = shaderedBG->getValueContainer("radius");
				GLint radiusLoc = glGetUniformLocation(shader->getProgram(), "radius");
				glUniform1f(radiusLoc, radius->getScale());

				auto strength = shaderedBG->getValueContainer("strength");
				GLint strengthLoc = glGetUniformLocation(shader->getProgram(), "strength");
				glUniform1f(strengthLoc, strength->getScale());

				auto blurSize = shaderedBG->getValueContainer("blurSize");
				GLint blurSizeLoc = glGetUniformLocation(shader->getProgram(), "blurSize");
				glUniform1f(blurSizeLoc, blurSize->getScale());

				shader->setUniformsForBuiltins();
			};
		bg->addChild(shaderedBG, 999);

		auto bg0 = CCSprite::create("bg0.png"_spr);
		bg0->setID("bg0"_spr);
		bg0->setPosition(CCPointMake(418.f, 92.f));
		bg->addChild(bg0);

		auto bg1 = CCSprite::create("bg1.png"_spr);
		bg1->setID("bg1"_spr);
		bg1->runAction(CCRepeatForever::create(CCRotateBy::create(1.f, 3)));
		bg1->setPosition(CCPointMake(448.f, 82.f));
		bg->addChild(bg1);

		auto bg2 = CCSprite::create("bg2.png"_spr);
		bg2->setID("bg2"_spr);
		bg2->runAction(CCRepeatForever::create(CCRotateBy::create(1.f, 1.5f)));
		bg2->setPosition(CCPointMake(305.f, 297.f));
		bg->addChild(bg2);

		this->addChild(SahderLayer::create("basic.vsh", "menu.fsh"), 1, 1337);

		this->schedule(schedule_selector(MenuLayerExt::upd));
		return true;
	}
};
