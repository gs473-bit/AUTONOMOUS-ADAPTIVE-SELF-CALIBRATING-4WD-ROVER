#include "control/ramp_limiter.h"
#include <math.h>
float ramp_step(float now,float target,float rate,float dt) {
    if (!isfinite(now) || !isfinite(target) || !isfinite(rate) || !isfinite(dt) || rate<=0.0f || dt<=0.0f) return now;
    const float step=rate*dt,d=target-now;
    return now+(d>step?step:(d< -step?-step:d));
}
