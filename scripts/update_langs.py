# Syncs the language and plural-forms files with Damned Lies.
#
# Has to be run at the root of the project:
# $ python scripts/update_langs.py
#
# Formated and linted with:
# $ ruff format update_langs.py
# $ ruff check --select=ALL --ignore=D,ANN101,S310 update_langs.py
from __future__ import annotations

import configparser
import json
import urllib
import urllib.request
from pathlib import Path
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    import io

API_PATH = "https://l10n.gnome.org/api/v1/languages/"
LANGS_FILE = "data/gtr-languages.ini"
PLURALS_FILE = "data/gtr-plural-forms.ini"


class Lang:
    def __init__(self, name: str, locale: str, plurals: str | None) -> None:
        self.name = name
        self.locale = locale
        self.plurals = None
        if plurals:
            plurals = plurals.strip(";")
            self.plurals = f"{plurals};"


def sort_langs(lang: Lang) -> str:
    return lang.locale


def main() -> None:
    req = urllib.request.Request(API_PATH)
    with urllib.request.urlopen(req) as response:
        data = response.read()
        values = json.loads(data)
        languages = [
            Lang(lang["name"], lang["locale"], lang["plurals"]) for lang in values
        ]
        languages.sort(key=sort_langs)

        with Path(LANGS_FILE).open("w", encoding="utf-8") as langs_w:
            write_languages(languages, langs_w)

        with Path(PLURALS_FILE).open("w", encoding="utf-8") as plurals_w:
            write_plurals(languages, plurals_w)


def write_languages(languages: list[Lang], w: io.TextIOWrapper) -> None:
    config = configparser.ConfigParser()
    config.optionxform = str
    config["Languages"] = {lang.locale: lang.name for lang in languages}
    config.write(w, space_around_delimiters=False)


def write_plurals(languages: list[Lang], w: io.TextIOWrapper) -> None:
    config = configparser.ConfigParser(interpolation=None)
    config.optionxform = str
    config["Plural Forms"] = {
        lang.locale: lang.plurals for lang in languages if lang.plurals
    }
    config.write(w, space_around_delimiters=False)


if __name__ == "__main__":
    main()
