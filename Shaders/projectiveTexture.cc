#include "renderState.h"
#include "ProjectiveTexture.h"
#include "textureManager.h"

ProjectiveTexture::ProjectiveTexture(const std::string &tex_name, Camera *cam) : 
    tex(TextureManager::instance()->createProj(tex_name)),
    cam(cam) {}

Texture *ProjectiveTexture::getTexture() {
    return tex;
}

Trfm3D &ProjectiveTexture::getMatrix() {
    return *trf;
}

void ProjectiveTexture::PlaceScene() {
    
}


