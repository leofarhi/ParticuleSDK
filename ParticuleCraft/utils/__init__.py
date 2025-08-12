import os

def normalize_path(path):
    path = os.path.normpath(path)
    path = os.path.abspath(path)
    return path.replace("\\", "/")

def CheckIfBinFileIsSame(path1, path2):
    """Check if binary files are the same."""
    if os.path.exists(path1) and os.path.exists(path2):
        with open(path1, "rb") as f1, open(path2, "rb") as f2:
            return f1.read() == f2.read()
    return False

def CheckIfContentIsSame(file_path, new_content):
    """Check if file content is the same."""
    if os.path.exists(file_path):
        with open(file_path, "r", encoding="utf-8") as f:
            text = f.read()
        return text == new_content
    return False