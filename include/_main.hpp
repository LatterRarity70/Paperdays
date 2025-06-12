#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
using namespace geode::prelude;

#include <regex>

#include <fs.hpp>

//lol
#define SETTING(type, key_name) Mod::get()->getSettingValue<type>(key_name)

#define MEMBER_BY_OFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
template<typename T, typename U> constexpr size_t OFFSET_BY_MEMBER(U T::* member) { return (char*)&((T*)nullptr->*member) - (char*)nullptr; }

#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

#define LOG_THIS_FILE $execute { log::debug("LOG_THIS_FILE: \n\n{}\n", fs::path(__FILE__).filename().string()); }

namespace geode::utils::string {
    inline std::vector<std::string> explode(std::string separator, std::string input) {
        std::vector<std::string> vec;
        for (int i{ 0 }; i < input.length(); ++i) {
            int pos = input.find(separator, i);
            if (pos < 0) { vec.push_back(input.substr(i)); break; }
            int count = pos - i;
            vec.push_back(input.substr(i, count));
            i = pos + separator.length() - 1;
        }
        if (vec.size() == 0) vec.push_back(input);
        return vec;
    }
}

namespace geode::cocos {
    inline std::string getFrameName(CCNode* node, bool preferTexture = false) {
        static std::unordered_map<CCSpriteFrame*, std::string> frameToName;
        static std::unordered_map<CCTexture2D*, std::string> texToName;
        static unsigned int lastFrameCount = 0;
        static unsigned int lastTexCount = 0;
        static auto updateFrameMap = []() {
            auto dict = CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames;
            auto count = dict->count();
            if (count == lastFrameCount) return;           // ничего нового
            lastFrameCount = count;
            frameToName.clear();                           // можно не чистить, но так надёжнее
            CCDictElement* el = nullptr;
            CCDICT_FOREACH(dict, el) {
                frameToName[static_cast<CCSpriteFrame*>(el->getObject())]
                    = el->getStrKey();
            }
            };
        static auto updateTextureMap = []() {
            auto dict = CCTextureCache::sharedTextureCache()->m_pTextures;
            auto count = dict->count();
            if (count == lastTexCount) return;
            lastTexCount = count;
            texToName.clear();
            CCDictElement* el = nullptr;
            CCDICT_FOREACH(dict, el) {
                texToName[static_cast<CCTexture2D*>(el->getObject())]
                    = el->getStrKey();
            }
            }; 
        if (!node) return "NIL_NODE";
        updateFrameMap();
        updateTextureMap();
        if (auto tp = typeinfo_cast<CCTextureProtocol*>(node)) {
            CCTexture2D* tex = tp->getTexture();
            if (tex) {
                if (preferTexture) {
                    auto itT = texToName.find(tex);
                    if (itT != texToName.end()) return itT->second;
                }
                if (auto sp = typeinfo_cast<CCSprite*>(node)) {
                    auto frame = sp->displayFrame();
                    if (frame) {
                        auto itF = frameToName.find(frame);
                        if (itF != frameToName.end()) return itF->second;
                    }
                }
                auto itT2 = texToName.find(tex);
                if (itT2 != texToName.end()) return itT2->second;
            }
        }
        auto sub = node ? node->getChildByType<CCTextureProtocol*>(0) : nullptr;
        return sub ? getFrameName(typeinfo_cast<CCNode*>(sub), preferTexture) : "UNKNOWN";
    }
    inline auto createDataNode(std::string id, std::string text = "", int tag = 0) {
        auto node = CCLabelBMFont::create("", "chatFont.fnt");
        node->setID(id);
        node->setString(text.c_str());
        if (tag != 0) node->setTag(tag);
        node->setVisible(0);
        return node;
    }
    inline auto findDataNode(CCNode* parent, std::string id) {
        auto node = typeinfo_cast<CCLabelBMFont*>(parent->getChildByIDRecursive(id));
        //if (!node) log::warn("FAILED TO FIND DATA NODE! id: {}", id);
        return node;
    }
    inline auto nodeName(CCNode* of) {
        auto rtn = of->getID();
        if (rtn.empty()) rtn = getFrameName(of);
        if (string::contains(rtn, "resources/")) {
            auto slExpl = string::explode("/", rtn);
            rtn = slExpl[slExpl.size() - 1];
        }
        auto slExpl = string::explode("/", rtn);
        rtn = slExpl[slExpl.size() - 1];
        rtn = string::toUpper(rtn);
        rtn = string::replace(rtn, "-BUTTON-", "");
        rtn = string::replace(rtn, "-BUTTON", "");
        rtn = string::replace(rtn, "001", " ");
        rtn = string::replace(rtn, "GJ_", " ");
        rtn = string::replace(rtn, "-ROUND", " ");
        rtn = string::replace(rtn, "_ROUND", " ");
        rtn = string::replace(rtn, "-LOGO", " ");
        rtn = string::replace(rtn, "_LOGO", " ");
        rtn = string::replace(rtn, "-UHD", " ");
        rtn = string::replace(rtn, "-HD", " ");
        rtn = string::replace(rtn, "_ICON", " ");
        rtn = string::replace(rtn, ".PNG", " ");
        rtn = string::replace(rtn, "-", " ");
        rtn = string::replace(rtn, "_", " ");
        rtn = string::replace(rtn, "   ", "");
        rtn = string::replace(rtn, "  ", "");
        return (rtn);
    }
};

