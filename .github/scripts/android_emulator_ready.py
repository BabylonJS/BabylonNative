import re
import subprocess
import time


def network_ready(connectivity):
    active = re.search(r"^Active default network: (\d+)\s*$", connectivity, re.MULTILINE)
    if not active:
        return False
    for line in connectivity.splitlines():
        if (
            "NetworkAgentInfo{" in line
            and f"network{{{active.group(1)}}}" in line
            and re.search(r"\bVALIDATED\b", line)
        ):
            return True
    return False


def wait_until_ready():
    started = time.monotonic()
    connectivity = ""
    while time.monotonic() - started < 120:
        booted = subprocess.check_output(
            ["adb", "shell", "getprop", "sys.boot_completed"], text=True, timeout=10,
        ).strip()
        if booted == "1":
            connectivity = subprocess.check_output(
                ["adb", "shell", "dumpsys", "connectivity"], text=True, timeout=10,
            )
            if network_ready(connectivity):
                print(f"Emulator boot and active network ready in {time.monotonic() - started:.1f}s")
                return
        time.sleep(2)
    print(connectivity, flush=True)
    raise RuntimeError("Android emulator did not become ready within 120 seconds")


if __name__ == "__main__":
    wait_until_ready()
