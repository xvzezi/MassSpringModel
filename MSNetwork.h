//
// Created by xvzezi on 2019/4/9.
//

#ifndef MASSSPRINGMODEL_MSNETWORK_H
#define MASSSPRINGMODEL_MSNETWORK_H

#include "MassSpring.h"
#include "MassSpringSolver.h"

class MS_Network;

class MS_Network
{
public:
    MS_Network(int rows, int cols, float rest_len, float mass, QX_Object* prim, QX_World* world)
    {
        generateNetwork(rows, cols, rest_len, mass, prim, world);
        setNetwork(world);
        solver = new MS_Solver(points);
        this->world = world;
    }

    void set_param(float g, float k, float d)
    {
        solver->set_param(g, k, d);
    }

    void solve_next(int mode, float time_step)
    {
        switch (mode)
        {
            case 0: // implicit
                solver->solve_implicit_next(time_step);
                break;
            case 1: // explicit
            case 2: // half-mix
            default:
                break;
        }
    }

    ~MS_Network()
    {
        delete solver;
        for(auto line : points)
        {
            for(auto p : line)
            {
                world->remove_object(p->name);
            }
        }
        for(auto s : springs)
        {
            world->remove_object(s->name);
        }
    }
private:
    std::vector<std::vector<MassPoint*>> points;
    std::vector<Spring*> springs;
    void generateNetwork(int rows, int cols, float rest_len, float mass, QX_Object* prim, QX_World* world);
    void setNetwork(QX_World*);

    QX_World* world;
    MS_Solver* solver = nullptr;
};

#endif //MASSSPRINGMODEL_MSNETWORK_H
