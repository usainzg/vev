#include <vector>
#include "skybox.h"
#include "tools.h"
#include "vector3.h"
#include "trfm3D.h"
#include "renderState.h"
#include "gObjectManager.h"
#include "nodeManager.h"
#include "textureManager.h"
#include "materialManager.h"
#include "shaderManager.h"


using std::vector;
using std::string;

// TODO: create skybox object given gobject, shader name of cubemap texture.
//
// This function does the following:
//
// - Create a new material.
// - Assign cubemap texture to material.
// - Assign material to geometry object gobj
// - Create a new Node.
// - Assign shader to node.
// - Assign geometry object to node.
// - Set sky node in RenderState.
//
// Parameters are:
//
//   - gobj: geometry object to which assign the new material (which incluides
//           cubemap texture)
//   - skyshader: The sky shader.
//   - ctexname: The name of the cubemap texture.
//
// Useful functions:
//
//  - MaterialManager::instance()->create(const std::string & matName): create a
//    new material with name matName (has to be unique).
//  - Material::setTexture(Texture *tex): assign texture to material.
//  - GObject::setMaterial(Material *mat): assign material to geometry object.
//  - NodeManager::instance()->create(const std::string &nodeName): create a new
//    node with name nodeName (has to be unique).
//  - Node::attachShader(ShaderProgram *theShader): attach shader to node.
//  - Node::attachGobject(GObject *gobj ): attach geometry object to node.
//  - RenderState::instance()->setSkybox(Node * skynode): Set sky node.

void CreateSkybox(GObject *gobj,
				  ShaderProgram *skyshader,
				  const std::string & ctexname) {
	if (!skyshader) {
		fprintf(stderr, "[E] Skybox: no sky shader\n");
		exit(1);
	}
	Texture *ctex = TextureManager::instance()->find(ctexname);
	if (!ctex) {
		fprintf(stderr, "[E] Cubemap texture '%s' not found\n", ctexname.c_str());
		exit(1);
	}
	/* =================== PUT YOUR CODE HERE ====================== */
	Material *mat = MaterialManager::instance()->create("skybox_mat");
	mat->setTexture(ctex);
	gobj->setMaterial(mat);
	Node *node = NodeManager::instance()->create("node_sky");
	node->attachShader(skyshader);
	node->attachGobject(gobj);
	RenderState::instance()->setSkybox(node);
	/* =================== END YOUR CODE HERE ====================== */
}

// TODO: display the skybox
//
// This function does the following:
//
// - Store previous shader
// - Move Skybox to camera location, so that it always surrounds camera.
// - Disable depth test.
// - Set skybox shader
// - Draw skybox object.
// - Restore depth test
// - Set previous shader
//
// Parameters are:
//
//   - cam: The camera to render from
//
// Useful functions:
//
// - RenderState::instance()->getShader: get current shader.
// - RenderState::instance()->setShader(ShaderProgram * shader): set shader.
// - RenderState::instance()->push(RenderState::modelview): push MODELVIEW
//   matrix.
// - RenderState::instance()->pop(RenderState::modelview): pop MODELVIEW matrix.
// - Node::getShader(): get shader attached to node.
// - Node::getGobject(): get geometry object from node.
// - GObject::draw(): draw geometry object.
// - glDisable(GL_DEPTH_TEST): disable depth testing.
// - glEnable(GL_DEPTH_TEST): disable depth testing.

void DisplaySky(Camera *cam) {

	RenderState *rs = RenderState::instance();

	Node *skynode = rs->getSkybox();
	if (!skynode) return;

	/* =================== PUT YOUR CODE HERE ====================== */
	
	// guardamos el shader actual
	ShaderProgram *prev_shader = rs->getShader();

	rs->push(RenderState::modelview);

	Trfm3D *tr = new Trfm3D();
	tr->setTrans(cam->getPosition());
	rs->addTrfm(RenderState::modelview, tr);

	// deshabilitamos depth test
	glDisable(GL_DEPTH_TEST);

	// seteamos el skybox como shader
	rs->setShader(skynode->getShader());
	
	// draw del skybox
	GObject *sky_obj = skynode->getGobject();
	sky_obj->draw();

	rs->pop(RenderState::modelview);
	
	// activamos gepth test
	glEnable(GL_DEPTH_TEST);
	
	// volvemos a poner el shader anterior
	if (prev_shader != 0) {
		// restore shader
		rs->setShader(prev_shader);
	}
	/* =================== END YOUR CODE HERE ====================== */
}
