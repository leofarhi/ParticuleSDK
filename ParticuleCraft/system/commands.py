from . import distribution_manager as dm
from . import working_dirs as wd
from .makefile_parser import MakefileParser
import os, sys
import json
import subprocess

def install(args):
    if args.target and args.target in dm.Distributions:
        classes = dm.Distributions[args.target]
        installer = classes["Installer"]()
        installer.install()
    else:
        raise ValueError("Error: --target is required and must be valid when using install")

def update(args):
    if args.target and args.target in dm.Distributions:
        classes = dm.Distributions[args.target]
        installer = classes["Installer"]()
        installer.update()
    else:
        raise ValueError("Error: --target is required and must be valid when using update")

def configure(args):
    if args.target and args.target in dm.Distributions:
        classes = dm.Distributions[args.target]
        installer = classes["Installer"]()
        installer.configure()
    else:
        raise ValueError("Error: --target is required and must be valid when using configure")

def uninstall(args):
    if args.target and args.target in dm.Distributions:
        classes = dm.Distributions[args.target]
        installer = classes["Installer"]()
        installer.uninstall()
    else:
        raise ValueError("Error: --target is required and must be valid when using uninstall")

def build(args):
    if args.target:
        if not args.makefile:
            args.makefile = os.path.join(os.getcwd(), "Makefile.json")
        if not os.path.exists(args.makefile):
            raise FileExistsError(f"Error: Makefile does not exist at {args.makefile}")
        with open(args.makefile, 'r', encoding="utf-8") as f:
            config = json.load(f)
        makefile = MakefileParser.load_data(config)

        if args.target not in makefile.makefiles:
            raise ValueError(f"Error: Unknown target '{args.target}'")

        print(f"Building target '{args.target}'...")
        if args.debug:
            print("Debug mode enabled")
        if not makefile.makefiles[args.target].build(args.makefile):
            raise RuntimeError(f"Build failed for target '{args.target}'")
        print(f"\033[92mBuild completed for target '{args.target}'\033[0m")
    else:
        raise ValueError("Error: --target is required when using build")

def clean(args):
    if not args.makefile:
        args.makefile = os.path.join(os.getcwd(), "Makefile.json")
    if not os.path.exists(args.makefile):
        raise FileExistsError(f"Error: Makefile does not exist at {args.makefile}")
    print(f"Cleaning target: {args.target}")
    #TODO: Implement the actual cleaning logic

def create(args):
    if not args.makefile:
        args.makefile = os.path.join(os.getcwd(), "Makefile.json")
    makefile = MakefileParser(args.library)
    with open(args.makefile, 'w', encoding="utf-8") as f:
        f.write(json.dumps(makefile.to_data(), indent=4))
    print(f"Makefile created at {args.makefile}")

def config(args):
    makefile = MakefileParser(args.library)
    if args.target:
        if args.target in makefile.makefiles:
            print(json.dumps(makefile.makefiles[args.target].to_config(), indent=4))
        else:
            raise ValueError(f"Error: Unknown target '{args.target}'")
    else:
        print(json.dumps(makefile.to_config(), indent=4))

# commands.py

def tools(args):
    available_tools = {
        'SpriteEditor': os.path.join(wd.tools_path,"SpriteEditor",'SpriteEditor.py'),
        'ParticuleCraftUI': os.path.join(wd.tools_path,'ParticuleCraftUI','ParticuleCraftUI.py'),
        'Documentation': os.path.join(wd.tools_path,'Documentation','app.py')
    }

    if args.tool:
        if args.tool in available_tools.keys():
            print(f"Lancement de l'outil : {args.tool}")
            subprocess.run([sys.executable, available_tools[args.tool]])

        else:
            print(f"Outil inconnu : {args.tool}")
            print("Outils disponibles :", ", ".join(available_tools.keys()))
    else:
        print("Outils disponibles :")
        for tool in available_tools.keys():
            print(f" - {tool}")
