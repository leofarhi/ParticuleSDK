from PIL import ImageFont, ImageDraw, Image
import struct
import string

class Character:
    def __init__(self, char, left, top, right, bottom, data):
        self.char = char
        self.left = left
        self.top = top
        self.right = right
        self.bottom = bottom
        self.data = data  # bytearray (1 bit par pixel)
        #print(f"Character {char}: ({left}, {top}, {right}, {bottom}), data size: {len(data)} bytes")

    def width(self):
        return self.right - self.left

    def height(self):
        return self.bottom - self.top
    
def sort_string_by_unicode(s):
    return ''.join(sorted(s, key=lambda c: ord(c)))

def unique_chars(s):
    seen = set()
    result = []
    for c in s:
        if c not in seen:
            seen.add(c)
            result.append(c)
    return ''.join(result)

def convert_font_to_binary(path_font, police_size, charset):
    charset = unique_chars(charset)
    charset = sort_string_by_unicode(charset)
    #print(f"Charset: {charset}")
    font = ImageFont.truetype(path_font, police_size)
    characters = []

    for char in charset:
        bbox = font.getbbox(char)
        if bbox is None:
            continue

        left, top, right, bottom = bbox
        width = right - left
        height = bottom - top

        if width <= 0 or height <= 0:
            # Cas des caractères comme " " ou invisibles : on prend leur largeur uniquement
            width = int(font.getlength(char))  # ou font.getmask(char).size[0]
            height = police_size  # hauteur fictive
            left, top, right, bottom = 0, 0, width, height
            packed = bytearray()
        else:
            image = Image.new("L", (width, height), 0)
            draw = ImageDraw.Draw(image)
            draw.text((-left, -top), char, fill=255, font=font)
            pixels = image.load()

            bits = []
            for y in range(height):
                for x in range(width):
                    bits.append(1 if pixels[x, y] >= 128 else 0)

            packed = bytearray()
            for i in range(0, len(bits), 8):
                byte = 0
                for b in bits[i:i+8]:
                    byte = (byte << 1) | b
                byte <<= (8 - len(bits[i:i+8]))
                packed.append(byte)

        image = Image.new("L", (width, height), 0)
        draw = ImageDraw.Draw(image)
        draw.text((-left, -top), char, fill=255, font=font)
        pixels = image.load()

        bits = []
        for y in range(height):
            for x in range(width):
                bits.append(1 if pixels[x, y] >= 128 else 0)

        packed = bytearray()
        for i in range(0, len(bits), 8):
            byte = 0
            for b in bits[i:i+8]:
                byte = (byte << 1) | b
            byte <<= (8 - len(bits[i:i+8]))
            packed.append(byte)

        codepoint = ord(char)
        characters.append((codepoint, Character(char, left, top, right, bottom, packed)))

    binaire = bytearray()
    binaire += struct.pack(">B", police_size)
    binaire += struct.pack(">H", len(characters))

    for codepoint, char in characters:
        binaire += struct.pack(">IbbbbH", codepoint, char.left, char.top,
                               char.right, char.bottom, len(char.data))
        binaire += char.data

    return binaire,characters

def convert_font_to_binary_file(path_font, police_size, output_file, charset):
    binaire,characters = convert_font_to_binary(path_font, police_size, charset)
    with open(output_file, "wb") as f:
        f.write(binaire)
    #print(f"Saved {len(characters)} characters to {output_file}")



def load_font_binary_file(path):
    with open(path, "rb") as f:
        data = f.read()

    return load_font_binary(data)

def load_font_binary(data):
    offset = 0
    police_size = data[offset]
    offset += 1

    num_chars = struct.unpack(">H", data[offset:offset+2])[0]
    offset += 2

    font = {}

    for _ in range(num_chars):
        codepoint, left, top, right, bottom, data_size = struct.unpack(">IbbbbH", data[offset:offset+10])
        offset += 10
        packed = data[offset:offset+data_size]
        offset += data_size
        char = chr(codepoint)
        font[char] = Character(char, left, top, right, bottom, packed)

    return font


def draw_text(image, font_dict, text, x, y, color=(0, 0, 0)):
    draw_x = x

    # Trouve le max ascent pour aligner les lettres sur la même ligne de base
    max_ascent = max(-char.top for c, char in font_dict.items())

    for c in text:
        if c not in font_dict:
            continue

        char = font_dict[c]
        w, h = char.width(), char.height()
        data = char.data

        draw_y = y + (max_ascent + char.top)  # on corrige ici avec top (qui est négatif)

        bit_index = 0
        for j in range(h):
            for i in range(w):
                byte_index = bit_index // 8
                bit_offset = 7 - (bit_index % 8)
                bit = (data[byte_index] >> bit_offset) & 1
                if bit:
                    image.putpixel((draw_x + i, draw_y + j), color)
                bit_index += 1

        draw_x += w  # ou ajoute un espacement si tu veux



if __name__ == "__main__":
    charset = " !ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789éà"
    convert_font_to_binary_file("The Wild Breath of Zelda.otf", 16, "font_data.bin", charset=charset)
    #convert_font_to_binary("PressStart2P.ttf", 8, "font_data.bin", charset=charset)
    font = load_font_binary_file("font_data.bin")

    img = Image.new("RGB", (800, 200), "white")
    draw_text(img, font, "Hello World! Léo", 0, 0, color=(0, 0, 0))
    img.save("output_text.png")
    print("Text drawn and saved to output_text.png")
    img.show()