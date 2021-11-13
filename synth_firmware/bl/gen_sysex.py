#!/usr/bin/env python3

import sys

def encode_4bytes(b):
    assert len(b) <= 4
    b = b + b"\xFF" * (4 - len(b))
    return bytes([b[0] & 0x7F, ((b[0] >> 7) | (b[1] << 1)) & 0x7F, ((b[1] >> 6) | (b[2] << 2)) & 0x7F, ((b[2] >> 5) | (b[3] << 3)) & 0x7F, b[3] >> 4])

def encode_addr(a):
    return bytes([(a >> 8) & 0x7F, (a >> 1) & 0x7F])

def encode_bytes(b):
    return b"".join(encode_4bytes(b[i:i+4]) for i in range(0, len(b), 4))

def noop(length):
    assert length >= 6
    return b"\xF0\x7D\x61\x27\x00" + b"\x00" * (length - 5) + b"\xF7"

def write_flash_page(addr, b):
    write_command = b"\xF0\x7D\x61\x26\x01" + encode_addr(addr) + encode_bytes(b) + b"\xF7"
    return write_command + noop(20) # 20 bytes > 4 ms (flash write time)

def write_flash(addr, b):
    return b"".join(write_flash_page(addr + i, b[i:i+64]) for i in range(0, len(b), 64))

def reset_into_bl():
    return b"\xFF" + noop(160) # 160 bytes > 50 ms (approx. reset time)

def reset_into_app():
    return b"\xFF"

with open(sys.argv[1], "rb") as f:
    b = f.read()

sysex_data = reset_into_bl() + write_flash(0x9000, b) + reset_into_app()

with open("app.syx", "wb") as f:
    f.write(sysex_data)
