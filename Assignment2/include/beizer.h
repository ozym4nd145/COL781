#pragma once

class Beizer{
    private:
        vector<float> coeffs;
        void init(){
            coeffs.resize(n+1);
            coeffs[0] = 1.0;
            coeffs[n] = 1.0;
            for(int i=1;i<n;i++){
                coeffs[i] = coeffs[i-1]*(n-i+1)/i;
            }
        }
    public:
        vector<glm::vec3> points;
        const int n;
        Beizer(vector<glm::vec3>& p): points(p), n(p.size()-1) {
            init();
        }
        glm::vec3 get_pt(float t){
            assert(t >= 0.0 && t<= 1.0);
            glm::vec3 pt(0.0f,0.0f,0.0f);

            float last_t_val = pow((1.0-t),n);
            float to_multiply = t/(1.0-t);

            for(int i=0;i<=n;i++){
                pt += coeffs[i]*last_t_val*points[i];
                last_t_val *= to_multiply;
            }
            return pt;
        }
        float length_of_curve(float EPSILON = 1e-4){
            float t = 0.0;
            float perimeter = 0.0;
            glm::vec3 old_pos = get_pt(t);
            while(t<1.0-2*EPSILON){
                glm::vec3 pos_next = get_pt(t+EPSILON);
                float ds = glm::length(pos_next-old_pos);
                perimeter += ds;
                t += EPSILON;
                old_pos = pos_next;
            }
            return perimeter;
        }

        glm::vec3 get_derivative(float t, float EPSILON = 1e-4){
            glm::vec3 old_pos = get_pt(t);
            glm::vec3 pos_next = get_pt(t+EPSILON);
            return glm::normalize(pos_next - old_pos);            
        }
};