//
// Created by xvzezi on 2019/4/9.
//

#ifndef MASSSPRINGMODEL_MSNETWORK_H
#define MASSSPRINGMODEL_MSNETWORK_H

#include <fstream>
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
        solver = new MS_Solver(points, springs);
        this->world = world;
    }

    void set_param(float g, float k, float d)
    {
        solver->set_param(g, k, d);
        float energy = solver->compute_system_energy();
        float chaos = solver->compute_system_chaos();
        e_float.push_back(energy);
        c_float.push_back(chaos);
    }

    void solve_next(int mode, float time_step)
    {
        switch (mode)
        {
            case 0: // implicit
                total_time +=solver->solve_implicit_next(time_step);
                break;
            case 1: // explicit
                total_time +=solver->solve_explicit_next(time_step);
                break;
            case 2: // half-mix
                total_time +=solver->solve_mix_next(time_step);
                break;
            default:
                break;
        }
        frame_num++;
        float energy = solver->compute_system_energy();
        float chaos = solver->compute_system_chaos();
        e_float.push_back(energy);
        c_float.push_back(chaos);
    }

    void log()
    {
        std::stringstream ss;
        ss << "../result/MSNet" << file_counter++ << ".txt";
        std::string fileName = ss.str();
        std::ofstream ofs(fileName);
        int amount = e_float.size();
        ofs << "E\tCh" << std::endl;
        for(int i = 0; i < amount; i++)
        {
            ofs << e_float[i] << '\t' << c_float[i] << std::endl;
        }
        ofs << "aver time: " << total_time / frame_num << std::endl;
    }

    ~MS_Network()
    {
        log();
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
    std::vector<float> e_float; // energy changing
    std::vector<float> c_float; // chaos changing
    float total_time = 0;
    float frame_num = 0;
    void generateNetwork(int rows, int cols, float rest_len, float mass, QX_Object* prim, QX_World* world);
    void setNetwork(QX_World*);

    QX_World* world;
    MS_Solver* solver = nullptr;
    static int file_counter;
};


#endif //MASSSPRINGMODEL_MSNETWORK_H
