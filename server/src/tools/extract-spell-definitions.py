#!/usr/bin/env python3
"""Extract the Alpha client Spell.dbc data used by the standalone server.

The recovered client keeps the 0.5.3 DBC files in dbc.MPQ.  This creates a
small JSON projection with spell effects, cast times, ranges, and durations so
the C# server can remain self-contained at runtime.
"""

from __future__ import annotations

import argparse
import json
import struct
from pathlib import Path
from typing import Iterable

import mpyq


def read_dbc(archive: mpyq.MPQArchive, name: str) -> list[tuple[int, ...]]:
    data = archive.read_file(f"DBFilesClient\\{name}.dbc", force_decompress=True)
    if not data or data[:4] != b"WDBC":
        raise ValueError(f"Could not read {name}.dbc")

    record_count, field_count, record_size, string_size = struct.unpack_from("<4I", data, 4)
    if record_size != field_count * 4:
        raise ValueError(f"Unexpected {name}.dbc record layout")

    records = []
    offset = 20
    for _ in range(record_count):
        records.append(struct.unpack_from(f"<{field_count}I", data, offset))
        offset += record_size
    if offset + string_size > len(data):
        raise ValueError(f"Unexpected {name}.dbc string block")
    return records


def signed(value: int) -> int:
    return value if value < 0x80000000 else value - 0x100000000


def as_float(value: int) -> float:
    return struct.unpack("<f", struct.pack("<I", value))[0]


def read_spell_support(archive: mpyq.MPQArchive) -> tuple[dict[int, int], dict[int, tuple[float, float]], dict[int, int]]:
    cast_times = {
        row[0]: max(0, signed(row[1]))
        for row in read_dbc(archive, "SpellCastTimes")
    }
    ranges = {
        row[0]: (as_float(row[1]), as_float(row[2]))
        for row in read_dbc(archive, "SpellRange")
    }
    durations = {
        row[0]: signed(row[1])
        for row in read_dbc(archive, "SpellDuration")
    }
    return cast_times, ranges, durations


def project_spells(archive: mpyq.MPQArchive) -> Iterable[dict[str, object]]:
    cast_times, ranges, durations = read_spell_support(archive)
    for row in read_dbc(archive, "Spell"):
        spell_id = row[0]
        if spell_id == 0:
            continue

        effects = []
        for index in range(3):
            effects.append({
                "effect_type": signed(row[53 + index]),
                "die_sides": signed(row[56 + index]),
                "base_dice": signed(row[59 + index]),
                "dice_per_level": signed(row[62 + index]),
                "real_points_per_level": as_float(row[65 + index]),
                "base_points": signed(row[68 + index]),
                "implicit_target_a": signed(row[71 + index]),
                "implicit_target_b": signed(row[74 + index]),
                "radius_index": signed(row[77 + index]),
                "aura_type": signed(row[80 + index]),
                "aura_period": signed(row[83 + index]),
                "amplitude": as_float(row[86 + index]),
                "chain_targets": signed(row[89 + index]),
                "item_type": signed(row[92 + index]),
                "misc_value": signed(row[95 + index]),
                "trigger_spell": row[98 + index],
            })

        cast_index = row[12]
        range_index = row[30]
        duration_index = row[24]
        min_range, max_range = ranges.get(range_index, (0.0, 30.0))
        effect_fields = {
            f"{key}{index + 1}": value
            for index, effect in enumerate(effects)
            for key, value in effect.items()
        }
        yield {
            "id": spell_id,
            "school": row[1],
            "category": row[2],
            "cast_ui": row[3],
            "attributes": row[4],
            "attributes_ex": row[5],
            "targets": row[7],
            "casting_time_index": cast_index,
            "cast_time_ms": cast_times.get(cast_index, 0),
            "recovery_time": row[13],
            "category_recovery_time": row[14],
            "range_index": range_index,
            "min_range": min_range,
            "max_range": max_range,
            "speed": as_float(row[31]),
            "mana_cost": row[26],
            "duration_index": duration_index,
            "duration_ms": durations.get(duration_index, 0),
            **effect_fields,
        }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--mpq", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    archive = mpyq.MPQArchive(str(args.mpq), listfile=False)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(
        json.dumps(list(project_spells(archive)), separators=(",", ":")),
        encoding="utf-8",
    )
    print(f"Extracted Spell.dbc to {args.output}")


if __name__ == "__main__":
    main()
