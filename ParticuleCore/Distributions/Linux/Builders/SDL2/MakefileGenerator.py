import os
import json
from typing import List, Tuple
from ParticuleCraft.utils import normalize_path
from ParticuleCraft.modules.uuid_manager import UUIDManager
from ParticuleCraft.utils.multi_platform import *
from ParticuleCraft.system.working_dirs import interface_path, engine_path
from ParticuleCraft.utils import CheckIfContentIsSame

class MakefileGenerator:
    def __init__(self, builder) -> None:
        self.builder = builder
        self.config = builder.config_data
        self.build_dir = builder.build_dir
        self.uuid_manager: UUIDManager = builder.uuid_manager
        self.project_path = builder.project_path
        self.distribution_path = os.path.dirname(builder.config.__config_path__)

        self.output_file = self.config["output_file"]
        self.include_paths = self.config["include_paths"]
        self.source_files = self.config["source_files"]
        self.defines = self.config.get("defines", {})
        self.compile_flags = self.config.get("compile_flags", "")
        self.link_flags = self.config.get("link_flags", "")

    def generate_makefile(self) -> None:
        include_dirs = self._collect_include_dirs()
        all_sources = self._collect_source_files()
        obj_defs, rule_defs, obj_refs = self._generate_objects_and_rules(all_sources)

        include_flags = " ".join(f"-I{path}" for path in include_dirs)
        define_flags = " ".join(f"-D{key}={value}" for key, value in self.defines.items())

        makefile = f"""\
CC = g++
CPPFLAGS = -MMD
CFLAGS = -std=c++20 -fcoroutines -D_GNU_SOURCE {define_flags} {self.compile_flags} {include_flags} `pkg-config --cflags sdl2 SDL2_image SDL2_ttf`
LDFLAGS =
LDLIBS = -lm `pkg-config --libs sdl2 SDL2_image SDL2_ttf` {self.link_flags}

OUTPUT = bin
BUILD_DIR = build
EXEC = $(OUTPUT)/{self.output_file}

OBJS = {" ".join(obj_refs)}
DEPS = $(OBJS:.o=.d)

{obj_defs}

all: $(EXEC)

$(EXEC): $(OBJS)
\tmkdir -p $(OUTPUT)
\t$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)
\t@echo "✅ Build completed."

{rule_defs}

-include $(DEPS)

.PHONY: all
"""

        os.makedirs(self.build_dir, exist_ok=True)
        makefile_path = os.path.join(self.build_dir, "Makefile")
        if not CheckIfContentIsSame(makefile_path, makefile):
            with open(makefile_path, "w", encoding="utf-8") as f:
                f.write(makefile.strip())

        self.uuid_manager.save()

    def _collect_include_dirs(self) -> List[str]:
        defaults = [
            os.path.join(self.build_dir, "include"),
            os.path.join(self.distribution_path, "Sources", "SDL2", "include"),
            os.path.join(interface_path, "include")
        ]
        all_dirs = defaults + self.include_paths
        return [GetPathLinux(normalize_path(p)) for p in all_dirs if os.path.isdir(p)]

    def _collect_source_files(self) -> List[str]:
        collected = []
        collected.append(os.path.join(self.build_dir, "Resources.cpp"))

        sdl_src = os.path.join(self.distribution_path, "Sources", "SDL2", "src")
        for root, _, files in os.walk(sdl_src):
            for f in files:
                if f.endswith((".cpp", ".c")):
                    collected.append(os.path.join(root, f))

        for src in self.source_files:
            if os.path.exists(src):
                collected.append(src)
            else:
                print(f"[WARNING] Source file not found: {src}")

        return [GetPathLinux(normalize_path(path)) for path in collected]

    def _generate_objects_and_rules(self, sources: List[str]) -> Tuple[str, str, List[str]]:
        obj_defs: List[str] = []
        rule_defs: List[str] = []
        obj_refs: List[str] = []

        for src in sources:
            uuid = self.uuid_manager.add(src, "code")
            src_var = f"{uuid}_SRC"
            obj_path = f"$(BUILD_DIR)/{uuid}.o"
            obj_refs.append(obj_path)

            obj_defs.append(f"{src_var} = {src}")
            rule = (
                f"{obj_path}: $({src_var})\n"
                f"\tmkdir -p $(dir $@)\n"
                f"\t$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@\n"
            )
            rule_defs.append(rule)

        return "\n".join(obj_defs), "\n".join(rule_defs), obj_refs
