#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

//WIP
class ViewportNode : public CCNode {
public:
	CREATE_FUNC(ViewportNode);
	static auto create(const CCSize& size) {
		auto rtn = create();
		if(rtn) rtn->setContentSize(size);
		return rtn;
	}
	bool init() {
		if (!CCNode::init()) return false;
		this->scheduleUpdate();
		return true;
	}
	void visit() {
		if (!isVisible()) return;

		auto _clipRect = boundingBox();
		_clipRect.size = getContentSize();
		_clipRect.origin = this->convertToWorldSpace(this->getPosition());

		glEnable(GL_SCISSOR_TEST);

		CCDirector::get()->getOpenGLView()->setScissorInPoints(
			_clipRect.origin.x, _clipRect.origin.y, _clipRect.size.width, _clipRect.size.height
		);

		CCNode::visit();

		glDisable(GL_SCISSOR_TEST);
	}
	void update(float dt) {
	}
};
