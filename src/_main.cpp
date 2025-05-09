#include <_main.hpp>
#include <fs.hpp>

#include <Geode/utils/web.hpp>
#include <SahderLayer.hpp>
#include <ViewportLayer.hpp>

#include <game.hpp>
#include <resources.hpp>

#include <hjfod.gmd-api/include/GMD.hpp>
class PartSelector : public CCLayer, CCMouseDelegate, CCKeyboardDelegate {
public:

	Ref<BoomScrollLayer> m_scroll;
	bool m_scrolling;

	virtual void keyDown(enumKeyCodes key) override {

		std::set isLeftKey = {
			key == enumKeyCodes::CONTROLLER_Left,
			key == enumKeyCodes::KEY_ArrowLeft,
			key == enumKeyCodes::KEY_Left,
		};
		if (isLeftKey.contains(true)) m_scroll->moveToPage(m_scroll->m_page - 1);

		std::set isRightKey = {
			key == enumKeyCodes::CONTROLLER_Right,
			key == enumKeyCodes::KEY_ArrowRight,
			key == enumKeyCodes::KEY_Right,
		};
		if (isRightKey.contains(true)) m_scroll->moveToPage(m_scroll->m_page + 1);

		std::set isScrollKeys{ false };
		for (auto a : isLeftKey) isScrollKeys.insert(a);
		for (auto a : isRightKey) isScrollKeys.insert(a);
		if (isScrollKeys.contains(true) and m_scrolling) FMODAudioEngine::get(
		)->playEffect("menuItemSelectSound.ogg");

		CCLayer::keyDown(key);

	}

	virtual void scrollWheel(float x, float y) override {

		if ((int)fabs(y) == 3) m_scroll->moveToPage(m_scroll->m_page + (y > 0.f ? 1 : -1));
		if ((int)fabs(x) == 3) m_scroll->moveToPage(m_scroll->m_page + (x > 0.f ? 1 : -1));
		if ((int)fabs(x) == 3) if (m_scrolling) FMODAudioEngine::get()->playEffect("menuItemSelectSound.ogg");

	}

	virtual void keyBackClicked(void) override {
		CCLayer::keyBackClicked();

		auto asd = CreatorLayer::create();
		asd->onBack(nullptr);
		asd->release();
	}

	virtual void update(float delta) override {
		CCLayer::update(delta);
		m_scrolling = m_scroll->m_extendedLayer->numberOfRunningActions();
		if (auto bg = typeinfo_cast<CCSprite*>(getChildByIDRecursive("selector_bg"_spr))) {
			auto fmod = FMODAudioEngine::sharedEngine();
			if (not fmod->m_metering) fmod->enableMetering();
			bg->setOpacity((255 - 160) + (fmod->m_pulse2 * 120));
		}
	}

	CREATE_FUNC(PartSelector);

