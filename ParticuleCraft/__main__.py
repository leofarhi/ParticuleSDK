import argparse
import os, sys
from .system import commands
from .system import distribution_manager as dm
from .system import working_dirs as wd
from .system.log import logger

def main():
    #add the Api directory to the system path
    sys.path.append(wd.api_path)
    dm.load_distributions()
    parser = argparse.ArgumentParser(
        description="ParticuleEngine CLI"
    )

    subparsers = parser.add_subparsers(dest="command", required=True)

    # install
    install = subparsers.add_parser('install', help='Install distribution')
    install.add_argument('--target', required=True)
    install.set_defaults(func=commands.install)

    # update
    update = subparsers.add_parser('update', help='Update distribution')
    update.add_argument('--target', required=True)
    update.set_defaults(func=commands.update)

    # configure
    configure = subparsers.add_parser('configure', help='Configure distribution')
    configure.add_argument('--target', required=True)
    configure.set_defaults(func=commands.configure)

    # uninstall
    uninstall = subparsers.add_parser('uninstall', help='Uninstall distribution')
    uninstall.add_argument('--target', required=True)
    uninstall.set_defaults(func=commands.uninstall)

    # build
    build = subparsers.add_parser('build', help='Build the project')
    build.add_argument('--target', required=True)
    build.add_argument('--makefile')
    build.add_argument('--debug', action='store_true')
    build.set_defaults(func=commands.build)

    # clean
    clean = subparsers.add_parser('clean', help='Clean the project')
    clean.add_argument('--makefile')
    clean.set_defaults(func=commands.clean)

    # create
    create = subparsers.add_parser('create', help='Create a new project')
    create.add_argument('--makefile')
    create.add_argument('--library', action='store_true', help='Is this a library?')
    create.set_defaults(func=commands.create)

    # config
    config = subparsers.add_parser('config', help='Show the configuration')
    config.add_argument('--target')
    config.add_argument('--library', action='store_true', help='Is this a library?')
    config.set_defaults(func=commands.config)

    # tools
    tools = subparsers.add_parser('tools', help='List or run tools')
    tools.add_argument('tool', nargs='?', help='Name of the tool to run')
    tools.set_defaults(func=commands.tools)

    args = parser.parse_args()
    args.func(args)

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        logger.exception(f"An error occurred: {e}")
        sys.exit(1)