import os
from ParticuleCraft.utils.multi_platform import GetPathLinux
from ParticuleCraft.utils import normalize_path
from ParticuleCraft.modules.uuid_manager import UUIDManager
from ParticuleCraft.system.working_dirs import interface_path, engine_path

class MakefileGenerator:
    def __init__(self, builder) -> None:
        self.builder = builder
        self.config = builder.config_data
        self.uuid_manager: UUIDManager = builder.uuid_manager
        self.output = self.config["output_file"]
        self.display_name = self.config["display_name"]
        self.project_path = builder.project_path
        self.build_dir = builder.build_dir

    def generate_makefile(self) -> None:
        source_files = [GetPathLinux(normalize_path(f)) for f in self.config["source_files"]]
        include_paths = [GetPathLinux(normalize_path(p)) for p in self.config["include_paths"]]
        library_paths = [GetPathLinux(normalize_path(p)) for p in self.config["library_paths"]]
        libraries = " ".join(self.config["libraries"])
        defines = " ".join(f"{k}={v}" for k, v in self.config.get("defines", {}).items())
        flags_compile = self.config.get("compile_flags", "")
        flags_link = self.config.get("link_flags", "")
        memtrack = "ON" if self.config.get("memtrack", False) else "OFF"

        engine_src_RECURSE = ""
        if self.config["include_engine"]:
            include_paths.insert(0, GetPathLinux(os.path.join(engine_path, "include")))
            engine_src_RECURSE = GetPathLinux(os.path.join(engine_path, "src"))+ "/*.cpp"

        local = GetPathLinux(self.builder.distribution_path)

        asset_lines = ""
        for cat in ("textures", "fonts", "other"):
            for asset in self.config["assets_files"].get(cat, []):
                if cat == "textures" and asset["external"]: continue
                ext = ".font" if cat == "fonts" else os.path.splitext(asset["path"])[1]
                asset_lines += f"assets/{self.uuid_manager.get_uuid(asset['path'])}{ext}\n"

        icon_uns = GetPathLinux(self.config["icon_uns"], self.build_dir)
        icon_sel = GetPathLinux(self.config["icon_sel"], self.build_dir)
        converter_path = GetPathLinux(os.path.join(self.builder.distribution_path, "Utils", "converters.py"), self.build_dir)

        source_files_join = "\n\t".join(source_files)
        include_paths_join = "\n\t".join(include_paths)
        library_paths_join = "\n\t".join(library_paths)

        interface_include_path = GetPathLinux(os.path.join(interface_path, "include"))

        content = f"""\
cmake_minimum_required(VERSION 3.15)
project("{self.output}")

include(GenerateG3A)
include(Fxconv)
find_package(Gint 2.11 REQUIRED)
find_package(LibProf 2.4 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE API_SRCS {local}/Sources/Gint/src/*.cpp {engine_src_RECURSE})
set(SOURCES ${{API_SRCS}} {source_files_join})

include_directories(
    ${{CMAKE_CURRENT_SOURCE_DIR}}/include
    {local}/Sources/Gint/include
    {interface_include_path}
    {include_paths_join}
)

link_directories({library_paths_join})
set(LIBRARIES {libraries})

set(ASSETS_cg
{asset_lines}
)

fxconv_declare_converters({converter_path})
fxconv_declare_assets(${{ASSETS}} ${{ASSETS_fx}} ${{ASSETS_cg}} WITH_METADATA)
add_executable({self.output} ${{SOURCES}} ${{ASSETS}} ${{ASSETS_${{FXSDK_PLATFORM}}}})
target_compile_options({self.output} PRIVATE -Os -std=c++20 -fno-freestanding {flags_compile})
target_compile_definitions({self.output} PRIVATE {defines})
option(MEMTRACK "Enable memory leak tracking" {memtrack})
if(MEMTRACK)
target_compile_options({self.output} PRIVATE
    -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -fno-builtin-calloc
    -fno-builtin-new -fno-builtin-delete)
target_compile_definitions({self.output} PRIVATE MEMTRACK_ENABLED=1)
endif()
target_link_libraries({self.output} Gint::Gint LibProf::LibProf ${{LIBRARIES}} -lsupc++ -lstdc++ {flags_link})

generate_g3a(TARGET {self.output} OUTPUT "{self.output}.g3a"
    NAME "{self.display_name}" ICONS "{icon_uns}" "{icon_sel}")"""
        with open(os.path.join(self.build_dir, "CMakeLists.txt"), "w") as f:
            f.write(content.strip())
