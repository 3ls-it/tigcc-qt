#!/usr/bin/env python3
"""
This file is part of TIGCC-Qt.
Copyright (c) 2026 J Adams <jfa63@duck.com>
SPDX-License-Identifier: BSD-2-Clause

Convert the legacy KTIGCC completion database into
the canonical TIGCC-Qt completion JSON format.

The legacy source is a development-time input only.
The generated JSON is the runtime completion database.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable


ENTRY_FIELD_RE = re.compile(
    r"^Entry\s+(\d+)\s+(.+?)=(.*)$"
)

SECTION_RE = re.compile(
    r"^\[(.*)\]$"
)


@dataclass
class LegacyEntry:
    section: str
    included: str = ""
    fields: dict[str, str] = field(default_factory=dict)


def decode_legacy_text(value: str) -> str:
    """
    Decode the small set of escapes observed in the legacy file.

    Unknown escapes are preserved rather than interpreted aggressively.
    """
    return (
        value
        .replace(r"\r", "\r")
        .replace(r"\n", "\n")
        .replace(r"\t", "\t")
        .replace(r"\s", " ")
    ).strip()


def parse_prefix(prefix: str) -> tuple[str, str]:
    """
    Convert a legacy Prefix field into:

        kind, return_type
    """
    value = decode_legacy_text(prefix)

    if not value:
        return "unknown", ""

    lowered = value.lower()

    if lowered.startswith("func"):
        return_type = value[4:].strip()
        if return_type == "?":
            return_type = ""
        return "function", return_type

    if lowered.startswith("var"):
        return_type = value[3:].strip()
        return "variable", return_type

    if lowered.startswith("type"):
        remainder = value[4:].strip()

        if remainder.lower().startswith("enum"):
            return "enum", ""

        return "type", ""

    if lowered.startswith("enum"):
        return "enum", ""

    if lowered.startswith("const"):
        return "constant", ""

    if lowered.startswith("macro"):
        return "macro", ""

    if lowered.startswith("language extension"):
        return "keyword", ""

    return value.split(maxsplit=1)[0].lower(), ""


def is_function_alias(
    kind: str,
    postfix: str,
) -> bool:
    """
    Return True when a function's Postfix is a bare
    symbol name rather than a parameter list.

    Example:
        Postfix=(short c);  -> normal function
        Postfix=fputchar    -> alias
    """
    return (
        kind == "function"
        and bool(postfix.strip())
        and not postfix.lstrip().startswith("(")
        and re.fullmatch(
            r"[A-Za-z_][A-Za-z0-9_]*",
            postfix.strip(),
        ) is not None
    )


def split_top_level_parameters(text: str) -> list[str]:
    """
    Split a function parameter list on top-level commas.

    This is intentionally lightweight. It is not a C parser.
    """
    value = text.strip()

    if value.startswith("("):
        value = value[1:]

    if value.endswith(");"):
        value = value[:-2]
    elif value.endswith(")"):
        value = value[:-1]

    value = value.strip()

    if not value or value == "void":
        return []

    result: list[str] = []
    current: list[str] = []
    nesting = 0

    for character in value:
        if character in "([{":
            nesting += 1
        elif character in ")]}":
            nesting = max(0, nesting - 1)

        if character == "," and nesting == 0:
            result.append("".join(current).strip())
            current.clear()
            continue

        current.append(character)

    if current:
        result.append("".join(current).strip())

    return [
        item
        for item in result
        if item
    ]


def parse_parameter(parameter: str) -> dict[str, str]:
    """
    Convert a simple parameter string into name/type fields.

    Examples:

        const char *format
            name: format
            type: const char *

        ...
            name: ...
            type: ""
    """
    value = parameter.strip()

    if value == "...":
        return {
            "name": "...",
            "type": "",
        }

    identifier_match = re.search(
        r"([A-Za-z_][A-Za-z0-9_]*)\s*$",
        value
    )

    if identifier_match is None:
        return {
            "name": "",
            "type": value,
        }

    name = identifier_match.group(1)
    parameter_type = value[:identifier_match.start()].strip()

    return {
        "name": name,
        "type": parameter_type,
    }


def make_signature(
    name: str,
    kind: str,
    postfix: str,
) -> str:
    """
    Produce a completion display string whose leading text
    remains the completion name.
    """
    postfix = decode_legacy_text(postfix)

    if not postfix:
        return name

    if kind == "function":
        return f"{name}{postfix}"

    if kind in {"constant", "enum"}:
        return f"{name} = {postfix}"

    if kind == "type":
        return f"{name} {postfix}"

    if kind == "variable":
        return f"{name} {postfix}"

    if kind == "macro":
        return f"{name}{postfix}"

    return f"{name} {postfix}"


def convert_entry(
    legacy_entry: LegacyEntry,
) -> dict:
    fields = legacy_entry.fields

    name = decode_legacy_text(
        fields.get("Text", "")
    )

    prefix = decode_legacy_text(
        fields.get("Prefix", "")
    )

    postfix = decode_legacy_text(
        fields.get("Postfix", "")
    )

    description = decode_legacy_text(
        fields.get("Comment", "")
    )

    kind, return_type = parse_prefix(
        prefix
    )

    alias_of = ""

    if is_function_alias(
        kind,
        postfix,
    ):
        alias_of = postfix.strip()

        signature = name

        parameters = []

    else:
        signature = make_signature(
            name,
            kind,
            postfix,
        )

        parameters = []

        if kind == "function":
            parameters = [
                parse_parameter(parameter)
                for parameter in split_top_level_parameters(
                    postfix
                )
            ]

    result = {
        "name": name,
        "kind": kind,
        "signature": signature,
        "returnType": return_type,
        "description": description,
        "header": legacy_entry.included,
        "parameters": parameters,
    }

    if alias_of:
        result["aliasOf"] = alias_of

    return result


def resolve_function_aliases(
    entries: list[dict],
    warnings: list[str],
) -> None:
    """
    Resolve function aliases after all entries have been
    parsed.

    Alias targets are preferred from the same header.
    """
    entries_by_key: dict[
        tuple[str, str, str],
        list[dict],
    ] = defaultdict(list)

    for entry in entries:
        key = (
            entry.get("name", ""),
            entry.get("header", ""),
            entry.get("kind", ""),
        )

        entries_by_key[key].append(
            entry
        )

    for entry in entries:
        alias_of = entry.get(
            "aliasOf",
            "",
        ).strip()

        if not alias_of:
            continue

        target_key = (
            alias_of,
            entry.get("header", ""),
            "function",
        )

        candidates = entries_by_key.get(
            target_key,
            [],
        )

        if len(candidates) == 0:
            warnings.append(
                f"Could not resolve alias "
                f"{entry.get('name', '')} -> {alias_of} "
                f"in {entry.get('header', '<unknown>')}"
            )

            continue

        if len(candidates) > 1:
            warnings.append(
                f"Ambiguous alias "
                f"{entry.get('name', '')} -> {alias_of} "
                f"in {entry.get('header', '<unknown>')}"
            )

            continue

        target = candidates[0]

        target_signature = target.get(
                "signature",
                "",
            )

        target_name = target.get(
                "name",
                "",
            )

        alias_name = entry.get(
                "name",
                "",
            )

        if target_signature.startswith(
            target_name
        ):
            entry["signature"] = alias_name + target_signature[
                    len(target_name):
                ]
        else:
            entry["signature"] = target_signature

        entry["returnType"] = target.get(
                "returnType",
                "",
            )

        entry["parameters"] = [
            dict(parameter)
            for parameter in target.get(
                "parameters",
                [],
            )
        ]


def parse_legacy_file(
    path: Path,
    strict: bool,
) -> tuple[list[dict], list[str]]:
    entries: list[dict] = []
    warnings: list[str] = []

    current_section = ""
    current_included = ""
    current_entry: LegacyEntry | None = None
    current_entry_number: int | None = None

    def flush_entry() -> None:
        nonlocal current_entry
        nonlocal current_entry_number

        if current_entry is None:
            return

        fields = current_entry.fields
        name = decode_legacy_text(
            fields.get("Text", "")
        )

        if not name:
            message = (
                f"{current_entry.section}: "
                f"entry {current_entry_number} has no Text field"
            )

            if strict:
                raise ValueError(message)

            warnings.append(message)
            current_entry = None
            current_entry_number = None
            return

        entries.append(
            convert_entry(current_entry)
        )

        current_entry = None
        current_entry_number = None

    with path.open(
        "r",
        encoding="utf-8",
        errors="replace",
    ) as source:
        for line_number, raw_line in enumerate(
            source,
            start=1,
        ):
            line = raw_line.rstrip("\n\r")

            section_match = SECTION_RE.match(
                line.strip()
            )

            if section_match is not None:
                flush_entry()

                current_section = section_match.group(1).strip()

                current_included = ""

                continue

            if not line.strip():
                continue

            if current_section.lower().endswith(
                " lines"
            ):
                # Legacy lookup/index section.
                continue

            if line.startswith("Included="):
                flush_entry()

                current_included = decode_legacy_text(
                    line.split(
                        "=",
                        1
                    )[1]
                )

                continue

            if line.startswith("Num Entries="):
                continue

            entry_match = ENTRY_FIELD_RE.match(
                line
            )

            if entry_match is None:
                warnings.append(
                    f"{path}:{line_number}: "
                    f"unrecognized line: {line}"
                )
                continue

            entry_number = int(
                entry_match.group(1)
            )

            field_name = entry_match.group(2).strip()
            field_value = entry_match.group(3)

            if (
                current_entry is None
                or current_entry_number != entry_number
            ):
                flush_entry()

                current_entry_number = entry_number
                current_entry = LegacyEntry(
                    section=current_section,
                    included=(
                        current_included
                        or current_section
                    ),
                )

            current_entry.fields[field_name] = field_value

    flush_entry()

    resolve_function_aliases(
        entries,
        warnings,
    )

    return entries, warnings


def main() -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Convert a legacy KTIGCC completion file "
            "to canonical TIGCC-Qt JSON."
        )
    )

    parser.add_argument(
        "input",
        type=Path,
        help="legacy KTIGCC completion file",
    )

    parser.add_argument(
        "output",
        type=Path,
        help="canonical JSON output file",
    )

    parser.add_argument(
        "--strict",
        action="store_true",
        help="fail on unrecognized or incomplete records",
    )

    args = parser.parse_args()

    try:
        entries, warnings = parse_legacy_file(
            args.input,
            args.strict,
        )
    except ValueError as error:
        print(
            f"error: {error}",
            file=sys.stderr,
        )
        return 1

    output = {
        "formatVersion": 1,
        "sourceFormat": "KTIGCC",
        "entries": entries,
    }

    args.output.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    with args.output.open(
        "w",
        encoding="utf-8",
        newline="\n",
    ) as destination:
        json.dump(
            output,
            destination,
            indent=2,
            ensure_ascii=False,
        )

        destination.write("\n")

    print(
        f"converted {len(entries)} completion entries"
    )

    if warnings:
        print(
            f"warnings: {len(warnings)}",
            file=sys.stderr,
        )

        for warning in warnings[:20]:
            print(
                f"  {warning}",
                file=sys.stderr,
            )

        if len(warnings) > 20:
            print(
                f"  ... {len(warnings) - 20} more",
                file=sys.stderr,
            )

    return 0


if __name__ == "__main__":
    raise SystemExit(
        main()
    )
