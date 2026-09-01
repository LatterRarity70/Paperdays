#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
using namespace geode::prelude;

#define saves getMod()->getSaveContainer

#include <regex>

#define IS_DEV_MODE (fs::fileExistsInSearchPaths("__ENABLE_DEV_MODE.txt") or fs::fileExistsInSearchPaths(CMAKE_CURRENT_SOURCE_DIR "/mod.json"))

namespace fs {
	using namespace std::filesystem;
	using namespace geode::cocos;
	auto err = std::error_code{};
	typedef CCFileUtils cocos;
};

auto static FLASHES_MODE = [] { 
	if (saves()["level"].asInt().unwrapOr(0) == 3) return true;
	srand(time(nullptr)); return rand() % 3 == 1; 
	}();
auto static FLASHES_SONG = [] { srand(time(nullptr)); return std::vector<const char*>{
	"flash2.mp3"_spr, "flash1.mp3"_spr,
	"usedcvnt - aesthetics of self-destruction.mp3"_spr,
}[rand() % 3]; };
auto static LOADING_SONG = [] { srand(time(nullptr)); return std::vector<const char*>{
	"""""loading.mp3"_spr, "loading.mp3"_spr,
		"loading__ 1485147_In-The-Distance.mp3"_spr, //sounds like deltarune..?
		"loading__ 1481139_i-feel-odd.mp3"_spr
}[rand() % 4]; };

void disableIMEInpMod() {
	auto mod = Loader::get()->getInstalledMod("alk.ime-input");
	if (mod) for (auto hook : mod->getHooks()) if (hook) hook->disable();
}
$on_mod(Loaded) { disableIMEInpMod(); }

// LOADING LAYER AND RESOURCE SETS

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayerExt, LoadingLayer) {
	static void resourceSetup() {
		auto resources_dir = getMod()->getResourcesDir();

		auto tp = CCTexturePack();
		tp.m_paths = {
			string::pathToString(CMAKE_CURRENT_SOURCE_DIR "/assets/files").c_str(),
			string::pathToString(dirs::getGameDir()).c_str(),
			string::pathToString(resources_dir).c_str(),
			string::pathToString(resources_dir.parent_path()).c_str() //resources/../id
		};
		tp.m_id = "resources"_spr;
		fs::cocos::get()->addTexturePack(tp);

		bool shouldRestartGame = false;
		for (const auto& entry : fs::recursive_directory_iterator(resources_dir)) {
			if (!entry.is_regular_file()) continue;

			auto ext = entry.path().extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			if (string::containsAny(ext, { ".png", ".jpg", ".jpeg", ".plist" })) {
				auto relativePath = fs::relative(entry.path(), resources_dir);
				auto sprite = CCSprite::create(string::pathToString(relativePath).c_str());
				if (sprite) CCSpriteFrameCache::get()->addSpriteFrame(
					sprite->displayFrame(), string::pathToString(relativePath).c_str()
				);
			}

			auto str = string::pathToString(entry.path());
			auto name = string::pathToString(entry.path().filename());
			auto nsub = string::replace(name, "..", "/");
			if (string::contains(str, "..")) fs::cocos::get()->m_fullPathCache[nsub] = fs::cocos::get()->fullPathForFilename(
				name.c_str(), 0
			);

			if (string::containsAny(ext, { ".geode" })) shouldRestartGame += fs::copy_file(
				entry.path(),
				dirs::getModsDir() / entry.path().filename(),
				fs::copy_options::update_existing, fs::err
			);
		}
		if (shouldRestartGame) game::restart("save yea");

		//happy
		fs::cocos::get()->m_fullPathCache["alphalaneous.happy_textures/bigFont.fnt"] = fs::cocos::get()->fullPathForFilename(
			"bigFont.fnt"_spr, 0
		);
		fs::cocos::get()->m_fullPathCache["alphalaneous.happy_textures/bigFont.png"] = fs::cocos::get()->fullPathForFilename(
			"bigFont.png"_spr, 0
		);

		fs::cocos::get()->m_fullPathCache["menuLoop.mp3"] = "";

#define defineFallbacks(format_str, count) \
    do { \
        for (int _i = 1; _i <= (count); _i++) { \
            std::string _key = fmt::format((format_str), _i); \
            std::string _fallback = fmt::format((format_str), 1); \
            fs::cocos::get()->m_fullPathCache[_key] = fs::cocos::get()->fullPathForFilename(_fallback.c_str(), 0); \
        } \
    } while(0)

		defineFallbacks("dialogIcon_{:03d}.png", 56);
		defineFallbacks("game_bg_{:02d}_001.png", 59);
		defineFallbacks("gjFont{:02d}.png", 59);
		defineFallbacks("gjFont{:02d}.fnt", 59);
		defineFallbacks("groundSquare_{:02d}_001.png", 22);
		defineFallbacks("groundSquare_{:02d}_2_001.png", 22);
		defineFallbacks("PlayerExplosion_{:02d}.png", 19);
		defineFallbacks("PlayerExplosion_{:02d}.plist", 19);
		defineFallbacks("portalEffect{:02d}.plist", 9);

		defineFallbacks("icons/bird_{:02d}.png", 149);
		defineFallbacks("icons/bird_{:02d}.plist", 149);
		defineFallbacks("icons/dart_{:02d}.png", 96);
		defineFallbacks("icons/dart_{:02d}.plist", 96);
		defineFallbacks("icons/jetpack_{:02d}.png", 8);
		defineFallbacks("icons/jetpack_{:02d}.plist", 8);
		defineFallbacks("icons/player_{:02d}.png", 485);
		defineFallbacks("icons/player_{:02d}.plist", 485);
		defineFallbacks("icons/robot_{:02d}.png", 68);
		defineFallbacks("icons/robot_{:02d}.plist", 68);
		defineFallbacks("icons/ship_{:02d}.png", 169);
		defineFallbacks("icons/ship_{:02d}.plist", 169);
		defineFallbacks("icons/spider_{:02d}.png", 69);
		defineFallbacks("icons/spider_{:02d}.plist", 69);
		defineFallbacks("icons/swing_{:02d}.png", 43);
		defineFallbacks("icons/swing_{:02d}.plist", 43);
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
			std::vector<std::string>{
				"""""hold on control\n" 
				"""""to seen avoid the",
				"""""hi!\n ",
				"""""i love you\n ",
				"""""hi again :>\n ",
				"""""lil scared\n ",
				"""""proper_dies\n ",
				"""""hold on alternatives\n ",
				"""""im watching you\n ",
				"""""im sorry\n ",
			}[rand() % 8] + "\n \n "
			":WARN:\n"
			"thats all is not real even",
			
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

		GameManager::get()->fadeInMusic(LOADING_SONG());

		queueInMainThread([this_ = Ref(this)]
			{
				fs::cocos::get()->m_fullPathCache["GJ_gradientBG.png"] = fs::cocos::get()->fullPathForFilename(
					"loadingBG.png", 0
				);
				auto menubg = geode::createLayerBG();
				fs::cocos::get()->m_fullPathCache.erase("GJ_gradientBG.png");
				if (Ref a = this_->getParent()) a->addChild(menubg, -3);
			}
		);
		
		return true;
	}
};

// MAIN MENU

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerExt, MenuLayer) {
	virtual void keyDown(cocos2d::enumKeyCodes p0) {
		CCLayer::keyDown(p0);
	};
	static cocos2d::CCScene* scene(bool isVideoOptionsOpen) {
		if (auto a = typeinfo_cast<CCScene*>(GameManager::get()->getUserObject("NextMenuScene"))) {
			queueInMainThread([] { GameManager::get()->fadeInMusic("mute_music_rn"); });
			GameManager::get()->setUserObject("NextMenuScene", nullptr);
			return a;
		}
		return MenuLayer::scene(isVideoOptionsOpen);
	};
	bool init() {

		if (saves()["level"].asInt().unwrapOr(0) == 4) {
			FLASHES_MODE = false;
			fs::cocos::get()->m_fullPathCache["menuLoop.mp3"] = fs::cocos::get(
			)->fullPathForFilename("loading__ 1485147_In-The-Distance.mp3"_spr, 0);
			fs::cocos::get()->m_fullPathCache["menuBG_1a.png"] = fs::cocos::get(
			)->fullPathForFilename("menuBG_lvl4a.png"_spr, 0);
			fs::cocos::get()->m_fullPathCache["menuBG_1b.png"] = fs::cocos::get(
			)->fullPathForFilename("menuBG_lvl4b.png"_spr, 0);
		}
		else {
			fs::cocos::get()->m_fullPathCache.erase("menuLoop.mp3");
			fs::cocos::get()->m_fullPathCache.erase("menuBG_1a.png");
			fs::cocos::get()->m_fullPathCache.erase("menuBG_1b.png");
		}

		if (FLASHES_MODE) fs::cocos::get()->m_fullPathCache[
			"menuLoop.mp3"
		] = fs::cocos::get()->fullPathForFilename(FLASHES_SONG(), 0);

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

		fs::cocos::get()->m_fullPathCache["GJ_gradientBG.png"] = fs::cocos::get()->fullPathForFilename(
			"menuBG_1a.png", 0
		);
		Ref bg = geode::createLayerBG();
		fs::cocos::get()->m_fullPathCache.erase("GJ_gradientBG.png");
		bg->setColor(ccWHITE);
		addChild(bg);
		Ref bganim1 = CCSprite::create("menuBG_1a.png");
		Ref bganim2 = CCSprite::create("menuBG_1b.png");
		bg->runAction(CCRepeatForever::create(CCSequence::create(
			CCDelayTime::create(1.0f),
			CallFuncExt::create([=] { bg->setDisplayFrame(bganim2->displayFrame()); }),
			CCDelayTime::create(1.0f),
			CallFuncExt::create([=] { bg->setDisplayFrame(bganim1->displayFrame()); }),
			nullptr
		)));

		fs::cocos::get()->m_fullPathCache["GJ_gradientBG.png"] = fs::cocos::get(
		)->fullPathForFilename("flashes1.png", 0);
		auto flashes = geode::createLayerBG();
		fs::cocos::get()->m_fullPathCache.erase("GJ_gradientBG.png");
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

		auto menu = CCMenu::create();
		menu->setID("menu"_spr);

		if (FLASHES_MODE) {
			auto static lastPulse = 0.0f;
			flashes->setVisible(false);
			flashes->stopAllActions();
			bg->stopAllActions();
			bg->runAction(CCRepeatForever::create(CCSequence::create(CallFuncExt::create(
				[bg = Ref(bg), menu = Ref(menu), _this = Ref(this), flashes_list] {
					auto fmod = FMODAudioEngine::get();
					if (!fmod->m_metering) fmod->enableMetering();
					auto pulse = fmod->m_pulse1;//(fmod->m_pulse1 + fmod->m_pulse2 + fmod->m_pulse3) / 3;
					lastPulse = pulse;

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

					menu->setScale(1.0f + pulse * 0.1f);

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

					if (pulse > 0.5) {
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

	float jitterX = (rand(vec2(floor(uv.y * 200.0), u_time * 10.0)) - 0.5) * 0.01 * glitch;
	float jitterY = (rand(vec2(floor(uv.x * 200.0), u_time * 10.0)) - 0.5) * 0.01 * glitch;
	vec2 jitter = vec2(jitterX, jitterY);

	vec4 color = texture2D(CC_Texture0, uv + jitter);

	float splitAmount = glitch * 0.015;
	float r = texture2D(CC_Texture0, uv + jitter + vec2(splitAmount, 0.0)).r;
	float g = color.g;
	float b = texture2D(CC_Texture0, uv + jitter - vec2(splitAmount, 0.0)).b;

	float lineNoise = rand(vec2(u_time * 2.0, floor(uv.y * 100.0)));
	if (lineNoise > 0.92 - glitch * 0.4) {
		float offset = (rand(vec2(u_time, uv.y)) - 0.5) * glitch * 0.2;
		vec2 distortedUV = vec2(uv.x + offset, uv.y);
		r = texture2D(CC_Texture0, distortedUV + vec2(splitAmount, 0.0)).r;
		g = texture2D(CC_Texture0, distortedUV).g;
		b = texture2D(CC_Texture0, distortedUV - vec2(splitAmount, 0.0)).b;
	}

	float brightness = (r + g + b) / 3.0;
	if (brightness > 0.7) {
		float flicker = sin(u_time * 20.0 + uv.x * 100.0 + uv.y * 100.0) * 0.5 + 0.5;
		flicker = mix(1.0, flicker, glitch * 0.4);
		r *= flicker;
		g *= flicker;
		b *= flicker;
	}

	vec4 finalColor = vec4(r, g, b, color.a);
	gl_FragColor = v_fragmentColor * finalColor;
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

		fs::cocos::get()->m_fullPathCache["GJ_gradientBG.png"] = fs::cocos::get(
		)->fullPathForFilename("menuBG_2.png", 0);
		auto menubg = geode::createLayerBG();
		fs::cocos::get()->m_fullPathCache.erase("GJ_gradientBG.png");
		addChild(menubg);

		addChild(menu);

		auto title = CCMenuItemExt::createSpriteExtra(
			SimpleTextArea::create(getMod()->getMetadataRef().getName(), "bigFont.fnt", 1.2f)->getLines()[0],
			[__this = Ref(this)](CCNode* item) { openInfoPopup(getMod()); }
		);
		menu->addChild(title);

		auto verl = SimpleTextArea::create(fmt::format(
			"SDK {} on {}, Release v{}",
			Mod::get()->getMetadataRef().getGeodeVersion().toVString(),
			GEODE_PLATFORM_NAME,
			fs::file_size(getMod()->getPackagePath(), fs::err)
		).c_str(), "chatFont.fnt", 0.6f)->getLines()[0];
		verl->setOpacity(63);
		menu->addChild(verl);

		menu->addChild(SimpleTextArea::create("             ", "bigFont.fnt", 1.5f)->getLines()[0]);

		auto play = CCMenuItemExt::createSpriteExtra(
			SimpleTextArea::create("continue", "chatFont.fnt", 1.0f)->getLines()[0],
			[__this = Ref(this)](CCNode* item) { __this->onPlay(item); }
		);
		menu->addChild(play);

		auto settings = CCMenuItemExt::createSpriteExtra(
			SimpleTextArea::create("settings", "chatFont.fnt", 1.0f)->getLines()[0],
			[__this = Ref(this)](CCNode* item) { __this->onOptions(item); }
		);
		menu->addChild(settings);

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
		if (IS_DEV_MODE) menu->addChild(geode);

		menu->setLayout(SimpleColumnLayout::create()->setGap(10.f)); 

		findFirstChildRecursive<CCNode>(menu,
			[](CCNode* node) {
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

		static auto id = getMod()->getID();
		static auto repo = getMod()->getMetadataRef().getLinks().getSourceURL().value_or("https://github.com/LatterRarity70/Paperdays");
		auto AltK = CCKeyboardDispatcher::get()->getAltKeyPressed();
		if (AltK or not IS_DEV_MODE) {
			auto webListener = new EventListener<web::WebTask>;
			webListener->bind(
				[_this = Ref(this), webListener](web::WebTask::Event* e) {
					if (web::WebProgress* prog = e->getProgress()) {
						//log::debug("{}", prog->downloadTotal());

						if (prog->downloadTotal() > 0) void(); else return;

						auto installed_size = fs::file_size(getMod()->getPackagePath(), fs::err);
						auto actual_size = prog->downloadTotal();

						if (installed_size == actual_size) return;

						auto pop = geode::createQuickPopup(
							"Update!",
							fmt::format(
								"Latest release size mismatch with installed one!"
								"\n" "Download latest release of mod?"
							),
							"Later.", "Yes", [_this](CCNode* pop, bool Yes) {
								if (!Yes) return;

								_this->setVisible(0);

								GameManager::get()->fadeInMusic(LOADING_SONG());

								auto req = web::WebRequest();

								Ref state_win = Notification::create("Downloading... (///%)");
								state_win->setTime(1337.f);
								state_win->show();

								if (state_win->m_pParent) {
									auto loading_bg = CCSprite::create("GJ_gradientBG.png");
									if (loading_bg) {
										loading_bg->setID("loading_bg");
										loading_bg->setAnchorPoint(CCPointMake(0.f, 0.f));
										loading_bg->setScaleX(_this->getContentWidth() / loading_bg->getContentWidth());
										loading_bg->setScaleY(_this->getContentHeight() / loading_bg->getContentHeight());
										state_win->m_pParent->addChild(loading_bg);
									}
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
												log::debug("{}", res->into(getMod()->getPackagePath()).err());
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
									repo + "/releases/download/nightly/" + id + ".geode"
								));

							}, false
						);
						pop->m_scene = _this;
						pop->show();

						e->cancel();
						webListener->disable();
						delete webListener;
					}
				}
			);
			webListener->setFilter(
				web::WebRequest().get(repo + "/releases/download/nightly/" + id + ".geode")
			);
		}
		else Notification::create("Update check was aborted because its a dev build...")->show();

		return true;
	}
};

// ENGINE NODES

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
	$override void addChild(CCNode* child, int zOrder, int tag) {
		if (child) CCNode::addChild(child, zOrder, tag);
		else log::error("Tried to add {} child to {}!", child, this);
	}
	$override void visit() {
		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return CCNode::visit();
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
		if (Ref(this)->getID() == "options-menu" and !Ref(this)->getUserObject("done"_spr)) {
			if (Ref a = Ref(this)->getChildByID("account-button")) a->setVisible(0);
			if (Ref a = Ref(this)->getChildByID("how-to-play-button")) a->setVisible(0);
			if (Ref a = Ref(this)->getChildByID("rate-button")) a->setVisible(0);
			if (Ref a = Ref(this)->getChildByID("songs-button")) a->setVisible(0);
			if (Ref a = Ref(this)->getChildByID("help-button")) a->setVisible(0);
			Ref(this)->setUserObject("done"_spr, this);
			if (IS_DEV_MODE) {
			//if (fileExistsInSearchPaths(string::pathToString(CMAKE_CURRENT_LIST_FILE).c_str())) {
				Ref input = TextInput::create(30.f, "lvl");
				input->setString(saves()["level"].dump());
				input->getInputNode()->m_cursor->setString("   level   \n \n \n");
				input->setCallback(
					[input](const std::string& p0) {
						input->getInputNode()->m_cursor->setString("   level   \n \n \n");
						auto parse = matjson::parse(p0);
						if (auto err = parse.err()) input->getInputNode()->m_cursor->setString(
							fmt::format("\n \n \n {}", err.value()).c_str()
						);
						else saves()["level"] = parse.unwrapOrDefault();
					}
				);
				input->setPosition({ 0, 0 });
				Ref(this)->CCNode::addChild(input);
			}
		}

		CCNode::visit();
	}
};

#include <Geode/modify/CCSprite.hpp>
class $modify(CCSpriteExt, CCSprite) {
	static CCSprite* createWithTexture(CCTexture2D * pTexture) {
		if (!pTexture) pTexture = CCSprite::create()->getTexture();
		auto rtn = CCSprite::createWithTexture(pTexture);
		return rtn ? rtn : CCSprite::create();
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
		//icon fallbacks
		if (string::endsWith(name, "_001.png") and string::containsAny(
			name, {"bird_", "dart_", "jetpack_", "player_", "robot_", "ship_", "spider_", "swing_"}
		)) {
			std::regex pattern(R"_(_(\d{2})_)_");
			name = std::regex_replace(name, pattern, "_01_");
		}
		return CCSpriteFrameCache::spriteFrameByName(name.c_str());
	};
};

// OPTIONS

#include <Geode/modify/MoreOptionsLayer.hpp>
class $modify(MoreOptionsLayerExt, MoreOptionsLayer) {
	void goToPage(int p0) {
		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return MoreOptionsLayer::goToPage(p0);
		MoreOptionsLayer::goToPage(p0);
		m_categoryLabel->setString(std::vector<const char*>{
			"""""Options!",
				"Options.",
				"Options",
				"Options :3",
				"options here..",
				"aw", "hi!", "ops"
				")", ":>", ":3", ":D"
		} [rand() % 10] );
	}
	void addToggle(char const* name, char const* tag, char const* desc) {
		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return MoreOptionsLayer::addToggle(name, tag, desc);
		log::debug("if (std::string(tag) == \"{}\") return; // {}", tag, matjson::Value(name).dump());

		if (std::string(tag) == "0094") return; // "More Comments"
		if (std::string(tag) == "0090") return; // "Load Comments"
		if (std::string(tag) == "0073") return; // "New Completed Filter"
		if (std::string(tag) == "0093") return; // "Increase Local Levels Per Page"
		if (std::string(tag) == "0084") return; // "Manual Level Order"
		if (std::string(tag) == "0099") return; // "Show Leaderboard Percentage"
		if (std::string(tag) == "0127") return; // "Save Gauntlets"

		if (std::string(tag) == "0125") return; // "Enable Normal Music In Editor"
		if (std::string(tag) == "0174") return; // "Hide Playtest Text"
		if (std::string(tag) == "0119") return; // "Disable Level Saving"
		if (std::string(tag) == "0042") return; // "Increase Maximum Levels"

		if (std::string(tag) == "0060") return; // "Default Mini Icon"
		if (std::string(tag) == "0061") return; // "Switch Spider Teleport Color"
		if (std::string(tag) == "0062") return; // "Switch Dash Fire Color"
		if (std::string(tag) == "0096") return; // "Switch Wave Trail Color"

		if (std::string(tag) == "0071") return; // "Hide Practice Buttons"
		if (std::string(tag) == "0135") return; // "Hide Attempts" (practice)

		if (std::string(tag) == "0024") return; // "Show Cursor In-Game"
		if (std::string(tag) == "0129") return; // "Disable Portal Guide" 
		if (std::string(tag) == "0130") return; // "Enable Orb Guide" 
		if (std::string(tag) == "0140") return; // "Disable Orb Scale"
		if (std::string(tag) == "0141") return; // "Disable Trigger Orb Scale"
		if (std::string(tag) == "0072") return; // "Disable Gravity Effect"
		if (std::string(tag) == "0100") return; // "Enable Death Effect"
		if (std::string(tag) == "0082") return; // "Disable High Object Alert"
		if (std::string(tag) == "0033") return; // "Change Custom Songs Location"
		if (std::string(tag) == "0083") return; // "Disable Song Alert"
		if (std::string(tag) == "0018") return; // "No Song Limit"
		if (std::string(tag) == "0168") return; // "Fast Menu"
		if (std::string(tag) == "0171") return; // "Disable Player Hitbox"
		if (std::string(tag) == "0068") return; // Enable Quick Checkpoints (Ctrl+S style)
		if (std::string(tag) == "0172") return; // Enable Shake
		if (std::string(tag) == "0014") return; // Enable Explosion Shake
		if (std::string(tag) == "0100") return; // Enable Death Effect
		if (std::string(tag) == "0155") return; // Disable Shader AA - pixel-art 
		if (std::string(tag) == "0066") return; // Increase Draw Capacity
		if (std::string(tag) == "0166") return; // Hide Hitboxes

		MoreOptionsLayer::addToggle(name, tag, desc);
	};
};

