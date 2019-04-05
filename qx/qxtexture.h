//
// Created by Sturmfy on 2019/3/23.
//

#ifndef HELLOCLION_QXTEXTURE_H
#define HELLOCLION_QXTEXTURE_H

#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "qxshader.h"

/**
 * QX Texture
 * @description
 *  - Load Texture from file
 *  - Help to use
 */

class QX_Texture {
public:
    // Attributes Field
    std::string name;
    int width, height, nChannels;
    unsigned int ID;

    QX_Texture(std::string name, const char* path)
    {
        this->name = name;
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);
        if(data)
        {
            if(nChannels == 3) // RBG
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else if (nChannels == 4)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            else
            {
                std::cout << "QX::TEX::LOAD::UNSUPPORTED CHANNEL AMOUNT" << std::endl;
                exit(-1);
            }
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "QX::TEX::LOAD::FAILED TO LOAD IMAGE" << std::endl;
        }
        stbi_image_free(data);
    }

    void bind_texture(QX_Shader* shader, GLenum loc)
    {
        // bind this texture to the target location
        glActiveTexture(loc);
        glBindTexture(GL_TEXTURE_2D, ID);

        // set the corresponding uniform variable
        int idx = loc - GL_TEXTURE0;
        name_buffer.str("");
        name_buffer << "texture" << idx;
        std::string name = name_buffer.str();
        shader->set_int(name.c_str(), idx);
    }
private:
    std::stringstream name_buffer;
};


#endif //HELLOCLION_QXTEXTURE_H
