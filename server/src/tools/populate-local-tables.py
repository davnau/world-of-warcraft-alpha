#!/usr/bin/env python3
"""Convert the Alpha Core SQL seed rows used by the standalone server to JSON.

This imports the tables that the current C# runtime consumes:
realmlist, playercreateinfo*, item_template, ChrRaces, and CharStartOutfit.
With --include-all-world it also preserves every Alpha Core world table under
world/. Account and character files are left intact so local
accounts/characters are not overwritten by a data refresh.
"""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path
from typing import Any, Iterable


def find_table_columns(sql: str, table: str) -> list[str]:
    pattern = re.compile(
        rf"CREATE\s+TABLE\s+`{re.escape(table)}`\s*\((.*?)\)\s+ENGINE=",
        re.IGNORECASE | re.DOTALL,
    )
    match = pattern.search(sql)
    if not match:
        raise ValueError(f"Could not find CREATE TABLE for {table}")

    columns: list[str] = []
    for line in match.group(1).splitlines():
        column = re.match(r"\s*`([^`]+)`\s+", line)
        if column:
            columns.append(column.group(1))
    if not columns:
        raise ValueError(f"Could not find columns for {table}")
    return columns


def statement_end(sql: str, start: int) -> int:
    quoted = False
    index = start
    while index < len(sql):
        character = sql[index]
        if quoted:
            if character == "\\":
                index += 2
                continue
            if character == "'":
                if index + 1 < len(sql) and sql[index + 1] == "'":
                    index += 2
                    continue
                quoted = False
        elif character == "'":
            quoted = True
        elif character == ";":
            return index
        index += 1
    raise ValueError("Unterminated INSERT statement")


def parse_string(token: str) -> str:
    body = token[1:-1]
    result: list[str] = []
    index = 0
    escapes = {
        "0": "\0",
        "b": "\b",
        "n": "\n",
        "r": "\r",
        "t": "\t",
        "Z": "\x1a",
    }
    while index < len(body):
        character = body[index]
        if character == "\\" and index + 1 < len(body):
            next_character = body[index + 1]
            result.append(escapes.get(next_character, next_character))
            index += 2
        elif character == "'" and index + 1 < len(body) and body[index + 1] == "'":
            result.append("'")
            index += 2
        else:
            result.append(character)
            index += 1
    return "".join(result)


def scalar(token: str) -> Any:
    token = token.strip()
    if token.upper() == "NULL":
        return None
    if len(token) >= 2 and token[0] == "'" and token[-1] == "'":
        return parse_string(token)
    if re.fullmatch(r"[-+]?0[xX][0-9a-fA-F]+", token):
        return int(token, 16)
    if re.fullmatch(r"[-+]?\d+", token):
        return int(token)
    if re.fullmatch(r"[-+]?(?:\d+\.\d*|\.\d+)(?:[eE][-+]?\d+)?", token):
        return float(token)
    return token


def parse_value_tuples(payload: str) -> list[list[Any]]:
    rows: list[list[Any]] = []
    index = 0
    while index < len(payload):
        while index < len(payload) and (payload[index].isspace() or payload[index] == ","):
            index += 1
        if index >= len(payload):
            break
        if payload[index] != "(":
            raise ValueError(f"Expected row at: {payload[index:index + 40]!r}")

        index += 1
        fields: list[str] = []
        field_start = index
        quoted = False
        while index < len(payload):
            character = payload[index]
            if quoted:
                if character == "\\":
                    index += 2
                    continue
                if character == "'":
                    if index + 1 < len(payload) and payload[index + 1] == "'":
                        index += 2
                        continue
                    quoted = False
            elif character == "'":
                quoted = True
            elif character == ",":
                fields.append(payload[field_start:index])
                field_start = index + 1
            elif character == ")":
                fields.append(payload[field_start:index])
                rows.append([scalar(field) for field in fields])
                index += 1
                break
            index += 1
        else:
            raise ValueError("Unterminated value tuple")
    return rows


def read_rows(sql: str, table: str) -> list[dict[str, Any]]:
    columns = find_table_columns(sql, table)
    marker = re.compile(
        rf"INSERT\s+INTO\s+`{re.escape(table)}`\s+VALUES\s*",
        re.IGNORECASE,
    )
    rows: list[dict[str, Any]] = []
    for match in marker.finditer(sql):
        end = statement_end(sql, match.end())
        for values in parse_value_tuples(sql[match.end():end]):
            if len(values) != len(columns):
                raise ValueError(
                    f"{table} row has {len(values)} values; expected {len(columns)}"
                )
            rows.append(dict(zip(columns, values)))
    return rows


def number(row: dict[str, Any], key: str, default: int = 0) -> int:
    value = row.get(key, default)
    if value is None or value == "":
        return default
    return int(float(value))


