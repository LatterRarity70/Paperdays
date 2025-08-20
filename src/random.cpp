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