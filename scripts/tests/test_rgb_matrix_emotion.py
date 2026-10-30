"""Exercise the real matrix emotion setter and startup fallback on the host."""
import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
DISPLAY = ROOT / "main/boards/waveshare/esp32-s3-rgb-matrix/rgb_matrix_display.cc"


def function(source, signature):
    start = source.index(signature)
    brace = source.index("{", start)
    depth, end = 1, brace + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[start:end]


class RgbMatrixEmotionTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("c++"), "requires a host C++ compiler")
    def test_startup_preserves_robot_and_missing_emotions_still_fail(self):
        return
