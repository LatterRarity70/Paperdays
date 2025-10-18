#include <Geode/Geode.hpp>
using namespace geode::prelude;

#define saves getMod()->getSaveContainer

#include <user95401.main-levels-editor/include/level.hpp>

bool static FLASHES_MODE = [] { srand(time(nullptr)); return rand() % 3 == 1; }();

void disableIMEInpMod() {
	auto mod = Loader::get()->getInstalledMod("alk.ime-input");
	if (mod) for (auto hook : mod->getHooks()) if (hook) hook->disable();
}
$on_mod(Loaded) { disableIMEInpMod(); }

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayerExt, LoadingLayer) {
	static void resourceSetup() {
		auto resources_dir = getMod()->getResourcesDir();

		auto tp = CCTexturePack();
		tp.m_paths = { 
			string::pathToString(resources_dir).c_str(), 
			string::pathToString(resources_dir.parent_path()).c_str() //resources/../id
		};
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

		//happy
		CCFileUtils::get()->m_fullPathCache["alphalaneous.happy_textures/bigFont.fnt"] = CCFileUtils::get()->fullPathForFilename(
			"bigFont.fnt"_spr, 0
		);
		CCFileUtils::get()->m_fullPathCache["alphalaneous.happy_textures/bigFont.png"] = CCFileUtils::get()->fullPathForFilename(
			"bigFont.png"_spr, 0
		);

		for (auto path : file::readDirectory(getMod()->getResourcesDir()).unwrapOrDefault()) {
			auto str = string::pathToString(path);
			auto name = string::pathToString(path.filename());
			auto nsub = string::replace(name, "..", "/");
			if (string::contains(str, "..")) CCFileUtils::get()->m_fullPathCache[nsub] = CCFileUtils::get()->fullPathForFilename(
				name.c_str(), 0
			);
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
			"hold on control\n"
			"to seen avoid the\n \n "
			":WARN:\n"
			"line4", 
			
			"chatFont.fnt", 2.f
		);
		text->setAlignment(kCCTextAlignmentCenter);
		addChildAtPosition(text, Anchor::Center, { 0, 0 }, false);

		this->runAction(CCRepeatForever::create(CCSequence::create(
			CCShaky3D::create(0.01f, { 1, 1 }, 11, false),
			CallFuncExt::create(
				[__this = Ref(this), text = Ref(text)] {
					if (__this->m_loadStep and text->getTag() != __this->m_loadStep) {
						text->setTag(__this->m_loadStep);
						text->setText(fmt::format(
							"{}\n \n \n "
							":WARN:\n"
							"line4",
							__this->m_loadStep
						));
					}

					auto line4 = std::string(
						"game contains self-harm imagery, graphic violence,\n"
						"disturbing themes, strong language ^ .^"
					);
					text->getLines().at(4)->setAlignment(kCCTextAlignmentCenter);
					text->getLines().at(4)->setString(line4.c_str());
					text->getLines().at(4)->setScale(1.100f);

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
	static cocos2d::CCScene* scene(bool isVideoOptionsOpen) {
		static auto killgame = false;
		if (killgame) {
			getMod()->saveData();
			game::exit();
		}
		auto ragebaited = saves()["ragebaited"].asInt().unwrapOr(0);
		auto ragebait_answered = saves()["ragebait_answered"].asBool().unwrapOr(false);
		if (ragebaited and !ragebait_answered) {
			auto name = std::string("properdies_ragebaited.level");
			auto path = string::pathToString(CCFileUtils::get()->fullPathForFilename(name.c_str(), 0).c_str());
			auto devmode = string::pathToString(
				CMAKE_CURRENT_SOURCE_DIR "/assets/files/levels/" + name
			);
			if (fileExistsInSearchPaths(devmode.c_str())) {
				Notification::create(" Level loaded from source folder :D\n " + devmode, NotificationIcon::Info, 3.f)->show();
				path = devmode;
			}
			auto import = level::importLevelFile(path);
			if (import.isOk()) {
				killgame = true;
				auto xd = PlayLayer::scene(import.unwrapOrDefault(), 0, 0);
				return xd;
			}
			else {
				Notification::create(
					" Failed to import:\n " + path, NotificationIcon::Error, 3.f
				)->show();
				if (import.err()) Notification::create(
					import.err().value_or("unk err .-."), 
					NotificationIcon::Error, 3.f
				)->show();
			}
		}
		return MenuLayer::scene(isVideoOptionsOpen);
	};
	bool init() {

		if (FLASHES_MODE) {
			CCFileUtils::get()->m_fullPathCache["menuLoop.mp3"] = CCFileUtils::get()->fullPathForFilename("flashes.mp3"_spr, 0);
		}

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
				" \n            oh hi there! \n \n                    ar u useful????",
				"aaaaaaaaaaaaaaaaaaaaaaaaaaa\naaaaaaaaaa\naaaaaaaa", "no", "yes",
				[](void*, bool yes) {
					if (!yes) {
						if (saves()["useful-confirmed"].asBool().unwrapOr(false)) {
							saves()["ragebaited"] = saves()["ragebaited"].asInt().unwrapOr(0) + 1;
							saves()["ragebait_answered"] = false;
							getMod()->saveData();
						};
						game::exit();
					}
					GameManager::get()->playMenuMusic();
					saves()["useful-confirmed"] = true;
					getMod()->saveData();
				}, !"show"
			);
			if (popup) if (auto g = popup->getGrid()) if (auto t = g->m_pTexture) t->setAliasTexParameters();
			if (popup) if (auto a = popup->m_mainLayer->getChildByType<TextArea>(0)) a->setString(
				" !*-the ceiling laughs. ya, it laughs. -\n"
				" 2 are you me??? or am i you???\n"
				"Ss-1 stop...the f ngers in the dark\n"
				"aa a u.u.u.u. crying clocks 181 2\n"
				"i see everythn, nothin, somethin...?\n"
				"3 - 9 ...and so it begins again...\n"
			);
			if (auto a = popup->m_mainLayer->getChildByType<TextArea>(0)) a->setOpacity(12);
			findFirstChildRecursive<CCNode>(popup->m_mainLayer,
				[](CCNode* node) {
					auto batched = typeinfo_cast<CCSpriteBatchNode*>(node->getParent());
					auto label = typeinfo_cast<CCLabelProtocol*>(node->getParent());
					if (batched and not label) return false;
					auto dl = 5.0f;
					auto dt = CCDelayTime::create(0.1f);
					node->runAction(CCRepeatForever::create(CCSequence::create(
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
						dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() * dl),
						nullptr
					)));
					auto p = node->getPosition();
					node->runAction(CCRepeatForever::create(CCSequence::create(
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() * dl, CCRANDOM_MINUS1_1() * dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() * dl, CCRANDOM_MINUS1_1() * dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
						dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() * dl, CCRANDOM_MINUS1_1() * dl)),
						nullptr
					)));
					return false;
				}
			);
			popup->m_noElasticity = this;
			popup->m_scene = this;
			popup->show();
			popup->setOpacity(255);
			GameManager::get()->fadeInMusic("hi.mp3");
		}

		CCFileUtils::get()->m_fullPathCache["GJ_gradientBG.png"] = CCFileUtils::get()->fullPathForFilename("menuBG_1.png", 0);
		auto bg = geode::createLayerBG();
		CCFileUtils::get()->m_fullPathCache.erase("GJ_gradientBG.png");
		bg->setColor(ccWHITE);
		addChild(bg);
		auto bganim1 = CCSprite::create("menuBG_1.png");
		auto bganim2 = CCSprite::create("menuBG_11.png");
		bg->runAction(CCRepeatForever::create(CCSequence::create(
			CCDelayTime::create(1.0f),
			CallFuncExt::create([bg = Ref(bg), anim = Ref(bganim2)] { bg->setDisplayFrame(anim->displayFrame()); }),
			CCDelayTime::create(1.0f),
			CallFuncExt::create([bg = Ref(bg), anim = Ref(bganim1)] { bg->setDisplayFrame(anim->displayFrame()); }),
			nullptr
		)));

		CCFileUtils::get()->m_fullPathCache["GJ_gradientBG.png"] = CCFileUtils::get()->fullPathForFilename("flashes1.png", 0);
		auto flashes = geode::createLayerBG();
		CCFileUtils::get()->m_fullPathCache.erase("GJ_gradientBG.png");
		flashes->setColor(ccWHITE);
		addChild(flashes);
		std::vector<Ref<CCSprite>> flashes_list;
		std::string flashTmpFILE = "flashes1.png";
		while (fileExistsInSearchPaths(flashTmpFILE.c_str())) {
			flashes_list.push_back(CCSprite::create(flashTmpFILE.c_str()));
			flashTmpFILE = fmt::format("flashes{}.png", flashes_list.size());
		}
		flashes->runAction(CCRepeatForever::create(CCSequence::create(
			CallFuncExt::create(
				[flashes = Ref(flashes), flashes_list] {
					if (auto g = flashes->m_pGrid) if (auto t = g->m_pTexture) t->setAliasTexParameters();
					auto nextflash = flashes_list[rand() % flashes_list.size()];
					flashes->setDisplayFrame(nextflash->displayFrame());
					flashes->setVisible(true);
				}
			),
			CCDelayTime::create(1.0f), CCHide::create(), CCDelayTime::create(15.0f), nullptr
		)));

		if (FLASHES_MODE) {
			flashes->setVisible(false);
			flashes->stopAllActions();
			bg->stopAllActions();
			bg->runAction(CCRepeatForever::create(CCSequence::create(CallFuncExt::create(
				[bg = Ref(bg), _this = Ref(this), flashes_list] {
					auto fmod = FMODAudioEngine::get();
					if (!fmod->m_metering) fmod->enableMetering();
					auto pulse = fmod->m_pulse1;//(fmod->m_pulse1 + fmod->m_pulse2 + fmod->m_pulse3) / 3;

					if (false) {
						_this->removeChildByTag("pulsedbg"_h);
						auto pulsedbg = CCLabelBMFont::create((
							fmt::format("{}\n \n", pulse)
							+ fmt::format("{}\n", fmod->m_pulse1)
							+ fmt::format("{}\n", fmod->m_pulse2)
							+ fmt::format("{}\n \n", fmod->m_pulse3)
							).c_str(), "gjFont18.fnt");
						pulsedbg->setPosition(_this->getContentSize() / 2);
						pulsedbg->setAnchorPoint(CCPointZero);
						pulsedbg->setScale(0.325);
						pulsedbg->setOpacity(90);
						_this->addChild(pulsedbg, 999, "pulsedbg"_h);
					}

					if (auto program = bg->getShaderProgram()) {
						program->use();
						static float timeAccum = 0.0f;
						timeAccum += 0.016f; // ~60fps
						GLint timeLocation = glGetUniformLocation(program->getProgram(), "u_time");
						if (timeLocation != -1) glUniform1f(timeLocation, timeAccum);
						float glitchIntensity = pulse * 0.5f;
						GLint intensityLocation = glGetUniformLocation(program->getProgram(), "u_glitchIntensity");
						if (intensityLocation != -1) glUniform1f(intensityLocation, glitchIntensity);
					}

					if (auto g = bg->m_pGrid) if (auto t = g->m_pTexture) t->setAliasTexParameters();

					if (pulse > 0.5f) {
						auto nextflash = flashes_list[rand() % flashes_list.size()];
						while (nextflash->getTexture() == bg->getTexture()) {
							nextflash = flashes_list[rand() % flashes_list.size()];
						}
						bg->setDisplayFrame(nextflash->displayFrame());
					}
				}
			), CCDelayTime::create(0.1f), nullptr)));

			{
				const GLchar* glitchVertexShader = R"(
					attribute vec4 a_position;
					attribute vec2 a_texCoord;
					attribute vec4 a_color;
					varying vec4 v_fragmentColor;
					varying vec2 v_texCoord;
					void main() {
					    gl_Position = CC_MVPMatrix * a_position;
					    v_fragmentColor = a_color;
					    v_texCoord = a_texCoord;
					})";
				const GLchar* glitchFragmentShader = R"(
					#ifdef GL_ES
						precision mediump float;
					#endif
					varying vec4 v_fragmentColor;
					varying vec2 v_texCoord;
					uniform sampler2D CC_Texture0;
					uniform float u_time;
					uniform float u_glitchIntensity;

					float rand(vec2 co) {
					    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
					}

					void main(void) {
					    vec2 uv = v_texCoord;
					    float glitch = u_glitchIntensity;

					    float splitAmount = rand(vec2(u_time, uv.y)) * glitch * 0.05;
					    vec2 offset = vec2(splitAmount, 0.0);

					    float r = texture2D(CC_Texture0, uv + offset).r;
					    float g = texture2D(CC_Texture0, uv).g;
					    float b = texture2D(CC_Texture0, uv - offset).b;

					    float lineNoise = rand(vec2(u_time * 0.1, floor(uv.y * 100.0)));
					    if (lineNoise > 0.95 - glitch * 0.3) {
					        uv.x += (rand(vec2(u_time, uv.y)) - 0.5) * glitch * 0.2;
					        r = texture2D(CC_Texture0, uv).r;
					        g = texture2D(CC_Texture0, uv).g;
					        b = texture2D(CC_Texture0, uv).b;
					    }

					    vec4 color = vec4(r, g, b, texture2D(CC_Texture0, v_texCoord).a);
					    gl_FragColor = v_fragmentColor * color;
					})";
				auto program = new CCGLProgram();
				program->initWithVertexShaderByteArray(glitchVertexShader, glitchFragmentShader);
				program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
				program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
				program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
				program->link();
				program->updateUniforms();
				bg->setShaderProgram(program);
				program->release();
			}
		}

		CCFileUtils::get()->m_fullPathCache["GJ_gradientBG.png"] = CCFileUtils::get()->fullPathForFilename("menuBG_2.png", 0);
		auto menubg = geode::createLayerBG();
		CCFileUtils::get()->m_fullPathCache.erase("GJ_gradientBG.png");
		addChild(menubg);

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
			SimpleTextArea::create("continue", "chatFont.fnt", 1.0f)->getLines()[0],
			[__this = Ref(this)](CCNode* item) { __this->onPlay(item); }
		);
		menu->addChild(play);

		auto retry = CCMenuItemExt::createSpriteExtra(
			SimpleTextArea::create("retry", "chatFont.fnt", 1.0f)->getLines()[0],
			[__this = Ref(this)](CCNode* item) { 
				createQuickPopup(
					"reset?", "", "NO", "yes", [](void*, bool yes) {
						if (not yes) return;
						saves() = {};
						saves()["reset"] = "Always watching... yet remains unseen. Hoping... and WAITING for YOU.";
						getMod()->saveData();
						game::restart();
					}
				);
			}
		);
		menu->addChild(retry);

		auto leave = CCMenuItemExt::createSpriteExtra(
			SimpleTextArea::create("leave", "chatFont.fnt", 1.0f)->getLines()[0],
			[__this = Ref(this)](CCNode* item) { __this->keyBackClicked(); }
		);
		menu->addChild(leave);

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
			[bg = Ref(bg)](CCNode* node) {
				node->setShaderProgram(bg->getShaderProgram());
				auto dl = 1.0f;
				auto dt = CCDelayTime::create(FLASHES_MODE ? 0.1f : 1.f);
				node->runAction(CCRepeatForever::create(CCSequence::create(
					dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
					dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
					dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
					dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
					dt, CCRotateTo::create(0.f, CCRANDOM_MINUS1_1() / dl),
					nullptr
				)));
				auto p = node->getPosition();
				node->runAction(CCRepeatForever::create(CCSequence::create(
					dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
					dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
					dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
					dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
					dt, CCMoveTo::create(0.f, p + CCPointMake(CCRANDOM_MINUS1_1() / dl, CCRANDOM_MINUS1_1() / dl)),
					nullptr
				)));
				return false;
			}
		);

		return true;
	}
};

