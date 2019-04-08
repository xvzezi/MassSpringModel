//
// Created by xvzezi on 2019/4/7.
//

#ifndef MASSSPRINGMODEL_MASSSPRINGOBJECT_H
#define MASSSPRINGMODEL_MASSSPRINGOBJECT_H

#include "qx/qxobject.h"

class MyObject : public QX_Object
{
public:
    MyObject(std::string name, QX_Object* prim)
    :QX_Object(name, prim)
    {}


    void mouse_select(int button)
    {
        std::cout << "selected" << std::endl;
        // try to init all the things that should be done
    }
    void mouse_drag(int button, QX_Camera* camera, glm::vec3 mouse, glm::vec3 old, glm::vec3& out)
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
        x += delta.x;
        y += delta.y;
//        z += delta.z;
        this->identity()->translate(x, y, 0);
        std::cout << x << ' ' << y << ' '<< z << std::endl;
    }
    void mouse_release(int button)
    {
        std::cout << "released" << std::endl;
        // try to init all the things that should be done
    }

    bool intersect(glm::vec3 camera, glm::vec3 mouse, glm::vec3& out)
    {
        // process as a ball
        glm::vec3 dir = mouse - camera;
        dir = glm::normalize(dir);
        glm::vec3 bb_origin = model_matrix * glm::vec4(box_origin, 1);  // the new origin point
        float surface_const = glm::dot(bb_origin, dir);
        float diss = glm::dot(camera, dir);
        float t = surface_const - diss;
        glm::vec3 inter = camera + t * dir;
        glm::vec3 dis = bb_origin - inter;
        float distance = glm::length(dis);

        // try to calculate the intersection
        glm::vec3 bb0 = box_origin;
        bb0.x += box_length;
        bb0 = model_matrix * glm::vec4(bb0, 1);
        float real_len = glm::length(bb0 - bb_origin);

        out = inter;

        return distance < real_len;
    }

private:
    float x = 0;
    float y = 0;
    float z = 0;
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

#endif //MASSSPRINGMODEL_MASSSPRINGOBJECT_H
