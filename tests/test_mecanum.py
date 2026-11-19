import pytest
from rover.control.kinematics import MecanumGeometry, inverse_mecanum

G = MecanumGeometry(0.04, 0.20, 0.18)

def test_straight_all_equal():
    assert inverse_mecanum(0.08,0,0,G,10) == pytest.approx((.2,.2,.2,.2))

def test_strafe_opposed_diagonals():
    assert inverse_mecanum(0,0.08,0,G,10) == pytest.approx((-.2,.2,.2,-.2))

def test_rotate_opposed_sides():
    assert inverse_mecanum(0,0,0.4,G,10) == pytest.approx((-.19,.19,-.19,.19))

def test_invalid_geometry():
    with pytest.raises(ValueError): MecanumGeometry(0,0.2,0.2)

def test_overspeed_rejected():
    with pytest.raises(ValueError): inverse_mecanum(4,0,0,G,10)

def test_config_geometry():
    assert MecanumGeometry.from_config({"mecanum":{"wheel_radius_m":0.04,"wheelbase_m":0.20,"track_width_m":0.18}}) == G
