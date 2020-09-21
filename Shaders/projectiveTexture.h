#pragma once

#include "camera.h"
#include "texture.h"
#include "trfm3D.h"
#include "string"

class ProjectiveTexture {
    public:
        ProjectiveTexture(const std::string &tex_name, Camera *cam);
    private:
        Camera *cam;
        Texture *tex;
        Trfm3D *trf;

        Trfm3D &getMatrix();
        Texture *getTexture();
        void PlaceScene();
};