// LEVELS

#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(LevelSelectLayerExt, LevelSelectLayer) {
	class hop : public CCLayer {
	public:
		CREATE_FUNC(hop);
		void sch(float) {
			auto id = getMod()->getSavedValue("level", 0);
			auto level = GJGameLevel::create();
			level->m_autoLevel = true;
			level->m_levelID = id;
			level->m_levelName = "pt" + fmt::format("{:03}", id);
			level->m_levelType = IS_DEV_MODE ? GJLevelType::Editor : GJLevelType::Main;
			level->m_isEditable = level->m_levelType == GJLevelType::Editor;
			level->m_levelString = LocalLevelManager::get()->getMainLevelString(id);
			CCDirector::get()->replaceScene(PlayLayer::scene(level, 0, 0));
		}
		void onEnterTransitionDidFinish() override {
			CCLayer::onEnterTransitionDidFinish();
			scheduleOnce(schedule_selector(hop::sch), 0.0f);
		};
	};
	static cocos2d::CCScene* scene(int p0) {
		auto scene = CCScene::create();
		scene->addChild(hop::create());
		return (scene);
	};
};

#include <Geode/modify/LevelTools.hpp>
class $modify(LevelToolsExt, LevelTools) {
	static bool verifyLevelIntegrity(gd::string p0, int p1) { return true; }
};

