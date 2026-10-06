#!/usr/bin/env python3
"""Check every built-in board, score threshold, and published solution route."""

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BOARD = (ROOT / "src/core/board.c").read_text()
CHEATS = (ROOT / "src/screens/cheatsheet.c").read_text()
SCORE = (ROOT / "src/core/score.c").read_text()
HEADER = (ROOT / "src/core/board.h").read_text()
COUNT = int(re.search(r"#define LEVEL_COUNT (\d+)", HEADER).group(1))
ROWS, COLS = 10, 15

board_data = BOARD.split("char levels[LEVEL_COUNT][ROWS][COLS + 1] = {", 1)[1].split(
    "const char *BoardLevelRow", 1
)[0]
board_rows = re.findall(r'"([_#.$@+* ]{15})"', board_data)
assert len(board_rows) == COUNT * ROWS, "wrong number of board rows"
boards = [board_rows[i:i + ROWS] for i in range(0, len(board_rows), ROWS)]

route_data = CHEATS.split("} ROUTE_DATA[LEVEL_COUNT] = {", 1)[1].split(
    "static CheatRoute routes", 1
)[0]
route_entries = re.findall(
    r'\{\s*((?:"[^"]*"\s*)+),\s*(\d+),\s*(\d+)\s*\}', route_data
)
assert len(route_entries) == COUNT, "wrong number of solution routes"
route_moves = [int(moves) for _, moves, _ in route_entries]
assert route_moves == sorted(route_moves), "levels are not ordered by route length"
assert route_moves[-1] > 900, "missing the 900+ move marathon level"

score_data = SCORE.split("MINIMUM_PUSHES[LEVEL_COUNT] = {", 1)[1].split("};", 1)[0]
thresholds = [int(n) for n in re.findall(r"\d+", score_data)]
assert len(thresholds) == COUNT, "wrong number of score thresholds"

assert len({tuple(board) for board in boards}) == COUNT, "duplicate board"
DIRECTIONS = {"U": (-1, 0), "D": (1, 0), "L": (0, -1), "R": (0, 1)}

for level, (board, (route_parts, moves, pushes)) in enumerate(
    zip(boards, route_entries), 1
):
    walls, goals, crates, players = set(), set(), set(), []
    for row, line in enumerate(board):
        assert len(line) == COLS
        for col, ch in enumerate(line):
            cell = (row, col)
            if ch in "#_":
                walls.add(cell)
            if ch in ".*+":
                goals.add(cell)
            if ch in "$*":
                crates.add(cell)
            if ch in "@+":
                players.append(cell)
    assert len(players) == 1, f"level {level}: expected one player"
    assert len(goals) == len(crates) > 0, f"level {level}: crate/goal mismatch"
    player = players[0]
    route = "".join(re.findall(r'"([^"]*)"', route_parts))
    steps = "".join(
        direction * int(repeat or 1)
        for direction, repeat in re.findall(r"([UDLR])(\d*)", route)
    )
    assert len(steps) == int(moves), f"level {level}: move count mismatch"
    actual_pushes = 0
    for step, direction in enumerate(steps, 1):
        dr, dc = DIRECTIONS[direction]
        ahead = (player[0] + dr, player[1] + dc)
        assert 0 <= ahead[0] < ROWS and 0 <= ahead[1] < COLS
        assert ahead not in walls, f"level {level}: blocked move {step}"
        if ahead in crates:
            beyond = (ahead[0] + dr, ahead[1] + dc)
            assert 0 <= beyond[0] < ROWS and 0 <= beyond[1] < COLS
            assert beyond not in walls and beyond not in crates, (
                f"level {level}: blocked push {step}"
            )
            crates.remove(ahead)
            crates.add(beyond)
            actual_pushes += 1
        player = ahead
    assert crates == goals, f"level {level}: route does not solve board"
    assert actual_pushes == int(pushes), f"level {level}: push count mismatch"
    assert thresholds[level - 1] == actual_pushes, (
        f"level {level}: score threshold mismatch"
    )

print(f"Validated {COUNT} distinct, solvable levels and their score thresholds.")
