// -*-C++-*-
#pragma once

#include <string>
#include "gObject.h"
#include "node.h"
#include "camera.h"
#include "shader.h"


// Create a new Skybox Node
void CreateSkybox(GObject *gobj,
				  ShaderProgram *skyShader,
				  const std::string &dirname);
// Display Skybox Node
void DisplaySky(Camera *cam);
