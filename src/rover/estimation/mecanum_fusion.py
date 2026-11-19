"""Mecanum wheel odometry + Pi MPU6050 yaw rate; not globally drift-free SLAM."""
from dataclasses import dataclass
import math

@dataclass(frozen=True)
class FusionGeometry:
    wheel_radius_m: float
    wheelbase_m: float
    track_width_m: float
    counts_per_wheel_rev: float

    @classmethod
    def from_config(cls, config):
        g = config["mecanum"]
        cpr = config["encoders"]["counts_per_wheel_revolution"]
        values = [g["wheel_radius_m"], g["wheelbase_m"], g["track_width_m"], cpr]
        if any(x is None or isinstance(x, bool) or not isinstance(x,(float,int))
               or not math.isfinite(x) or x<=0 for x in values):
            raise ValueError("Measure geometry and output encoder CPR before starting fusion")
        return cls(*values)

@dataclass(frozen=True)
class Pose2D:
    x_m: float
    y_m: float
    yaw_rad: float
    vx_m_s: float
    vy_m_s: float
    valid: bool

class MecanumFusion:
    def __init__(self, geometry: FusionGeometry, gyro_weight=0.9):
        if not 0 <= gyro_weight <= 1:
            raise ValueError("gyro_weight outside [0,1]")
        self.g = geometry
        self.gyro_weight = gyro_weight
        self.previous_ticks = None
        self.x=self.y=self.yaw=0.0

    def update(self, ticks, gyro_z_rad_s, dt_s, imu_valid=True):
        if len(ticks) != 4 or not 0 < dt_s < 2 or not math.isfinite(dt_s):
            raise ValueError("invalid odometry sample interval/ticks")
        if self.previous_ticks is None:
            self.previous_ticks = tuple(ticks)
            return Pose2D(self.x,self.y,self.yaw,0,0,False)
        changes = [int(a)-int(b) for a,b in zip(ticks,self.previous_ticks)]
        self.previous_ticks = tuple(ticks)
        rads = [delta*math.tau / (self.g.counts_per_wheel_rev*dt_s) for delta in changes]
        r = self.g.wheel_radius_m
        k = (self.g.wheelbase_m+self.g.track_width_m)/2
        vx = r*(rads[0]+rads[1]+rads[2]+rads[3])/4
        vy = r*(-rads[0]+rads[1]+rads[2]-rads[3])/4
        wheel_wz = r*(-rads[0]+rads[1]-rads[2]+rads[3])/(4*k)
        heading_rate=(self.gyro_weight*gyro_z_rad_s+(1-self.gyro_weight)*wheel_wz
                      if imu_valid and math.isfinite(gyro_z_rad_s) else wheel_wz)
        mid_yaw=self.yaw + heading_rate*dt_s/2
        self.x += (vx*math.cos(mid_yaw)-vy*math.sin(mid_yaw))*dt_s
        self.y += (vx*math.sin(mid_yaw)+vy*math.cos(mid_yaw))*dt_s
        self.yaw=math.atan2(math.sin(self.yaw+heading_rate*dt_s), math.cos(self.yaw+heading_rate*dt_s))
        return Pose2D(self.x,self.y,self.yaw,vx,vy,True)
