import fxconv

def convert(input, output, params, target):
    if params["custom-type"] == "api_font":
        with open(input, "rb") as f:
            raw = f.read()
        s = fxconv.Structure()
        s += fxconv.u32(len(raw))
        s += fxconv.ptr(bytearray(raw))
        fxconv.elf(s, output, "_" + params["name"], **target)
        return 0
    return 1