#include <user95401.game-objects-factory/include/main.hpp>
#include <user95401.game-objects-factory/include/impl.hpp>



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
			GameManager::get()->playMenuMusic();
			this->addChild(geode::createLayerBG());
			this->scheduleOnce(schedule_selector(GJGameLoadingLayerWhatTheF::xd), 5.f);
			auto text = CCLabelBMFont::create("-_-", "chatFont.fnt");
			text->setOpacity(0);
			text->runAction(CCFadeIn::create(5.f));
			text->setAlignment(kCCTextAlignmentCenter);
			text->setPosition(this->getContentSize() / 2);
			limitNodeSize(text, this->getContentSize(), 0.6f, 0.3f);
			this->addChild(text);
		}
	};
};

#include <Geode/modify/GameManager.hpp>
class $modify(GameManagerSetsForGV, GameManager) {
	bool getGameVariable(char const* p0) {
		if (p0 == std::string("0024")) return true; //show mouse
		return GameManager::getGameVariable(p0);
	};
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
	plrinputtrigger->spawnObject([](GJBaseGameLayer* a, GameObject*aaa , double, gd::vector<int> const& aa) {
		plrinputtrigger->m_triggerObject((EffectGameObject*)aaa, GameManager::get()->getGameLayer(), 0, nullptr);
		});
	plrinputtrigger->registerMe();


	class DialogTriggerDelegate : public DialogDelegate {
	public:
		DialogLayer* m_dialogLayer = nullptr;
		GJBaseGameLayer* m_game = nullptr;
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
	};
	static auto sharedDialogTriggerDelegate = new DialogTriggerDelegate();

	GameObjectsFactory::registerGameObject(GameObjectsFactory::createTriggerConfig(
		UNIQ_ID("papperdays-dialog-trigger"),
		"edit_eEventLinkBtn_001.png",
		[](EffectGameObject* trigger, GJBaseGameLayer* game, int p1, gd::vector<int> const* p2)
		{
			if (auto DialogTriggerDataNode = typeinfo_cast<CCNode*>(trigger->getUserObject("data"_spr))) {
				sharedDialogTriggerDelegate->m_game = game;
				auto raw_data = "[" + DialogTriggerDataNode->getID() + "]";
				auto parse = matjson::parse(raw_data);
				matjson::Value data = parse.err()
					? matjson::parse("[ \"<cr>err: " + parse.err().value().message + "</c>\" ]").unwrapOrDefault()
					: parse.unwrapOrDefault();

				/*
					example text:
					```
					1,"char:the noone","<cr>hi!</c> why ar u here ever?","!","its feels bad you here"
					```
				*/

				DialogChatPlacement placement = DialogChatPlacement::Center;

				auto no_pause = false;
				auto not_skippable = true;
				auto character = std::string("");
				auto characterFrame = 0;

				auto dialogObjectsArr = CCArrayExt<DialogObject>();

				for (auto& val : data) {
					if (val.isNumber()) {
						characterFrame = val.asInt().unwrapOrDefault();
					}
					if (val.isString()) {
						auto text = val.asString().unwrapOrDefault();
						text = string::replace(text, "!char:", "!c:");
						if (string::startsWith(text, "!c:")) {
							character = string::replace(text, "!c:", "");
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
						if (text == "!") {
							not_skippable = false;
							continue;
						}
						if (string::contains(text, "->")) {
							auto val = string::split(text, "->");
							if (val.size() == 2) if (fileExistsInSearchPaths(val[0].c_str())) { //replace texture
								CCFileUtils::get()->m_fullPathCache[val[0]] = CCFileUtils::get()->fullPathForFilename(val[1].c_str(), 0);
								auto result = CCTextureCache::get()->reloadTexture(val[0].c_str());
								if (!result) dialogObjectsArr.push_back(DialogObject::create(
									"Failed to reload texture!", ("<cr>Key: " + val[0] + "\nBy: " + val[1] + "\n"), 0, 1.f, 1, ccWHITE
								));
								sharedDialogTriggerDelegate->m_replacedTextures += val[0] + ",";
								continue;
							}
						}
						if (string::startsWith(text, "!no_pause")) {
							no_pause = true;
							continue;
						}
						if (string::startsWith(text, "!plr_speed:")) {
							auto speed = utils::numFromString<float>(string::replace(text, "!plr_speed:", "")).unwrapOrDefault();
							std::vector<Ref<PlayerObject>> ps = { game->m_player1, game->m_player2 };
							for (auto& plr : ps) if (plr) plr->m_speedMultiplier = (speed);
							continue;
						}
						bool idle = game->m_player1->m_isOnGround and fabs(game->m_player1->m_platformerXVelocity) < 0.25f;
						if (string::startsWith(text, "!if_idle")) {
							if (!idle) {
								dialogObjectsArr.inner()->removeAllObjects();
								break;
							}
							continue;
						}
						dialogObjectsArr.push_back(DialogObject::create(
							character, text, characterFrame, 1.f, not not_skippable, ccWHITE
						));
					}
				}

				if (false) log::debug("placement {}", static_cast<int>(placement));

				if (dialogObjectsArr.size()) {
					auto& dialog = sharedDialogTriggerDelegate->m_dialogLayer;
					if (dialog) dialog->removeFromParent();
					dialog = DialogLayer::createDialogLayer(
						dialogObjectsArr[0], dialogObjectsArr.inner(), 1
					);
					dialog->m_delegate = sharedDialogTriggerDelegate;
					dialog->updateChatPlacement(placement);
					dialog->animateInRandomSide();
					dialog->addToMainScene();
					dialog->runAction(CCRepeatForever::create(CCSequence::create(CallFuncExt::create(
						[dialog = Ref(dialog), layer = Ref(dialog->m_mainLayer)]() {
							auto textArea = layer->getChildByType<TextArea*>(0);
							auto someSprite = layer->getChildByType<CCSprite*>(0); //2 is icon, 3 is continue mark
							if (textArea and someSprite) {
								auto hasIcon = someSprite->getZOrder() == 2;
								textArea->setAnchorPoint(hasIcon ? CCPointMake(0.000f, 0.5f) : CCPointMake(0.1f, 0.5f));
							}
						}
					), nullptr)));

					if (game and not no_pause) {
						game->setKeyboardEnabled(false);
						game->setTouchEnabled(false);
						game->pauseSchedulerAndActions();
					}
				}
			}
		},
		[](EditTriggersPopup* popup, EffectGameObject* trigger, CCArray* objects)
		{
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
				input->setFilter(" !\"#$ % &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
				input->getInputNode()->m_allowedChars = " !\"#$ % &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
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

}

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