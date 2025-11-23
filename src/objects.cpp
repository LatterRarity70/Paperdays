#include <user95401.game-objects-factory/include/main.hpp>
#include <user95401.game-objects-factory/include/impl.hpp>

#define saves getMod()->getSaveContainer

#include <Geode/modify/GJGameLoadingLayer.hpp>
class $modify(GJGameLoadingLayerWhatTheF, GJGameLoadingLayer) {
	inline static Ref<EditLevelLayer> sex;
	void xdddd(float) {
		if (!sex) return;
		if (!this) return;
		if (!typeinfo_cast<GJGameLoadingLayer*>(this)) return;
		if (!this->isRunning()) return;
		this->m_editor ? sex->onEdit(sex) : sex->onPlay(sex);
	}
	void xd(float) {
		if (!this) return;
		if (!typeinfo_cast<GJGameLoadingLayer*>(this)) return;
		if (!this->isRunning()) return;
		if (this->m_level) {
			sex = EditLevelLayer::create(this->m_level);
			this->getParent()->addChild(sex, -999);
			this->scheduleOnce(schedule_selector(GJGameLoadingLayerWhatTheF::xdddd), 0.01f);
		}
	}
	virtual void onEnter() {
		GJGameLoadingLayer::onEnter();
		if (!this) return;
		if (!typeinfo_cast<GJGameLoadingLayer*>(this)) return;
		if (this->m_level) {
			this->scheduleOnce(schedule_selector(GJGameLoadingLayerWhatTheF::xd), 5.f);
		}
	};
};

#include <Geode/modify/GameObject.hpp>
class $modify(GameObjectsExt, GameObject) {
	virtual void customSetup() {
		GameObject::customSetup();
		if (this and typeinfo_cast<GameObject*>(this)) {
			if (auto tex = this->getTexture())
				tex->setAliasTexParameters();
			if (auto g = this->getGrid())
				if (auto t = g->m_pTexture) t->setAliasTexParameters();
		};
	}
};

#include <Geode/modify/CCActionInterval.hpp>
class $modify(DialogTextAnimExt, CCActionInterval) {
	$override void startWithTarget(CCNode * p0) {
		//log::debug("{}->{}({})", this, __FUNCTION__, p0);
		//CCFadeIn, ::startWithTarget({ CCFontSprite, 
		if (typeinfo_cast<CCFadeIn*>(this)) if (typeinfo_cast<CCFontSprite*>(p0)) {
			Ref fade = typeinfo_cast<CCFadeIn*>(this);
			Ref sprite = typeinfo_cast<CCFontSprite*>(p0);
			if (sprite) sprite->runAction(CCSequence::createWithTwoActions(
				CCDelayTime::create(fade ? fade->getDuration() : 0.1f), CallFuncExt::create(
					[sprite] {
						if (!sprite) return;
						sprite->setVisible(1);
						sprite->setOpacity(255);
						if (not sprite->getContentSize().isZero()) {
							FMODAudioEngine::get()->playEffect("_text.ogg"_spr);
						};
					}
				)
			));
			return;
		}
		return CCActionInterval::startWithTarget(p0);
	}
};

class DataNode : public CCNode {
public:
	std::string _json_str;
	auto get(std::string_view key) {
		return matjson::parse(_json_str).unwrapOrDefault()[key];
	}
	void set(std::string_view key, matjson::Value val) {
		auto parse = matjson::parse(_json_str).unwrapOrDefault();
		parse[key] = val;
		_json_str = parse.dump(matjson::NO_INDENTATION);
	}
	CREATE_FUNC(DataNode);
	static auto create(std::string id) {
		auto a = create();
		a->setID(id);
		return a;
	}
	static auto at(CCNode* container, std::string id = "") {
		auto me = typeinfo_cast<DataNode*>(container->getUserObject(id));
		if (!me) {
			me = create();
			container->setUserObject(id, me);
			log::debug("creating data node {} for {}...", me, container);
		}
		return me;
	}
};