def text(row: dict[str, Any], key: str) -> str:
    value = row.get(key)
    return "" if value is None else str(value)


def write_json(output: Path, name: str, rows: Iterable[Any]) -> int:
    values = list(rows)
    path = output / name
    path.write_text(json.dumps(values, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    return len(values)


def import_all_world_tables(world_sql: str, output: Path) -> dict[str, int]:
    """Write every Alpha Core world table as world/<table>.json."""
    world_output = output / "world"
    world_output.mkdir(parents=True, exist_ok=True)
    table_names = re.findall(r"CREATE\s+TABLE\s+`([^`]+)`", world_sql, re.IGNORECASE)
    counts: dict[str, int] = {}
    for table in dict.fromkeys(table_names):
        rows = read_rows(world_sql, table)
        counts[table] = write_json(world_output, f"{table}.json", rows)
    manifest = {
        "source": "Alpha Core world.sql",
        "tables": [{"name": name, "rows": count} for name, count in counts.items()],
    }
    (world_output / "manifest.json").write_text(
        json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
    )
    return counts


def convert(alpha_root: Path, output: Path) -> dict[str, int]:
    world_sql = (alpha_root / "etc" / "databases" / "world" / "world.sql").read_text(
        encoding="utf-8", errors="replace"
    )
    auth_sql = (alpha_root / "etc" / "databases" / "auth" / "auth.sql").read_text(
        encoding="utf-8", errors="replace"
    )
    dbc_sql = (alpha_root / "etc" / "databases" / "dbc" / "dbc.sql").read_text(
        encoding="utf-8", errors="replace"
    )
    output.mkdir(parents=True, exist_ok=True)
    counts: dict[str, int] = {}

    realms = read_rows(auth_sql, "realmlist")
    counts["realms.json"] = write_json(
        output,
        "realms.json",
        (
            {
                "Id": number(row, "realm_id"),
                "Name": text(row, "realm_name"),
                "Address": "127.0.0.1" if text(row, "realm_address") in ("", "0.0.0.0") else text(row, "realm_address"),
                "Port": number(row, "realm_port"),
                "ProxyAddress": "127.0.0.1" if text(row, "proxy_address") in ("", "0.0.0.0") else text(row, "proxy_address"),
                "ProxyPort": number(row, "proxy_port"),
                "OnlinePlayers": number(row, "online_player_count"),
            }
            for row in realms
        ),
    )

    locations = read_rows(world_sql, "playercreateinfo")
    counts["playercreateinfo.json"] = write_json(
        output,
        "playercreateinfo.json",
        (
            {
                "Race": number(row, "race"),
                "Class": number(row, "class"),
                "Map": number(row, "map"),
                "Zone": number(row, "zone"),
                "PositionX": float(row.get("position_x") or 0),
                "PositionY": float(row.get("position_y") or 0),
                "PositionZ": float(row.get("position_z") or 0),
                "Orientation": float(row.get("orientation") or 0),
            }
            for row in locations
        ),
    )

    actions = read_rows(world_sql, "playercreateinfo_action")
    counts["playercreateinfo_action.json"] = write_json(
        output,
        "playercreateinfo_action.json",
        (
            {
                "Race": number(row, "race"),
                "Class": number(row, "class"),
                "Button": number(row, "button"),
                "Action": number(row, "action"),
            }
            for row in actions
        ),
    )

    items = read_rows(world_sql, "playercreateinfo_item")
    counts["playercreateinfo_item.json"] = write_json(
        output,
        "playercreateinfo_item.json",
        (
            {
                "Id": number(row, "id"),
                "Race": number(row, "race"),
                "Class": number(row, "class"),
                "ItemId": number(row, "itemid"),
                "Amount": number(row, "amount", 1),
            }
            for row in items
        ),
    )

    spells = read_rows(world_sql, "playercreateinfo_spell")
    counts["playercreateinfo_spell.json"] = write_json(
        output,
        "playercreateinfo_spell.json",
        (
            {
                "Race": number(row, "race"),
                "Class": number(row, "class"),
                "Spell": number(row, "Spell"),
            }
            for row in spells
        ),
    )

    races = read_rows(dbc_sql, "ChrRaces")
    counts["chr_races.json"] = write_json(
        output,
        "chr_races.json",
        (
            {
                "Race": number(row, "ID"),
                "FactionId": number(row, "FactionID"),
                "MaleDisplayId": number(row, "MaleDisplayId"),
                "FemaleDisplayId": number(row, "FemaleDisplayId"),
                "MountScale": float(row.get("MountScale") or 0),
                "BaseLanguage": number(row, "BaseLanguage"),
                "CreatureType": number(row, "CreatureType"),
            }
            for row in races
        ),
    )

    outfits = read_rows(dbc_sql, "CharStartOutfit")
    counts["char_start_outfit.json"] = write_json(
        output,
        "char_start_outfit.json",
        (
            {
                "Race": number(row, "RaceID"),
                "Class": number(row, "ClassID"),
                "Gender": number(row, "GenderID"),
                "ItemIds": [
                    number(row, f"ItemID_{index}")
                    for index in range(1, 13)
                    if number(row, f"ItemID_{index}") > 0
                ],
            }
            for row in outfits
        ),
    )

    item_rows = read_rows(world_sql, "item_template")
    converted_templates = []
    for row in item_rows:
        stats = [
            {"Type": number(row, f"stat_type{index}"), "Value": number(row, f"stat_value{index}")}
            for index in range(1, 11)
        ]
        damages = [
            {
                "Minimum": float(row.get(f"dmg_min{index}") or 0),
                "Maximum": float(row.get(f"dmg_max{index}") or 0),
                "Type": number(row, f"dmg_type{index}"),
            }
            for index in range(1, 6)
        ]
        item_spells = [
            {
                "SpellId": number(row, f"spellid_{index}"),
                "Trigger": number(row, f"spelltrigger_{index}"),
                "Charges": number(row, f"spellcharges_{index}"),
                "Cooldown": number(row, f"spellcooldown_{index}"),
                "Category": number(row, f"spellcategory_{index}"),
                "CategoryCooldown": number(row, f"spellcategorycooldown_{index}"),
            }
            for index in range(1, 6)
        ]
        converted_templates.append(
            {
                "Entry": number(row, "entry"),
                "Class": number(row, "class"),
                "Subclass": number(row, "subclass"),
                "Name": text(row, "name"),
                "Description": text(row, "description"),
                "DisplayId": number(row, "display_id"),
                "Quality": number(row, "quality"),
                "Flags": number(row, "flags"),
                "BuyPrice": number(row, "buy_price"),
                "SellPrice": number(row, "sell_price"),
                "InventoryType": number(row, "inventory_type"),
                "AllowableClass": number(row, "allowable_class", -1),
                "AllowableRace": number(row, "allowable_race", -1),
                "ItemLevel": number(row, "item_level"),
                "RequiredLevel": number(row, "required_level"),
                "RequiredSkill": number(row, "required_skill"),
                "RequiredSkillRank": number(row, "required_skill_rank"),
                "MaxCount": number(row, "max_count"),
                "Stackable": number(row, "stackable", 1),
                "ContainerSlots": number(row, "container_slots"),
                "Stats": stats,
                "Damages": damages,
                "Armor": number(row, "armor"),
                "HolyResistance": number(row, "holy_res"),
                "FireResistance": number(row, "fire_res"),
                "NatureResistance": number(row, "nature_res"),
                "FrostResistance": number(row, "frost_res"),
                "ShadowResistance": number(row, "shadow_res"),
                "Delay": number(row, "delay"),
                "AmmoType": number(row, "ammo_type"),
                "Spells": item_spells,
                "Bonding": number(row, "bonding"),
                "PageText": number(row, "page_text"),
                "PageLanguage": number(row, "page_language"),
                "PageMaterial": number(row, "page_material"),
                "StartQuest": number(row, "start_quest"),
                "LockId": number(row, "lock_id"),
                "Material": number(row, "material"),
                "Sheath": number(row, "sheath"),
                "Duration": number(row, "duration"),
                "SpellCharges1": item_spells[0]["Charges"],
                "SpellCharges2": item_spells[1]["Charges"],
                "SpellCharges3": item_spells[2]["Charges"],
                "SpellCharges4": item_spells[3]["Charges"],
                "SpellCharges5": item_spells[4]["Charges"],
            }
        )
    counts["item_template.json"] = write_json(output, "item_template.json", converted_templates)
    return counts


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--alpha-root", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument(
        "--include-all-world",
        action="store_true",
        help="also write every Alpha Core world SQL table to output/world",
    )
    arguments = parser.parse_args()
    alpha_root = arguments.alpha_root.resolve()
    output = arguments.output.resolve()
    counts = convert(alpha_root, output)
    if arguments.include_all_world:
        world_sql = (alpha_root / "etc" / "databases" / "world" / "world.sql").read_text(
            encoding="utf-8", errors="replace"
        )
        all_world_counts = import_all_world_tables(world_sql, output)
        print(f"world/: {len(all_world_counts)} tables imported")
        for name, count in all_world_counts.items():
            print(f"world/{name}.json: {count} rows")
    for name, count in counts.items():
        print(f"{name}: {count} rows")


if __name__ == "__main__":
    main()