#include <Geode/modify/MusicDownloadManager.hpp>
class $modify(MusicDownloadManagerExt, MusicDownloadManager) {
	gd::string pathForSFX(int id) {
		fs::path ref = MusicDownloadManager::pathForSFX(id).c_str();
		auto as = fmt::format("s{}{}", id, ref.extension());
		if (fs::fileExistsInSearchPaths(as.c_str())) return fs::cocos::get()->fullPathForFilename(as.c_str(), 0);
		return MusicDownloadManager::pathForSFX(id);
	};
	gd::string pathForSong(int id) {
		fs::path ref = MusicDownloadManager::pathForSong(id).c_str();
		auto as = fmt::format("{}{}", id, ref.extension());
		if (fs::fileExistsInSearchPaths(as.c_str())) return fs::cocos::get()->fullPathForFilename(as.c_str(), 0);
		return MusicDownloadManager::pathForSong(id);
	}
};

#include <Geode/modify/EditorPauseLayer.hpp>
class $modify(MLE_EditorPauseLayer, EditorPauseLayer) {
	$override void saveLevel() {
		EditorPauseLayer::saveLevel();
		auto level = m_editorLayer->m_level;
		if (!level->m_autoLevel) return;
		auto file = fmt::format("levels/{}.txt", level->m_levelID);
		file::writeString(fs::cocos::get()->fullPathForFilename(file.c_str(), 0).c_str(), level->m_levelString.c_str()).err();
		LocalLevelManager::get()->m_mainLevels[level->m_levelID] = level->m_levelString.c_str();
		// also save to resources if it exists
		if (fs::fileExistsInSearchPaths((CMAKE_CURRENT_SOURCE_DIR "/resources/" + file).c_str())) {
			file = CMAKE_CURRENT_SOURCE_DIR "/resources/" + file;
			file::writeString(file.c_str(), level->m_levelString.c_str()).err();
		}
	}
};
