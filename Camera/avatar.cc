#include "tools.h"
#include "avatar.h"
#include "scene.h"

Avatar::Avatar(const std::string &name, Camera * cam, float radius) :
	m_name(name), m_cam(cam), m_walk(false) {
	Vector3 P = cam->getPosition();
	m_bsph = new BSphere(P, radius);
}

Avatar::~Avatar() {
	delete m_bsph;
}

bool Avatar::walkOrFly(bool walkOrFly) {
	bool walk = m_walk;
	m_walk = walkOrFly;
	return walk;
}

//
// AdvanceAvatar: advance 'step' units
//
// @@ TODO: Change function to check for collisions. If the destination of
// avatar collides with scene, do nothing.
//
// Return: true if the avatar moved, false if not.

bool Avatar::advance(float step) {
	bool ret = true;
	Node *rootNode = Scene::instance()->rootNode(); // root node of scene

	if (m_walk) m_cam->walk(step);
	else m_cam->fly(step);
	
	m_bsph->setPosition(m_cam->getPosition());
	if (rootNode->checkCollision(m_bsph) != 0) { // collision detected
		if (m_walk) m_cam->walk(-step);
		else m_cam->fly(-step);
		
		m_bsph->setPosition(m_cam->getPosition());
		ret = false;
	}

	return ret;
}

void Avatar::leftRight(float angle) {
	if (m_walk)
		m_cam->viewYWorld(angle);
	else
		m_cam->yaw(angle);
}

void Avatar::upDown(float angle) {
	m_cam->pitch(angle);
}

void Avatar::print() const { }
