//
// Created by Sturmfy on 2019/3/23.
//

#ifndef HELLOCLION_QXOBJECT_H
#define HELLOCLION_QXOBJECT_H

#include <glm/gtc/matrix_transform.hpp>
#include "qxtexture.h"
#include "qxshader.h"
#include <GLFW/glfw3.h>

/**
 * QX Object
 * @description
 *  - Store the vertices, texture's coordinates & the defined colors
 *  - Record the indices of Textures & Shaders
 *  - Hint the rendering details
 */

class QX_Object {
private: // static parameters
    static const int unit_len = 8;      // 8 float to describe a vertex
    static const int loc_offset = 0;    // location offset in the data
    static const int col_offset = 3;    // color offset
    static const int tex_offset = 6;    // texture's coordinates' offset
public:
    std::string name;

    unsigned int VB_ID; // vertices buffer ID
    unsigned int EB_ID; // elements buffer ID
    unsigned int VA_ID; // vertices attributes ID

    GLenum DRAW_MODE = GL_STATIC_DRAW;

    glm::mat4 model_matrix;

    QX_Object(std::string name,
            float* vertices, float* colors, float* coords,
            int v_amount, GLenum draw_mode = GL_STATIC_DRAW,
            unsigned int* elements = nullptr, int e_amount = 0)
    {
        // pre process
        this->name = name;
        model_matrix = glm::mat4(1.0f);

        if(v_amount < 0) v_amount = 0;
        point_amount = v_amount;
        this->DRAW_MODE = draw_mode;

        if(e_amount < 0) e_amount = 0;
        element_amount = e_amount;

        // try to load from the data
        // the arrangement is :
        //  - x, y, z, r, g, b, t_s, t_t
        raw_data = new float[point_amount * unit_len];
        raw_element = new unsigned int[element_amount];
        set_vertices(vertices);
        set_colors_rgb(colors);
        if(coords)
            set_tex_coords(coords);
        set_elements(elements);


        // set the openGL settings
        glGenBuffers(1, &VB_ID);
        glGenBuffers(1, &EB_ID);
        glGenVertexArrays(1, &VA_ID);
        glBindVertexArray(VA_ID);
        // copy the vertices buffer
        glBindBuffer(GL_ARRAY_BUFFER, VB_ID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * point_amount * unit_len, raw_data, DRAW_MODE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EB_ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * element_amount, raw_element, DRAW_MODE);
        // set the pointer attribute
        // location
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, unit_len*sizeof(float), (void*)(loc_offset * sizeof(float)));
        glEnableVertexAttribArray(0);
        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, unit_len*sizeof(float), (void*)(col_offset * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Text
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, unit_len*sizeof(float), (void*)(tex_offset * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    QX_Object(std::string name, const QX_Object* tar)
    {
        this->name = name;
        this->model_matrix = glm::mat4(1.0f);

        point_amount = tar->point_amount;
        DRAW_MODE = tar->DRAW_MODE;

        element_amount = tar->element_amount;

        // try to load from the data
        raw_data = new float[point_amount * unit_len];
        raw_element = new unsigned int[element_amount];
        memcpy(raw_data, tar->raw_data, sizeof(float) * point_amount * unit_len);
        memcpy(raw_element, tar->raw_element, sizeof(unsigned int) * element_amount);

        // set the openGL settings
        glGenBuffers(1, &VB_ID);
        glGenBuffers(1, &EB_ID);
        glGenVertexArrays(1, &VA_ID);
        glBindVertexArray(VA_ID);
        // copy the vertices buffer
        glBindBuffer(GL_ARRAY_BUFFER, VB_ID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * point_amount * unit_len, raw_data, DRAW_MODE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EB_ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * element_amount, raw_element, DRAW_MODE);
        // set the pointer attribute
        // location
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, unit_len*sizeof(float), (void*)(loc_offset * sizeof(float)));
        glEnableVertexAttribArray(0);
        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, unit_len*sizeof(float), (void*)(col_offset * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Text
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, unit_len*sizeof(float), (void*)(tex_offset * sizeof(float)));
        glEnableVertexAttribArray(2);

        // others
        texture = tar->texture;
        shader = tar->shader;
        draw_elements = tar->draw_elements;
        use_textures = tar->use_textures;
        disabled = tar->disabled;
    }

    QX_Object* duplicate(std::string name)
    {
        QX_Object* copy = new QX_Object();
        // pre process
        copy->name = name;
        copy->model_matrix = glm::mat4(1.0f);

        copy->point_amount = this->point_amount;
        copy->DRAW_MODE = this->DRAW_MODE;

        copy->element_amount = this->element_amount;

        // try to load from the data
        copy->raw_data = new float[point_amount * unit_len];
        copy->raw_element = new unsigned int[element_amount];
        memcpy(copy->raw_data, this->raw_data, sizeof(float) * point_amount * unit_len);
        memcpy(copy->raw_element, this->raw_element, sizeof(unsigned int) * element_amount);

        // set the openGL settings
        glGenBuffers(1, &copy->VB_ID);
        glGenBuffers(1, &copy->EB_ID);
        glGenVertexArrays(1, &copy->VA_ID);
        glBindVertexArray(copy->VA_ID);
        // copy the vertices buffer
        glBindBuffer(GL_ARRAY_BUFFER, copy->VB_ID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * point_amount * unit_len, copy->raw_data, DRAW_MODE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, copy->EB_ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * element_amount, copy->raw_element, DRAW_MODE);
        // set the pointer attribute
        // location
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, unit_len*sizeof(float), (void*)(loc_offset * sizeof(float)));
        glEnableVertexAttribArray(0);
        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, unit_len*sizeof(float), (void*)(col_offset * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Text
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, unit_len*sizeof(float), (void*)(tex_offset * sizeof(float)));
        glEnableVertexAttribArray(2);

        // others
        copy->texture = this->texture;
        copy->shader = this->shader;
        copy->draw_elements = this->draw_elements;
        copy->use_textures = this->use_textures;
        copy->disabled = this->disabled;

        return copy;
    }


