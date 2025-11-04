#ifndef TEXTURE2D_H 
#define TEXTURE2D_H

#include <iostream>
#include "../ew/external/glad.h"
#include "../ew/external/stb_image.h"

namespace shaderz {
    class Texture2D
    {
    public:
        Texture2D(const char* filePath, int filterMode, int wrapMode);
        ~Texture2D();
        void Bind(unsigned int slot = 0); //Bind to a specific texture unit
    private:
        unsigned int m_id; //GL texture handle
        int m_width, m_height, m_nrChannels;
    };
}
#endif