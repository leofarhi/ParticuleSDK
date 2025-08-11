import os
import sys
current_working_directory = os.path.abspath(os.getcwd())
particulecraft_path = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..'))
sdk_path = os.path.abspath(os.path.join(particulecraft_path, '..'))
packages_path = os.path.abspath(os.path.join(sdk_path, 'Packages'))
distributions_path = os.path.abspath(os.path.join(sdk_path,"ParticuleCore", 'Distributions'))
interface_path = os.path.abspath(os.path.join(sdk_path,"ParticuleCore", 'Interface'))
engine_path = os.path.abspath(os.path.join(sdk_path,"ParticuleEngine"))
tools_path = os.path.abspath(os.path.join(sdk_path,"ParticuleTools"))