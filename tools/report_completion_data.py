#!/usr/bin/env python3
"""
Report statistics and potential issues in the canonical
TIGCC completion database.

This is a development-time tool. It is not installed.
"""

from __future__ import annotations

import argparse
import json
import sys
from collections import Counter, defaultdict
from pathlib import Path
from typing import Any



def load_database(path: Path) -> dict[str, Any]:
    try:
        with path.open(
            "r",
            encoding="utf-8",
        ) as source:
            data = json.load(source)
    except OSError as error:
        raise RuntimeError(
            f"could not read {path}: {error}"
        ) from error
    except json.JSONDecodeError as error:
        raise RuntimeError(
            f"invalid JSON in {path}: {error}"
        ) from error

    if not isinstance(data, dict):
        raise RuntimeError(
            "database root must be a JSON object"
        )

    return data


def entry_identity(entry: dict[str, Any]) -> tuple:
    """
    Full-content identity used to detect exact duplicates.
    """
    parameters = tuple(
        (
            parameter.get("name", ""),
            parameter.get("type", ""),
        )
        for parameter in entry.get("parameters", [])
    )

    return (
        entry.get("name", ""),
        entry.get("kind", ""),
        entry.get("signature", ""),
        entry.get("returnType", ""),
        entry.get("description", ""),
        entry.get("header", ""),
        parameters,
    )


def compact_entry(entry: dict[str, Any]) -> str:
    return (
        f"{entry.get('name', '')!r} "
        f"[{entry.get('kind', '')}] "
        f"header={entry.get('header', '')!r} "
        f"signature={entry.get('signature', '')!r}"
    )


def print_grouped_entries(
    title: str,
    groups: dict[Any, list[dict[str, Any]]],
    limit: int,
) -> None:
    print()
    print(title)
    print("-" * len(title))

    if not groups:
        print("none")
        return

    shown = 0

    for key in sorted(groups, key=str):
        entries = groups[key]

        print(
            f"{key!r}: {len(entries)} entries"
        )

        for entry in entries[:limit]:
            print(f"  {compact_entry(entry)}")

        if len(entries) > limit:
            print(
                f"  ... {len(entries) - limit} more"
            )

        shown += 1

        if shown >= limit:
            remaining = len(groups) - shown

            if remaining > 0:
                print(
                    f"... {remaining} more groups"
                )

            break


def main() -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Report statistics and duplicate/suspicious "
            "entries in the canonical completion database."
        )
    )

    parser.add_argument(
        "database",
        type=Path,
        help="canonical completion JSON file",
    )

    parser.add_argument(
        "--limit",
        type=int,
        default=10,
        help=(
            "maximum number of groups and entries to "
            "show in detailed sections"
        ),
    )

    args = parser.parse_args()

    if args.limit < 1:
        parser.error("--limit must be positive")

    try:
        database = load_database(
                args.database
                )
    except RuntimeError as error:
        print(
            f"error: {error}",
            file=sys.stderr,
        )

        return 1

    entries = database.get(
            "entries",
            [],
            )

    if not isinstance(entries, list):
        print(
            "error: 'entries' must be an array",
            file=sys.stderr,
        )

        return 1

    valid_entries: list[dict[str, Any]] = []

    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            print(
                f"warning: entry {index} is not an object",
                file=sys.stderr,
            )

            continue

        valid_entries.append(entry)

    kind_counts = Counter(
        entry.get("kind", "")
        for entry in valid_entries
    )

    names: dict[str, list[dict[str, Any]]] = defaultdict(list)
    identities: dict[tuple, list[dict[str, Any]]] = defaultdict(list)

    missing_name: list[dict[str, Any]] = []
    missing_kind: list[dict[str, Any]] = []
    missing_signature: list[dict[str, Any]] = []

    for entry in valid_entries:
        name = str(
                   entry.get(
                       "name",
                       "",
                   )
                  ).strip()

        kind = str(
                entry.get(
                    "kind",
                    "",
                )
            ).strip()

        signature = str(
                entry.get(
                    "signature",
                    "",
                )
            ).strip()

        names[name].append(entry)
        identities[
            entry_identity(entry)
        ].append(entry)

        if not name:
            missing_name.append(entry)

        if not kind:
            missing_kind.append(entry)

        if not signature:
            missing_signature.append(entry)

    exact_duplicates = {
        identity: group
        for identity, group in identities.items()
        if len(group) > 1
    }

    same_name_duplicates = {
        name: group
        for name, group in names.items()
        if name and len(group) > 1
    }

    meaningful_duplicate_groups = {}

    for name, group in same_name_duplicates.items():
        distinct_identities = {
            entry_identity(entry)
            for entry in group
        }

        if len(distinct_identities) > 1:
            meaningful_duplicate_groups[name] = group

    print(
        f"Database: {args.database}"
    )

    print(
        f"Format version: "
        f"{database.get('formatVersion', '<missing>')}"
    )

    print(
        f"Source format: "
        f"{database.get('sourceFormat', '<missing>')}"
    )

    print()
    print("Summary")
    print("-------")
    print(
        f"Raw entries:             {len(entries)}"
    )
    print(
        f"Valid object entries:    {len(valid_entries)}"
    )
    print(
        f"Unique symbol names:     {len(names)}"
    )
    print(
        f"Exact duplicate groups:  {len(exact_duplicates)}"
    )
    print(
        f"Meaningful duplicate names: "
        f"{len(meaningful_duplicate_groups)}"
    )

    print()
    print("Entries by kind")
    print("---------------")

    for kind, count in sorted(
        kind_counts.items(),
        key=lambda item: item[0],
    ):
        label = kind or "<empty>"
        print(
            f"{label:20} {count:6}"
        )

    print()
    print("Suspicious entries")
    print("------------------")
    print(
        f"Missing name:            {len(missing_name)}"
    )
    print(
        f"Missing kind:            {len(missing_kind)}"
    )
    print(
        f"Missing signature:       {len(missing_signature)}"
    )

    print_grouped_entries(
        "Meaningful duplicate names",
        meaningful_duplicate_groups,
        args.limit,
    )

    exact_duplicate_display = {
        compact_entry(group[0]): group
        for group in exact_duplicates.values()
    }

    print_grouped_entries(
        "Exact duplicate entries",
        exact_duplicate_display,
        args.limit,
    )

    if missing_name:
        print()
        print("Entries missing names")
        print("--------------------")

        for entry in missing_name[:args.limit]:
            print(
                f"  {compact_entry(entry)}"
            )

    if missing_kind:
        print()
        print("Entries missing kinds")
        print("--------------------")

        for entry in missing_kind[:args.limit]:
            print(
                f"  {compact_entry(entry)}"
            )

    return 0


if __name__ == "__main__":
    raise SystemExit(
        main()
    )
