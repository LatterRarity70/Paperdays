#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

//made for dynamic value adding and change, 
//see: 
//SahderLayer::addValueContainer(id)
//SahderLayer::getValueContainer(id)
class ShaderValueContainer : public CCLayer {
public:
	CREATE_FUNC(ShaderValueContainer);
};

class SahderLayer : public CCSprite {
public:
	Ref<CCGLProgram> m_shaderProgram;
	Ref<CCRenderTexture> m_renderTexture;
	Ref<ShaderValueContainer> m_mainValueContainer;
	float m_time;

	std::function<void()> m_onDrawBegin = []() {};
	std::function<void()> m_onDrawEnd = []() {};
	std::function<void()> m_onUpdateBegin = []() {};
	std::function<void()> m_onUpdateEnd = []() {};

	static SahderLayer* create(const char* vShaderFilename, const char* fShaderFilename) {
		SahderLayer* pRet = new SahderLayer();
		if (pRet && pRet->init(vShaderFilename, fShaderFilename))
		{
			pRet->autorelease(); return pRet;
		}
		else { delete pRet; pRet = 0; return 0; }
	};

	virtual bool init(const char* vShaderFilename, const char* fShaderFilename) {
		if (!CCSprite::init()) return false;

		m_mainValueContainer = ShaderValueContainer::create();

		m_renderTexture = CCRenderTexture::create(111, 111);

		std::filesystem::path vertexPath = (std::string)CCFileUtils::get()->fullPathForFilename(
			vShaderFilename, false
		);
		auto vertexSource = file::readString(vertexPath);
		if (!vertexSource) log::error(
			"failed to read vertex shader at path {}: {}",
			vertexPath.string(), vertexSource.unwrapErr()
		);

		std::filesystem::path fragmentPath = (std::string)CCFileUtils::get()->fullPathForFilename(
			fShaderFilename, false
		);
		auto fragmentSource = file::readString(fragmentPath);
		if (!fragmentSource) log::error(
			"failed to read fragment shader at path {}: {}", 
			fragmentPath.string(), fragmentSource.unwrapErr()
		);

		m_shaderProgram = new CCGLProgram();
		log::debug(
			"{}.shaderInitResult({}) = {}", __func__, fShaderFilename,
			m_shaderProgram->initWithVertexShaderByteArray(
				vertexSource.unwrap().c_str(), 
				fragmentSource.unwrap().c_str()
			)
		);
		m_shaderProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		m_shaderProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
		m_shaderProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
		m_shaderProgram->link();
		m_shaderProgram->updateUniforms();

		this->scheduleUpdate();

		return true;
	}

	virtual void update(float dt) {
		m_time += dt;

		m_onUpdateBegin();

		auto parent = this->getParent();
		if (!parent) return;

		if (parent->getContentSize() != m_renderTexture->getContentSize()) {
			m_renderTexture->initWithWidthAndHeight(
				parent->getContentSize().width,
				parent->getContentSize().height,
				kTexture2DPixelFormat_RGBA8888
			);
			m_renderTexture->setContentSize(parent->getContentSize());
		}

		bool myOldVisible = this->isVisible();
		this->setVisible(false);

		m_renderTexture->clear(0, 0, 0, 0);
		m_renderTexture->begin();
		parent->visit();
		m_renderTexture->end();

		this->setVisible(myOldVisible);

		this->initWithSpriteFrame(CCSpriteFrame::createWithTexture(
			m_renderTexture->getSprite()->getTexture(),
			parent->boundingBox()
		));
		this->setShaderProgram(m_shaderProgram);
		this->setFlipY(true);

		this->setPosition(ccp(parent->getContentSize().width / 2, parent->getContentSize().height / 2));

		findFirstChildRecursive<ShaderValueContainer>(
			this, [this](ShaderValueContainer* node) {
				node->setContentSize(this->boundingBox().size);
				return false;
			}
		);

		m_onUpdateEnd();
	}

	virtual void draw() {

		m_onDrawBegin();

		CCSprite::draw();
		CC_NODE_DRAW_SETUP();

		GLint timeLocation = glGetUniformLocation(m_shaderProgram->getProgram(), "time");
		glUniform1f(timeLocation, m_time);

		GLint u_timeLocation = glGetUniformLocation(m_shaderProgram->getProgram(), "u_time");
		glUniform1f(u_timeLocation, m_time);

		auto winSize = this->getParent() ? this->getParent()->getContentSize() : CCSizeMake(0, 0);

		GLint resolutionLoc = glGetUniformLocation(m_shaderProgram->getProgram(), "resolution");
		glUniform2f(resolutionLoc, winSize.width, winSize.height);

		GLint u_resolutionLoc = glGetUniformLocation(m_shaderProgram->getProgram(), "u_resolution");
		glUniform2f(u_resolutionLoc, winSize.width, winSize.height);

		m_onDrawEnd();

	}

	auto addValueContainer(std::string id) {
		auto newone = ShaderValueContainer::create();
		newone->setID(id);
		this->addChild(newone);
		return newone;
	}

	auto getValueContainer(std::string id) {
		auto casted = typeinfo_cast<ShaderValueContainer*>(this->getChildByID(id));
		return casted ? casted : m_mainValueContainer.data();
	}

};
