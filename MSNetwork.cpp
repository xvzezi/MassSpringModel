//
// Created by xvzezi on 2019/4/9.
//
#include <sstream>
#include "MSNetwork.h"

/**************************************************************************/
/*       Network       */
void MS_Network::generateNetwork(int rows, int cols, QX_Object *prim, QX_World* world) {
    prim->enable();
    std::stringstream ss;
    static float width = 100;
    static float height = 50;
    static float st_x = 10;
    static float st_y = 10;
    float cur_y = st_y, cur_x = st_x;
    float step_x = width / (cols - 1);
    float step_y = height / (rows - 1);
    for(int row = 0; row < rows; row++)
    {
        points.push_back(std::vector<MassPoint*>());
        for (int col = 0; col < cols; col++)
        {
            ss << row << ',' << col;
            MassPoint* mp = new MassPoint("n"+ss.str(), prim, 0, 1, 0, 1);
            mp->x = cur_x; mp->y = cur_y;
            world->add_object(mp);
            ss.str("");
            points[row].push_back(mp);
            cur_x += step_x;
        }
        cur_y += step_y;
        cur_x = st_x;
    }
    // add spring
    for(int row = 0; row < rows - 1; row++)
    {
        for (int col = 0; col < cols - 1; col++)
        {
            ss << springs.size();
            // first
            Spring* sp = new Spring("s"+ss.str());
            sp->connect_left_right(points[row][col], points[row][col+1]);
            world->add_object(sp);
            springs.push_back(sp);
            // second
            sp = new Spring("s"+ss.str());
            sp->connect_up_down(points[row][col], points[row+1][col]);
            world->add_object(sp);
            springs.push_back(sp);
            ss.str("");
        }
    }
    int row = rows - 1; // last row
    for (int col = 0; col < cols - 1; col++)
    {
        ss << springs.size();
        Spring* sp = new Spring("s"+ss.str());
        sp->connect_left_right(points[row][col], points[row][col+1]);
        world->add_object(sp);
        springs.push_back(sp);
        ss.str("");
    }
    int col = cols - 1;
    for (int row = 0; row < rows - 1; row++)
    {
        ss << springs.size();
        Spring* sp = new Spring("s" + ss.str());
        sp->connect_up_down(points[row][col], points[row+1][col]);
        world->add_object(sp);
        springs.push_back(sp);
        ss.str("");
    }
    // report
    std::cout << "Points: " << rows << 'x' << cols << '=' << points.size() * points[0].size() << std::endl;
    std::cout << "Springs: " << springs.size() << std::endl;
    prim->disable();
}

void MS_Network::setNetwork(QX_World* world)
{
    // make the edge points sticky
    int rows = points.size();
    for(int row = 0; row < rows; row++)
    {
        points[row][0]->set_sticky(true);
        points[row][0]->set_colors_rgb(0, 1, 1);
        points[row].back()->set_sticky(true);
        points[row].back()->set_colors_rgb(0, 1, 1);
    }
    for(Spring* s : springs)
    {
        s->ref_shader(world->get_shader("basic"));
    }
};

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

