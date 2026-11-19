import pytest
from rover.estimation.mecanum_fusion import MecanumFusion, FusionGeometry

def test_requires_calibrated_dimensions():
    with pytest.raises(ValueError):
        FusionGeometry.from_config({"mecanum": {"wheel_radius_m":None,"wheelbase_m":.2,"track_width_m":.2},"encoders":{"counts_per_wheel_revolution":100}})

def test_forward_strafe_with_mocked_ticks():
    g=FusionGeometry(.04,.20,.18,400)
    f=MecanumFusion(g,gyro_weight=1)
    assert not f.update([0,0,0,0],0,.1).valid
    forward=f.update([100,100,100,100],0,.5)
    assert forward.valid and forward.x_m == pytest.approx(.04*3.141592653589793/2)
    sideways=f.update([0,200,200,0],0,.5)
    assert sideways.y_m>0
