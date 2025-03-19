#include <_main.hpp>
#include <fs.hpp>

#include <Geode/utils/web.hpp>
#include <SahderLayer.hpp>
#include <ViewportLayer.hpp>

#include <player.hpp>
#include <resources.hpp>

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
			auto page = [](int id, std::string name, std::string file)
				{

					auto level = GameLevelManager::get()->getMainLevel(id, false);
					level->m_levelID = id;
					level->m_levelName = name.c_str();

					if (id == 116102387) level->m_levelString = (
						"H4sIAAAAAAAACrVUSW7jMBD8UFvojYuQU96QBxA-5wt5_PRCjROMMhAR58Ki3NXVK_3-Jh1oKA4exGXI4FIGUQIn5I86bjTqIEQcbdCg4kcfOPqgDxohgXxNgn4usZ9KOCcdLonwqchSO-z75xr6jGLKcP_vGntZpn4rs5jPU4pqzxA5H9GaCJ3vyqLI-dtZFDmbDry_kkAnxwIBNUHBzry3OLkAuiG_qPvXm-zxI0OxU-IehlcNVloJk8SpkzTWhJRTTKCEyUwpThVJm6RNKrAzU0xTTLMEzbRFEjIVyfQl4_n_l8OexcwqasJMlzJ5gxcCYzLoXkCgwu4BqFg0t1REs1mvzUZYoLRptUwt97LtHs2zF_drftOKcca9txCi3UbBcFMX4l6mRjiJe23VmZ6d-GEM9vgU0WuEj-j82dOHg_8MB2M49HvDwf8MB58yHHoMJzqg7dEBiQ4IRMqexp3uN7obfGXrErstsecqXKbLGr2u0fcLdFRfcmkyiz12uIPGJuunLS6Gufzl785K-7Kzigo3sldDPk8lH6fG4NS2Dbdi2GPzGauE1zFCb8Z8YXZWi8xHKrghRTZ5wXhsR1rximwJL1fhjUinRxne2ukkB5F877aqL38Aa8rQofsIAAA="
						);

					auto menu = CCMenu::create();
					menu->setTag(id);

					auto title = SimpleTextArea::create(
						name, "bigFont.fnt", 0.6f
					)->getLines()[0];
					title->setAnchorPoint({ 0.5f, 0.5f });
					menu->addChildAtPosition(title, Anchor::Bottom, { 0.f, 42.f });

					auto image = CCSprite::create(file.c_str());
					limitNodeHeight(image, 246.f, 999.f, 0.1f);
					menu->addChildAtPosition(CCMenuItemExt::createSpriteExtra(image, [level, id](CCNode* a)
						{
							CCDirector::get()->replaceScene(PlayLayer::scene(level, false, false));
						}
					), Anchor::Center, {0.f, 33.f});

					return menu;

				};
			pages.push_back(page(116102387, "part 1: you know when u should stop", "Screenshot_76.png"));
			pages.push_back(page(5001, "part 2: test", "Screenshot_166.png"));
			pages.push_back(page(5002, "part 3: test", "Screenshot_181.png"));
			pages.push_back(page(5003, "part 4: test", "Screenshot_195.png"));
			pages.push_back(page(5004, "part 5: test", "p5.png"));

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
class $modify(ColorsController, CCNode) {
	$override void visit() {
		if (this == nullptr) return;
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
		};
		if (auto node = rgba_layr) {
			repl(ccc3(191, 114, 62), ccc3(6, 6, 6));
			repl(ccc3(161, 88, 44), ccc3(10, 10, 10));
			repl(ccc3(194, 114, 62), ccc3(8, 8, 8));
		};
		CCNode::visit();
	}
};

#include <Geode/modify/GameManager.hpp>
class $modify(GameManagerExt, GameManager) {
	$override gd::string getMenuMusicFile() {
		static bool other_menu_music = rndb(4);
		if (other_menu_music) return ("the_last_thing_she_sent_me.mp3");
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
			verLabel->setAnchorPoint(CCPointMake(-0.025f, 1.5f));
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

		if(auto bg = typeinfo_cast<CCSprite*>(getChildByIDRecursive("bg0"_spr))) {
			auto fmod = FMODAudioEngine::sharedEngine();
			if (not fmod->m_metering) fmod->enableMetering();
			bg->setOpacity((255 - 160) + (fmod->m_pulse1 * 120));
		}

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

		if (key == enumKeyCodes::KEY_Tab) return this->openOptions(0);

		//secrets maybe?
		/*static std::stringstream latest_keys_in_menu;
		latest_keys_in_menu << CCKeyboardDispatcher::get()->keyToString(((int)key > 1 ? key : KEY_ApplicationsKey));
		log::debug("latest_keys_in_menu:{}", latest_keys_in_menu.str());*/

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
		{

			GameManager::get()->fadeInMusic("the_last_thing_she_sent_me.mp3");

			auto menu = CCMenu::create();
			menu->setID("dependencies_alert"_spr);
			addChild(menu, 999, 54645);

			auto stream = std::stringstream();
			for (auto dep : getMod()->getMetadata().getDependencies()) {
				stream << "- " << (Loader::get()->isModLoaded(dep.id) ? "\\[<cg>WAS LOADED</c>\\]" : "\\[<cr>NOT LOADED</c>\\]");
				stream << fmt::format(": [{}](mod:{})", dep.id, dep.id) << std::endl;
			}
			log::debug("{}", stream.str());
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

		auto get_release_data_listener = new EventListener<web::WebTask>;

		//get mod json
		get_release_data_listener->bind(
			[this, get_release_data_listener](web::WebTask::Event* e) {
				if (web::WebResponse* res = e->getValue()) {
					delete get_release_data_listener;

					auto str = res->string().unwrapOr("xd");

					auto parsed = matjson::parse(str).unwrapOrDefault();

					auto installed_size = fs::file_size(getMod()->getPackagePath(), fs::last_err_code);
					auto actual_size = parsed["release"]["assets"][0]["size"].asInt().unwrapOrDefault();

					if (installed_size == actual_size) return;

					auto pop = geode::createQuickPopup(
						"Update!",
						fmt::format(
							"Dev release size mismatch with installed one :D"
							"\n" "Download latest dev release of mod?"
						),
						"Later.", "Yes", [this, parsed](CCNode* pop, bool Yes) {
							if (!Yes) return;

							this->setVisible(0);

							GameManager::get()->fadeInMusic("the_last_thing_she_sent_me.mp3");

							auto req = web::WebRequest();

							auto state_win = Notification::create("Downloading... (///%)");
							state_win->setTime(1337.f);
							state_win->show();

							if (state_win->m_pParent) state_win->m_pParent->addChild(
								SahderLayer::create("basic.vsh", "menu.fsh"),
								state_win->getZOrder() + 1
							);
							
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
								parsed["release"]["assets"][0]["downloadUrl"].asString().unwrapOrDefault()
							));

						}, false
					);
					pop->m_scene = this;
					pop->show();
				}
			}
		);
		get_release_data_listener->setFilter(
			web::WebRequest().get("https://ungh.cc/repos/user95401/Umbral-Abyss/releases/latest")
		);

		this->schedule(schedule_selector(MenuLayerExt::upd));
		return true;
	}
};