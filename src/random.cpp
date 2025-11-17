#include <Geode/Geode.hpp>
using namespace geode::prelude;

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

#include <Geode/modify/GameManager.hpp>
class $modify(GameManagerSetsForGV, GameManager) {
	cocos2d::ccColor3B colorForIdx(int p0) {
		if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return GameManager::colorForIdx(p0);
		return ccWHITE;
	}
	bool getGameVariable(char const* tag) {

		if (std::string(tag) == "0094") return not "More Comments";
		if (std::string(tag) == "0090") return not "Load Comments";
		if (std::string(tag) == "0073") return not "New Completed Filter";
		if (std::string(tag) == "0093") return not "Increase Local Levels Per Page";
		if (std::string(tag) == "0084") return not "Manual Level Order";
		if (std::string(tag) == "0099") return not "Show Leaderboard Percentage";
		if (std::string(tag) == "0127") return not "Save Gauntlets";

		//if (std::string(tag) == "0125") return "Enable Normal Music In Editor";
		if (std::string(tag) == "0174") return "Hide Playtest Text";
		if (std::string(tag) == "0119") return "Disable Level Saving";
		if (std::string(tag) == "0042") return "Increase Maximum Levels";

		if (std::string(tag) == "0060") return not "Default Mini Icon";
		if (std::string(tag) == "0061") return not "Switch Spider Teleport Color";
		if (std::string(tag) == "0062") return not "Switch Dash Fire Color";
		if (std::string(tag) == "0096") return not "Switch Wave Trail Color";

		if (std::string(tag) == "0071") return "Hide Practice Buttons";
		if (std::string(tag) == "0135") return "Hide Attempts (practice)";

		if (std::string(tag) == "0024") return "Show Cursor In-Game";
		if (std::string(tag) == "0129") return "Disable Portal Guide";
		if (std::string(tag) == "0130") return not "Enable Orb Guide";
		if (std::string(tag) == "0140") return not "Disable Orb Scale";
		if (std::string(tag) == "0141") return not "Disable Trigger Orb Scale";
		if (std::string(tag) == "0072") return "Disable Gravity Effect";
		if (std::string(tag) == "0100") return not "Enable Death Effect";
		if (std::string(tag) == "0082") return "Disable High Object Alert";
		if (std::string(tag) == "0033") return "Change Custom Songs Location";
		if (std::string(tag) == "0083") return "Disable Song Alert";
		if (std::string(tag) == "0018") return "No Song Limit";
		if (std::string(tag) == "0168") return "Fast Menu";
		if (std::string(tag) == "0171") return "Disable Player Hitbox";
		if (std::string(tag) == "0068") return not "Enable Quick Checkpoints (Ctrl+S style)";
		if (std::string(tag) == "0172") return "Enable Shake";
		if (std::string(tag) == "0014") return not "Enable Explosion Shake";
		if (std::string(tag) == "0100") return not "Enable Death Effect";
		if (std::string(tag) == "0155") return "Disable Shader AA"; // pixel-art style
		if (std::string(tag) == "0066") return "Increase Draw Capacity";
		if (std::string(tag) == "0166") return "Hide Hitboxes";

		return GameManager::getGameVariable(tag);
	};
};