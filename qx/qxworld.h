//
// Created by Sturmfy on 2019/3/23.
//

#ifndef HELLOCLION_QXWORLD_H
#define HELLOCLION_QXWORLD_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "qxobject.h"
#include "qxshader.h"
#include "qxtexture.h"
#include "qxcamera.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * QX World
 * @description
 *  - Manage Objects
 *  - Manage Textures
 *  - Manage Shaders
 *  - Event Loop
 *  - Rendering
 */

class QX_World {
public:
    std::string name;
    int width, height;
    std::string msg;
    QX_Camera camera;
    GLFWwindow* main_window;

    QX_World(std::string name, int width, int height)
        :name(name), width(width), height(height),
         camera(glm::vec3(60, 30, 80),
                   glm::vec3(0, 0, -1),
                   glm::vec3(0, 1, 0),
                   45.0f, width, height, 0.1f, 100.0f)
    {
        glfwSetErrorCallback(this->error_cb);
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        main_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
        if (main_window == NULL)
        {
            msg = "QX::WORLD::INIT\tFailed to create GLFW window";
            error_exit();
        }
        glfwMakeContextCurrent(main_window);

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            msg = "QX::WORLD::INIT\tFailed to initialize GLAD";
            error_exit();
        }

        glViewport(0, 0, width, height);

        // features
        glEnable(GL_DEPTH_TEST);

        // call backs
        gui_render = nullptr;
        input_pro = this->input_basic;
        set_frame_buffer_size_cb(this->fbs_cb);
    }

    bool add_object(QX_Object* object)
    {
        if(!object) return false;
        // check if name or ID existed
        bool checker = contains<std::string, QX_Object>(objects, object->name) &&
                contains<unsigned int, QX_Object>(id_objects, object->VA_ID);
        if(checker) // exist such an object
            return false;
        objects[object->name] = object;
        id_objects[object->VA_ID] = object;
        return true;
    }

    bool add_texture(QX_Texture* texture)
    {
        if(!texture) return false;
        // check if name or ID existed
        bool checker = contains<std::string, QX_Texture>(textures, texture->name) &&
                contains<unsigned int, QX_Texture>(id_textures, texture->ID);
        if(checker)
            return false;
        textures[texture->name] = texture;
        id_textures[texture->ID] = texture;
        return true;
    }

    bool add_shader(QX_Shader* shader)
    {
        if(!shader) return false;
        bool checker = contains<std::string, QX_Shader>(shaders, shader->name) &&
                       contains<unsigned int, QX_Shader>(id_shaders, shader->ID);
        if(checker)
            return false;
        shaders[shader->name] = shader;
        id_shaders[shader->ID] = shader;
        return true;
    }

    QX_Object* get_object(std::string name)
    {
        return get<std::string, QX_Object>(objects, name);
    }

    QX_Object* get_object(unsigned int id)
    {
        return get<unsigned int, QX_Object>(id_objects, id);
    }

    QX_Texture* get_texture(std::string name)
    {
        return get<std::string, QX_Texture>(textures, name);
    }

    QX_Texture* get_texture(unsigned int id)
    {
        return get<unsigned int, QX_Texture>(id_textures, id);
    }

    QX_Shader* get_shader(std::string name)
    {
        return get<std::string, QX_Shader>(shaders, name);
    }

    QX_Shader* get_shader(unsigned int id)
    {
        return get<unsigned int, QX_Shader>(id_shaders, id);
    }

    void remove_object(std::string name)
    {
        QX_Object* tar = get_object(name);
        if(tar)
        {
            remove<std::string, QX_Object>(objects, tar->name);
            remove<unsigned int, QX_Object>(id_objects, tar->VA_ID);
        }
    }

    void remove_object(unsigned int id)
    {
        QX_Object* tar = get_object(id);
        if(tar)
        {
            remove<std::string, QX_Object>(objects, tar->name);
            remove<unsigned int, QX_Object>(id_objects, tar->VA_ID);
        }
    }

    void set_frame_buffer_size_cb(GLFWframebuffersizefun func)
    {
        glfwSetFramebufferSizeCallback(main_window, func);
    }

    void set_input_processor(void (*func)(GLFWwindow*))
    {
        input_pro = func;
    }

    void render_gui(void(*render_func)(QX_World*))
    {
        this->gui_render = render_func;
    }

    // event functions
    void event_loop()
    {
        // process inputs
        if(input_pro)
            input_pro(main_window);

        // do the rendering
        render();

        // render gui
        if(gui_render)
            gui_render(this);

        // back wards
        glfwSwapBuffers(main_window);
        glfwPollEvents();
    }


    // other
    void error_exit()
    {
        delete this;
        exit(-1);
    }

    bool should_close()
    {
        return glfwWindowShouldClose(main_window);
    }

    ~QX_World()
    {
        if(msg == "")
            msg = "TERMINATE::CLEAR";
        std::cout << "QX::WORLD::" << msg << std::endl;
        glfwTerminate();
        // delete all the data
        clear_data<QX_Texture>(id_textures);
        clear_data<QX_Shader>(id_shaders);
        clear_data<QX_Object>(id_objects);

        // main window ignored
    }

private:
    void (*input_pro)(GLFWwindow*);
    void (*gui_render)(QX_World*);

    // Get by name
    std::map<std::string, QX_Texture*> textures;
    std::map<std::string, QX_Shader*> shaders;
    std::map<std::string, QX_Object*> objects;

    // Get by ID
    std::map<unsigned int, QX_Texture*> id_textures;
    std::map<unsigned int, QX_Shader*> id_shaders;
    std::map<unsigned int, QX_Object*> id_objects;

    // rendering loop
    void render()
    {
        // settings
        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // clear buffer bits
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render each object
        std::map<unsigned int, QX_Object*>::iterator iter;
        for(iter = id_objects.begin(); iter != id_objects.end(); iter++)
        {
            if(iter->second)
            {
                iter->second->render(camera.matrix());
            }
        }

    }

    // cb_func
    static void fbs_cb(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    static void input_basic(GLFWwindow* window)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    static void error_cb(int error, const char* desc)
    {
        std::cerr << "GLFW::ERROR::" << error << "::" << desc;
    }

    // helper func
    template<typename T>
    void clear_data(std::map<unsigned int, T*> tar)
    {
        typename std::map<unsigned int, T*>::iterator st_iter;
        for(st_iter = tar.begin(); st_iter != tar.end(); st_iter++)
        {
            if(st_iter->second)
            {
                delete st_iter->second;
                st_iter->second = nullptr;
            }
        }
    }

    template <typename T, typename K>
    bool contains(std::map<T, K*> tar, T key)
    {
        typename std::map<T, K*>::iterator iter;
        iter = tar.find(key);
        return iter != tar.end();
    }

    template <typename T, typename K>
    K* get(std::map<T, K*> tar, T key)
    {
        typename std::map<T, K*>::iterator iter;
        iter = tar.find(key);
        if(iter == tar.end())
            return nullptr;
        return iter->second;
    }

    template <typename T, typename K>
    void remove(std::map<T, K*> tar, T key)
    {
        typename std::map<T, K*>::iterator iter;
        iter = tar.find(key);
        if(iter == tar.end())
            return;
        tar.erase(iter);
    }
};


#endif //HELLOCLION_QXWORLD_H
