#include "mass.h"
#include "rope.h"
#include "spring.h"
#include "CGL/vector2D.h"
#include <vector>
#include <iostream>

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, std::vector<int> pinned_nodes)
    {
        // TODO (Part 1): Create a rope starting at `start`, ending at `end`, and containing `num_nodes` nodes.
        auto delta = (1.0 / (num_nodes - 1)) * (end - start);
        for (int i = 0; i < num_nodes; i++)
        {
            auto position = start + i * delta;
            masses.emplace_back(new Mass(position, node_mass, false));
            if (i > 0)
            {
                springs.emplace_back(new Spring(masses[i - 1], masses[i], k));
            }
        }
        for (auto i : pinned_nodes)
        {
            masses[i]->pinned = true;
        }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto& s : springs)
        {
            // TODO (Part 2): Use Hooke's law to calculate the force on a node
            auto d = s->m2->position - s->m1->position;
            auto d_norm = d.norm();
            auto f_ab = s->k * (d / d_norm) * (d_norm - s->rest_length);
            s->m1->forces += f_ab;
            s->m2->forces -= f_ab;
        }

        for (auto& m : masses)
        {
            if (!m->pinned)
            {
                // TODO (Part 2): Add the force due to gravity, then compute the new velocity and position
                m->forces += gravity * m->mass;
                // TODO (Part 2): Add global damping
                m->forces -= k_d * m->velocity;
                m->velocity = m->velocity + delta_t * (m->forces / m->mass);
                m->position = m->last_position + delta_t * m->velocity;
            }
            // Reset all forces on each mass
            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto& s : springs)
        {
            // TODO (Part 3): Simulate one timestep of the rope using explicit Verlet （solving constraints)
            auto d = s->m2->position - s->m1->position;
            auto d_norm = d.norm();
            auto f_ab = s->k * (d / d_norm) * (d_norm - s->rest_length);
            s->m1->forces += f_ab;
            s->m2->forces -= f_ab;
        }

        for (auto& m : masses)
        {
            if (!m->pinned)
            {
                auto temp_position = m->position;
                // TODO (Part 3.1): Set the new position of the rope mass
                // TODO (Part 4): Add global Verlet damping
                m->forces += gravity * m->mass;
                m->position = m->position + (1 - k_d) * (m->position - m->last_position) + (m->forces / m->mass) * delta_t * delta_t;
                // Update last position.
                m->last_position = temp_position;
            }
            // Reset all forces on each mass.
            m->forces = Vector2D(0, 0);
        }
    }
}