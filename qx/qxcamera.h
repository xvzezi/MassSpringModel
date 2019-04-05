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
        view = glm::lookAt(loc, loc + front, up);
//        view = glm::lookAt(loc, glm::vec3(0,0,0), up);
        projection = glm::perspective(glm::radians(fov), ((float)width) / height, near_pl, far_pl);
    }

    glm::mat4& matrix()
    {
        if(updated)
            combine = projection * view;
        return combine;
    }



private:
    glm::mat4 view;
    glm::mat4 projection;

    bool updated = true;
    glm::mat4 combine;
};


#endif //HELLOCLION_QXCAMERA_H
