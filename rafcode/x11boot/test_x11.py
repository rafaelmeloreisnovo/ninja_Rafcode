#!/usr/bin/env python3
"""Hosted-only test fixture, never linked to freestanding ELFs."""
import os
import secrets
import socket
import subprocess
import threading
from pathlib import Path

path = "/tmp/x11-" + secrets.token_hex(4)
assert len(path) == 17
elf = str(Path(__file__).with_name("x11boot-test-x86_64.elf"))
subprocess.run([
    "clang", "--target=x86_64-linux-gnu",
    '-DRAF_X11_SOCKET="' + path + '"',
    "-x", "assembler-with-cpp", "-nostdlib", "-static", "-fno-pie",
    "-fuse-ld=lld", "-Wl,-e,_start", "-Wl,--build-id=none", "-Wl,-s",
    str(Path(__file__).with_name("x11boot.S")), "-o", elf,
], check=True)


def fixture(ok, words=8):
    try:
        os.unlink(path)
    except FileNotFoundError:
        pass
    ready = threading.Event()
    seen = []

    def serve():
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as server:
            server.bind(path)
            server.listen(1)
            ready.set()
            conn, _ = server.accept()
            with conn:
                request = b""
                while len(request) < 12:
                    chunk = conn.recv(12 - len(request))
                    if not chunk:
                        break
                    request += chunk
                seen.append(request)
                reply = bytes([1 if ok else 0, 0, 11, 0, 0, 0,
                               words & 255, words >> 8])
                conn.sendall(reply[:1])
                conn.sendall(reply[1:3])
                conn.sendall(reply[3:])
                if ok and words == 8:
                    conn.sendall(bytes(32))

    thread = threading.Thread(target=serve, daemon=True)
    thread.start()
    assert ready.wait(3), "mock did not start"
    try:
        payload = b"xy\0\xff" * 1300
        proc = subprocess.run([elf], input=payload, capture_output=True, timeout=5)
        assert proc.returncode == 0, proc
        assert proc.stdout.endswith(payload), len(proc.stdout)
        assert (b"X11_FRAME_OK" in proc.stdout) is (ok and words == 8), proc.stdout
        assert (b"X11_UNAVAILABLE" in proc.stdout) is (not (ok and words == 8)), proc.stdout
        assert seen == [bytes([0x6c, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0])], seen
    finally:
        thread.join(timeout=3)
        os.unlink(path)


fixture(True)
fixture(False)
fixture(True, 65535)
print("PASS: X11 setup framing, fragmented/denied/oversized reply, binary VT transport")
