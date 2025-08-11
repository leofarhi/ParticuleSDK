from ParticuleCraft.core.package_base import Package
import os,sys

class Particule3D(Package):
    def __init__(self, builder) -> None:
        super().__init__("Particule3D", builder)
        self.package_path = os.path.dirname(os.path.abspath(__file__))
        """
        self.include_paths.append(os.path.join(self.package_path, "include"))
        src_dir = os.path.join(self.package_path, "src")
        for root, _, files in os.walk(src_dir):
            for file in files:
                if file.endswith((".cpp", ".c")):
                    self.src_files.append(os.path.join(root, file))
        """
        #print("Particule3D package initialized.")
        print("Particule3D : This package is currently not implemented.")