void SetupObjects();
$on_mod(Loaded) { SetupObjects(); }
inline void SetupObjects() {
	static auto plrinputtrigger = GameObjectsFactory::createTriggerConfig(
		UNIQ_ID("pd-plr-input-crtl"), "edit_eEventLinkBtn_001.png",
		[](EffectGameObject* object, GJBaseGameLayer* game, int, gd::vector<int> const*) {
			GameOptionsTrigger* options = typeinfo_cast<GameOptionsTrigger*>(object);
			if (!options) return log::error("options object cast == {} from {}", options, object);
			//option assignments
			typedef GameOptionsSetting Is;
			auto player = options->m_streakAdditive;
			auto jump = options->m_hideGround;
			auto left = options->m_hideMG;
			auto right = options->m_hideP1;
			//affected players
			std::vector<Ref<PlayerObject>> ps = { game->m_player1, game->m_player2 };
			if (player != Is::Disabled) ps = {
				player == Is::On ? game->m_player1 : game->m_player2
			};
			auto pb = &PlayerObject::pushButton;
			typedef PlayerButton For;
			//jump
			if (jump != Is::Disabled) for (auto p : ps) if (p) jump == Is::On ? p->pushButton(For::Jump) : p->releaseButton(For::Jump);
			//left
			if (left != Is::Disabled) for (auto p : ps) if (p) left == Is::On ? p->pushButton(For::Left) : p->releaseButton(For::Left);
			//right
			if (right != Is::Disabled) for (auto p : ps) if (p) right == Is::On ? p->pushButton(For::Right) : p->releaseButton(For::Right);
		}
	)->refID(2899)->insertIndex((12 * 7) + 1)->onEditObject(
		[](EditorUI* a, GameObject* aa) -> bool {
			queueInMainThread(
				[a = Ref(a), aa = Ref(aa)] {
					if (!CCScene::get()) return log::error("CCScene::get() == {}", CCScene::get());
					auto popup = CCScene::get()->getChildByType<SetupOptionsTriggerPopup>(0);
					if (!popup) return log::error("popup == {}", popup);
					auto object = typeinfo_cast<EffectGameObject*>(aa.data());
					if (!object) return log::error("object == {} ({})", object, aa);

					auto main = popup->m_mainLayer;
					auto menu = popup->m_buttonMenu;

					if (auto aaa = main->getChildByType<CCLabelBMFont>(0)) aaa->setString("Extended Player Control");

					//xd
					if (auto aaa = main->getChildByType<CCLabelBMFont>(6 - 2)) aaa->setString(R"(Only For)");
					if (auto aaa = main->getChildByType<CCLabelBMFont>(7 - 2)) aaa->setString(R"(P1)");
					if (auto aaa = main->getChildByType<CCLabelBMFont>(8 - 2)) aaa->setString(R"(P2)");
					//jump buffer
					if (auto aaa = main->getChildByType<CCLabelBMFont>(9 - 2)) aaa->setString(R"(jump buffer)");
					//holding left
					if (auto aaa = main->getChildByType<CCLabelBMFont>(12 - 2)) aaa->setString(R"(holding left)");
					//holding right
					if (auto aaa = main->getChildByType<CCLabelBMFont>(15 - 2)) aaa->setString(R"(holding right)");

					//other shit
					{
						auto low_iq = 18;
						while (auto aaa = main->getChildByType<CCNode>(low_iq++)) aaa->setVisible(false);
					};

					//other shit in menu
					{
						auto low_iq = 13;
						while (auto aaa = menu->getChildByType<CCNode>(low_iq++)) aaa->setVisible(false);
					};
				}
			);
			return false;
		}
	);
	plrinputtrigger->registerMe();

	static GameObjectsFactory::GameObjectConfig* svcondtrigger = GameObjectsFactory::createTriggerConfig(
		UNIQ_ID("pd-sv-cond-toggle"), "edit_eItemCompBtn_001.png",
		[](EffectGameObject* object, GJBaseGameLayer* game, int p0, gd::vector<int> const* p1) {
			if (!object) return;
			if (!game) return;

			//set:key:value (setups value)
			auto data = typeinfo_cast<CCNode*>(object->getUserObject("data"_spr));
			if (!data) return log::error("data == {}", data);
			auto split = string::split(data->getID(), ":");
			if (split.size() == 3 and split[0] == "set") {
				getMod()->getSaveContainer()[split[1]] = matjson::parse(split[2]).unwrapOrDefault();
			}

			object->m_objectID = svcondtrigger->m_refObjectID;
			object->triggerObject(game, p0, p1);
			object->m_objectID = svcondtrigger->m_objectID;
		}
	)->refID(1049)->insertIndex(7)->onEditObject(
		[](EditorUI* a, GameObject* aa) -> bool {
			if (!a) return false;
			if (!aa) return false;
			queueInMainThread(
				[a = Ref(a), aa = Ref(aa)] {
					if (!CCScene::get()) return log::error("CCScene::get() == {}", CCScene::get());
					auto popup = CCScene::get()->getChildByType<SetupObjectTogglePopup>(0);
					if (!popup) return log::error("popup == {}", popup);
					auto object = typeinfo_cast<EffectGameObject*>(aa.data());
					if (!object) return log::error("object == {} ({})", object, aa);
					auto data = typeinfo_cast<CCNode*>(object->getUserObject("data"_spr));
					if (!data) return log::error("data == {}", data);

					if (popup->getUserObject("got-custom-setup-for-sv-cond-toggle")) return;
					popup->setUserObject("got-custom-setup-for-sv-cond-toggle", aa);

					auto main = popup->m_mainLayer;
					auto menu = popup->m_buttonMenu;

					if (auto aaa = main->getChildByType<CCLabelBMFont>(0)) aaa->setString(" \nSave Value Based\n   Toggle Group");

					if (auto aaa = main->getChildByType<CCLabelBMFont>(-1)) aaa->setVisible(false);
					if (auto aaa = menu->getChildByType<CCMenuItem>(-1)) aaa->setVisible(false);

					auto input = TextInput::create(228.700f, "asd:=true (key:[!][=,<,>,*][value])\nset:key:value (setups value)", "chatFont.fnt");
					input->setFilter(" !\"#$ * &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
					input->getInputNode()->m_allowedChars = " !\"#$ * &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
					if (!data->getID().empty()) input->setString(data->getID());
					input->setPositionY(76.000);
					input->setCallback(
						[data = Ref(data)](const std::string& p0) {
							data->setID(p0);
						}
					);
					popup->m_buttonMenu->addChild(input);

					auto dmpinf = CCMenuItemExt::createSpriteExtra(
						ButtonSprite::create("dump"), [](void*) {
							MDPopup::create(
								"Save container dump",
								"```\n" + getMod()->getSaveContainer().dump() + "\n```",
								"oh wow ok, fk..."
							)->show();
						}
					);
					dmpinf->setPosition({ 116.000f, 196.000f });
					popup->m_buttonMenu->addChild(dmpinf);
				}
			);
			return false;
		}
	)->customSetup(
		[](GameObject* object)
		{
			if (!object) return object;
			Ref<CCRepeatForever> action;
			action = CCRepeatForever::create(CCSequence::create(CallFuncExt::create(
				[__this = Ref(object), action] {
					Ref object = typeinfo_cast<EffectGameObject*>(__this.data());
					if (!object) return GameManager::get()->stopAction(action);
					Ref data = typeinfo_cast<CCNode*>(object->getUserObject("data"_spr));
					if (!data) return GameManager::get()->stopAction(action);
					//data str
					auto str = data->getID();
					//update basic stuff
					if (Ref sub = object->getChildByType<CCSprite>(0)) {
						sub->setZOrder(1);
						sub->setScale(0.675f);
						sub->setPositionY(25.5f);
						sub->setPositionX(object->getContentSize().width / 2.f);
						sub->setColor(object->m_activateGroup ?
							cc3bFromHexString("#00FF28").unwrapOrDefault()
							: cc3bFromHexString("#FF0049").unwrapOrDefault()
						);
						if (string::contains(str, "set:")) sub->setColor(
							cc3bFromHexString("#0066FF").unwrapOrDefault()
						);
					}
					//"asd:=true (key:[!][=,<,>,*][value])"
					if (str.empty()) return;
					auto split = string::split(str, ":");
					if (split.size() != 2) return;
					if (split[0].empty()) return void(); // log::error("split[0].empty()");
					if (split[1].empty()) return void(); // log::error("split[1].empty()");
					;;;; std::string key = split[0];
					;; std::string cond = &split[1].at(0);
					matjson::Value value = matjson::parse(split[1].substr(1)).unwrapOrDefault();
					if (key.empty() or cond.empty()) return void(); // log::error("key == {}, cond == {}", key, cond);
					//log::debug("key == {}, cond == {}, value == {}", key, cond, value.dump());
					auto sv = getMod()->getSaveContainer()[key];
					//log::debug("sv == {}", sv.dump());
					auto inv = string::contains(cond, "!");
					auto& v = object->m_activateGroup;
					namespace s = string;
					if (s::contains(cond, "=")) v = (sv == value) - inv;
					if (s::contains(cond, "<")) v = (sv < value) - inv;
					if (s::contains(cond, ">")) v = (sv > value) - inv;
					if (s::contains(cond, "*")) v = s::contains(sv.dump(), value.dump()) - inv;
				}), nullptr
			));
			if (Ref a = GameManager::get()->m_gameLayer) a->runAction(action);
			object->setUserObject("data"_spr, CCNode::create());
			return object;
		}
	)->saveString(
		[](std::string str, GameObject* object, GJBaseGameLayer* level)
		{
			if (!object) return gd::string(str.c_str());
			if (!level) return gd::string(str.c_str());
			object->m_objectID = svcondtrigger->m_refObjectID;
			str = string::replace(
				object->getSaveString(level).c_str(),
				fmt::format("{},", svcondtrigger->m_refObjectID).c_str(),
				fmt::format("{},", svcondtrigger->m_objectID).c_str()
			).c_str();
			object->m_objectID = svcondtrigger->m_objectID;
			if (auto data = typeinfo_cast<CCNode*>(object->getUserObject("data"_spr))) {
				str += ",228,";
				str += ZipUtils::base64URLEncode(data->getID().c_str()).c_str();
			}
			return gd::string(str.c_str());
		}
	)->objectFromVector(
		[](GameObject* object, gd::vector<gd::string>& p0, gd::vector<void*>&, void*, bool)
		{
			if (!object) return object;
			auto data = typeinfo_cast<CCNode*>(object->getUserObject("data"_spr));
			if (data) data->setID(ZipUtils::base64URLDecode(p0[228].c_str()).c_str());
			return object;
		}
	);
	svcondtrigger->registerMe();

}

#include <Geode/modify/DialogLayer.hpp>
class $modify(DialogTrigger, DialogLayer) {

	class Delegate : public DialogDelegate, public CCNode {
	public:
		inline static Delegate* s_pForNextDialogLayer;
		CREATE_FUNC(Delegate);

		Ref<DialogLayer> m_dialogLayer;
		Ref<GJBaseGameLayer> m_game;
		std::string m_replacedTextures = "";
		virtual void dialogClosed(DialogLayer* p0) {
			m_dialogLayer = nullptr;
			if (m_game) m_game->resumeSchedulerAndActions();
			if (m_game) m_game->setKeyboardEnabled(true);
			if (m_game) m_game->setTouchEnabled(true);
			for (auto name : string::split(m_replacedTextures, ",")) {
				CCFileUtils::get()->m_fullPathCache.erase(name.c_str());
				auto result = CCTextureCache::get()->reloadTexture(name.c_str());
			}
		};

		DialogChatPlacement placement = DialogChatPlacement::Center;
		bool hide = false;
		bool no_pause = false;
		bool unskipable = false;
		std::string character = ("");
		int characterFrame = 0;
		bool hadCharacterFrame = false;
		std::optional<GLuint> opacity = std::nullopt;
		;; std::optional<float> scale = std::nullopt;
		;;;;; std::optional<float> px = std::nullopt;
		;;;;; std::optional<float> py = std::nullopt;
		;; std::optional<int> animate = std::nullopt;
	};

	inline static GameObjectsFactory::GameObjectConfig* conf;
	static void setup() {

		GameObjectsFactory::registerGameObject(GameObjectsFactory::createTriggerConfig(
			UNIQ_ID("papperdays-dialog-trigger"),
			"edit_eEventLinkBtn_001.png",
			[](EffectGameObject* trigger, GJBaseGameLayer* game, int p1, gd::vector<int> const* p2)
			{
				if (!trigger) return;
				if (!game) return;

				auto sharedDialogTriggerDelegate = typeinfo_cast<Delegate*>(
					trigger->getUserObject("dialog-delegate")
				);
				if (!sharedDialogTriggerDelegate) return;

				auto DialogTriggerDataNode = typeinfo_cast<CCNode*>(
					trigger->getUserObject("data"_spr)
				);
				if (!DialogTriggerDataNode) return;

				sharedDialogTriggerDelegate->m_game = game;
				auto raw_data = "[" + DialogTriggerDataNode->getID() + "]";
				auto parse = matjson::parse(raw_data);
				matjson::Value data = parse.err()
					? matjson::parse("[ \"<cr>err: " + parse.err().value().message + "</c>\" ]").unwrapOrDefault()
					: parse.unwrapOrDefault();

				auto dialogObjectsArr = CCArrayExt<DialogObject>();

				auto& placement = sharedDialogTriggerDelegate->placement;
				auto& hide = sharedDialogTriggerDelegate->hide;
				auto& no_pause = sharedDialogTriggerDelegate->no_pause;
				auto& unskipable = sharedDialogTriggerDelegate->unskipable;
				auto& character = sharedDialogTriggerDelegate->character;
				auto& characterFrame = sharedDialogTriggerDelegate->characterFrame;
				auto& hadCharacterFrame = sharedDialogTriggerDelegate->hadCharacterFrame;
				auto& opacity = sharedDialogTriggerDelegate->opacity;
				auto& scale = sharedDialogTriggerDelegate->scale;
				auto& px = sharedDialogTriggerDelegate->px;
				auto& py = sharedDialogTriggerDelegate->py;
				auto& animate = sharedDialogTriggerDelegate->animate;

				for (auto& val : data) {
					if (val.isNumber()) {
						characterFrame = val.asInt().unwrapOrDefault();
						hadCharacterFrame = true;
					}
					if (val.isString()) {
						auto text = val.asString().unwrapOrDefault();

						if (string::contains(text, "->")) {
							auto val = string::split(text, "->");
							if (val.size() == 2) if (fileExistsInSearchPaths(val[0].c_str())) { //replace texture
								CCFileUtils::get()->m_fullPathCache.erase(val[0].c_str());
								CCFileUtils::get()->m_fullPathCache.erase(val[1].c_str());
								CCFileUtils::get()->m_fullPathCache[val[0].c_str()] = CCFileUtils::get()->fullPathForFilename(
									val[1].c_str(), 0
								);
								CCTextureCache::get()->reloadTexture(val[0].c_str());
								sharedDialogTriggerDelegate->m_replacedTextures += val[0] + ",";
								continue;
							}
						}

						bool idle = true;
						idle = game->m_player1->m_isOnGround ? idle : false;
						idle = fabs(game->m_player1->m_platformerXVelocity) < 0.01f ? idle : false;
						idle = fabs(game->m_player1->m_yVelocity) < 0.01f ? idle : false;
						if (string::startsWith(text, "!if_idle")) {
							if (!idle) {
								dialogObjectsArr.inner()->removeAllObjects();
								break;
							}
							continue;
						}

						if (string::startsWith(text, "!no_pause")) { no_pause = true; continue; }
						if (string::startsWith(text, "!hide")) { hide = true; continue; }
						if (text == "!") { unskipable = !unskipable; continue; }

						if (auto a = "!op:"; string::startsWith(text, a)) {
							opacity = utils::numFromString<int>(
								string::replace(text, a, "")
							).unwrapOrDefault();
							continue;
						}
						if (auto a = "!s:"; string::startsWith(text, a)) {
							scale = utils::numFromString<float>(
								string::replace(text, a, "")
							).unwrapOrDefault();
							continue;
						}
						if (auto a = "!px:"; string::startsWith(text, a)) {
							px = utils::numFromString<float>(
								string::replace(text, a, "")
							).unwrapOrDefault();
							continue;
						}
						if (auto a = "!py:"; string::startsWith(text, a)) {
							py = utils::numFromString<float>(
								string::replace(text, a, "")
							).unwrapOrDefault();
							continue;
						}
						if (auto a = "!anim:"; string::startsWith(text, a)) {
							animate = utils::numFromString<int>(
								string::replace(text, a, "")
							).unwrapOrDefault();
							continue;
						}

						text = string::replace(text, "!place:", "!p:");
						if (string::startsWith(text, "!p:")) {
							auto place = string::replace(text, "!p:", "");
							if (place == "t") placement = DialogChatPlacement::Top;
							if (place == "c") placement = DialogChatPlacement::Center;
							if (place == "b") placement = DialogChatPlacement::Bottom;
							continue;
						}

						text = string::replace(text, "!char:", "!c:");
						if (string::startsWith(text, "!c:")) {
							character = string::replace(text, "!c:", "");
							continue;
						}

						dialogObjectsArr.push_back(DialogObject::create(
							character, text, characterFrame, 1.f, unskipable, ccWHITE
						));
					}
				}

				if (false) log::debug("placement {}", static_cast<int>(placement));

				auto& dialog = sharedDialogTriggerDelegate->m_dialogLayer;
				if (dialogObjectsArr.size()) {
					if (dialog) dialog->removeFromParent();
					Delegate::s_pForNextDialogLayer = sharedDialogTriggerDelegate;
					dialog = DialogLayer::createDialogLayer(
						dialogObjectsArr[0], dialogObjectsArr.inner(), 1
					);
					dialog->updateChatPlacement(placement);
					if (animate.has_value()) dialog->animateIn(
						(DialogAnimationType)animate.value()
					); else dialog->animateInRandomSide();
					if (game and game->isRunning()) {
						auto scene = CCDirector::get()->m_pRunningScene;
						CCDirector::get()->m_pRunningScene = (CCScene*)game->m_uiLayer;
						CCDirector::get()->m_pRunningScene->setVisible(1);
						dialog->addToMainScene();
						CCDirector::get()->m_pRunningScene = scene;
					}
					dialog->runAction(CCRepeatForever::create(CCSequence::create(CallFuncExt::create(
						[dialog = Ref(dialog), l = Ref(dialog->m_mainLayer), opacity, scale, px, py]() {
							auto someSprite = l->getChildByType<CCSprite*>(0); //2 is icon, 3 is continue mark
							auto hasIcon = !someSprite ? false : someSprite->getZOrder() == 2;
							if (Ref a = l->getChildByType<TextArea*>(0)) {
								a->setPositionX(hasIcon ? -92.000f : -180.000f);
							}
							if (Ref a = l->getChildByType<CCLabelBMFont*>(0)) {
								a->setPositionX(hasIcon ? -93.000f : -176.000f);
							}

							if (Ref a = l->getChildByType<CCScale9Sprite*>(1)) a->setOpacity(0);

							if (opacity.has_value()) dialog->setOpacity(opacity.value());
							if (scale.has_value()) dialog->m_mainLayer->setScale(scale.value());

							auto ptmp = dialog->m_mainLayer->getAnchorPoint();
							if (px.has_value()) ptmp.x = px.value();
							if (py.has_value()) ptmp.y = py.value();
							dialog->m_mainLayer->setAnchorPoint(ptmp);
						}
					), nullptr)));

					if (game and not no_pause) {
						game->setKeyboardEnabled(false);
						game->setTouchEnabled(false);
						game->pauseSchedulerAndActions();
					}
				}
				if (dialog and hide) dialog->removeFromParent();

			},
			[](EditTriggersPopup* popup, EffectGameObject* trigger, CCArray* objects)
			{
				if (!popup) return;
				if (!trigger) return;
				if (!objects) return;
				if (auto data = typeinfo_cast<CCNode*>(trigger->getUserObject("data"_spr))) {
					if (auto title = popup->getChildByType<CCLabelBMFont*>(0)) {
						title->setString("");
						title->setAnchorPoint(CCPointMake(0.5f, 0.3f));
					}
					if (auto inf = popup->m_buttonMenu->getChildByType<InfoAlertButton*>(0)) {
						inf->setVisible(false);
					}
					if (auto a = popup->m_mainLayer->getChildByType<CCScale9Sprite*>(0)) {
						a->setAnchorPoint(CCPointMake(0.5f, 0.900f));
					}

					auto input = TextInput::create(422.f, "Dialog data string...", "chatFont.fnt");
					input->setFilter(" !\"#$ * &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
					input->getInputNode()->m_allowedChars = " !\"#$ * &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
					input->getInputNode()->m_textLabel->setWidth(input->getContentWidth() - 28.f);
					input->setString(data->getID());
					input->setPositionY(174.000f);
					input->getBGSprite()->setContentHeight(350.000f);
					input->setCallback(
						[data = Ref(data)](const std::string& p0) {
							data->setID(p0);
						}
					);
					popup->m_buttonMenu->addChild(input);
					popup->m_mainLayer->setPositionY(-92.000f);

					auto editor = CCMenuItemExt::createSpriteExtra(
						ButtonSprite::create("String Guide"), [input = Ref(input)](CCMenuItem*) {
							web::openLinkInBrowser(
								"https://github.com/LatterRarity70/Dialog-Trigger/tree/master?tab=readme-ov-file#dialog-string-format"
							);
						}
					);
					editor->setPosition(CCPointMake(100.f, 28.f));
					editor->setScale(0.55f);
					editor->m_baseScale = (0.55f);
					editor->m_scaleMultiplier = (1.0f + (0.61f - 0.55f));
					popup->m_buttonMenu->addChild(editor);

					auto Run = CCMenuItemExt::createSpriteExtra(
						ButtonSprite::create("Run"), [trigger = Ref(trigger)](CCMenuItem*) {
							trigger->triggerObject(GameManager::get()->m_gameLayer, 0, nullptr);
						}
					);
					Run->setPosition(CCPointMake(144.f, 64.f));
					popup->m_buttonMenu->addChild(Run);
				}
			}
		)->customSetup(
			[](GameObject* object)
			{
				if (!object) return object;
				object->setUserObject("dialog-delegate", Delegate::create());
				auto data = CCNode::create();
				object->setUserObject("data"_spr, data);
				object->m_objectType = GameObjectType::CustomRing;
				object->m_hasNoEffects = true;
				((RingObject*)object)->RingObject::m_claimTouch = true; //evil
				return object;
			}
		)->saveString(
			[](std::string str, GameObject* object, GJBaseGameLayer* level)
			{
				if (!object) return gd::string(str.c_str());
				if (!level) return gd::string(str.c_str());
				if (auto data = typeinfo_cast<CCNode*>(object->getUserObject("data"_spr))) {
					str += ",228,";
					str += ZipUtils::base64URLEncode(data->getID().c_str()).c_str();
				}
				return gd::string(str.c_str());
			}
		)->objectFromVector(
			[](GameObject* object, gd::vector<gd::string>& p0, gd::vector<void*>&, void*, bool)
			{
				if (!object) return object;

				auto data = typeinfo_cast<CCNode*>(object->getUserObject("data"_spr));
				if (data) {
					data->setID(ZipUtils::base64URLDecode(p0[228].c_str()).c_str());
				};

				return object;
			}
		)->activatedByPlayer(
			[](EnhancedGameObject* asd, PlayerObject* lsd) {
				asd->triggerObject(lsd->m_gameLayer, 0, nullptr);
			}
		));

	};
	static void onModify(auto&) { setup(); }

	void skip(bool close = false) {
		close ? queueInMainThread(
			[xd = Ref(this)] { if (xd) xd->onClose(); }
		) : queueInMainThread(
			[xd = Ref(this)] { if (xd) xd->handleDialogTap(); }
		);
	};

	bool processDialogObject(DialogObject* object) {

		Ref del = typeinfo_cast<Delegate*>(m_delegate);
		if (!del) return false;

		if (!del->m_game) return false;

		auto& placement = del->placement;
		auto& hide = del->hide;
		auto& no_pause = del->no_pause;
		auto& unskipable = del->unskipable;
		auto& character = del->character;
		auto& characterFrame = del->characterFrame;
		auto& hadCharacterFrame = del->hadCharacterFrame;
		auto& opacity = del->opacity;
		auto& scale = del->scale;
		auto& px = del->px;
		auto& py = del->py;
		auto& animate = del->animate;

		std::string text = object->m_text.c_str();
		//log::warn("{}", text.c_str());

		if (string::contains(text, "->")) {
			auto val = string::split(text, "->");
			if (val.size() == 2) if (fileExistsInSearchPaths(val[0].c_str())) { //replace texture
				CCFileUtils::get()->m_fullPathCache.erase(val[0].c_str());
				CCFileUtils::get()->m_fullPathCache.erase(val[1].c_str());
				CCFileUtils::get()->m_fullPathCache[val[0].c_str()] = CCFileUtils::get()->fullPathForFilename(
					val[1].c_str(), 0
				);
				CCTextureCache::get()->reloadTexture(val[0].c_str());
				del->m_replacedTextures += val[0] + ",";
				return true;
			}
		}

		if (string::startsWith(text, "!levelup")) {
			saves()["level"] = saves()["level"].asInt().unwrapOr(0) + 1;
			Ref playlayer = typeinfo_cast<PlayLayer*>(del->m_game.data());
			if (playlayer and playlayer->isRunning()) {
				if (playlayer) playlayer->pauseGame(0);
				if (playlayer) playlayer->pauseAudio();
				if (playlayer) playlayer->removeFromParent();
				auto a = CCDirector::get();
				a->replaceScene(CCScene::create());
				queueInMainThread([a] { a->replaceScene(LevelSelectLayer::scene(0)); });
			}
			return true;
		}
		if (auto a = "!level:"; string::startsWith(text, a)) {
			auto id = utils::numFromString<int>(string::replace(text, a, "")).unwrapOrDefault();
			saves()["level"] = id;
			Ref playlayer = typeinfo_cast<PlayLayer*>(del->m_game.data());
			if (playlayer and playlayer->isRunning()) {
				if (playlayer) playlayer->pauseGame(0);
				if (playlayer) playlayer->pauseAudio();
				if (playlayer) playlayer->removeFromParent();
				auto a = CCDirector::get();
				a->replaceScene(CCScene::create());
				queueInMainThread([a] { a->replaceScene(LevelSelectLayer::scene(0)); });
			}
			return true;
		}

		if (string::startsWith(text, "!exit")) { game::exit(); return true; }
		if (string::startsWith(text, "!restart")) { game::restart(); return true; }

		if (auto a = "!activate:"; string::startsWith(text, a)) {
			auto id = utils::numFromString<int>(string::replace(text, a, "")).unwrapOrDefault();
			if (del->m_game) del->m_game->spawnGroup(id, false, 0, gd::vector<int>(), -1, -1);
			return true;
		}
		if (auto a = "!toggle:"; string::startsWith(text, a)) {
			auto id = utils::numFromString<float>(string::replace(text, a, "")).unwrapOrDefault();
			if (del->m_game) del->m_game->toggleGroup(id, id > (int)id); //123,0 is on and 123,1 is off
			return true;
		}

		if (auto a = "!song:"; string::startsWith(text, a)) {
			auto song = string::replace(text, a, "");
			FMODAudioEngine::get()->playMusic(song, 1, 0.f, 0);
			return true;
		}
		if (auto a = "!sfx:"; string::startsWith(text, a)) {
			auto sfx = string::replace(text, a, "");
			FMODAudioEngine::get()->playEffect(sfx);
			return true;
		}

		if (auto a = "!ntfy:"; string::startsWith(text, a)) {
			auto args = string::split(string::replace(text, a, ""), "//");

			auto str = args[0];
			auto icon = NotificationIcon::None;
			auto sprite = (CCSprite*)nullptr;
			auto time = NOTIFICATION_DEFAULT_TIME;

			if (args.size() > 1) {
				auto id = utils::numFromString<int>(args[1]);
				if (id.isOk()) icon = (NotificationIcon)id.unwrapOrDefault();
				else {
					sprite = CCSprite::createWithSpriteFrameName(args[1].c_str());
					if (auto a = CCSprite::create(args[1].c_str())) sprite = a;
				}
			}
			if (args.size() > 2) time = utils::numFromString<float>(
				args[2]
			).unwrapOrDefault();

			if (sprite) Notification::create(str, sprite, time)->show();
			else Notification::create(str, icon, time)->show();

			return true;
		}

		if (auto a = "!popup:"; string::startsWith(text, a)) {
			auto args = string::split(string::replace(text, a, ""), "//");

			auto title = args[0];
			auto cap = std::string("");
			auto btn1 = std::string("OK");
			auto btn2 = std::string();
			auto group1 = 0;
			auto group2 = 0;

			if (args.size() > 1) cap = args[1];
			if (args.size() > 2) {
				auto spl = string::split(args[2], "->");
				btn1 = spl[0];
				if (spl.size() > 1) group1 = utils::numFromString<int>(spl[1]).unwrapOrDefault();
			}
			if (args.size() > 3) {
				auto spl = string::split(args[3], "->");
				btn2 = spl[0];
				if (spl.size() > 1) group2 = utils::numFromString<int>(spl[1]).unwrapOrDefault();
			}

			if (del->m_game and del->m_game->isRunning()) {
				auto scene = CCDirector::get()->m_pRunningScene;
				CCDirector::get()->m_pRunningScene = (CCScene*)del->m_game->m_uiLayer;
				CCDirector::get()->m_pRunningScene->setVisible(1);
				if (not no_pause) {
					del->m_game->setKeyboardEnabled(false);
					del->m_game->setTouchEnabled(false);
					del->m_game->pauseSchedulerAndActions();
				}
				auto popup = createQuickPopup(
					title.c_str(), cap.c_str(),
					btn1.empty() ? nullptr : btn1.c_str(),
					btn2.empty() ? nullptr : btn2.c_str(),
					[_ = Ref(this), object = Ref(object), 
					game = Ref(del->m_game), del = Ref(del), 
					group1, group2](CCNode* a, bool btn2) {
						del->dialogClosed(nullptr);
						if (game) game->spawnGroup(
							btn2 ? group2 : group1, false, 0, gd::vector<int>(), -1, -1
						);
						if (_) {
							a->getParent()->addChild(_);
							_->setUserObject("call-org-display", object);
							_->skip();
						}
					}
				);
				CCDirector::get()->m_pRunningScene = scene;

				if (placement != DialogChatPlacement::Center) {
					popup->m_mainLayer->ignoreAnchorPointForPosition(false);
					popup->m_mainLayer->setAnchorPoint(
						{ 0.f, [](DialogChatPlacement placement) -> float {
							if (placement == DialogChatPlacement::Top) return -0.250f;
							if (placement == DialogChatPlacement::Bottom) return 0.250f;
							return 0.f;
						}(placement) }
					);
				}
				if (opacity.has_value()) popup->setOpacity(opacity.value());
				if (scale.has_value()) popup->setScale(scale.value());

				auto ptmp = popup->m_mainLayer->getAnchorPoint();
				if (px.has_value()) ptmp.x = px.value();
				if (py.has_value()) ptmp.y = py.value();
				popup->m_mainLayer->setAnchorPoint(ptmp);

			}

			setUserObject("dont-skip", object);
			return true;
		}

		if (auto a = "!plr_speed:"; string::startsWith(text, a)) {
			auto speed = utils::numFromString<float>(string::replace(text, a, "")).unwrapOrDefault();
			std::vector<Ref<PlayerObject>> ps = { del->m_game->m_player1, del->m_game->m_player2 };
			for (auto& plr : ps) if (plr) plr->m_speedMultiplier = (speed);
			return true;
		}

		if (string::startsWith(text, "!close")) {
			queueInMainThread([xd = Ref(this)] { xd->onClose(); });
		}
		if (string::startsWith(text, "!tap")) {
			queueInMainThread([xd = Ref(this)] { xd->handleDialogTap(); });
		}

		return false;
	}

	void displayDialogObject(DialogObject* object) {
		if (getUserObject("call-org-display") == object) {
			return DialogLayer::displayDialogObject(object);
		}
		if (processDialogObject(object)) {
			if (getUserObject("dont-skip") == object) queueInMainThread(
				[xd = Ref(this)] { xd->removeFromParentAndCleanup(false); }
			); else skip();
		}
		else DialogLayer::displayDialogObject(object);
	};

	bool init(DialogObject* object, cocos2d::CCArray* objects, int background) {
		m_delegate = m_delegate ? m_delegate : Delegate::s_pForNextDialogLayer;
		if (!DialogLayer::init(object, objects, background)) return false;
		if (this) this->runAction(CCSequence::createWithTwoActions(
			CCDelayTime::create(0.1f), CallFuncExt::create(
				[_this = Ref(this)] { _this->m_handleTap = (1); }
			)));
		return true;
	};

	void displayNextObject_() {};

};

#include <Geode/modify/EffectGameObject.hpp>
class $modify(MenuItemGameObject, EffectGameObject) {

	class CCMenuItem : public cocos2d::CCMenuItem {
	public:
		CREATE_FUNC(CCMenuItem);
		virtual bool init() {
			CCMenuItem::initWithTarget(this, nullptr);
			this->setAnchorPoint({ 0.5f, 0.5f });
			this->setEnabled(true);
			return true;
		};
		std::function<void(void)> m_onActivate = []() {};
		std::function<void(void)> m_onSelected = []() {};
		std::function<void(void)> m_onUnselected = []() {};
		virtual void activate() { if (m_onActivate) m_onActivate(); };
		virtual void selected() { if (m_onSelected) m_onSelected(); };
		virtual void unselected() { if (m_onUnselected) m_onUnselected(); };
		auto onActivate(std::function<void(void)> onActivate) { m_onActivate = onActivate; return this; }
		auto onSelected(std::function<void(void)> onSelected) { m_onSelected = onSelected; return this; }
		auto onUnselected(std::function<void(void)> onUnselected) { m_onUnselected = onUnselected; return this; }
	};

#define MenuItemObjectData(ring) DataNode::at(ring, "menu-item-data")
	inline static GameObjectsFactory::GameObjectConfig* conf;

	static void setupMenuItemPopup(EditorUI*, EffectGameObject * obj, SetupCollisionStateTriggerPopup * popup) {

		if (popup->getUserObject("got-custom-setup-for-menu-item")) return;
		popup->setUserObject("got-custom-setup-for-menu-item", obj);

		auto main = popup->m_mainLayer;
		auto menu = popup->m_buttonMenu;
		if (auto aaa = main->getChildByType<CCLabelBMFont>(0)) aaa->setString("Menu Item");

		if (auto aaa = main->getChildByType<CCLabelBMFont>(1)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCLabelBMFont>(2)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCScale9Sprite>(1)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCScale9Sprite>(2)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCTextInputNode>(0)) aaa->setVisible(false);
		if (auto aaa = main->getChildByType<CCTextInputNode>(1)) aaa->setVisible(false);

		while (auto aaa = menu->getChildByTag(51)) aaa->removeFromParentAndCleanup(false);
		while (auto aaa = menu->getChildByTag(71)) aaa->removeFromParentAndCleanup(false);

		auto data = MenuItemObjectData(obj);

		//activate
		auto activate = TextInput::create(52.f, "ID");
		activate->setFilter("0123456789");
		activate->getInputNode()->m_allowedChars = "0123456789";
		activate->setString(utils::numToString(data->get("activate").asInt().unwrapOr(0)));
		activate->setPositionY(95.000f);
		activate->setCallback(
			[data = Ref(MenuItemObjectData(obj))](const std::string& p0) {
				data->set("activate", utils::numFromString<int>(p0).unwrapOr(0));
			}
		);
		menu->addChild(activate);
		auto activateLabel = CCLabelBMFont::create("Activate:\n \n \n \n ", "goldFont.fnt");
		activateLabel->setScale(0.5f);
		activate->getInputNode()->addChild(activateLabel);

		//selected
		auto selected = TextInput::create(54.f, "ID");
		selected->setFilter("0123456789");
		selected->getInputNode()->m_allowedChars = "0123456789";
		selected->setString(utils::numToString(data->get("selected").asInt().unwrapOr(0)));
		selected->setPosition(-95.000f, 77.f);
		selected->setCallback(
			[data = Ref(MenuItemObjectData(obj))](const std::string& p0) {
				data->set("selected", utils::numFromString<int>(p0).unwrapOr(0));
			}
		);
		menu->addChild(selected);
		auto selectedLabel = CCLabelBMFont::create("Selected:\n \n \n \n ", "goldFont.fnt");
		selectedLabel->setScale(0.5f);
		selected->getInputNode()->addChild(selectedLabel);

		//unselected
		auto unselected = TextInput::create(48.f, "ID");
		unselected->setFilter("0123456789");
		unselected->getInputNode()->m_allowedChars = "0123456789";
		unselected->setString(utils::numToString(data->get("unselected").asInt().unwrapOr(0)));
		unselected->setPosition(95.000f, 77.f);
		unselected->setCallback(
			[data = Ref(MenuItemObjectData(obj))](const std::string& p0) {
				data->set("unselected", utils::numFromString<int>(p0).unwrapOr(0));
			}
		);
		menu->addChild(unselected);
		auto unselectedLabel = CCLabelBMFont::create("Unselected:\n \n \n \n ", "goldFont.fnt");
		unselectedLabel->setScale(0.5f);
		unselected->getInputNode()->addChild(unselectedLabel);
	}

	static void setup() {
		conf = GameObjectsFactory::createRingConfig(
			UNIQ_ID("menu-item"), ""
		)->refID(3640)->tab(12)->insertIndex((12 * 6) + 3)->onEditObject(
			[](EditorUI* a, GameObject* aa) -> bool {
				queueInMainThread(
					[a = Ref(a), aa = Ref(aa)] {
						if (!CCScene::get()) return log::error("CCScene::get() == {}", CCScene::get());
						auto popup = CCScene::get()->getChildByType<SetupCollisionStateTriggerPopup>(0);
						if (!popup) return log::error("popup == {}", popup);
						auto object = typeinfo_cast<EffectGameObject*>(aa.data());
						if (!object) return log::error("object == {} ({})", object, aa);
						setupMenuItemPopup(a, object, popup);
					}
				);
				return false;
			}
		)->saveString(
			[](std::string str, GameObject* object, GJBaseGameLayer* level)
			{
				str += ",228,";
				str += ZipUtils::base64URLEncode(MenuItemObjectData(object)->_json_str.c_str()).c_str();
				return str;
			}
		)->objectFromVector(
			[](GameObject* object, gd::vector<gd::string>& p0, gd::vector<void*>& p1, GJBaseGameLayer* p2, bool p3)
			{
				auto parsed = matjson::parse(
					ZipUtils::base64URLDecode(p0[228].c_str()).c_str()
				).unwrapOrDefault();
				for (auto& [key, value] : parsed) MenuItemObjectData(object)->set(key, value);
				return object;
			}
		)->customSetup(
			[](GameObject* object) {
				object->m_outerSectionIndex = -1;
				object->m_isInvisible = false;
				object->setDisplayFrame(object->m_editorEnabled ? 
					object->displayFrame() : CCSprite::create()->displayFrame()
				);
			}
		)->resetObject(
			[](GameObject* pObj) {
				if (!GameManager::get()->m_gameLayer) return;
				Ref game(GameManager::get()->m_gameLayer);

				Ref object(pObj);

				int uid = hash(object->getSaveString(game).c_str());
				object->setTag(uid);

				Ref menu = typeinfo_cast<CCMenu*>(game->getUserObject("objects-menu"));
				if (!menu) {
					menu = CCMenu::create();
					menu->setID("objects-menu");
					menu->setPosition(CCSizeZero);
					menu->setContentSize(CCSizeZero);
					menu->setAnchorPoint(CCPointZero);
					game->setUserObject("objects-menu", menu);
					game->m_uiTriggerUI->addChild(menu);
				}

				while (menu->getChildByTag(uid)) menu->removeChildByTag(uid);

				Ref item = CCMenuItem::create();
				if (item) {
					//virtual void spawnGroup(int group, bool ordered, double delay, gd::vector<int> const& remapKeys, int triggerID, int controlID);
					item->onActivate([game, data = Ref(MenuItemObjectData(object))] {
						if (game) game->spawnGroup(data->get("activate").asInt().unwrapOr(0), false, 0, gd::vector<int>(), -1, -1);
						});
					item->onSelected([game, data = Ref(MenuItemObjectData(object))] {
						if (game) game->spawnGroup(data->get("selected").asInt().unwrapOr(0), false, 0, gd::vector<int>(), -1, -1);
						});
					item->onUnselected([game, data = Ref(MenuItemObjectData(object))] {
						if (game) game->spawnGroup(data->get("unselected").asInt().unwrapOr(0), false, 0, gd::vector<int>(), -1, -1);
						});
					item->setTag(uid);
				} else return;
				menu->setTouchEnabled(false);
				menu->setTouchEnabled(true);

				Ref action = menu->getActionByTag(uid);
				if (!action) {
					action = CCRepeatForever::create(CCSequence::create(CallFuncExt::create(
						[object, item, menu, game] {
							if (!game) return;
							if (!object) return;
							if (!item) return;
							if (!menu) return;
							if (item->getParent() != menu) {
								item->removeFromParentAndCleanup(false);
								menu->addChild(item);
								menu->setTouchEnabled(false);
								menu->setTouchEnabled(true);
							}
							menu->setVisible(game->m_uiLayer->isVisible());
							item->setContentWidth(object->m_width);
							item->setContentHeight(object->m_height);
							item->setAnchorPoint(CCPointMake(0.5, 0.5) * not object->m_editorEnabled);
							item->setAdditionalTransform(CCAffineTransformConcat(
								object->nodeToWorldTransform(),
								CCAffineTransformInvert(menu->nodeToWorldTransform())
							));
							item->updateTransform();
						}
					), nullptr));
					action->setTag(uid);
					menu->runAction(action);
				}
			}
		);
		conf->registerMe();
	}
	static void onModify(auto&) { setup(); }

	virtual void resetObject() {
		EffectGameObject::resetObject();
	};

};

#include <Geode/modify/UILayer.hpp>
class $modify(UILayerKeysExt, UILayer) {
	void customUpdate(float) {
		this->setKeyboardEnabled(false);
		this->setKeyboardEnabled(this->isVisible());
	}
	bool init(GJBaseGameLayer * p0) {
		if (!UILayer::init(p0)) return false;
		this->schedule(schedule_selector(UILayerKeysExt::customUpdate));
		return true;
	};
	void handleKeypress(cocos2d::enumKeyCodes key, bool p1) {
		UILayer::handleKeypress(key, p1);

		auto eventID = 120000 + (int)key;
		m_gameLayer->gameEventTriggered((GJGameEvent)eventID, 0, 0);
		m_gameLayer->gameEventTriggered((GJGameEvent)eventID, 0, 1 + !p1);
	}
};

class KeyEventListener : public CCNode, public CCKeyboardDelegate {
public:
	KeyEventListener() { CCKeyboardDispatcher::get()->addDelegate(this); };
	void onExit() override {
		CCNode::onExit();
		CCKeyboardDispatcher::get()->removeDelegate(this);
	}
	CREATE_FUNC(KeyEventListener);
	void keyDown(enumKeyCodes key) override {
		if (m_keyDown) m_keyDown(key);
	}
	void keyUp(enumKeyCodes key) override {
		if (m_keyUp) m_keyUp(key);
	}
	auto onKeyDown(std::function<void(enumKeyCodes key)> keyDown) { m_keyDown = keyDown; return this; }
	auto onKeyUp(std::function<void(enumKeyCodes key)> keyUp) { m_keyUp = keyUp; return this; }

	std::function<void(enumKeyCodes key)> m_keyDown = [](enumKeyCodes) {};
	std::function<void(enumKeyCodes key)> m_keyUp = [](enumKeyCodes) {};
};

#include <Geode/modify/SelectEventLayer.hpp>
class $modify(SelectEventLayerKeysExt, SelectEventLayer) {
	bool init(SetupEventLinkPopup * p0, gd::set<int>&p1) {
		if (!SelectEventLayer::init(p0, p1)) return false;

		auto keyEventsExpandBtn = CCMenuItemExt::createToggler(
			ButtonSprite::create("Keys", "goldFont.fnt", "GJ_button_04.png", 0.6f),
			ButtonSprite::create("Keys", "goldFont.fnt", "GJ_button_02.png", 0.6f),
			[popup = Ref(this)](CCMenuItemToggler* keyEventsExpandBtn) {

				while (auto a = popup->m_buttonMenu->getChildByID("key-list-item")) a->removeFromParent();
				while (auto a = popup->getChildByType<KeyEventListener>(0)) a->removeFromParent();

				if (!keyEventsExpandBtn->isOn()) return;

				auto posY = keyEventsExpandBtn->getPositionY() + keyEventsExpandBtn->getContentSize().height + 4.f;

				for (auto eventID : popup->m_eventIDs) if (eventID >= 120000 and eventID < 130000) {
					std::string name = CCKeyboardDispatcher::get()->keyToString((enumKeyCodes)(eventID - 120000));
					auto item = CCMenuItemExt::createSpriteExtra(
						ButtonSprite::create((" " + name + " ").c_str(), "goldFont.fnt", "GJ_button_05.png", 0.5f)
						, [popup, eventID, keyEventsExpandBtn](void*) {
							popup->m_eventIDs.erase(eventID);
							popup->m_eventsChanged = true;
							keyEventsExpandBtn->activate();
							keyEventsExpandBtn->activate();
						}
					);
					item->setID("key-list-item");
					item->setPositionY(posY);
					item->setPositionX(keyEventsExpandBtn->getPositionX());
					popup->m_buttonMenu->addChild(item, 999);

					posY += item->getContentSize().height + 4.f;
				}

				auto inf = CCLabelBMFont::create(" \nPress any key...", "chatFont.fnt");
				inf->setID("key-list-item");
				inf->setScale(0.625f);
				inf->setZOrder(999);
				popup->m_buttonMenu->addChildAtPosition(
					inf, Anchor::BottomLeft, { keyEventsExpandBtn->getPositionX(), posY }, false
				);

				popup->addChild(KeyEventListener::create()->onKeyDown(
					[popup, keyEventsExpandBtn](enumKeyCodes key) {
						popup->m_eventIDs.insert(120000 + (int)key);
						popup->m_eventsChanged = true;
						keyEventsExpandBtn->activate();
						keyEventsExpandBtn->activate();
					}
				), 999);
			}
		);
		keyEventsExpandBtn->setID("key-events-expand-btn");
		keyEventsExpandBtn->setPositionX(142.000f);
		keyEventsExpandBtn->setPositionY(0.f);
		keyEventsExpandBtn->activate();
		Ref(this)->m_buttonMenu->addChild(keyEventsExpandBtn);

		return true;
	}
};