    void set_vertices(float* vertices, int v_amount = -1)
    {
        set_data(vertices, v_amount, loc_offset, 3);
    }

    void set_colors_rgb(float* colors, int v_amount = -1)
    {
        set_data(colors, v_amount, col_offset, 3);
        glBindVertexArray(VA_ID);
        glBindBuffer(GL_ARRAY_BUFFER, VB_ID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * point_amount * unit_len, raw_data);
    }

    virtual void set_colors_rgb(float r, float g, float b, int v_amount = -1)
    {
        float rgb[3] = {r, g, b};
        form_data(rgb, v_amount, col_offset, 3);
        glBindVertexArray(VA_ID);
        glBindBuffer(GL_ARRAY_BUFFER, VB_ID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * point_amount * unit_len, raw_data);
    }

    void set_tex_coords(float* coord, int v_amount = -1)
    {
        set_data(coord, v_amount, tex_offset, 2);
    }

    void set_elements(unsigned int* indices, int e_amount = -1)
    {
        if(e_amount < 0 || e_amount > element_amount)
            e_amount = element_amount;

        for(unsigned i = 0; i < e_amount; i++)
        {
            raw_element[i] = indices[i];
        }
    }

    void ref_texture(QX_Texture* tex)
    {
        texture = tex;
        use_textures = true;
    }

    void ref_shader(QX_Shader* shader)
    {
        this->shader = shader;
    }

    void render_draw_elements(bool drawElementsOrNot)
    {
        if(drawElementsOrNot == this->draw_elements)
            return;
        if(drawElementsOrNot && element_amount <= 0)
            return;
        this->draw_elements = drawElementsOrNot;
    }

    void render_use_textures(bool useTextureOrNot)
    {
        if(texture)
            use_textures = useTextureOrNot;
    }

    QX_Object* scale(float x, float y, float z)
    {
        model_matrix = glm::scale(model_matrix, glm::vec3(x, y, z));
        return this;
    }

    QX_Object* rotate(float radian, float x, float y, float z)
    {
        model_matrix = glm::rotate(model_matrix, radian, glm::vec3(x, y, z));
        return this;
    }

    QX_Object* translate(float x, float y, float z)
    {
        model_matrix = glm::translate(model_matrix, glm::vec3(x, y, z));
        return this;
    }

    QX_Object* identity()
    {
        model_matrix = glm::mat4(1.0f);
        return this;
    }

    void disable()
    {
        disabled = true;
    }

    void enable()
    {
        disabled = false;
    }

    bool enabled()
    {
        return !disabled;
    }

    virtual void render(glm::mat4& global_matrix)
    {
        if(disabled) return;
        // prepare
        if(!shader)
        {
            std::cout << "QX::OBJECT::RENDER::SHADER INVALID" << " - Name: " << name << std::endl;
            return;
        }
        shader->use();
        if(texture && use_textures)
        {
            texture->bind_texture(shader, GL_TEXTURE0);
        }
        shader->set_bool("useTexture", use_textures);
        // matrix trying
        shader->set_mat4("viewTransform", global_matrix);
        shader->set_mat4("modelTransform", model_matrix);
        // draw
        glBindVertexArray(VA_ID);
        if(draw_elements)
            glDrawElements(GL_TRIANGLES, element_amount, GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, point_amount);
        glBindVertexArray(0);   // unbind
    }

    //
    ~QX_Object()
    {
        if(raw_data) delete[] raw_data;
        if(raw_element) delete[] raw_element;
    }
private:
    QX_Object(){}
    //
    int point_amount = 0;
    float* raw_data = nullptr;
    int element_amount = 0;
    unsigned int* raw_element = nullptr;
    QX_Texture* texture = nullptr;
    QX_Shader* shader = nullptr;
    bool draw_elements = false;
    bool use_textures = false;
    bool disabled = false;
    //
    void set_data(float* origin, int amount, int offset, int dim)
    {
        if(amount < 0 || amount > point_amount)
            amount = point_amount;

        for(unsigned i = 0; i < amount; i++)
        {
            unsigned base_i = i * unit_len + offset;
            unsigned vi = i * dim;
            for(int j = 0; j < dim; j++)
            {
                raw_data[base_i] = origin[vi];
                base_i++;vi++;
            }
        }
    }

    void form_data(float* origin, int amount, int offset, int dim)
    {
        if(amount < 0 || amount > point_amount)
            amount = point_amount;

        for(unsigned i = 0; i < amount; i++)
        {
            unsigned base_i = i * unit_len + offset;
            for(int j = 0; j < dim; j++)
            {
                raw_data[base_i] = origin[j];
                base_i++;
            }
        }
    }

    void print(float* vs, int amount)
    {
        for(int i = 0; i < amount; i++)
        {
            std::cout << vs[i] << '\t';
        }
        std::cout << std::endl;
    }
};


#endif //HELLOCLION_QXOBJECT_H