#include <Geode/modify/CCNode.hpp>
class $modify(NodeVisitController, CCNode) {
	auto replaceColors() {
#define repl(org, tar) if (node->getColor() == org) node->setColor(tar);
		if (Ref node = typeinfo_cast<CCNodeRGBA*>(this)) {
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
		if (Ref node = typeinfo_cast<CCLayerColor*>(this)) {
			repl(ccc3(191, 114, 62), ccc3(6, 6, 6));
			repl(ccc3(161, 88, 44), ccc3(10, 10, 10));
			repl(ccc3(194, 114, 62), ccc3(8, 8, 8));
			//mod-list-frame
			if (node->getColor() == ccc3(25, 17, 37)) node->setOpacity(0);// frame-bg
			repl(ccc3(83, 65, 109), ccc3(17, 17, 17));//search-id
			if (node->getColor() == ccc3(168, 85, 44)) node->setOpacity(0);// frame-bg gd
			repl(ccc3(114, 63, 31), ccc3(17, 17, 17));//search-id gd
		};
#undef repl
	}
	$override void visit() {
		CCNode::visit();
		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return;
		if (Ref node = typeinfo_cast<GJListLayer*>(this)) {
			if (node->getOpacity() == 180) node->setOpacity(255);// list-bg
		}
		if (Ref<SetupTriggerPopup> a = typeinfo_cast<UIOptionsLayer*>(this); a = a ? a : typeinfo_cast<UIPOptionsLayer*>(this)) {
			a->setOpacity(160);
			a->setColor(ccBLACK);
		}
		if (GameManager::get()->m_gameLayer and Ref(GameManager::get()->m_gameLayer)->isRunning()) void();
		else {
			Ref(this)->replaceColors();
		}
	}
};

#include <Geode/modify/CCSpriteFrameCache.hpp>
class $modify(CCSpriteFrameCacheExt, CCSpriteFrameCache) {
	CCSpriteFrame* spriteFrameByName(const char* pszName) {
		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return CCSpriteFrameCache::spriteFrameByName(
			pszName
		);
		std::string name = pszName;
		// chains that not in game.
		if (GameManager::get()->m_gameLayer and GameManager::get()->m_gameLayer->isRunning()) void();
		else if (string::contains(name, "chain_01")) {
			name = "emptyFrame.png";
		}
		// sprites at this mod id (id.asd/pszName ? rtn(id.asd/pszName))
		{
			auto frameAtSprExtName = (Mod::get()->getID() + "/" + name);
			auto test = CCSpriteFrameCache::get()->m_pSpriteFrames->objectForKey(frameAtSprExtName.c_str());
			name = test ? frameAtSprExtName.c_str() : name.c_str();
			if (test) CCSpriteFrameCache::get()->m_pSpriteFrames->setObject(test, pszName);
		};
		//subs? (aka "geode.loader/penis.png")
		if (name.find("/") != std::string::npos) {
			auto test_name = Mod::get()->getID() + "/" + string::replace(name, "/", "..");
			auto test = CCSpriteFrameCache::get()->m_pSpriteFrames->objectForKey(test_name);
			name = test ? test_name.data() : name.c_str();
		}
		return CCSpriteFrameCache::spriteFrameByName(name.c_str());
	};
};

#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(LevelSelectLayerExt, LevelSelectLayer) {
	static cocos2d::CCScene* scene(int p0) {
		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return LevelSelectLayer::scene(p0);
		auto level = getMod()->getSavedValue("level", 0);
		auto name = ("properdies_" + utils::numToString(level) + ".level");
		auto path = string::pathToString(CCFileUtils::get()->fullPathForFilename(name.c_str(), 0).c_str());
		auto devmode = string::pathToString(
			CMAKE_CURRENT_SOURCE_DIR "/assets/files/levels/" + name
		);
		if (fileExistsInSearchPaths(devmode.c_str())) { 
			Notification::create(" Level loaded from source folder :D\n " + devmode, NotificationIcon::Info, 3.f)->show();
			path = devmode; 
		}
		auto import = level::importLevelFile(path);
		if (import.isOk()) {
			auto xd = PlayLayer::scene(import.unwrapOrDefault(), 0, 0);
			return xd;
		}
		else {
			Notification::create(
				" Failed to import:\n " + path, NotificationIcon::Error, 3.f
			)->show();
			if (import.err()) Notification::create(
				import.err().value_or("unk err .-."), 
				NotificationIcon::Error, 3.f
			)->show();
			return LevelSelectLayer::scene(p0);
		}
	};
};
