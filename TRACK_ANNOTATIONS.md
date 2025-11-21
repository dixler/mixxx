# Track Annotations Feature

## Overview

Track annotations allow you to add colored indicators with labels underneath a track's waveform to annotate the track structure or points of interest. This helps DJs understand the song structure at a glance.

## Usage

### Creating Annotation Files

To add annotations to a track, create a JSON file alongside the audio file with the extension `.annotations.json`.

For example, if your track is named:
- `song.mp3` → create `song.annotations.json`
- `my_track.flac` → create `my_track.annotations.json`

### JSON Format

The annotation file should contain a JSON object with an `annotations` array:

```json
{
  "annotations": [
    {
      "start": 0.0,
      "end": 30.0,
      "label": "Intro",
      "color": "#0000FF"
    },
    {
      "start": 30.0,
      "end": 90.0,
      "label": "Verse",
      "color": "#00FF00"
    },
    {
      "start": 90.0,
      "end": 120.0,
      "label": "Chorus",
      "color": "#FF0000"
    }
  ]
}
```

### Annotation Fields

Each annotation object supports the following fields:

- **start** (number, required): Start time in seconds
- **end** (number, required): End time in seconds
- **label** (string, required): Text label to display
- **color** (string, required): Color in hex format (e.g., `#FF0000` for red)

### Examples

#### Example 1: Simple Song Structure

```json
{
  "annotations": [
    {"start": 0, "end": 30, "label": "Intro", "color": "#0000FF"},
    {"start": 30, "end": 90, "label": "Verse 1", "color": "#00FF00"},
    {"start": 90, "end": 120, "label": "Chorus", "color": "#FF0000"},
    {"start": 120, "end": 180, "label": "Verse 2", "color": "#00FF00"},
    {"start": 180, "end": 210, "label": "Chorus", "color": "#FF0000"},
    {"start": 210, "end": 240, "label": "Outro", "color": "#FFFF00"}
  ]
}
```

#### Example 2: DJ Cue Points

```json
{
  "annotations": [
    {"start": 0, "end": 16, "label": "Count-in", "color": "#808080"},
    {"start": 16, "end": 48, "label": "Build", "color": "#FFA500"},
    {"start": 48, "end": 80, "label": "Drop", "color": "#FF0000"},
    {"start": 80, "end": 144, "label": "Breakdown", "color": "#00FFFF"},
    {"start": 144, "end": 176, "label": "Build 2", "color": "#FFA500"},
    {"start": 176, "end": 240, "label": "Drop 2", "color": "#FF0000"}
  ]
}
```

## How It Works

1. When a track is loaded in Mixxx, the system automatically looks for a `.annotations.json` file next to the audio file.
2. If found, the annotations are parsed and loaded.
3. The annotations are rendered as semi-transparent colored rectangles underneath the waveform.
4. Labels are displayed at the bottom of each annotation (on horizontal waveforms).

## Technical Notes

- Times are in seconds (decimal values supported)
- Colors must be valid hex color codes (e.g., `#RRGGBB`)
- Invalid annotations (negative times, start >= end) are skipped
- Annotations are not saved to the database - they're loaded from the file each time
- If the annotation file is missing or invalid, no error is shown (track loads normally)
