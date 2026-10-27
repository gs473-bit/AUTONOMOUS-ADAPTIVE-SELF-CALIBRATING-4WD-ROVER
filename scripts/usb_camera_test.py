
"""Diagnostic for USB cameras connected to the rover's Raspberry Pi.

This script is for USB/UVC cameras only. It does not test CSI cameras
or parallel-interface cameras such as many OV7670 modules.
"""

import argparse
import sys

try:
    import cv2
except ImportError:
    print("ERROR: OpenCV is not installed.")
    print("Install it on the Raspberry Pi before running this script.")
    sys.exit(1)


def main():
    parser = argparse.ArgumentParser(
        description="Check whether a USB camera can deliver frames."
    )
    parser.add_argument(
        "--index",
        type=int,
        default=0,
        help="Camera device index (default: 0)",
    )
    args = parser.parse_args()

    camera = cv2.VideoCapture(args.index)

    if not camera.isOpened():
        print(f"FAIL: Could not open camera index {args.index}.")
        print("Check the camera connection and device compatibility.")
        sys.exit(1)

    try:
        success, frame = camera.read()

        if not success or frame is None:
            print("FAIL: Camera opened, but no frame was received.")
            sys.exit(1)

        height, width = frame.shape[:2]
        print("PASS: Camera opened and a frame was received.")
        print(f"Resolution: {width} x {height}")
        print("This confirms a frame was received, not full rover integration.")

    finally:
        camera.release()


if __name__ == "__main__":
    main()
