# Sokoban

A detective-noir Sokoban puzzle game written in C99 with
[raylib](https://www.raylib.com/). The game includes 16 levels, scoring and
star ratings, undo and hint systems, level selection, persistent progress,
configurable movement, music, and sound effects.

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

### Windows

1. Install a **64-bit MinGW-w64 GCC** toolchain.
2. Add the toolchain's `bin` directory to `PATH`.
3. Open Command Prompt or PowerShell in the repository root.
4. Run the build script:

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

## Dependencies

| Platform | Required software | Notes |
| --- | --- | --- |
| All builds | A C99 compiler and raylib | The bundled Windows files target raylib 5.5. The macOS build also compiles with raylib 6.0. |
| macOS | Xcode Command Line Tools, Homebrew, and the Homebrew `raylib` formula | `run.sh` links the Cocoa, OpenGL, IOKit, and CoreVideo system frameworks. |
| Windows | 64-bit MinGW-w64 GCC | The required raylib 5.5 headers, x86-64 DLL import library, and `raylib.dll` are included in the repository. Visual C++ is not supported by the provided script. |

Beyond the platform dependencies listed above, no additional C libraries,
environment variables, or asset-generation steps are required.

Third-party license notices are kept in:

- `docs/licenses/raylib_LICENSE.txt`
- `assets/fonts/LICENSE-RobotoSlab.txt`
- `assets/fonts/OFL-Oswald.txt`
- `assets/fonts/OFL-Rye.txt`

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

## Important runtime setup

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

- `data/progress.txt` stores the current unlocked level and best scores.
- `data/settings.txt` stores sound and movement preferences.

The project includes initial versions of both files, so no manual
configuration is needed for a first run. Progress can also be erased from the
in-game settings screen.

On Windows, keep `build\raylib.dll` beside `build\sokoban.exe`. The provided
build script copies it automatically.

## Controls

| Input | Action |
| --- | --- |
| Arrow keys or `W`, `A`, `S`, `D` | Move the player and push crates |
| `U` | Undo one move |
| `R` | Restart the current level; replay after completion |
| `P` | Pause during gameplay |
| `M` | Mute or unmute audio |
| `H` | Show a hint |
| `Enter` | Continue to the next level after completion |
| Mouse | Navigate menus and on-screen controls |

The objective is to push every crate onto a goal tile. Crates can be pushed
but cannot be pulled, so plan ahead to avoid trapping one against a wall or in
a corner.

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
