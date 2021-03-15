#ifndef TOPAZ_PHYS_BODY_HPP
#define TOPAZ_PHYS_BODY_HPP
#include "core/vector.hpp"
#include <vector>

namespace tz::phys
{
    struct Body
    {
        tz::Vec3 position;
        tz::Vec3 velocity;
        tz::Vec3 force;
        float mass;
    };


    class World
    {
    public:
        using UniformForceID = std::size_t;
        World() = default;

        UniformForceID register_uniform_force(tz::Vec3 force_value);
        void unregister_uniform_force(UniformForceID force);
        void add_body(Body& body);
        void remove_body(Body& body);
        void update(float delta_millis);
    private:
        void motion_integrate(float delta_millis);
        tz::Vec3 fnet_uniform() const;

        std::vector<Body*> bodies;
        std::vector<tz::Vec3> uniform_forces;
    };
}

#endif // TOPAZ_PHYS_BODY_HPP