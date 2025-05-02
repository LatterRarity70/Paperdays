#pragma once

#include <Geode/modify/GManager.hpp>
class $modify(ResourcesLoader, GManager) {
	$override void setup() {

        std::filesystem::path resourcesDir = getMod()->getResourcesDir();
        for (auto& entry : fs::glob::glob(resourcesDir.string() + "/*")) {
            std::string originalPath = entry.string();
            std::string fileName = entry.filename().string();
            std::string newPathStr = originalPath; //initialize new path as original one

            if (originalPath.find("[.!") == std::string::npos) continue; //process only if path contains "[.!"
            if (!entry.has_filename()) continue; //skip directories

            //if the tag [.!SAVE_DIR] exists
            if (newPathStr.find("[.!SAVE_DIR]") != std::string::npos) {
                //remove tag and set the save path as the base one
                fileName = newPathStr.substr(newPathStr.find("[.!SAVE_DIR]") + std::string("[.!SAVE_DIR]").length());
                newPathStr = dirs::getSaveDir().string() + "/" + fileName;
            }

            //tag [.!SUB_DIR] is present in the original path
            if (newPathStr.find("[.!SUB_DIR]") != std::string::npos) {
                //replace all occurrences of [.!SUB_DIR] with '/'
                size_t pos; while ((pos = newPathStr.find("[.!SUB_DIR]")) != std::string::npos) {
                    newPathStr.replace(pos, std::string("[.!SUB_DIR]").length(), "/");
                }
            }

            std::filesystem::path newPath = newPathStr;
            std::error_code ec;

            //create directories if they do not exist
            std::filesystem::create_directories(newPath.parent_path(), ec);
            if (ec) {
                log::error("create_directories err at {}: {}", newPath.string(), ec.message());
                continue;
            }
            //move file
            std::filesystem::rename(entry, newPath, ec);
            if (ec) {
                log::error("rename err of {} to {}: {}", entry.string(), newPath.string(), ec.message());
            }
            else {
                //log::debug("renamed: \n{}\n v v v \n{}", entry.string(), newPath.string());
            }
        }

		CCFileUtils::sharedFileUtils()->addPriorityPath(
			getMod()->getResourcesDir().string().c_str()
		);

		GManager::setup();
        
	}
};

#include <Geode/modify/MusicDownloadManager.hpp>
class $modify(MusicDownloadManagerExt, MusicDownloadManager) {
    gd::string pathForSFX(int p0) {
        if (fileExistsInSearchPaths(fmt::format("sfx.{}.mp3", p0).c_str())) return CCFileUtils::get(
        )->fullPathForFilename(fmt::format("sfx.{}.mp3", p0).c_str(), false);
        if (fileExistsInSearchPaths(fmt::format("sfx.{}.ogg", p0).c_str())) return CCFileUtils::get(
        )->fullPathForFilename(fmt::format("sfx.{}.ogg", p0).c_str(), false);
        return MusicDownloadManager::pathForSFX(p0);
    };
    gd::string pathForSong(int p0) {
        if (fileExistsInSearchPaths(fmt::format("music.{}.mp3", p0).c_str())) return CCFileUtils::get(
        )->fullPathForFilename(fmt::format("music.{}.mp3", p0).c_str(), false);
        if (fileExistsInSearchPaths(fmt::format("music.{}.ogg", p0).c_str())) return CCFileUtils::get(
        )->fullPathForFilename(fmt::format("music.{}.ogg", p0).c_str(), false);
        return MusicDownloadManager::pathForSong(p0);
    };
};

#include <Geode/modify/CCSpriteFrameCache.hpp>
class $modify(CCSpriteFrameCacheExt, CCSpriteFrameCache) {
	CCSpriteFrame* spriteFrameByName(const char* pszName) {
        std::string name = pszName;
        //sprites at this mod id (id.asd/pszName ? rtn(id.asd/pszName))
        {
            auto frameAtSprExtName = (Mod::get()->getID() + "/" + name);
            auto test = CCSpriteFrameCache::get()->m_pSpriteFrames->objectForKey(frameAtSprExtName);
            name = test ? frameAtSprExtName.data() : name.c_str();
        };
        //kill yourself chain
		if (string::contains(name, "chain_01")) name = "emptyFrame.png";
        //subs? (aka "geode.loader/penis.png")
        if (name.find("/") != std::string::npos) {
            auto test_name = Mod::get()->getID() + "/" + string::replace(name, "/", "[.!SUB_DIR]");
            auto test = CCSpriteFrameCache::get()->m_pSpriteFrames->objectForKey(test_name);
            name = test ? test_name.data() : name.c_str();
        }
		return CCSpriteFrameCache::spriteFrameByName(name.c_str());
	};
};
