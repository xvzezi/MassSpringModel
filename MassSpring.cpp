//
// Created by xvzezi on 2019/4/10.
//

#include "MassSpring.h"

/**************************************************************************/
/*       Mass Point       */
void MassPoint::mouse_drag_callback(int button, float dx, float dy, float dz)
{
    if(sticky)
        return;
    float old_x = x, old_y = y;
    x += dx;
    y += dy;
    for(Spring* s : springs)
    {
        if(s->check_collision())
        {
            x = old_x;
            y = old_y;
            break;
        }
    }
}

void MassPoint::calculate_force(float spring_k, float& out_f_x, float& out_f_y)
{
    float f_x = 0, f_y = 0;
    // do the iteration
    for(Spring* s : springs)
    {
        float tmp_x, tmp_y;
        tmp_x = tmp_y = 0;
        s->get_spring_force(this, spring_k, tmp_x, tmp_y);
        f_x += tmp_x;
        f_y += tmp_y;
    }

    // give out
    out_f_x = f_x;
    out_f_y = f_y;
}

void Spring::get_spring_force(MassPoint *tar, float spring_k, float &out_x, float &out_y) {
    if(!tar) return;
    MassPoint* center;
    MassPoint* out;
    if(tar == first)
    {
        center = first; out = second;
    }
    else if(tar == second)
    {
        center = second; out = first;
    }
    else
        return;
    // try to calculate
    float x = center->x - out->x;
    float y = center->y - out->y;
    float cur_len = sqrtf(x * x + y * y);
    float delta_len = rest_len - cur_len;
    float force = spring_k * delta_len;
    // resolve to x-y
    out_x = force * x / cur_len;
    out_y = force * y / cur_len;
}
