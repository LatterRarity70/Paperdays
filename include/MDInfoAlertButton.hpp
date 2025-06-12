#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

//InfoAlertButton but with md popup
class MDInfoAlertButton : public InfoAlertButton {
public:
    static MDInfoAlertButton* create(gd::string title, gd::string desc, float spriteScale) {
        auto button = InfoAlertButton::create(title, desc, spriteScale);
        if (button) {
            CCMenuItemExt::assignCallback<InfoAlertButton>(
                button, [title, desc](InfoAlertButton* item){
					MDPopup::create(title, desc, "OK")->show();
                }
            );
        }
        return static_cast<MDInfoAlertButton*>(button);
    };
};
#include <Geode/modify/InfoAlertButton.hpp>
class $modify(InfoAlertButtonActivateFixExt, InfoAlertButton) {
    void activate() {
        if (m_pfnSelector) this->CCMenuItemSpriteExtra::activate();
        else InfoAlertButton::activate();
    };
};