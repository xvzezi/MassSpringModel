//
// Created by xvzezi on 2019/4/7.
//

#ifndef MASSSPRINGMODEL_TANGIBLE_H
#define MASSSPRINGMODEL_TANGIBLE_H

#include "qx/qxobject.h"

class Tangible : public QX_Object
{
public:
    Tangible(std::string name, QX_Object* prim)
    :QX_Object(name, prim)
    {}

    void set_bounding_ball(float center_x, float center_y, float center_z, float radius)
    {
        this->set_bounding_box(center_x, center_y, center_z, radius);
    }

    /********************************************
    * Usable Functions of mouse click
    ********************************************
    * */
    virtual void mouse_select_callback(int button){}
    virtual void mouse_release_callback(int button){}
    virtual void mouse_drag_callback(int button, float dx_3d, float dy_3d, float dz_3d){}


    /********************************************
     * Helper Funcs of mouse click
     ********************************************
     * */
    void mouse_select(int button) final
    {
        // try to init all the things that should be done
        mouse_select_callback(button);
    }
    void mouse_drag(int button, QX_Camera* camera, glm::vec3 mouse, glm::vec3 old, glm::vec3& out)final
    {
        glm::vec3 dir = camera->dir();
        dir = glm::normalize(dir);
        float surface_const = glm::dot(old, dir);
        float diss = glm::dot(camera->transform(), dir);
        float res_const = surface_const - diss;
        glm::vec3 new_dir = mouse - camera->transform();
        float t = res_const / glm::dot(new_dir, dir);

        glm::vec3 inter = camera->transform() + t * new_dir;

        out = inter;

        glm::vec3 delta = inter - old;
        // call back
        mouse_drag_callback(button, delta.x, delta.y, delta.z);
    }
    void mouse_release(int button)final
    {
        // try to init all the things that should be done
        mouse_release_callback(button);
    }
    bool intersect(glm::vec3 camera, glm::vec3 mouse, glm::vec3& out)final
    {
        bounding_box_update();
        // process as a ball
        glm::vec3 dir = mouse - camera;
        dir = glm::normalize(dir);
        glm::vec3 bb_origin = box_cur_origin;  // the new origin point
        float surface_const = glm::dot(bb_origin, dir);
        float diss = glm::dot(camera, dir);
        float t = surface_const - diss;
        glm::vec3 inter = camera + t * dir;
        glm::vec3 dis = bb_origin - inter;
        float distance = glm::length(dis);

        // try to calculate the intersection
        float real_len = box_cur_length;

        out = inter;

        return distance < real_len;
    }

private:
};

/*
        glm::vec3 bb0 = box_origin, bb1 = box_origin, bb2 = box_origin;
        bb0.x += box_length; bb1.y += box_length; bb2.z += box_length;
        glm::vec3 bb_origin = model_matrix * glm::vec4(box_origin, 1);
        bb0 = model_matrix * glm::vec4(bb0, 1);
        bb1 = model_matrix * glm::vec4(bb1, 1);
        bb2 = model_matrix * glm::vec4(bb2, 1);
        bb0 -= bb_origin;
        bb1 -= bb_origin;
        bb2 -= bb_origin;



 */

#endif //MASSSPRINGMODEL_TANGIBLE_H
