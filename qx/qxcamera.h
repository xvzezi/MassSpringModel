//
// Created by xvzezi on 2019/3/26.
//

#ifndef HELLOCLION_QXCAMERA_H
#define HELLOCLION_QXCAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * QX Camera
 * @description:
 *  - Manage the view & perspective matrix
 *  - Help to do the matrix changing
 */

class QX_Camera
{
public:
    QX_Camera(glm::vec3 loc, glm::vec3 front, glm::vec3 up,
            float fov, int width, int height, float near_pl, float far_pl)
    {
        this->loc = loc;
        this->front = front;
        view = glm::lookAt(loc, loc + front, up);
//        view = glm::lookAt(loc, glm::vec3(0,0,0), up);
        projection = glm::perspective(glm::radians(fov), ((float)width) / height, near_pl, far_pl);
    }

    glm::mat4& matrix()
    {
        if(updated) update();
        return combine;
    }

    glm::mat4& inverse()
    {
        if(updated) update();
        return inv;
    }

    void update()
    {
        if(!updated) return;
        combine = projection * view;
        inv = glm::inverse(combine);
        updated = false;
    }

    glm::vec3& transform()
    {
        return loc;
    }

    glm::vec3& dir()
    {
        return front;
    }

    void unproject(double win_x, double win_y, double depth, glm::vec4 viewport, glm::vec3& store)
    {
        glm::vec4 tmp = glm::vec4(win_x, win_y, depth, 1);
        tmp.x = (tmp.x - viewport[0]) / viewport[2];
        tmp.y = (tmp.y - viewport[1]) / viewport[3];
#		if GLM_DEPTH_CLIP_SPACE == GLM_DEPTH_ZERO_TO_ONE
        tmp.x = tmp.x * 2 - 1;
			tmp.y = tmp.y * 2 - 1;
#		else
        tmp = tmp * float(2) - float(1);
#		endif

        glm::vec4 obj = inverse() * tmp;
        obj /= obj.w;

        store = glm::vec3(obj);
    }

    glm::vec3 unproject_device(glm::vec4 tmp)
    {
        glm::vec4 obj = inverse() * tmp;
        obj /= obj.w;

        return glm::vec3(obj);
    }

    glm::vec3 project(glm::vec3 coords)
    {
        glm::vec4 result = matrix() * glm::vec4(coords, 1.0);
        result /= result.w;
        return glm::vec3(result);
    }


private:
    glm::vec3 loc;
    glm::vec3 front;

    glm::mat4 view;
    glm::mat4 projection;

    bool updated = true;
    glm::mat4 combine;
    glm::mat4 inv;
};


#endif //HELLOCLION_QXCAMERA_H
