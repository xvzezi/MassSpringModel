//
// Created by xvzezi on 2019/4/10.
//

#ifndef MASSSPRINGMODEL_MASSSPRING_H
#define MASSSPRINGMODEL_MASSSPRING_H

#include "qx/qxheader.h"
#include "Tangible.h"

class Spring;

class MassPoint : public Tangible
{
public:
    /*********************************************/
    /** Constructor */
    MassPoint(std::string name, float mass,
                QX_Object* prim, float center_x, float center_y, float center_z, float radius)
            :Tangible(name, prim)
    {
        // in fact, this box is a ball
        this->set_bounding_ball(center_x, center_y, center_z, radius);
        // init
        x = y = 0;
        vx = vy = 0;
        this->mass = mass;
    }

    void set_init_loc(float x, float y)
    {
        original_y = y;
        this->x = x;
        this->y = y;
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

    void calculate_force(float spring_k, float& out_f_x, float& out_f_y);
    float calculate_energy(const float& gravity)
    {
        float v2 = vx * vx + vy * vy;
        float motion_e = mass * v2 / 2;
        float grad_e = gravity * (y - original_y);
        return motion_e + grad_e;
    }
    float calculate_abs_dis_square(float _x, float _y)
    {
        float delta_x = _x - x;
        float delta_y = _y - y;
        return delta_x * delta_x + delta_y * delta_y;
    }
    int get_spring_amount()
    {
        return springs.size();
    }

    /*********************************************/
    /** Mouse Event */
    void mouse_select_callback(int button)
    {
        selected = true;
        vx = vy = 0;
    }
    void mouse_release_callback(int button)
    {
        selected = false;
    }
    void mouse_drag_callback(int button, float dx, float dy, float dz);

    /*********************************************/
    /** Render */
    void render(glm::mat4& global_matrix, GLenum draw_type = GL_TRIANGLES)
    {
        if(sticky)
            this->identity()->translate(x, y, 0)->scale(0.5, 0.5, 0.5);
        else
            this->identity()->translate(x, y, 0)->scale(0.2, 0.2, 0.2);

        Tangible::render(global_matrix, draw_type);
    }

public:
    // Connected Springs
    std::vector<Spring*> springs;
    bool sticky = false;
    // Location indicator
    float original_y;
    float x, y;
    float vx, vy;
    float mass;
    // Mouse Event Field
    bool selected = false;
};

class Spring : public QX_Object
{
public:
    Spring(std::string name, float rest_len)
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
        this->rest_len = rest_len;
    }

    float calculate_energy(const float& spring_k)
    {
        float dx = second->x - first->x;
        float dy = second->y - first->y;
        float dis2 = dx * dx + dy * dy;
        float dis = sqrtf(dis2);
        float delta_x = dis - rest_len;
        return spring_k * delta_x * delta_x / 2;
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

    void get_spring_force(MassPoint* tar, float spring_k, float& out_x, float& out_y);

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
    float rest_len;
};

#endif //MASSSPRINGMODEL_MASSSPRING_H
