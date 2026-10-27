
# Rover Camera Setup and Verification

**Project:** ECE357 Autonomous Adaptive Self-Calibrating 4WD Rover  
**Status:** Camera hardware and availability not yet confirmed.

## 1. Identify the Camera

Before connecting anything, record:

- Manufacturer and exact model
- Connector type and cable
- Camera board photographs
- Raspberry Pi OS version
- Available interface or adapter

The project inventory lists an OV7670 VGA camera. Do not assume it connects directly to the Raspberry Pi CSI camera connector. Verify the interface and required controller first.

## 2. Record the Raspberry Pi Environment

Run these commands on the Raspberry Pi when you have access to it:

```bash
uname -m
cat /etc/os-release
python3 --version
```

Record the actual output in `docs/build-log.md`.

## 3. Test a Compatible CSI Camera

Use this only if the exact camera and cable are confirmed compatible with the Pi CSI connector.

Power down the Pi before connecting or disconnecting the camera ribbon.

On a supported Raspberry Pi OS installation, check camera detection:

```bash
rpicam-hello --list-cameras
```

If the camera is detected, capture a test image:

```bash
mkdir -p data/camera
rpicam-still --output data/camera/test.jpg
```

Check that the image exists and inspect it.

## 4. Test a USB UVC Camera

Use this only if the selected camera supports USB video capture.

Install the required diagnostic tools if appropriate for the installed OS:

```bash
sudo apt update
sudo apt install -y v4l-utils python3-opencv
```

Identify the camera:

```bash
lsusb
v4l2-ctl --list-devices
ls -l /dev/video*
```

Record the device name and actual device index before attempting capture.

## 5. ArUco Marker Detection

After a real image has been captured, verify whether the installed OpenCV build supports ArUco:

```bash
python3 -c "import cv2; print(cv2.__version__); print(hasattr(cv2, 'aruco'))"
```

The selected marker dictionary must match the one used by the detection software.

Successful marker detection demonstrates basic visual detection; it does not by itself prove accurate localization or mapping.

## 6. Evidence to Record

- [ ] Camera model and interface confirmed.
- [ ] Raspberry Pi OS and Python versions recorded.
- [ ] Camera detected successfully.
- [ ] Real image captured and visually inspected.
- [ ] ArUco marker detection tested, if applicable.
- [ ] Terminal output and photographs saved.
- [ ] Errors and corrective actions documented.

## 7. Test Results

**Date:** Not yet tested  
**Camera model:** Not confirmed  
**Capture method:** Not confirmed  
**Result:** Pending physical hardware verification

Never mark this test as passed until it has been performed on the actual hardware.
