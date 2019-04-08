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
        input_pro = nullptr;
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
        input_basic();
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

    glm::vec4 get_viewport()
    {
        return glm::vec4(0, 0, width, height);
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

    void input_basic()
    {
        if(glfwGetKey(main_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(main_window, true);
        int mouse_state = glfwGetMouseButton(main_window, GLFW_MOUSE_BUTTON_LEFT);
        if(mouse_state == GLFW_PRESS)
            mouse_pressed(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
        else if (mouse_state == GLFW_RELEASE)
            mouse_released(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE);
    }

    static void error_cb(int error, const char* desc)
    {
        std::cerr << "GLFW::ERROR::" << error << "::" << desc;
    }

    // mouse state
    bool mouse_left_held = false;
    QX_Object* selected_object = nullptr;
    glm::vec3 mouse_3d_last;

    void print_vec3(glm::vec3& a, std::string name)
    {
        std::cout << name << ":\t";
        std::cout << a.x << ' ' << a.y << ' '<< a.z << ' ' << std::endl;
    }

    void mouse_pressed(int button, int action)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            // get the current window position
            double mouse_xpos, mouse_ypos;
            mouse_pos(mouse_xpos, mouse_ypos);
            glm::vec4 vp = get_viewport();
            if(mouse_left_held) // its not the first time, so call the target objects' func
            {
                if(selected_object)
                {
                    // calculate the movement
                    glm::vec3 mouse_3d, inter_pt;
                    camera.unproject(mouse_xpos, mouse_ypos, 1.0, vp, mouse_3d);
                    print_vec3(mouse_3d_last, "3d last");
                    selected_object->mouse_drag(button, &camera, mouse_3d, mouse_3d_last, inter_pt);
                    mouse_3d_last = inter_pt;
                    print_vec3(mouse_3d_last, "3d this");
                }
            }
            else    // the first time
            {
                mouse_left_held = true;
                // try to calculate the mouse's 3d position
                glm::vec3 mouse_3d, inter_pt;
                camera.unproject(mouse_xpos, mouse_ypos, 1.0, vp, mouse_3d);
                // try to iterate the objects and find if crossed
                for(auto data : objects)
                {
                    if(data.second->enabled() &&
                        data.second->intersect(camera.transform(), mouse_3d, inter_pt))
                    {
                        data.second->mouse_select(button);
                        selected_object = data.second;
                        print_vec3(inter_pt, "inter");
                        break;
                    }
                }
                // sth selected
                if(selected_object)
                {
                    mouse_3d_last = inter_pt;
                }
            }
        }
    }

    void mouse_released(int button, int action)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(mouse_left_held)
            {
                std::cout << "QX::WORLD::MOUSE::LEFT::RELEASE" << std::endl;
                if(selected_object)
                    selected_object->mouse_release(button);
                mouse_left_held = false;
                selected_object = nullptr;
            }
        }
    }

    void mouse_pos(double& xpos, double& ypos)
    {
        double x, y;
        glfwGetCursorPos(main_window, &x, &y);
        xpos = x;
        ypos = height - y - 1;
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
