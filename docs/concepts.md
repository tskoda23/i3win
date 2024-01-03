# Concepts

This document will explain concepts that are used in this project.

## Main window

Each layout is built from a list of windows. First window in a list is given a special treatment and we call it "Main window". It has [certain configuration options](docs/config.md) that can be applied to it, like a custom size to make it bigger or smaller than other windows in a layout, along with key bindings to resize it dynamically.

Main window is the leftmost one in the split/stacked layouts, and center one in the centered layout.

## Layouts

Layout is an arrangement of windows on a certain screen. There are few layouts currently supported:
- `LAYOUT_TYPE_SPLIT` - stack windows side by side, split vertically
- `LAYOUT_TYPE_CENTERED` - main window in center, all the others tile around it
- `LAYOUT_TYPE_STACKED` - stacks windows on top of each other, with some margin at their ends as an indicator of other windows