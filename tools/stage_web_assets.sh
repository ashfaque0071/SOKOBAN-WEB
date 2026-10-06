#!/bin/sh
# Builds a size-reduced copy of assets/ for the browser build.
#
# The source art is authored far larger than the game ever draws it: board
# tiles are 1254px square but capped at 68px on screen, and the completion
# plate is 5120x2880 on an 1512x880 window. Loading that from a local disk is
# free; downloading it is not, so the web build ships a resized, requantised
# copy. assets/ itself is never touched.
#
# Caps below are each asset's largest on-screen footprint rounded up to a power
# of two. Geometry is safe to rescale because every source rectangle in the
# game is computed as a fraction of the texture (see PauseInk in ui.c), never
# in absolute source pixels.
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$script_dir/.."

source_dir=assets
stage_dir=${1:-build/web/assets}

for tool in sips oxipng pngquant; do
  if ! command -v "$tool" >/dev/null 2>&1; then
    echo "Error: $tool is required. Install it with: brew install oxipng pngquant" >&2
    exit 1
  fi
done

# Longest-side pixel cap by asset. 1512x880 is the whole window.
cap_for() {
  case ${1#"$source_dir"/} in
    # Full-window art.
    menu/menu_background.png|gameplay/screen_main_menu.png) echo 1512 ;;
    levels/screen_level_select_decorated.png) echo 1512 ;;
    cheatsheet/cheatsheet_background.png|cheatsheet/overlay_cheatsheet_locked.png) echo 1512 ;;
    completion/completion_noir.png|completion/edge_collage_overlay.png) echo 1512 ;;
    high_scores/panel_high_scores.png) echo 1512 ;;
    settings/settings_panel.png) echo 1512 ;;
    gameplay/ui/ui_hud_panel_noir.png) echo 1512 ;;

    # Large panels and the title plate.
    menu/menu_panel_large.png|pause/pause_panel.png) echo 1024 ;;
    cheatsheet/panel_cheatsheet.png) echo 1024 ;;
    settings/reset_confirmation_panel_v2.png) echo 1024 ;;
    menu/title_logo.png) echo 1024 ;;
    gameplay/ui/ui_icons.png) echo 1024 ;;

    # Board tiles, characters and the small icon set: drawn at 68px or less.
    gameplay/tiles/*|gameplay/character/*) echo 256 ;;
    shared/icon_*) echo 256 ;;
    completion/star_*|levels/star_gold.png|levels/star_grey.png) echo 256 ;;
    cheatsheet/direction_arrow_*) echo 256 ;;

    # Buttons, labels, cards, stamps and tabs.
    *) echo 512 ;;
  esac
}

rm -rf "$stage_dir"
mkdir -p "$stage_dir"

echo "Staging assets for the web build..."

find "$source_dir" -type f ! -name '.DS_Store' | while IFS= read -r file; do
  relative=${file#"$source_dir"/}
  target="$stage_dir/$relative"
  mkdir -p "$(dirname -- "$target")"

  case $file in
    *.png)
      cap=$(cap_for "$file")
      width=$(sips -g pixelWidth "$file" | awk '/pixelWidth/{print $2}')
      height=$(sips -g pixelHeight "$file" | awk '/pixelHeight/{print $2}')
      longest=$width
      [ "$height" -gt "$longest" ] && longest=$height

      if [ "$longest" -gt "$cap" ]; then
        sips -Z "$cap" "$file" --out "$target" >/dev/null
      else
        cp -- "$file" "$target"
      fi

      # Quantise, then recompress losslessly. pngquant declines to write a file
      # it cannot keep inside the quality floor, so a refusal is not an error:
      # that asset simply stays at full colour depth.
      pngquant --quality=65-95 --speed 1 --strip \
               --force --output "$target" "$target" 2>/dev/null || true
      oxipng --quiet --opt 4 --strip safe "$target" >/dev/null 2>&1 || true
      ;;
    assets/audio/music/finale.wav)
      # MP3 instead of 22s of raw PCM. audio.c points at the .mp3 on web.
      ffmpeg -nostdin -loglevel error -y -i "$file" \
             -c:a libmp3lame -b:a 128k "${target%.wav}.mp3"
      ;;
    *)
      cp -- "$file" "$target"
      ;;
  esac
done

before=$(du -sk "$source_dir" | awk '{print $1}')
after=$(du -sk "$stage_dir" | awk '{print $1}')
echo "Assets staged in $stage_dir"
printf 'Size: %.1f MiB -> %.1f MiB\n' \
  "$(echo "$before" | awk '{print $1/1024}')" \
  "$(echo "$after" | awk '{print $1/1024}')"