	virtual bool init() override {
		if (!CCLayer::init()) return false;

		auto bg = CCSprite::create("selector_bg.png");
		bg->setID("selector_bg"_spr);
		limitNodeHeight(bg, this->getContentHeight(), 999.f, 0.1f);
		bg->setAnchorPoint({ 1.f, 0.5f });
		this->addChildAtPosition(bg, Anchor::Right, {}, 0);

		//listing
		{
			auto pages = CCArrayExt<CCLayer>();
			auto page = [](std::string name, std::string level_file, std::string image_file)
				{

					Ref<GJGameLevel> level;

					if (cocos::fileExistsInSearchPaths(level_file.c_str())) {
						level = gmd::importGmdAsLevel((std::string)CCFileUtils::get()->fullPathForFilename(
							level_file.c_str(), false
						)).unwrapOrDefault();
						level->m_levelName = name.c_str();
						level->m_levelType = GJLevelType::Local;
					}
					else {
						level = GJGameLevel::create();
					}

					auto menu = CCMenu::create();
					menu->setID(name);

					auto title = SimpleTextArea::create(
						name, "bigFont.fnt", 0.6f
					)->getLines()[0];
					title->setAnchorPoint({ 0.5f, 0.5f });
					menu->addChildAtPosition(title, Anchor::Bottom, { 0.f, 42.f });

					auto preview = CCSprite::create(image_file.c_str());
					limitNodeHeight(preview, 246.f, 999.f, 0.1f);
					menu->addChildAtPosition(CCMenuItemExt::createSpriteExtra(preview, [level](CCNode* a)
						{
							CCDirector::get()->replaceScene(PlayLayer::scene(level, false, false));
							//switchToScene(LevelInfoLayer::create(level, 0));
						}
					), Anchor::Center, {0.f, 33.f});

					return menu;

				};
			pages.push_back(page("part 1: you know when u should stop", "ua1.gmd", "p1.png"));
			pages.push_back(page("part 2: test", "nah", "p2.png"));
			pages.push_back(page("part 3: test", "nah", "p3.png"));
			pages.push_back(page("part 4: test", "nah", "p4.png"));
			pages.push_back(page("part 5: test", "nah", "p5.png"));

			m_scroll = BoomScrollLayer::create(pages.inner(), 0, 0);
			m_scroll->setScale(0.825f);
			m_scroll->setAnchorPoint(CCPointMake(0.5f, 0.15f));

			addChild(m_scroll);
		};

		if (auto menu = CCMenu::create()) {

			auto title = SimpleTextArea::create("s e l e c t   p a r t", "bigFont.fnt", 0.9f)->getLines()[0];
			title->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChildAtPosition(title, Anchor::Top, { 0, -28 });

			auto back = CCMenuItemExt::createSpriteExtraWithFrameName(
				"GJ_arrow_01_001.png", 1.0f, [this](CCNode* ADs) { this->keyBackClicked(); }
			);
			menu->addChildAtPosition(back, Anchor::TopLeft, { 36, -36 });

			auto left = CCMenuItemExt::createSpriteExtraWithFrameName(
				"navArrowBtn_001.png", 0.8f, [this](CCNode* ADs) { this->keyDown(KEY_Left); }
			);
			left->getNormalImage()->setRotation(-180);
			menu->addChildAtPosition(left, Anchor::Left, { 32, 0 });

			auto right = CCMenuItemExt::createSpriteExtraWithFrameName(
				"navArrowBtn_001.png", 0.8f, [this](CCNode* ADs) { this->keyDown(KEY_Right); }
			);
			menu->addChildAtPosition(right, Anchor::Right, { -32, 0 });

			addChild(menu);
		}

		//(this);

		addChild(SahderLayer::create("basic.vsh", "menu.fsh"));

		this->setTouchEnabled(1);
		this->setKeyboardEnabled(1);
		this->setKeypadEnabled(1);
		this->setMouseEnabled(1);

		this->scheduleUpdate();

		return true;
	}

};

