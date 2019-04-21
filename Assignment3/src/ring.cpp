#include "ring.h"
#include "explosion.h"
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

Ring::Ring(ParticleSystem* psys, glm::vec3 sphere_center, float sphere_radius,
           float phi_mid, float phi_spread, float explosion_radius,float currentTime)
    : psystem{psys}, generator(rd()),
      sphere_center{sphere_center},
      sphere_radius{sphere_radius},
      phi_mid{phi_mid},
      phi_spread{phi_spread},
      explosion_radius{explosion_radius},
      explosion_displacement_dist{phi_mid - phi_spread / 2.0,
                                  phi_mid + phi_spread / 2.0},
      density{25.0f} {

    // cout<<"****** Making ring ******"<<endl;
    // cout<<phi_mid<<" --> "<<phi_spread<<endl;

    
    float ring_radius = sphere_radius * cos(glm::radians(phi_mid));
    circumference =
        2 * 3.14159265359 * ring_radius;
    int num_explosion = circumference * density;

    std::uniform_real_distribution<double> theta_dist{0.0f, 360.0f};
    std::normal_distribution<double> end_time_dist{5.0f, 1.0f};

    const float speed = 1.0f;
    const float life = 0.5f;
    const int volume = 200.0f; 

    for (int i = 0; i < num_explosion; i++) {
        float phi = glm::radians(explosion_displacement_dist(generator));
        float theta = glm::radians(theta_dist(generator));
        glm::vec3 local_center = sphere_radius*glm::vec3(sin(phi),0.0f,0.0f) + ring_radius*glm::vec3(0.0f,sin(theta),cos(theta));

        glm::vec3 init_point = sphere_center + local_center;
        glm::vec3 normal = glm::normalize(local_center);

        Explosion *e = new Explosion(psystem,init_point,normal,explosion_radius,speed,life,volume,currentTime + (float)end_time_dist(generator));

        explosions.push_back(e);

    }
}

void Ring::Update(float deltaTime,float currentTime,const glm::vec3& cameraPosition){
    for(auto e: explosions){
        e->Generate(deltaTime,currentTime,cameraPosition);
    }
}
