#!/usr/bin/env python3
"""Pruebas visuales seguras para una matriz 8x8 conectada a PixelIt."""

import argparse
import json
import sys
import urllib.error
import urllib.request


RGB565 = {
    "red": 0xF800,
    "green": 0x07E0,
    "blue": 0x001F,
    "white": 0xFFFF,
    "off": 0x0000,
}


def request(host: str, path: str, payload: dict | None = None) -> dict:
    data = None if payload is None else json.dumps(payload).encode("utf-8")
    method = "GET" if data is None else "POST"
    req = urllib.request.Request(
        f"http://{host}{path}",
        data=data,
        method=method,
        headers={"Content-Type": "application/json"},
    )
    with urllib.request.urlopen(req, timeout=5) as response:
        return json.loads(response.read().decode("utf-8"))


def bitmap_payload(data: list[int]) -> dict:
    return {
        "brightness": 20,
        "autobrightness": False,
        "bitmap": {
            "position": {"x": 0, "y": 0},
            "size": {"width": 8, "height": 8},
            "data": data,
        },
    }


def sparse_color(color: str) -> list[int]:
    """Ocho LED diagonales: color verificable sin encender matriz completa."""
    value = RGB565[color]
    return [value if x == y else 0 for y in range(8) for x in range(8)]


def single_pixel(index: int) -> list[int]:
    data = [0] * 64
    data[index] = RGB565["white"]
    return data


def corner_marker(color: str, points: list[tuple[int, int]]) -> list[int]:
    data = [0] * 64
    for x, y in points:
        data[y * 8 + x] = RGB565[color]
    return data


def orientation_payload() -> dict:
    """Cuatro marcas L: identifica orden y rotación de cada panel 8x8."""
    markers = [
        ("red", [(0, 0), (1, 0), (0, 1), (0, 2)]),
        ("green", [(7, 0), (6, 0), (7, 1), (7, 2)]),
        ("blue", [(0, 7), (1, 7), (0, 6), (0, 5)]),
        ("white", [(7, 7), (6, 7), (7, 6), (7, 5)]),
    ]
    bitmaps = []
    for panel, (color, points) in enumerate(markers):
        bitmaps.append(
            {
                "position": {"x": panel * 8, "y": 0},
                "size": {"width": 8, "height": 8},
                "data": corner_marker(color, points),
            }
        )
    return {"brightness": 20, "autobrightness": False, "bitmaps": bitmaps}


def text_payload(value: str) -> dict:
    return {
        "brightness": 20,
        "autobrightness": False,
        "text": {
            "textString": value,
            "bigFont": False,
            "scrollText": False,
            "centerText": True,
            "hexColor": "#ffffff",
            "position": {"x": 0, "y": 1},
        },
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("host", help="IP o hostname de PixelIt")
    parser.add_argument(
        "command",
        choices=["status", "red", "green", "blue", "pixel", "orientation", "text", "off"],
    )
    parser.add_argument("--index", type=int, default=0, help="Píxel lógico 0..63")
    parser.add_argument("--text", default="HOLA", help="Texto para comando text")
    args = parser.parse_args()

    try:
        if args.command == "status":
            result = request(args.host, "/api/matrixinfo")
        elif args.command == "pixel":
            if not 0 <= args.index <= 63:
                parser.error("--index debe estar entre 0 y 63")
            result = request(args.host, "/api/screen", bitmap_payload(single_pixel(args.index)))
        elif args.command == "off":
            result = request(args.host, "/api/screen", bitmap_payload([0] * 64))
        elif args.command == "orientation":
            result = request(args.host, "/api/screen", orientation_payload())
        elif args.command == "text":
            result = request(args.host, "/api/screen", text_payload(args.text))
        else:
            result = request(args.host, "/api/screen", bitmap_payload(sparse_color(args.command)))
    except (OSError, urllib.error.URLError, json.JSONDecodeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1

    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
