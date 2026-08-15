#!/usr/bin/env python3

import sys
import argparse

import configparser

from pathlib import Path
import xml.etree.ElementTree as ET


class Profile:
    active: bool = False
    profile_name: str = ""
    auth_token: str = ""
    author_name: str = ""
    author_email: str = ""
    language_name: str = ""
    language_code: str = ""
    charset: str = ""
    encoding: str = ""
    group_email: str = ""
    plural_forms: str = ""

    def dump_to_dict(
        self,
    ) -> dict[str, str]:
        result = {}

        if (active := self.active) and active:
            result["active"] = "true"

        if auth_token := self.auth_token:
            result["auth_token"] = auth_token

        if author_name := self.author_name:
            result["author_name"] = author_name

        if author_name := self.author_name:
            result["author_name"] = author_name

        if author_email := self.author_email:
            result["author_email"] = author_email

        if language_name := self.language_name:
            result["language_name"] = language_name

        if language_code := self.language_code:
            result["language_code"] = language_code

        if charset := self.charset:
            result["charset"] = charset

        if encoding := self.encoding:
            result["encoding"] = encoding

        if group_email := self.group_email:
            result["group_email"] = group_email

        if plural_forms := self.plural_forms:
            result["plural_forms"] = plural_forms

        return result


def load_xml(path: Path) -> list[Profile]:
    tree = ET.parse(path)
    root = tree.getroot()
    profiles = []
    for child in root:
        profile = Profile()

        if child.tag != "profile":
            print(f"child has a tag {child.tag} instead of 'profile', skipping", file=sys.stderr)
            continue

        if active := child.attrib.get("active"):
            profile.active = bool(active)

        for child in child:
            match child.tag:
                case "profile_name":
                    profile.profile_name = child.text or ""
                case "auth_token":
                    profile.auth_token = child.text or ""
                case "author_name":
                    profile.author_name = child.text or ""
                case "author_email":
                    profile.author_email = child.text or ""
                case "language_name":
                    profile.language_name = child.text or ""
                case "language_code":
                    profile.language_code = child.text or ""
                case "charset":
                    profile.charset = child.text or ""
                case "encoding":
                    profile.encoding = child.text or ""
                case "group_email":
                    profile.group_email = child.text or ""
                case "plural_forms":
                    profile.plural_forms = child.text or ""
                case unknown:
                    print(f"Found unknown tag {unknown}, skipping", file=sys.stderr)
                    continue

        if not profile.profile_name:
            print("Profile missing a profile_name, skipping", file=sys.stderr)
            continue

        profiles.append(profile)

    return profiles


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("path", type=Path)
    parser.add_argument("--output", type=Path)

    args = parser.parse_args()

    path = args.path

    profiles = load_xml(path)

    config = configparser.ConfigParser()

    for profile in profiles:
        config[profile.profile_name] = profile.dump_to_dict()

    if output := args.output:
        with output.open("w", encoding="utf-8") as w:
            config.write(w, space_around_delimiters=False)
    else:
        config.write(sys.stdout, space_around_delimiters=False)


if __name__ == "__main__":
    main()
