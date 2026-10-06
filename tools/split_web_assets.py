#!/usr/bin/env python3
"""Keep the menu in Emscripten's preload; fetch the rest after it appears."""

import hashlib
import json
import shutil
import sys
from pathlib import Path


STARTUP = {
    "menu/menu_background.png",
    "menu/title_logo.png",
    "menu/menu_panel_large.png",
    "menu/menu_text_continue.png",
    "menu/menu_text_level_select.png",
    "menu/menu_text_settings.png",
    "menu/menu_text_quit.png",
    "shared/button_normal.png",
    "shared/button_hover.png",
    "shared/icon_continue.png",
    "shared/icon_settings.png",
    "fonts/Rye-Regular.ttf",
    "fonts/Oswald-Bold.ttf",
    "audio/music/finale.mp3",
    "audio/sfx/ui_hover.wav",
    "audio/sfx/ui_click.wav",
}


def main() -> None:
    source, startup, output = map(Path, sys.argv[1:4])
    shutil.rmtree(startup, ignore_errors=True)
    startup.mkdir(parents=True)
    output.mkdir(parents=True, exist_ok=True)
    seen = set()
    entries = []
    digest = hashlib.sha256()
    offset = 0
    with (output / "content.data").open("wb") as archive:
        for path in sorted(p for p in source.rglob("*") if p.is_file()):
            relative = path.relative_to(source).as_posix()
            seen.add(relative)
            if relative in STARTUP:
                target = startup / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(path, target)
                continue
            contents = path.read_bytes()
            archive.write(contents)
            digest.update(contents)
            entries.append({"path": "/assets/" + relative,
                            "start": offset, "end": offset + len(contents)})
            offset += len(contents)

    missing = STARTUP - seen
    if missing:
        raise SystemExit("missing menu assets: " + ", ".join(sorted(missing)))
    (output / "content.json").write_text(json.dumps({
        "sha256": digest.hexdigest(), "size": offset, "files": entries
    }, separators=(",", ":")) + "\n")
    startup_size = sum(path.stat().st_size for path in startup.rglob("*")
                       if path.is_file())
    print(f"  menu assets: {startup_size / 1048576:.2f} MiB")
    print(f"  background content: {offset / 1048576:.2f} MiB")


if __name__ == "__main__":
    main()
