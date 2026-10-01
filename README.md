# Sokoban

A detective-noir Sokoban puzzle game written in C99 with
[raylib](https://www.raylib.com/). The game includes 16 levels, scoring and
star ratings, undo and hint systems, level selection, persistent progress,
configurable movement, music, and sound effects.

## Project team

- Ashfaque Ahmed Nur — Student ID: 2505103
- Imtiaz Ahmed — Student ID: 2505113

- **Department:** Computer Science and Engineering (CSE)
- **University:** Bangladesh University of Engineering and Technology (BUET)
- **Supervisor:** Junaed Younous Khan

> [!IMPORTANT]
> Build and run the game from the repository root. The game loads assets and
> save data with relative paths. The supplied scripts switch to the correct
> directory automatically.

## Dependencies

| Platform | Required software | Notes |
| --- | --- | --- |
| All builds | A C99 compiler and raylib | The bundled Windows files target raylib 5.5. The macOS build also compiles with raylib 6.0. |
| macOS | Xcode Command Line Tools, Homebrew, and the Homebrew `raylib` formula | `run.sh` links the Cocoa, OpenGL, IOKit, and CoreVideo system frameworks. |
| Windows | 64-bit MinGW-w64 GCC | The required raylib 5.5 headers, x86-64 DLL import library, and `raylib.dll` are included in the repository. Visual C++ is not supported by the provided script. |

Beyond the platform dependencies listed above, no additional C libraries,
environment variables, asset downloads, or asset-generation steps are
required. All textures, fonts, shaders, music, and sound effects used by the
game are included in `assets/`.

## Quick start

### macOS

Install the required developer tools and raylib:

```sh
xcode-select --install
brew install raylib
```

From the repository root, build and start the game:

```sh
./run.sh
```

The script finds Clang and Homebrew, compiles the project to
`build/sokoban`, and then starts it.

If macOS reports that `run.sh` is not executable, fix its permission once and
try again:

```sh
chmod +x run.sh
./run.sh
```

### Windows

1. Install a **64-bit MinGW-w64 GCC** toolchain.
2. Add the toolchain's `bin` directory to `PATH`.
3. Open Command Prompt or PowerShell in the repository root.
4. Double-click `build.bat`, or run the build script from a terminal:

   **Command Prompt**

   ```bat
   build.bat
   ```

   **PowerShell**

   ```powershell
   .\build.bat
   ```

The script builds `build\sokoban.exe`, copies the bundled `raylib.dll` next
to it, and starts the game. raylib headers, the MinGW import library, and the
runtime DLL required by the Windows build are already included in `include\`
and `lib\`.

## Compiling without immediately running

Both provided scripts accept `--build-only` as their first argument.

### macOS

```sh
./run.sh --build-only
./build/sokoban
```

### Windows Command Prompt

```bat
build.bat --build-only
build\sokoban.exe
```

### Windows PowerShell

```powershell
.\build.bat --build-only
.\build\sokoban.exe
```

## Controls

| Context | Input | Action |
| --- | --- | --- |
| Menus | Left click | Select menu items and on-screen controls |
| Gameplay | Arrow keys or `W`, `A`, `S`, `D` | Move the player and push crates |
| Gameplay | `U` | Undo one move |
| Gameplay | `R` | Restart the current level |
| Gameplay | `P` | Pause the game |
| Gameplay | `M` | Mute or unmute audio |
| Gameplay | `H` | Show a hint |
| Paused | `P` or the Resume button | Resume the game |
| Level complete | `Enter` | Continue to the next level |
| Level complete | `R` | Replay the completed level |
| Developer shortcut | `[` / `]` | Load the previous / next level |

The objective is to push every crate onto a goal tile. Crates can be pushed
but cannot be pulled, so plan ahead to avoid trapping one against a wall or in
a corner.

## Configuration and saved data

Always launch the executable **from the repository root**. Asset and save-file
paths are relative to the current working directory:

```text
assets/...
data/progress.txt
data/settings.txt
```

For example, use `./build/sokoban` while your terminal is in the repository
root. Running `./sokoban` from inside `build/` will prevent the game from
finding its textures, fonts, shaders, audio, and data files.

The `data/` directory must be writable if you want progress and settings to
persist. The game updates these files during normal use:

- `data/progress.txt` stores the current level, scores, star ratings, pushes,
  and move records.
- `data/settings.txt` stores sound and movement preferences.

The project includes initial versions of both files, so no manual
configuration is needed for a first run. Master sound, music volume, effects
volume, movement mode, and movement speed are changed through the in-game
Settings screen and saved when that screen is closed.

The Settings screen's reset action clears all level progress and score
records, then returns the game to Level 1. It does not erase the selected
sound or movement settings.

On Windows, keep `build\raylib.dll` beside `build\sokoban.exe`. The provided
build script copies it automatically.

## Project layout

```text
.
├── assets/          Textures, fonts, shaders, music, and sound effects
├── build/           Compiled executable and Windows runtime DLL
├── data/            Persistent progress and settings
├── docs/            Level cheatsheet and third-party license notices
├── include/         Bundled raylib 5.5 headers for the Windows build
├── lib/             Bundled Windows raylib libraries and DLL
├── src/
│   ├── audio/       Music and sound-effect management
│   ├── core/        Game loop, board logic, scoring, hints, and saves
│   ├── render/      Asset loading and shared UI rendering
│   └── screens/     Menu, settings, pause, level, and results screens
├── build.bat        Windows build-and-run script
└── run.sh           macOS build-and-run script
```

Every `.c` file in the four `src/` subdirectories is compiled by the build
scripts. New source files placed elsewhere must be added to the relevant build
command.

Key modules include:

- `src/core/sokoban.c`: application startup, screen transitions, main loop,
  and shutdown;
- `src/core/board.c`: built-in level maps, player movement, crate movement,
  undo history, and board rendering;
- `src/core/score.c`: scores, star ratings, best records, and level unlocking;
- `src/core/hint.c`: state-aware gameplay hints;
- `src/core/saves.c`: progress and settings persistence;
- `src/audio/audio.c`: music, sound effects, volume, and mute handling;
- `src/render/assets.c`: textures, fonts, and shader loading; and
- `src/screens/`: menu, settings, pause, level-select, completion, controls,
  credits, high-score, and cheatsheet screens.

The optional solution reference in `docs/LEVEL_CHEATSHEET.md` lists a complete
move sequence for every level.

## Troubleshooting

### `clang is required` on macOS

Install or repair the Xcode Command Line Tools:

```sh
xcode-select --install
```

### `Homebrew is required` or `raylib is not installed` on macOS

Install [Homebrew](https://brew.sh/) if necessary, then install raylib:

```sh
brew install raylib
```

### `GCC for Windows is required`

Install a 64-bit MinGW-w64 GCC distribution and ensure its `bin` directory is
on `PATH`. Open a new terminal and verify it with:

```bat
gcc --version
```

### Windows reports that `raylib.dll` is missing

Re-run `build.bat`; it copies `lib\raylib.dll` to `build\raylib.dll`. If the
executable is moved elsewhere, copy the DLL with it.

### The window opens but assets or audio are missing

Close the game, change to the repository root, and launch the executable from
there. Do not start it with `build/` as the working directory.

### Progress or settings do not persist

Check that `data/progress.txt` and `data/settings.txt` exist and that your user
account can write to both files and the `data/` directory.

### The game has no sound

Check that master sound is enabled and both volume sliders are above zero in
Settings. Also verify that the operating system has an active audio output
device. The music and sound-effect files are loaded from `assets/audio/`.

## Third-party notices

The bundled Windows headers and libraries are raylib 5.5. Its license is in
`docs/licenses/raylib_LICENSE.txt`. Font license notices are stored beside the
fonts:

- `assets/fonts/LICENSE-RobotoSlab.txt`
- `assets/fonts/OFL-Oswald.txt`
- `assets/fonts/OFL-Rye.txt`

No separate license for the remaining project code or assets is declared in
this repository.
