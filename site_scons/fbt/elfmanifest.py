from dataclasses import dataclass
import os

import struct
from dataclasses import dataclass, field

from .appmanifest import FlipperApplication


_MANIFEST_MAGIC = 0x52474448


@dataclass
class ElfManifestBaseHeader:
    manifest_version: int
    api_version: int

    manifest_magic: int = 0x52474448

    def as_bytes(self):
        return struct.pack(
            "<III",
            self.manifest_magic,
            self.manifest_version,
            self.api_version,
        )


@dataclass
class ElfManifestV1:
    stack_size: int
    app_version: int
    name: str
    icon: bytes = field(default=b"")

    def as_bytes(self):
        return struct.pack(
            "<hI32s?32s",
            self.stack_size,
            self.app_version,
            bytes(self.name.encode("ascii")),
            bool(self.icon),
            self.icon,
        )


def assemble_manifest_data(app_manifest: FlipperApplication, sdk_version):
    image_data = b""
    if app_manifest.fapp_icon:
        from flipper.assets.icon import file2image

        image = file2image(os.path.join(app_manifest._apppath, app_manifest.fapp_icon))
        if (image.width, image.height) != (10, 10):
            raise ValueError(
                f"Flipper app icon must be 10x10 pixels, but {image.width}x{image.height} was given"
            )
        if len(image.data) > 32:
            raise ValueError(
                f"Flipper app icon must be 32 bytes or less, but {len(image.data)} bytes were given"
            )
        image_data = image.data

    app_version_as_int = ((app_manifest.version[0] & 0xFFFF) << 16) | (
        app_manifest.version[1] & 0xFFFF
    )

    data = ElfManifestBaseHeader(1, sdk_version).as_bytes()
    data += ElfManifestV1(
        app_manifest.stack_size, app_version_as_int, app_manifest.name, image_data
    ).as_bytes()

    return data
