#include "control/mecanum_kinematics.h"
#include <math.h>
int mecanum_inverse(const mecanum_geometry_t *g,float vx,float vy,float wz,float w[4]) {
    if (!g || !w || !isfinite(vx) || !isfinite(vy) || !isfinite(wz) ||
        !isfinite(g->wheel_radius_m) || !isfinite(g->wheelbase_m) || !isfinite(g->track_width_m) ||
        !isfinite(g->max_wheel_rad_s) || g->wheel_radius_m<=0.0f || g->wheelbase_m<=0.0f ||
        g->track_width_m<=0.0f || g->max_wheel_rad_s<=0.0f) return 0;
    const float k=(g->wheelbase_m+g->track_width_m)*0.5f;
    const float r=g->wheel_radius_m;
    /* Sign convention: x forward, y left, yaw CCW; FL,FR,RL,RR. */
    w[0]=(vx-vy-k*wz)/r; w[1]=(vx+vy+k*wz)/r;
    w[2]=(vx+vy-k*wz)/r; w[3]=(vx-vy+k*wz)/r;
    for(unsigned i=0;i<4u;i++) if(!isfinite(w[i]) || fabsf(w[i])>g->max_wheel_rad_s) return 0;
    return 1; /* Caller rejects infeasible commands; no silent scaling. */
}
