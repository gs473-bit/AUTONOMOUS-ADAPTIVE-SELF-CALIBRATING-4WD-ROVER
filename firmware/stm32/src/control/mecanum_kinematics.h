#ifndef AETHER_KINEMATICS_H
#define AETHER_KINEMATICS_H
typedef struct {float wheel_radius_m,wheelbase_m,track_width_m,max_wheel_rad_s;} mecanum_geometry_t;
int mecanum_inverse(const mecanum_geometry_t *g,float vx,float vy,float wz,float wheels[4]);
#endif
