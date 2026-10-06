# Level sources

The original 49-case campaign remains in the game. The 51 additional compact
layouts used to expand it to 100 cases are adapted from David W. Skinner's
Microban I collection as distributed in the
[`rkirov/sokoban-ai`](https://github.com/rkirov/sokoban-ai) level corpus.

Every added layout fits the game's fixed 10×15 board. Its included reference
route was solved and independently replayed against the final embedded board.
`tools/validate_levels.py` checks all 100 layouts, move counts, push counts,
score thresholds, uniqueness, solution completion, and campaign ordering.