#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
class $modify(CCMenuItemSpriteExtraExt, CCMenuItemSpriteExtra) {
	$override void selected() {
		if (m_selectSound.empty()) this->m_selectSound = "menuItemSelectSound.ogg";
		if (m_activateSound.empty()) this->m_activateSound = "menuItemActivateSound.ogg";
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

#include <Geode/modify/CCNode.hpp>
class $modify(NodeVisitController, CCNode) {
	auto addShader() {
		//add shader
		if (typeinfo_cast<CCLayer*>(this)) if (!getChildByType<SahderLayer>(-1)) {
			bool add = 0;
			add = typeinfo_cast<PauseLayer*>(this) ? 1 : add;
			add = typeinfo_cast<GJDropDownLayer*>(this) ? 1 : add;
			add = typeinfo_cast<MoreOptionsLayer*>(this) ? 1 : add;
			add = typeinfo_cast<FLAlertLayer*>(this) ? 1 : add;
			add = this->getChildByID("background") ? 1 : add;
			add = findFirstChildRecursive<CCScrollLayerExt>(this, [](auto) {return true; }) ? 0 : add;
			if (add) this->addChild(
				SahderLayer::create("basic.vsh", "menu.fsh"), 1337, 1337
			);
		}
		else {
			auto sahder = getChildByType<SahderLayer>(-1);
			bool remove = 0;
			remove = findFirstChildRecursive<CCScrollLayerExt>(this, [](auto) {return true; }) ? 1 : remove;
			remove = typeinfo_cast<MenuLayer*>(this) ? 0 : remove;
			if (remove) sahder->removeFromParent();
		}
	}
	auto replaceColors() {
		auto rgba_node = typeinfo_cast<CCNodeRGBA*>(this);
		auto rgba_layr = typeinfo_cast<CCLayerRGBA*>(this);
#define repl(org, tar) if (node->getColor() == org) node->setColor(tar);
		if (auto node = rgba_node) {
			repl(ccc3(0, 102, 255), ccc3(255, 255, 255));
			repl(ccc3(0, 75, 100), ccc3(255, 255, 255));
			repl(ccc3(0, 56, 141), ccc3(22, 22, 22));
			repl(ccc3(0, 39, 98), ccc3(17, 17, 17));
			repl(ccc3(0, 46, 117), ccc3(14, 14, 14));
			repl(ccc3(0, 36, 91), ccc3(10, 10, 10));
			repl(ccc3(0, 31, 79), ccc3(10, 10, 10));
			repl(ccc3(244, 212, 142), ccc3(92, 92, 92));
			repl(ccc3(245, 174, 125), ccc3(255, 255, 255));
			repl(ccc3(236, 137, 124), ccc3(92, 92, 92));
			repl(ccc3(213, 105, 133), ccc3(255, 255, 255));
			repl(ccc3(173, 84, 146), ccc3(92, 92, 92));
			repl(ccc3(113, 74, 154), ccc3(255, 255, 255));
		};
		if (auto node = rgba_layr) {
			if (node->getColor() == ccc3(51, 68, 153)) { node->setOpacity(160); node->setColor(ccBLACK); }// UI OPTIONS
			repl(ccc3(191, 114, 62), ccc3(6, 6, 6));
			repl(ccc3(161, 88, 44), ccc3(10, 10, 10));
			repl(ccc3(194, 114, 62), ccc3(8, 8, 8));
			//mod-list-frame
			if (node->getColor() == ccc3(25, 17, 37)) node->setOpacity(0);// frame-bg
			repl(ccc3(83, 65, 109), ccc3(17, 17, 17));//search-id
			if (node->getColor() == ccc3(168, 85, 44)) node->setOpacity(0);// frame-bg gd
			repl(ccc3(114, 63, 31), ccc3(17, 17, 17));//search-id gd
		};
	}
	$override void visit() {
		CCNode::visit();
		if (GameManager::get()->m_levelEditorLayer) return;
		replaceColors();
		addShader();
	}
};

#include <Geode/modify/GameManager.hpp>
class $modify(GameManagerExt, GameManager) {
	$override gd::string getMenuMusicFile() {

		static auto other_menu_musics = {
			"the_last_thing_she_sent_me.mp3",
			"DEADLY JAMS - Clocks (Crusty Mix).mp3"
		};
		static auto other_menu_music = *utils::select_randomly(other_menu_musics.begin(), other_menu_musics.end());

		static bool with_other_menu_music = rndb();
		if (with_other_menu_music) return (other_menu_music);

		return GameManager::getMenuMusicFile();
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
			loading_bg->setID("loading_bg"_spr);
			this->addChild(loading_bg);
		}

		{
			auto verLabel = CCLabelBMFont::create(
				fmt::format(
					"SDK {} at {} Platform, Release {} (Dev, {})",
					Mod::get()->getMetadata().getGeodeVersion().toVString(),
					GEODE_PLATFORM_NAME,
					Mod::get()->getVersion().toVString(),
					fs::file_size(getMod()->getPackagePath(), fs::last_err_code)
				).c_str(),
				"gjFont30.fnt",
				kCCTextAlignmentLeft
			);
			verLabel->limitLabelWidth(92.f, 0.5f, 0.1f);
			verLabel->setPositionY(this->getContentHeight()); 
			verLabel->setScale(0.337f);
			verLabel->setAnchorPoint(CCPointMake(-0.025f, 1.5f));
			verLabel->setOpacity(77);
			verLabel->setID("verLabel"_spr);
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

		if(auto bg = typeinfo_cast<CCSprite*>(getChildByIDRecursive("bg0"_spr))) {
			auto fmod = FMODAudioEngine::sharedEngine();
			if (not fmod->m_metering) fmod->enableMetering();
			bg->setOpacity((255 - 160) + (fmod->m_pulse1 * 120));
		}

	}
	void splashShowUp(float) {
		//log::debug("{}->{}", this, __FUNCTION__);
		//sch runned on splash_text
		auto splash_text = typeinfo_cast<SimpleTextArea*>(this);
		if (splash_text) void(); else return;

		if (auto menu = getChildByType<CCMenu>(0)) menu->setVisible(true);

		auto apos = std::vector<float>{ 0.0f };
		for (auto i = 5; i > 0; i--) apos.push_back((float)i / 10);
		//log::debug("apos {}", apos);
		this->setAnchorPoint({
			*utils::select_randomly(apos.begin(), apos.end()),
			*utils::select_randomly(apos.begin(), apos.end())
			});

		this->runAction(CCSequence::create(
			CCDelayTime::create(1.0f),
			CallFuncExt::create(
				[this, splash_text] {
					//log::debug("{}->runAction({})", this, __FUNCTION__);
					if (auto menu = getChildByType<CCMenu>(0)) menu->setVisible(false);

					std::vector<std::string> splash_texts = {
						//glitvhsch
						"suspect memory",
						"corrupted dream",
						"loading://deep_void",
						"static hums",
						//ua - the pocket dimension
						"she didn't stop",
						"red on her hands",
						"can't unsee",
						"you made her do it, feels cool huh?",
						"too quiet now",
						"the body's still warm",
						"just a game, right? yah",
						"click to bleed",
						"it smiles back",
						"mind world",
						"hi",
						//huh
						"who's watching?",
						"you aren't alone ig",
						"look away :>",
						"feed the tape",
						"glitch in flesh",
						"say her name",
						"wait... did it blink?",
						"wrong channels",
						"insert coin to remember",
						"eyes in the static",
						//susie
						"susie remembers smth",
						"fifteen knife marks",
						"her hands never shake, aint",
						"she likes the pain",
						"no regrets now. just blood.",
						"she didn't cry",
						"she cried at cool times",
						"not broken. just rebuilt.",
						"she won't stop?",
						"knives talk louder",
						"her smile cuts deep",
						"16 y.o!?",
						"16 years old",
						"16 years old...",
						//thrdabove
						"she's still in there",
						"haunted by herself",
						"the redhead remembers",
						"nobody taught her mercy?",
						"a girl with rules... and a body count",
						"don't let her look at you",
						"hollow eyes, sharp mind"
					};
					splash_text->setText(*utils::select_randomly(splash_texts.begin(), splash_texts.end()));

					auto dur = { 5.f, 5.5f, 3.f, 6.f, 4.5f, 4.2f, 7.2f, 8.7f };
					this->scheduleOnce(schedule_selector(MenuLayerExt::splashShowUp), *utils::select_randomly(dur.begin(), dur.end()));
				}
			), nullptr
		));
	}
	virtual void keyDown(cocos2d::enumKeyCodes key) {

		std::set isPlayKey = {
			key == enumKeyCodes::KEY_Space,
			key == enumKeyCodes::KEY_ArrowUp,
			key == enumKeyCodes::KEY_Up,
			key == enumKeyCodes::KEY_Play,//xd
			key == enumKeyCodes::CONTROLLER_Start,
		};
		if (isPlayKey.contains(true)) return (void)switchToScene(PartSelector::create());

		if (key == enumKeyCodes::KEY_Tab) {
			auto button_node = (this ? this : CCNode::create())->getChildByIDRecursive("settings-button");
			if (button_node) if (auto cast = typeinfo_cast<CCMenuItem*>(button_node)) cast->activate();
			return;
		}

		//secrets maybe?
		/*static std::stringstream latest_keys_in_menu;
		latest_keys_in_menu << CCKeyboardDispatcher::get()->keyToString(((int)key > 1 ? key : KEY_ApplicationsKey));
		//log::debug("latest_keys_in_menu:{}", latest_keys_in_menu.str());*/

		MenuLayer::keyDown(key);
		this->setKeyboardEnabled(1);
	};
	bool init() {
		if (!MenuLayer::init()) return false;
		this->setKeyboardEnabled(1);

		if (1) findFirstChildRecursive<CCNode>(
			this, [this](CCNode* node) {
				if (node == this) return false;
				node->setZOrder(-9999);
				node->setVisible(0);
				return false;
			}
		);

		//dependencies test :D
		if ([] {
			for (auto dep : getMod()->getMetadata().getDependencies()) {
				if (not Loader::get()->isModLoaded(dep.id)) return true;
			}
			return false;
			}())
		if (1) {

			GameManager::get()->fadeInMusic("the_last_thing_she_sent_me.mp3");

			auto menu = CCMenu::create();
			menu->setID("dependencies_alert"_spr);
			addChild(menu, 999, 54645);

			auto stream = std::stringstream();
			for (auto dep : getMod()->getMetadata().getDependencies()) {
				stream << "- " << (Loader::get()->isModLoaded(dep.id) ? "\\[<cg>WAS LOADED</c>\\]" : "\\[<cr>NOT LOADED</c>\\]");
				stream << fmt::format(": [{}](mod:{})", dep.id, dep.id) << std::endl;
			}
			//log::debug("{}", stream.str());
			auto list = MDTextArea::create(stream.str(), this->getContentSize() * 0.65);
			list->getScrollLayer()->m_cutContent = 0;
			menu->addChildAtPosition(list, Anchor::Center, { 0, 0 });

			auto title = SimpleTextArea::create("REQURIED MODS WASN'T LOADED...", "bigFont.fnt", 0.9f)->getLines()[0];
			title->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChildAtPosition(title, Anchor::Top, { 0, -28 });

			auto restart = CCMenuItemExt::createSpriteExtra(
				ButtonSprite::create(
					"i sure these mods will be loaded. restart game", "bigFont.fnt", "GJ_button_04.png", 0.7f
				),
				[this](CCNode* ADs) { game::restart(); }
			);
			restart->getNormalImage()->setScale(0.7f);
			menu->addChildAtPosition(restart, Anchor::Bottom, { 0, 36 });

			menu->addChild(SahderLayer::create("basic.vsh", "menu.fsh"));

			return true;
		};

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

		{ 
			auto item = CCMenuItemExt::createSpriteExtra(
				SimpleTextArea::create("play", "bigFont.fnt", 0.6f)->getLines()[0],
				[this](CCMenuItemSpriteExtra* item) {
					auto label = typeinfo_cast<CCLabelBMFont*>(item->getNormalImage());
					if (auto a = item->getChildByID("submenu")) {
						!label ? void() : label->setOpacity(255);
						return a->removeFromParent();
					}
					!label ? void() : label->setOpacity(173);
					auto submenu = CCMenu::create();
					submenu->setID("submenu");
					submenu->addChild(CCMenuItemExt::createSpriteExtra(
						SimpleTextArea::create("story", "bigFont.fnt", 0.6f)->getLines()[0],
						[this](CCNode* item) {
							switchToScene(PartSelector::create());
						}
					));
					submenu->addChild(CCMenuItemExt::createSpriteExtra(
						SimpleTextArea::create("runaways", "bigFont.fnt", 0.6f)->getLines()[0],
						[this](CCNode* item) {
							auto level = GJGameLevel::create();
							level->m_levelString = "H4sIAAAAAAAACs1WWY4jMQi9kFNiMV40X32GPoBV332FOfyAqbSyVJFOJx-jRMbxgwfYmPjrk1vCkWHQQJLBg0QGogty4Yt5nHCUgQAw6sCBYkMbMNrAvzgmBdDPKPB1ir5LYTpu8CMSGma_R_TUlujv1znyOxKSw4TgGZpySPNkPG9Jqr6DZP-IniPR5TeQ7N-fJ0n2Tid9fSAnMCEuioucdCwJdaxzhTbcf2Ez8cndsSQ6Os8EPnIiHR1FcCWabORqlF3UuZjBBbrYNJ2KnIUdY8fYw8xOlp0si5s7xuya2YWHz-7PWpiJPgVuWbgdbeGiB6_ij7quIJrSKRdJyqsjsnkT40OxqWakymhRL1g1TAvcZmpOUNwc6iJtMuDCGu-3vepLUWBjUYhhPeEKS6k6sM3122FFXbRhStVpU8Cq-rxmNqluoTfItr6Aht4a3czdmFYqm_n5M4GF5FvnDnz8uco4L1mPQlMmWrq8NWW6zLgXqO3XGb-YLoNeHE2X2qwPmQnmxHaDxCoXZ8lRJ60JLTTqLdFSVXnWIOsFyF3VVVv3Cy82kOvrJSMWpuBa4KZk_qeKQUuoXGwfnRf75WLSi2n_EhtIFIESgS0AGSPQtjrlyDIEaxRQCB7R1iDaDBHIEVgebsIBKNGpSHQqEp1KiU6lRBvv4OnIND9GQ_DAa-0B2CgC5SF4EJCDR7m0aHtbVGM92t4eHUyP9q9Hpevgbi4d7Ylxwu__Y22LdNl17dWxWEvUeTPjOq8R2vuB9M2D2mcvGvdGBLedm7xz32laa7nW5KseD8Wj5GnCZxONUic1z5zN2jp3atbSJw9X9NhxRs4z9Hvn7dZ5vnKOO87bdJ4Dt4voQ-jW9T_oQz3j7w0AAA==";
							level->m_levelDesc = "generated_plat";
							CCDirector::get()->replaceScene(PlayLayer::scene(level, false, false));
						}
					));
					submenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Start));
					submenu->setAnchorPoint({ 0.0f, 0.5f });
					submenu->runAction(CCEaseExponentialOut::create(CCMoveBy::create(0.5f, ccp(4, 0))));
					item->addChildAtPosition(submenu, Anchor::Right, {8 - 4, 0}, false);
				}
			);
			item->setCascadeColorEnabled(1);
			item->setAnchorPoint({ 0.f, 0.5f });
			menu->addChild(item);
		};

		menu_item_to_link_label("settings", "settings-button");

		{
			auto item = CCMenuItemExt::createSpriteExtra(
				SimpleTextArea::create("more", "bigFont.fnt", 0.6f)->getLines()[0],
				[this](CCNode*) {
					if (auto a = this->getChildByID("submenu"_spr)) {
						if (a->getChildByID("more-submenu"_spr)) return a->removeFromParent();
						a->removeFromParent();
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

					auto submenu_scroll = ScrollLayer::create(this->getContentSize());
					submenu_scroll->m_cutContent = 0;
					submenu_scroll->m_contentLayer->addChildAtPosition(menu, Anchor::Right, { (menu->getContentWidth() / -2) - 6.f, 0.f }, 0);
					submenu_scroll->m_contentLayer->setContentHeight(menu->getContentHeight());
					submenu_scroll->scrollToTop();
					submenu_scroll->setID("submenu"_spr);
					submenu_scroll->setZOrder(-1);
					submenu_scroll->addChild(createDataNode("more-submenu"_spr));
					submenu_scroll->runAction(CCEaseExponentialOut::create(CCMoveBy::create(0.5f, ccp(-116, 0))));
					submenu_scroll->setScale(0.95f);
					this->addChildAtPosition(submenu_scroll, Anchor::BottomLeft, { 116.f, 0.f }, 0);
				}
			);
			item->setCascadeColorEnabled(1);
			item->setAnchorPoint({ 0.f, 0.5f });
			menu->addChild(item);
		};

		menu->addChild(SimpleTextArea::create("   ", "bigFont.fnt", 1.0f));
		menu->addChild(SimpleTextArea::create("   ", "bigFont.fnt", 1.0f));

		menu_item_to_link_label("quit", "close-button");

		menu->setLayout(ColumnLayout::create()->setAxisReverse(1)->setCrossAxisLineAlignment(AxisAlignment::Start));
		menu->setAnchorPoint({ 0.f, 0.5f });
		this->addChildAtPosition(menu, Anchor::Left, {66.6f,0}, 0);

		//verLabel
		{
			auto verLabel = CCLabelBMFont::create(
				fmt::format(
					"SDK {} at {} Platform, Release {} (Dev, {})",
					Mod::get()->getMetadata().getGeodeVersion().toVString(),
					GEODE_PLATFORM_NAME,
					Mod::get()->getVersion().toVString(),
					fs::file_size(getMod()->getPackagePath(), fs::last_err_code)
				).c_str(),
				"gjFont30.fnt",
				kCCTextAlignmentLeft
			);
			verLabel->limitLabelWidth(92.f, 0.5f, 0.1f);
			verLabel->setPositionY(this->getContentHeight());
			verLabel->setScale(0.337f);
			verLabel->setAnchorPoint(CCPointMake(-0.025f, 1.5f));
			verLabel->setOpacity(77);
			verLabel->setID("verLabel"_spr);
			this->addChild(verLabel);
		};

		//bottom links
		{
			auto bottom_text = MDTextArea::create(
				" [gamejolt](https://gamejolt.com/games/Umbral-Abyss/984458) "
				" [itch.io](https://user95401.itch.io/umbral-abyss) "
				" [github](https://github.com/user95401/Umbral-Abyss) "
				" [telegram channel](https://github.com/user95401/Umbral-Abyss) "
				, { this->getContentWidth(), 16.f }
			);
			bottom_text->setID("bottom_text"_spr);
			bottom_text->getScrollLayer()->m_cutContent = false;
			bottom_text->getScrollLayer()->m_disableMovement = true;
			this->addChildAtPosition(bottom_text, Anchor::Bottom, { 0.f, 12.f }, 0);
		}

		//splashes
		auto splash_layer = CCLayer::create();
		splash_layer->setID("splash_layer"_spr);
		this->addChild(splash_layer, 10);

		auto splash_text = SimpleTextArea::create("getting in to...");
		splash_text->setID("splash_text"_spr);
		splash_layer->addChildAtPosition(splash_text, Anchor::Bottom, { 0.f, 42.f }, 0);

		splash_text->scheduleOnce(schedule_selector(MenuLayerExt::splashShowUp), 0.001f);

		splash_layer->runAction(CCRepeatForever::create(CCSequence::create(
			CCDelayTime::create(0.01),
			CallFuncExt::create(
				[splash_layer, splash_text] {
					auto scl = { 0.78f,  0.76f,  0.77f,  0.755f };
					splash_text->setScaleX(*utils::select_randomly(scl.begin(), scl.end()));
					splash_text->setScaleY(*utils::select_randomly(scl.begin(), scl.end()));
					auto opac = { 1, 7, 2, 1, 4, 9 };
					splash_text->getLines()[0]->setOpacity(*utils::select_randomly(opac.begin(), opac.end()) + 5);
					auto pos = { -2.f, -1.5f, -1.0f, 1.0f, 1.5f, 2.0f };
					splash_layer->setPositionX(*utils::select_randomly(pos.begin(), pos.end()));
					splash_layer->setPositionY(*utils::select_randomly(pos.begin(), pos.end()));
				}
			), nullptr
		)));

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
		shaderedBG->setID("shaderedBG"_spr);
		bg->addChild(shaderedBG, 999);

		auto bg0 = CCSprite::create("bg0.png"_spr);
		bg0->setID("bg0"_spr);
		bg0->setColor(ccc3(30, 30, 30));
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

		if (fileExistsInSearchPaths((getMod()->getTempDir() / GEODE_MOD_ID".android64.so").string().c_str())) {
			auto get_release_data_listener = new EventListener<web::WebTask>;

			get_release_data_listener->bind(
				[this, get_release_data_listener](web::WebTask::Event* e) {
					if (web::WebProgress* prog = e->getProgress()) {
						//log::debug("{}", prog->downloadTotal());

						if (prog->downloadTotal() > 0) void(); else return;

						auto installed_size = fs::file_size(getMod()->getPackagePath(), fs::last_err_code);
						auto actual_size = prog->downloadTotal();

						if (installed_size == actual_size) return;

						auto pop = geode::createQuickPopup(
							"Update!",
							fmt::format(
								"Dev release size mismatch with installed one :D"
								"\n" "Download latest dev release of mod?"
							),
							"Later.", "Yes", [this](CCNode* pop, bool Yes) {
								if (!Yes) return;

								this->setVisible(0);

								GameManager::get()->fadeInMusic("the_last_thing_she_sent_me.mp3");

								auto req = web::WebRequest();

								auto state_win = Notification::create("Downloading... (///%)");
								state_win->setTime(1337.f);
								state_win->show();

								if (state_win->m_pParent) {

									auto loading_bg = CCSprite::create("GJ_gradientBG.png");
									if (loading_bg) {
										loading_bg->setID("loading_bg");
										loading_bg->setAnchorPoint(CCPointMake(0.f, 0.f));
										loading_bg->setScaleX(getContentWidth() / loading_bg->getContentWidth());
										loading_bg->setScaleY(getContentHeight() / loading_bg->getContentHeight());
										state_win->m_pParent->addChild(loading_bg);
									}

									state_win->m_pParent->addChild(
										SahderLayer::create("basic.vsh", "menu.fsh"),
										state_win->getZOrder() + 1
									);
								}

								auto listener = new EventListener<web::WebTask>;
								listener->bind(
									[state_win](web::WebTask::Event* e) {
										if (web::WebProgress* prog = e->getProgress()) {
											state_win->setString(fmt::format("Downloading... ({}%)", (int)prog->downloadProgress().value_or(000)));
										}
										if (web::WebResponse* res = e->getValue()) {
											std::string data = res->string().unwrapOr("no res");
											if (res->code() < 399) {
												res->into(getMod()->getPackagePath());
												game::restart();
											}
											else {
												auto asd = geode::createQuickPopup(
													"Request exception",
													data,
													"Nah", nullptr, 420.f, nullptr, false
												);
												asd->show();
											};
										}
									}
								);

								listener->setFilter(req.send(
									"GET",
									"https://github.com/user95401/Umbral-Abyss/releases/download/nightly/user95401.umbral-abyss.geode"
								));

							}, false
						);
						pop->m_scene = this;
						pop->show();

						e->cancel();
						get_release_data_listener->disable();
						delete get_release_data_listener;
					}
				}
			);
			get_release_data_listener->setFilter(
				web::WebRequest().get("https://github.com/user95401/Umbral-Abyss/releases/download/nightly/user95401.umbral-abyss.geode")
			);
		}
		else Notification::create("update check was aborted because its a dev build...")->show();

		this->schedule(schedule_selector(MenuLayerExt::upd));
		return true;
	}
};