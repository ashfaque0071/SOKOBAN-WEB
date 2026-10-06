# Sokoban browser build

This is a separate browser copy of the native Sokoban project. Its source art
stays in `assets/`; the web build creates smaller copies without modifying it.

## Build and test

```sh
RAYLIB_WEB=/path/to/raylib/lib ./run_web.sh --build-only
python3 -m http.server 8765 --directory build/web
```

The web build needs Emscripten, raylib 6.0 compiled for WebGL 2, `sips`,
`pngquant`, `oxipng`, `ffmpeg`, Python, and `zip`. If `libraylib.web.a` and
`raylib.h` are in the same directory, point `RAYLIB_WEB` there. A `lib/` and
`include/` sibling layout also works.

The menu's art, fonts and sounds are preloaded with the game code. After the
menu appears, `content.data` downloads in the background. Choosing Continue,
Level Select, Settings, or Credits while it is downloading shows a percentage
and waits for the required files. The 100 level layouts themselves are only a
few kilobytes and are compiled into WebAssembly; shared gameplay art and other
screens make up the background package. The package is SHA-256 checked before
it is installed into the in-memory filesystem.

Progress, settings, and personal bests use IndexedDB for this browser origin.
They do not transfer between devices or domains. There is no online score
backend or shared leaderboard.

## Phone controls

Tap anywhere on the title screen to enable audio and open the game. Touch the
game's own menu and HUD buttons for navigation, Hint, Undo, Restart, and Pause.
The only controls outside the game picture are the four movement arrows, shown
during levels.
Holding an arrow repeats movement when that option is enabled in Settings.
Rotate to landscape for a larger game view. Desktop keyboard and mouse
controls are unchanged.

## Deploy to Vercel

`build/web/` is a standalone static site, including `vercel.json`. After
building, deploy that folder with the Vercel CLI:

```sh
vercel deploy build/web --prod --yes --project sokoban-web
```

The root `sokoban-web.zip` contains the same playable files for itch.io or
other static hosts. Serve the files over HTTP(S); opening `index.html` directly
from disk cannot load WebAssembly.

## Desktop behavior

The desktop build still loads assets and audio at launch and keeps its existing
save paths under `data/`. The browser-only build uses a WebGL shader and MP3
menu music, and saves under `/data` in IndexedDB.

## Rights note

`assets/audio/music/bgmus.mp3` is Michael Giacchino's Spider-Man theme and
`assets/completion/completion_noir.png` is Spider-Noir key art. Both are Sony's
and are included in the web package. Replace them if you do not have rights to
publish them.
