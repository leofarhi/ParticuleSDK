import os
import sys
current_working_directory = os.path.abspath(os.getcwd())
package_path = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..'))
api_path = os.path.abspath(os.path.join(package_path, '..'))
distributions_path = os.path.abspath(os.path.join(api_path,"ParticuleCore", 'Distributions'))
interface_path = os.path.abspath(os.path.join(api_path,"ParticuleCore", 'Interface'))
engine_path = os.path.abspath(os.path.join(api_path,"ParticuleEngine"))
tools_path = os.path.abspath(os.path.join(api_path,"ParticuleTools"))