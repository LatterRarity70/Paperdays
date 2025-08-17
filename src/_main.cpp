#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <hjfod.gmd-api/include/GMD.hpp>

#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
class $modify(CCMenuItemSpriteExtraExt, CCMenuItemSpriteExtra) {
	$override void selected() {
		if (m_selectSound.empty()) this->m_selectSound = "menuItemSelectSound.ogg"_spr;
		if (m_activateSound.empty()) this->m_activateSound = "menuItemActivateSound.ogg"_spr;
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

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayerExt, LoadingLayer) {
	static void resourceSetup() {
		auto resources_dir = getMod()->getResourcesDir();

		auto tp = CCTexturePack();
		tp.m_paths = { string::pathToString(resources_dir).c_str() };
		tp.m_id = "resources"_spr;
		CCFileUtils::get()->addTexturePack(tp);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(resources_dir)) {
			if (!entry.is_regular_file()) continue;

			auto ext = entry.path().extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			if (string::containsAny(ext, { ".png", ".jpg", ".jpeg", ".plist" })) {
				auto relativePath = std::filesystem::relative(entry.path(), resources_dir);
				CCSpriteFrameCache::get()->addSpriteFrame(
					CCSprite::create(string::pathToString(relativePath).c_str())->displayFrame(),
					string::pathToString(relativePath).c_str()
				);
			}
		}
	}
	bool init(bool penis) {
		resourceSetup();
		if (!LoadingLayer::init(penis)) return false;

		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return true;

		if (1) findFirstChildRecursive<CCNode>(
			this, [this](CCNode* node) {
				if (node == this) return false;
				node->setZOrder(-9999);
				node->setVisible(0);
				return false;
			}
		);

		auto text = SimpleTextArea::create(
			"hold on control to seen avoid the              ", "chatFont.fnt"
		);
		addChildAtPosition(text, Anchor::Center, { 0, 0 }, false);

		this->runAction(CCRepeatForever::create(CCSequence::create(
			CCShaky3D::create(0.01f, { 1, 1 }, 11, false),
			CallFuncExt::create(
				[__this = Ref(this), text = Ref(text)] {
					if (__this->m_loadStep) text->setText(fmt::format(
						"                   {}                 ", 
						__this->m_loadStep
					));

					if (auto g = __this->m_pGrid) if (auto t = g->m_pTexture) t->setAliasTexParameters();
				}
			),
			nullptr
		)));

		FMODAudioEngine::get()->setBackgroundMusicVolume(GameManager::get()->m_bgVolume);
		FMODAudioEngine::get()->setEffectsVolume(GameManager::get()->m_sfxVolume);

		GameManager::get()->fadeInMusic("loading.mp3");
		
		return true;
	}
};

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerExt, MenuLayer) {
	virtual void keyDown(cocos2d::enumKeyCodes p0) {
		CCLayer::keyDown(p0);
	};
	bool init() {
		if (!MenuLayer::init()) return false;

		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return true;

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
			for (auto dep : getMod()->getMetadataRef().getDependencies()) {
				if (not Loader::get()->isModLoaded(dep.id)) return true;
			}
			return false;
			}())
		if (1) {

			GameManager::get()->fadeInMusic("loading.wav"_spr);

			auto menu = CCMenu::create();
			menu->setID("dependencies_alert"_spr);
			addChild(menu, 999, 54645);

			auto stream = std::stringstream();
			for (auto dep : getMod()->getMetadataRef().getDependencies()) {
				stream << "- " << (Loader::get()->isModLoaded(dep.id) ? "\\[<cg>WAS LOADED</c>\\]" : "\\[<cr>NOT LOADED</c>\\]");
				stream << fmt::format(": [{}](mod:{})", dep.id, dep.id) << std::endl;
			}
			//log::debug("{}", stream.str());
			auto list = MDTextArea::create(stream.str(), this->getContentSize() * 0.65);
			list->getScrollLayer()->m_cutContent = 0;
			menu->addChildAtPosition(list, Anchor::Center, { 0, 0 });

			auto title = SimpleTextArea::create("REQUIRED MODS WASN'T LOADED...", "bigFont.fnt", 0.9f)->getLines()[0];
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

			return true;
		};

		auto static useful = 0;
		if (!useful++) {
			auto popup = createQuickPopup(
				" \n                   oh hi there! \n \n                                ar u useful????",
				"aaaaaaaaaaaaaaaaaaaaaaaaaaa", "no", "yes",
				[](void*, bool yes) {
					if (!yes) game::exit();
					GameManager::get()->fadeInMusic("menuLoop.mp3");
				}, !"show"
			);
			if (popup) if (auto g = popup->getGrid()) if (auto t = g->m_pTexture) t->setAliasTexParameters();
			if (popup) if (auto a = popup->m_mainLayer->getChildByType<TextArea>(0)) a->setString(
				" !*-the ceiling laughs. yes, it laughs. -\n"
				" 2 are you me??? or am i you???\n"
				"Sss Ss-1 stop...stop...the f ngers in the dark\n"
				"aa a u.u.u.u.u.u.u. crying clocks 181 2\n"
				"i see everything, nothing, something...?\n"
				"3 - 9 ...and so it begins again... 6667\n"
			);
			if (auto a = popup->m_mainLayer->getChildByType<TextArea>(0)) a->setOpacity(12);
			popup->m_mainLayer->runAction(CCRepeatForever::create(CCSequence::create(
				CCMoveTo::create(0.01f, { 0, 0.21 }),
				CCMoveTo::create(0.01f, { 0, -0.21 }),
				CCMoveTo::create(0.01f, { 0.21, 0 }),
				CCMoveTo::create(0.01f, { -0.21, 0 }),
				CCMoveTo::create(0.01f, { 0.21, 0.21 }),
				CCMoveTo::create(0.01f, { -0.21, -0.21 }),
				nullptr
			)));
			popup->m_noElasticity = this;
			popup->m_scene = this;
			popup->show();
			popup->setOpacity(255);
			GameManager::get()->fadeInMusic("hi.mp3");
		}

		for (auto file : { "menuBG_1.png", "menuBG_2.png" }) {
			CCFileUtils::get()->m_fullPathCache["GJ_gradientBG.png"] = CCFileUtils::get()->fullPathForFilename(file, 0);
			auto bg = geode::createLayerBG();
			bg->setColor(ccWHITE);
			addChild(bg);
			CCFileUtils::get()->m_fullPathCache.erase("GJ_gradientBG.png");
			;
			if (file == std::string("menuBG_1.png")) {
				auto anim1 = CCSprite::create("menuBG_1.png");
				auto anim2 = CCSprite::create("menuBG_11.png");
				this->runAction(CCRepeatForever::create(CCSequence::create(
					CCDelayTime::create(1.0f),
					CallFuncExt::create([bg = Ref(bg), anim = Ref(anim1)] { bg->setDisplayFrame(anim->displayFrame()); }),
					CCDelayTime::create(1.0f),
					CallFuncExt::create([bg = Ref(bg), anim = Ref(anim2)] { bg->setDisplayFrame(anim->displayFrame()); }),
					nullptr
				)));
			}
		};

		auto menu = CCMenu::create();
		menu->setID("menu"_spr);
		addChild(menu);

		menu->addChild(SimpleTextArea::create(getMod()->getMetadataRef().getName(), "bigFont.fnt", 1.2f)->getLines()[0]);

		auto verl = SimpleTextArea::create(fmt::format(
			"SDK {} on {}, Release {} (Dev, {})",
			Mod::get()->getMetadataRef().getGeodeVersion().toVString(),
			GEODE_PLATFORM_NAME,
			Mod::get()->getVersion().toVString(),
			std::filesystem::file_size(getMod()->getPackagePath())
		).c_str(), "chatFont.fnt", 0.6f)->getLines()[0];
		verl->setOpacity(63);
		menu->addChild(verl);

		menu->addChild(SimpleTextArea::create("             ", "bigFont.fnt", 1.5f)->getLines()[0]);

		auto play = CCMenuItemExt::createSpriteExtra(
			SimpleTextArea::create("play", "chatFont.fnt", 1.0f)->getLines()[0],
			[__this = Ref(this)](CCNode* item) { __this->onPlay(item); }
		);
		menu->addChild(play);

		auto geode = CCMenuItemExt::createSpriteExtra(
			SimpleTextArea::create("geode", "chatFont.fnt", 1.0f)->getLines()[0],
			[__this = Ref(this)](CCNode* item) { 
				if (auto item = typeinfo_cast<CCMenuItem*>(__this->querySelector(
					"geode.loader/geode-button"
				))) item->activate();
			}
		);
		menu->addChild(geode);

		auto settings = CCMenuItemExt::createSpriteExtra(
			SimpleTextArea::create("settings", "chatFont.fnt", 1.0f)->getLines()[0],
			[__this = Ref(this)](CCNode* item) { __this->onOptions(item); }
		);
		menu->addChild(settings);

		menu->setLayout(SimpleColumnLayout::create()->setGap(10.f)); 

		findFirstChildRecursive<CCNode>(menu,
			[](CCNode* node) {
				auto s = 5.f;
				auto p = node->getPosition();
				node->runAction(CCRepeatForever::create(CCSequence::create(
					(CCRotateTo::create(s - CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1())),
					(CCRotateTo::create(s - CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1())),
					(CCRotateTo::create(s - CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1())),
					(CCRotateTo::create(s - CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1())),
					(CCRotateTo::create(s - CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1())),
					nullptr
				)));
				node->runAction(CCRepeatForever::create(CCSequence::create(
					(CCMoveTo::create(s - CCRANDOM_MINUS1_1(), p + CCPointMake(CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1()))),
					(CCMoveTo::create(s - CCRANDOM_MINUS1_1(), p + CCPointMake(CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1()))),
					(CCMoveTo::create(s - CCRANDOM_MINUS1_1(), p + CCPointMake(CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1()))),
					(CCMoveTo::create(s - CCRANDOM_MINUS1_1(), p + CCPointMake(CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1()))),
					(CCMoveTo::create(s - CCRANDOM_MINUS1_1(), p + CCPointMake(CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1()))),
					nullptr
				)));
				return false;
			}
		);

		return true;
	}
};

#include <Geode/modify/GManager.hpp>
class $modify(GManagersOvertake, GManager) {
	void setup() {
		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return GManager::setup();

		m_fileName = ("___properdiess_" + string::toLower(m_fileName.c_str())).c_str();
		GManager::setup();
	}
};