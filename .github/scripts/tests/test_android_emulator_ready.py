import contextlib
import importlib.util
import io
from pathlib import Path
import subprocess
import unittest
from unittest.mock import patch


spec = importlib.util.spec_from_file_location(
    "android_emulator_ready", Path(__file__).parents[1] / "android_emulator_ready.py"
)
emulator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(emulator)


CONNECTED = """Active default network: 100
Current Networks:
  NetworkAgentInfo{network{100} nc{[ Transports: WIFI Capabilities: INTERNET&VALIDATED ]}}
"""


class AndroidEmulatorReadyTests(unittest.TestCase):
    def test_requires_a_validated_active_default_network(self):
        self.assertTrue(emulator.network_ready(CONNECTED))
        for connectivity in (
            CONNECTED.replace("100\n", "none\n"),
            CONNECTED.replace("network{100}", "network{101}"),
            CONNECTED.replace("&VALIDATED", ""),
            CONNECTED.replace("&VALIDATED", "&NOT_VALIDATED"),
            "Active default network: 100\nPreviously connected: network{100} VALIDATED",
        ):
            with self.subTest(connectivity=connectivity):
                self.assertFalse(emulator.network_ready(connectivity))

    def test_waits_for_boot_and_network_before_returning(self):
        with patch.object(emulator.subprocess, "check_output", side_effect=[
            "0\n", "1\n", CONNECTED.replace("&VALIDATED", ""), "1\r\n", CONNECTED,
        ]) as adb, patch.object(emulator.time, "sleep") as sleep:
            emulator.wait_until_ready()
        self.assertEqual(sleep.call_count, 2)
        self.assertEqual(adb.call_count, 5)
        for call in adb.call_args_list:
            self.assertEqual(call.kwargs["timeout"], 10)

    def test_readiness_timeout_fails_with_connectivity_diagnostics(self):
        connectivity = CONNECTED.replace("&VALIDATED", "")
        with patch.object(emulator.subprocess, "check_output", side_effect=["1\n", connectivity]), \
                patch.object(emulator.time, "monotonic", side_effect=[0, 0, 121]), \
                patch.object(emulator.time, "sleep"), \
                contextlib.redirect_stdout(io.StringIO()) as output:
            with self.assertRaisesRegex(RuntimeError, "did not become ready"):
                emulator.wait_until_ready()
        self.assertIn(connectivity, output.getvalue())

    def test_adb_errors_are_not_hidden(self):
        with patch.object(
            emulator.subprocess, "check_output",
            side_effect=subprocess.CalledProcessError(1, ["adb"]),
        ):
            with self.assertRaises(subprocess.CalledProcessError):
                emulator.wait_until_ready()


if __name__ == "__main__":
    unittest.main()
