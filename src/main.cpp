#include <_main.hpp>
#include <fs.hpp>

#include <SahderLayer.hpp>
#include <ViewportLayer.hpp>

#include <player.hpp>
#include <resources.hpp>

class PartSelector : public CCLayer {
	public:
	CREATE_FUNC(PartSelector);
	virtual void keyBackClicked(void) override {
		auto asd = CreatorLayer::create();
		asd->onBack(nullptr);
		asd->release();
	}
	virtual bool init() override {
		if (!CCLayer::init()) return false;

		this->setTouchEnabled(1);
		this->setKeyboardEnabled(1);
		this->setKeypadEnabled(1);

		auto menu = CCMenu::create();

		auto back = CCMenuItemExt::createSpriteExtraWithFrameName(
			"GJ_arrow_01_001.png", 1.0f, [this](CCNode* ADs) { this->keyBackClicked(); }
		);
		menu->addChildAtPosition(back, Anchor::TopLeft, { 36, -36 });

		auto title = SimpleTextArea::create("s e l e c t   p a r t", "bigFont.fnt", 0.9f)->getLines()[0];
		title->setAnchorPoint({ 0.5f, 0.5f });
		menu->addChildAtPosition(title, Anchor::Top, { 0, -28 });

		addChild(menu);

		auto pages = CCArrayExt<CCLayer>();
		auto createLevelPage = [](std::string name, std::string file)
			{
				auto menu = CCMenu::create();

				auto title = SimpleTextArea::create(
					name, "bigFont.fnt", 0.6f
				)->getLines()[0];
				title->setAnchorPoint({ 0.5f, 0.5f });
				menu->addChildAtPosition(title, Anchor::Bottom, { 0.f, 42.f });

				auto image = CCSprite::create(file.c_str());
				limitNodeHeight(image, 246.f, 999.f, 0.1f);
				menu->addChildAtPosition(image, Anchor::Center, { 0.f, 33.f });

				return menu;
			};
		pages.push_back(createLevelPage("part 1: you know when u should stop", "Screenshot_76.png"));
		pages.push_back(createLevelPage("part 2: test", "Screenshot_166.png"));
		pages.push_back(createLevelPage("part 3: test", "Screenshot_181.png"));
		pages.push_back(createLevelPage("part 4: test", "Screenshot_195.png"));

		auto scroll = BoomScrollLayer::create(pages.inner(), 0, 0);
		scroll->setScale(0.825f);
		scroll->setAnchorPoint(CCPointMake(0.5f, 0.15f));

		addChild(scroll);

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
		menu_item_to_link_label("geode", "geode.loader/geode-button");

		menu->addChild(SimpleTextArea::create("   ", "bigFont.fnt", 1.0f));

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
					submenu_scroll->m_contentLayer->addChildAtPosition(menu, Anchor::Right, { (menu->getContentWidth()/-2) - 6.f, 0.f }, 0);
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