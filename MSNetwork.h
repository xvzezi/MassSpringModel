//
// Created by xvzezi on 2019/4/9.
//

#ifndef MASSSPRINGMODEL_MSNETWORK_H
#define MASSSPRINGMODEL_MSNETWORK_H

#include "qx/qxheader.h"
#include "Tangible.h"

class MassPoint;
class Spring;
class MS_Network;

class MassPoint : public Tangible
{
public:
    /*********************************************/
    /** Constructor */
    MassPoint(std::string name, QX_Object* prim, float center_x, float center_y, float center_z, float radius)
        :Tangible(name, prim)
    {
        // in fact, this box is a ball
        this->set_bounding_ball(center_x, center_y, center_z, radius);
        // init
        x = y = 0;
    }

    void add_spring(Spring* spring)
    {
        if(spring)
            springs.push_back(spring);
    }
    void set_sticky(bool is_sticky)
    {
        sticky = is_sticky;
    }

    /*********************************************/
    /** Mouse Event */
    void mouse_select_callback(int button)
    {
        if(sticky)
            return;
        selected = true;
    }
    void mouse_release_callback(int button)
    {
        if(sticky)
            return;
        selected = false;
    }
    void mouse_drag_callback(int button, float dx, float dy, float dz);

    /*********************************************/
    /** Render */
    void render(glm::mat4& global_matrix, GLenum draw_type = GL_TRIANGLES)
    {
        this->identity()->translate(x, y, 0)->scale(0.5, 0.5, 0.5);
        Tangible::render(global_matrix, draw_type);
    }

public:
    // Connected Springs
    std::vector<Spring*> springs;
    bool sticky = false;
    // Location indicator
    float x, y;
    // Mouse Event Field
    bool selected = false;

};

class Spring : public QX_Object
{
public:
    Spring(std::string name)
    {
        // empty init values
        float vertices[6] = {
                0, 0, 0,
                0, 0, 0
        };
        float colors[6] = {
                0, 0.8, 0.3,
                0, 0.8, 0.3
        };
        unsigned int eles[2] = {
                0, 1
        };
        init(name, vertices, colors, nullptr, 2, GL_DYNAMIC_DRAW, eles, 2);
    }

    void connect_left_right(MassPoint* left, MassPoint* right)
    {
        if(connected)
            return;
        if(!left || !right)
            return;
        first = left;
        second = right;
        first->add_spring(this);
        second->add_spring(this);
        connected = true;
    }
    void connect_up_down(MassPoint* up, MassPoint* down)
    {
        if(connected)
            return;
        if(!up || !down)
            return;
        first = up;
        second = down;
        first->add_spring(this);
        second->add_spring(this);
        connected = true;
    }

    bool check_collision()
    {
        float dx = second->x - first->x;
        float dy = second->y - first->y;
        float dis2 = dx * dx + dy * dy;
        float col_dis2 = first->box_cur_length * first->box_cur_length
                + second->box_cur_length * second->box_cur_length;
        return dis2 < col_dis2;
    }

    void render(glm::mat4& global_matrix, GLenum draw_type = GL_TRIANGLES)
    {
        first->bounding_box_update(); second->bounding_box_update();
        glm::vec3 a = first->box_cur_origin, b = second->box_cur_origin;
        float vs[6] = {
                a.x, a.y, a.z,
                b.x, b.y, b.z
        };
        set_vertices(vs);
        glLineWidth(2);
        QX_Object::render(global_matrix, GL_LINES);
    }
private:
    bool connected = false;
    MassPoint* first;
    MassPoint* second;

};

class MS_Network
{
public:
    MS_Network(int rows, int cols, QX_Object* prim, QX_World* world)
    {
        generateNetwork(rows, cols, prim, world);
        setNetwork(world);
    }

private:
    std::vector<std::vector<MassPoint*>> points;
    std::vector<Spring*> springs;
    void generateNetwork(int rows, int cols, QX_Object* prim, QX_World* world);
    void setNetwork(QX_World*);
};

#endif //MASSSPRINGMODEL_MSNETWORK_H
