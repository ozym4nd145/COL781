#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations
#include <learnopengl/mesh.h>

class FBO {
    private:
        unsigned int framebuffer;
        unsigned int texColorBuffer;
        unsigned int rbo;
        int screenX;
        int screenY;
    public:
        FBO(int screenX, int screenY);
        ~FBO();
        void setupFBO();
        Texture getTexture();
        void mount();
        void unmount();
};