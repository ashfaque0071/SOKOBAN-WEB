// Runs before main(). Gives the game a real, persistent /data directory.
//
// Emscripten's default filesystem is in-memory, so the save files the game
// writes would vanish on reload. IDBFS backs /data with IndexedDB instead;
// the syncfs(true) below reads any existing save in before main() starts, and
// SavesCommit() in saves.c pushes each write back out.
Module["preRun"] = Module["preRun"] || [];
Module["preRun"].push(function () {
  try {
    FS.mkdir("/data");
    FS.mount(IDBFS, {}, "/data");
  } catch (err) {
    console.error("sokoban: could not mount save storage", err);
    return;
  }

  // Hold main() until the save has actually been read out of IndexedDB.
  Module.addRunDependency("sokoban-load-saves");
  FS.syncfs(true, function (err) {
    if (err) console.error("sokoban: could not read saved progress", err);
    Module.removeRunDependency("sokoban-load-saves");
  });
});