#include  <random>
#include  <iterator>
namespace geode::utils {
    template<typename Iter, typename RandomGenerator>
    Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    };
    template<typename Iter>
    Iter select_randomly(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return select_randomly(start, end, gen);
    }
    bool rndb(int rarity = 1) {
        auto varsVec = std::vector<bool>();
        varsVec.push_back(true);
        while (rarity > 0) {
            rarity = rarity - 1;
            varsVec.push_back(false);
        }
        auto rtn = *select_randomly(varsVec.begin(), varsVec.end());
        //log::debug("{}({}) = {} of {}", __func__, variants, rtn, varsVec);
        return rtn;
    }
};

struct GJScoreKey {
    inline static auto TotalJumps = "1";
    inline static auto TotalAttempts = "2";
    inline static auto CompletedLevels = "3";
    inline static auto CompletedOnlineLevels = "4";
    inline static auto Demons = "5";
    inline static auto Stars = "6";
    inline static auto CompletedMapPacksCount = "7";
    inline static auto GoldCoins = "8";
    inline static auto PlayersDestroyed = "9";
    inline static auto LikedLevelsCount = "10";
    inline static auto RatedLevelsCount = "11";
    inline static auto UserCoins = "12";
    inline static auto Diamonds = "13";
    inline static auto CurrentOrbs = "14";
    inline static auto DailyCompletionCount = "15";
    inline static auto FireShards = "16";
    inline static auto IceShards = "17";
    inline static auto PoisonShards = "18";
    inline static auto ShadowShards = "19";
    inline static auto LavaShards = "20";
    inline static auto DemonKeys = "21";
    inline static auto TotalOrbs = "22";
    inline static auto EarthShards = "23";
    inline static auto BloodShards = "24";
    inline static auto MetalShards = "25";
    inline static auto LightShards = "26";
    inline static auto SoulShards = "27";
    inline static auto Moons = "28";
    inline static auto DiamondShards = "29";
    inline static auto FirePathStarProgress = "30";
    inline static auto IcePathStarProgress = "31";
    inline static auto PoisonPathStarProgress = "32";
    inline static auto ShadowPathStarProgress = "33";
    inline static auto LavaPathStarProgress = "34";
    inline static auto EarthPathStarProgress = "35";
    inline static auto BloodPathStarProgress = "36";
    inline static auto MetalPathStarProgress = "37";
    inline static auto LightPathStarProgress = "38";
    inline static auto SoulPathStarProgress = "39";
    inline static auto CompletedGauntlets = "40";
    inline static auto ListRewardsCollected = "41";
};

#ifdef GEODE_IS_ANDROID
#define debug error
#endif // GEODE_IS_ANDROID