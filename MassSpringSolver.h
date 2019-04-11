//
// Created by xvzezi on 2019/4/7.
//

#ifndef MASSSPRINGMODEL_MASSSPRINGSOLVER_H
#define MASSSPRINGMODEL_MASSSPRINGSOLVER_H

#include <vector>
#include "Eigen/Dense"
#include "MassSpring.h"
using namespace Eigen;

/**
 * Solver for the Mass Spring Network
 * - 2D
 * -
 */

class MS_Solver
{
public:
    MS_Solver(std::vector<std::vector<MassPoint*>>& points, std::vector<Spring*>& springs)
        :points(points), springs(springs)
    {
        int rows = points.size();
        int cols = points[0].size();
        for(int row = 0; row < rows; row++)
        {
            deltas.push_back(std::vector<Vector4f, Eigen::aligned_allocator<Vector4f>>());
            for(int col = 0; col < cols; col++)
            {
                deltas[row].push_back(Vector4f::Zero());
            }
        }
    }

    void set_param(float gravity, float k, float d)
    {
        this->gravity = gravity;
        this->spring_k = k;
        this->damping_d = d;
    }

    float solve_implicit_next(float time_step)
    {
        float timer = glfwGetTime();
        int rows = points.size();
        int cols = points[0].size();
        for(int row = 0; row < rows; row++)
        {
            for(int col = 0; col < cols; col++)
            {
                MassPoint* p = points[row][col];
                if(p->selected || p->sticky)
                    continue;
                solve_point_implicit(p, time_step, deltas[row][col]);
            }
        }
        // apply all the delta at once
        apply_deltas();
        return glfwGetTime() - timer;
    }

    float solve_explicit_next(float time_step)
    {
        float timer = glfwGetTime();
        int rows = points.size();
        int cols = points[0].size();
        for(int row = 0; row < rows; row++)
        {
            for(int col = 0; col < cols; col++)
            {
                MassPoint* p = points[row][col];
                if(p->selected || p->sticky)
                    continue;
                solve_point_explicit(p, time_step, deltas[row][col]);
            }
        }
        // apply all the delta at once
        apply_deltas();
        return glfwGetTime() - timer;
    }

    float solve_mix_next(float time_step)
    {
        float timer = glfwGetTime();
        int rows = points.size();
        int cols = points[0].size();
        for(int row = 0; row < rows; row++)
        {
            for(int col = 0; col < cols; col++)
            {
                MassPoint* p = points[row][col];
                if(p->selected || p->sticky)
                    continue;
                Vector4f im_matrix, ex_matrix;
                solve_point_implicit(p, time_step, im_matrix);
                solve_point_explicit(p, time_step, ex_matrix);
                deltas[row][col] = im_matrix / 2 + ex_matrix / 2;
            }
        }
        // apply all the delta at once
        apply_deltas();
        return glfwGetTime() - timer;
    }

    float compute_system_energy()
    {
        float total = 0;
        for(auto line : points)
        {
            for(auto p : line)
            {
                total += p->calculate_energy(gravity);
            }
        }
        for(auto s : springs)
        {
            total += s->calculate_energy(spring_k);
        }
        return total;
    }

    float compute_system_chaos()
    {
        float cx = 0;
        float cy = 0;
        int amount = points.size() * points[0].size();
        for(auto line : points)
        {
            for(auto p : line)
            {
                cx += p->x;
                cy += p->y;
            }
        }
        cx /= amount;
        cy /= amount;
        float chaos = 0;
        for(auto line : points)
        {
            for(auto p : line)
            {
                chaos += p->calculate_abs_dis_square(cx, cy);
            }
        }
        chaos /= (amount - 1);
        return chaos;
    }
private:

    void apply_deltas()
    {
        int rows = points.size();
        int cols = points[0].size();
        for(int row = 0; row < rows; row++)
        {
            for(int col = 0; col < cols; col++)
            {
                MassPoint* p = points[row][col];
                Vector4f& delta = deltas[row][col];
                if(p->selected || p->sticky)
                    continue;
                p->x += delta(0);
                p->y += delta(1);
                p->vx += delta(2);
                p->vy += delta(3);
            }
        }
    }

    void solve_point_implicit(MassPoint* tar, float time_step, Vector4f& out_delta)
    {
        float partial_damp = - damping_d / tar->mass;
        float partial_spring = - spring_k / tar->mass * tar->get_spring_amount();
        float in = 1 - time_step * partial_damp - time_step * time_step * partial_spring;
        Matrix2f cof;
        cof << in, 0, 0, in;
        // std::cout << cof << std::endl;
        // 总体上，我们需要求得所有的力，并且分解到x,y方向上
        // 获得弹簧力
        float spring_x, spring_y;
        tar->calculate_force(spring_k, spring_x, spring_y);
        // 获得damping力
        float dam_x = - tar->vx * damping_d;
        float dam_y = - tar->vy * damping_d;
        // 当前总加速度
        float acc_x = (spring_x + dam_x) / tar->mass;
        float acc_y = (spring_y + dam_y) / tar->mass - gravity;
        // 为了构成向量，还有，
        float acc_delta_x = time_step * partial_spring * tar->vx;
        float acc_delta_y = time_step * partial_spring * tar->vy;
        // 构成向量
        float total_x = (acc_x + acc_delta_x) * time_step;
        float total_y = (acc_y + acc_delta_y) * time_step;
        Vector2f right(total_x, total_y);
        // solve the linear func
        Vector2f delta_v = cof.householderQr().solve(right);
        float new_vx = tar->vx + delta_v(0);
        float new_vy = tar->vy + delta_v(1);
        float dx = new_vx * time_step;
        float dy = new_vy * time_step;
        out_delta << dx, dy, delta_v;
        // std::cout << out_delta << std::endl;
        // std::cout << right << std::endl;
        // std::cout << "-----------" << std::endl;
    }
    void solve_point_explicit(MassPoint* tar, float time_step, Vector4f& out_delta)
    {
        // 总体上，我们需要求得所有的力，并且分解到x,y方向上
        // 获得弹簧力
        float spring_x, spring_y;
        tar->calculate_force(spring_k, spring_x, spring_y);
        // 获得damping力
        float dam_x = - tar->vx * damping_d;
        float dam_y = - tar->vy * damping_d;
        // 当前总加速度
        float acc_x = (spring_x + dam_x) / tar->mass;
        float acc_y = (spring_y + dam_y) / tar->mass - gravity;


        float dvx = acc_x * time_step;
        float dvy = acc_y * time_step;
        float dx = tar->vx * time_step;
        float dy = tar->vy * time_step;
        out_delta << dx, dy, dvx, dvy;
    }

    std::vector<std::vector<MassPoint*>>& points;
    std::vector<Spring*>& springs;
    std::vector<std::vector<Vector4f, Eigen::aligned_allocator<Vector4f>>> deltas;
    float gravity = 0;
    float spring_k = 0;
    float damping_d = 0;
};

#endif //MASSSPRINGMODEL_MASSSPRINGSOLVER_H
