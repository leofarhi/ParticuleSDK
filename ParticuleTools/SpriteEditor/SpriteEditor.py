import os
import tkinter as tk
from tkinter import filedialog, simpledialog, messagebox
import json
import time
from PIL import Image, ImageTk
from SmartCanvas import SmartCanvas, CanvasFont

ZOOM_LEVELS = [1, 2, 3, 4, 6, 8, 12]
WHITE = (255, 255, 255)
RED = (255, 0, 0)

BACKGROUND_COLORS = {
    "dark": (30, 30, 30),
    "light": (200, 200, 200),
    "blue": (0, 0, 255),
    "green": (0, 255, 0),
    "red": (255, 0, 0),
    "yellow": (255, 255, 0),
    "purple": (128, 0, 128),
}

def rgb_to_hex(rgb):
    return '#{:02x}{:02x}{:02x}'.format(*rgb)

def get_background_color():
    def choose(color_name):
        nonlocal selected_color
        selected_color = BACKGROUND_COLORS[color_name]
        top.destroy()

    selected_color = BACKGROUND_COLORS["dark"]
    top = tk.Toplevel()
    top.title("Choisir une couleur de fond")
    top.resizable(False, False)
    tk.Label(top, text="Clique sur une couleur :", font=("Arial", 12)).pack(pady=5)

    for name, rgb in BACKGROUND_COLORS.items():
        fg = (255 - rgb[0] + 10, 255 - rgb[1] - 10, (255 - rgb[2]) // 2)
        fg = tuple(max(0, min(255, c)) for c in fg)
        fg_hex = rgb_to_hex(fg)
        btn = tk.Button(top, text=name.capitalize(), width=15,
                        bg=rgb_to_hex(rgb), fg=fg_hex,
                        command=lambda n=name: choose(n))
        btn.pack(pady=2)

    top.grab_set()
    top.wait_window()
    return selected_color

class HelpWindow(tk.Toplevel):
    TEXTS = {
        "fr": (
            "Aide de l'Éditeur de Sprites",
            "\nContrôles :\n- Clic gauche : Créer une sélection\n- Clic molette : Déplacer la vue\n- Molette : Zoom\n- Clic droit : Supprimer un sprite\n- Shift + clic droit : Renommer un sprite\n- Shift + clic sur les poignées : Redimensionner un sprite\n\nFonctionnalités :\n- Sauvegarde/chargement sprites\n- Couleur de fond personnalisable"
        ),
        "en": (
            "Sprite Editor Help",
            "\nControls:\n- Left click: Create selection\n- Middle click: Move view\n- Scroll wheel: Zoom\n- Right click: Delete sprite\n- Shift + right click: Rename sprite\n- Shift + click on handles: Resize sprite\n\nFeatures:\n- Save/load sprites\n- Custom background color"
        ),
    }

    def __init__(self, master):
        super().__init__(master)
        self.title("Help")
        self.geometry("500x300")
        self.resizable(False, False)

        self.lang_var = tk.StringVar(value="fr")
        lang_menu = tk.OptionMenu(self, self.lang_var, *self.TEXTS.keys(), command=self.update_text)
        lang_menu.pack(anchor="ne", padx=10, pady=5)

        self.label_title = tk.Label(self, font=("Arial", 14, "bold"))
        self.label_title.pack(pady=(5, 0))

        self.label_text = tk.Label(self, justify="left", font=("Arial", 11), anchor="nw")
        self.label_text.pack(fill="both", expand=True, padx=10, pady=5)

        self.update_text("fr")

    def update_text(self, lang):
        title, text = self.TEXTS.get(lang, ("", ""))
        self.label_title.config(text=title)
        self.label_text.config(text=text)


class SpriteEditor:
    def __init__(self):
        self.root = tk.Tk()
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.root.geometry("800x600")
        self.root.title("Sprite Editor")
        self.canvas = SmartCanvas(self.root, callback=self.update_frame, bg_color=rgb_to_hex(BACKGROUND_COLORS["dark"]))
        self.canvas.pack(fill=tk.BOTH, expand=True)

        self.sprites = []
        self.image = None
        self.image_path = ""
        self.image_display = None

        self.zoom_index = 0
        self.scale = ZOOM_LEVELS[self.zoom_index]
        self.offset = [0, 0]

        self.shift_held = False
        self.dragging = False
        self.resizing = None
        self.drawing = False
        self.start_pos = None
        self.current_rect = None
        self.last_mouse = None
        self.last_zoom_time = 0

        self.background_color = BACKGROUND_COLORS["dark"]
        self.border_color = (255, 0, 0)
        self.font = CanvasFont(size=12)

        self.root.bind_all("<KeyPress-Shift_L>", lambda e: self.set_shift(True))
        self.root.bind_all("<KeyRelease-Shift_L>", lambda e: self.set_shift(False))
        self.root.bind_all("<KeyPress-Shift_R>", lambda e: self.set_shift(True))
        self.root.bind_all("<KeyRelease-Shift_R>", lambda e: self.set_shift(False))

        self.menu_bar = tk.Menu(self.root)
        self.file_menu = tk.Menu(self.menu_bar, tearoff=0)
        self.file_menu.add_command(label="Charger Image", command=self.load_image)
        self.file_menu.add_command(label="Importer sprites", command=self.import_sprites)
        self.file_menu.add_command(label="Sauvegarder sprites", command=self.save_sprites, state=tk.DISABLED if not self.image_path else tk.NORMAL)
        self.file_menu.add_separator()
        self.file_menu.add_command(label="Quitter", command=self.root.quit)
        self.menu_bar.add_cascade(label="Fichier", menu=self.file_menu)
        self.menu_bar.add_command(label="Couleur de fond", command=self.change_background_color)
        self.menu_bar.add_command(label="Aide", command=lambda: HelpWindow(self.root))
        self.root.config(menu=self.menu_bar)

    def on_closing(self):
        if messagebox.askokcancel("Quitter", "Les données non sauvegardées seront perdues.\nÊtes-vous sûr de vouloir quitter ?"):
            self.root.destroy()

    def mainloop(self):
        self.root.mainloop()

    def set_shift(self, state):
        self.shift_held = state

    def load_image(self):
        path = filedialog.askopenfilename(filetypes=[("Images", "*.png;*.jpg;*.jpeg;*.bmp")])
        if not path:
            return
        self.image_path = path
        #verifier si l'image ne possède pas déjà un .sprites
        if os.path.exists(path + ".sprites"):
            #afficher un message d'avertissement et de confirmation
            if not messagebox.askyesno("Avertissement", "Cette image possède déjà un fichier .sprites.\nVoulez-vous le remplacer ?"):
                return
        self.image = Image.open(path).convert("RGBA")
        self.update_scaled_image()
        #enabling the save button
        self.file_menu.entryconfig("Sauvegarder sprites", state=tk.NORMAL)

    def update_scaled_image(self):
        if self.image:
            w, h = self.image.size
            self.image_display = self.image.resize((w * self.scale, h * self.scale), Image.NEAREST)

    def screen_to_world(self, pos):
        return (int((pos[0] - self.offset[0]) / self.scale), int((pos[1] - self.offset[1]) / self.scale))

    def world_to_screen(self, pos):
        return (pos[0] * self.scale + self.offset[0], pos[1] * self.scale + self.offset[1])

    def change_background_color(self):
        color = get_background_color()
        self.background_color = color
        self.canvas.bg_color = rgb_to_hex(color)
        self.border_color = tuple(max(0, min(255, c)) for c in (255 - color[0] + 10, 255 - color[1] - 10, (255 - color[2]) // 2))

    def update_frame(self, canvas):
        inp = canvas.input
        surface = canvas.surface

        if not self.image:
            surface.clear()
            surface.fill(self.background_color)
            surface.text("Aucune image", (10, 10), self.font, color="white")
            surface.display()
            return

        if inp.is_mouse_down(3):
            wx, wy = self.screen_to_world(inp.mouse_position())
            for sprite in self.sprites:
                sx, sy, sw, sh = sprite["x"], sprite["y"], sprite["w"], sprite["h"]
                if sx <= wx < sx + sw and sy <= wy < sy + sh:
                    if self.shift_held:
                        new_name = simpledialog.askstring("Renommer le sprite", f"Nom actuel : {sprite['name']}\nNouveau nom :")
                        if new_name:
                            sprite["name"] = new_name
                    else:
                        if messagebox.askyesno("Supprimer", f"Supprimer le sprite '{sprite['name']}' ?"):
                            self.sprites.remove(sprite)
                    break

        if inp.is_mouse_down(2):
            self.dragging = True
            self.last_mouse = inp.mouse_position()
        if inp.is_mouse_up(2):
            self.dragging = False
        if self.dragging and self.last_mouse:
            x, y = inp.mouse_position()
            dx, dy = x - self.last_mouse[0], y - self.last_mouse[1]
            self.offset[0] += dx
            self.offset[1] += dy
            self.last_mouse = (x, y)

        scroll = inp.get_scroll()
        now = time.time()
        if scroll != 0 and now - self.last_zoom_time > 0.2:
            mouse = inp.mouse_position()
            before = self.screen_to_world(mouse)
            if scroll > 0 and self.zoom_index < len(ZOOM_LEVELS) - 1:
                self.zoom_index += 1
            elif scroll < 0 and self.zoom_index > 0:
                self.zoom_index -= 1
            self.scale = ZOOM_LEVELS[self.zoom_index]
            self.update_scaled_image()
            after = self.screen_to_world(mouse)
            self.offset[0] += (after[0] - before[0]) * self.scale
            self.offset[1] += (after[1] - before[1]) * self.scale
            self.last_zoom_time = now

        if inp.is_mouse_down(1):
            wx, wy = self.screen_to_world(inp.mouse_position())
            if self.shift_held:
                for sprite in self.sprites:
                    sx, sy, sw, sh = sprite["x"], sprite["y"], sprite["w"], sprite["h"]
                    handles = [
                        (sx + sw // 2, sy), (sx + sw, sy + sh // 2),
                        (sx + sw // 2, sy + sh), (sx, sy + sh // 2)
                    ]
                    for h_index, (hx, hy) in enumerate(handles):
                        if abs(hx - wx) <= 1 and abs(hy - wy) <= 1:
                            self.resizing = (sprite, h_index)
                            break
            if not self.resizing:
                self.drawing = True
                self.start_pos = inp.mouse_position()

        if inp.is_mouse_up(1):
            if self.resizing:
                self.resizing = None
            elif self.drawing:
                self.drawing = False
                end_pos = inp.mouse_position()
                x1, y1 = self.screen_to_world(self.start_pos)
                x2, y2 = self.screen_to_world(end_pos)
                x, y = min(x1, x2), min(y1, y2)
                w, h = abs(x2 - x1), abs(y2 - y1)
                if w > 0 and h > 0:
                    name = simpledialog.askstring("Nom du sprite", "Entrer le nom du sprite :")
                    if name:
                        self.sprites.append({"name": name, "x": x, "y": y, "w": w, "h": h})

        if self.resizing:
            wx, wy = self.screen_to_world(inp.mouse_position())
            sprite, idx = self.resizing
            if idx == 0:
                sprite["h"] += sprite["y"] - wy
                sprite["y"] = wy
            elif idx == 1:
                sprite["w"] = wx - sprite["x"]
            elif idx == 2:
                sprite["h"] = wy - sprite["y"]
            elif idx == 3:
                sprite["w"] += sprite["x"] - wx
                sprite["x"] = wx

        surface.clear()
        surface.fill(self.background_color)

        if self.image_display:
            surface.blit(self.image_display, tuple(self.offset))
            # add border
            width, height = self.image_display.size
            surface.rectangle([self.offset[0], self.offset[1], self.offset[0] + width, self.offset[1] + height], outline=rgb_to_hex(self.border_color), width=2)

        for sprite in self.sprites:
            x, y = self.world_to_screen((sprite["x"], sprite["y"]))
            w, h = sprite["w"] * self.scale, sprite["h"] * self.scale
            surface.rectangle([x, y, x + w, y + h], outline=rgb_to_hex(self.border_color), width=2)
            if self.shift_held:
                surface.text(sprite["name"], (x + 2, y + 2), self.font, color="white", stroke_width=1)
                surface.text(f"{sprite['w']}x{sprite['h']}", (x + 2, y + h - 16), self.font, color="white", stroke_width=1)
                surface.text(f"{sprite['x']}, {sprite['y']}", (x + 2, y + h - 32), self.font, color="white", stroke_width=1)
                handles = [
                    (sprite["x"] + sprite["w"] // 2, sprite["y"]),
                    (sprite["x"] + sprite["w"], sprite["y"] + sprite["h"] // 2),
                    (sprite["x"] + sprite["w"] // 2, sprite["y"] + sprite["h"]),
                    (sprite["x"], sprite["y"] + sprite["h"] // 2),
                ]
                for px, py in handles:
                    cx, cy = self.world_to_screen((px, py))
                    surface.rectangle((cx - 3, cy - 3, cx + 3, cy + 3), outline="white")

        if self.drawing:
            x1, y1 = self.snap_to_grid(self.start_pos)
            x2, y2 = self.snap_to_grid(inp.mouse_position())
            #check if x2 et y2 sont plus grands que x1 et y1
            if x2 < x1:
                x1, x2 = x2, x1
            if y2 < y1:
                y1, y2 = y2, y1
            surface.rectangle([x1, y1, x2, y2], outline="cyan")

        mx, my = inp.mouse_position()
        wx, wy = self.screen_to_world((mx, my))
        sx, sy = self.world_to_screen((wx, wy))
        s = self.scale
        surface.rectangle((sx, sy, sx + s, sy + s), outline="red")

        info_text = f"Position: {wx}, {wy}"
        if self.drawing:
            w = abs(mx - self.start_pos[0]) // self.scale
            h = abs(my - self.start_pos[1]) // self.scale
            info_text += f" | Creation: {w}x{h}"
        elif self.resizing:
            s, _ = self.resizing
            info_text += f" | Edition: {s['w']}x{s['h']}"

        surface.text(info_text, (10, self.canvas.winfo_height() - 20), self.font, color="white", stroke_width=1)
        surface.display()

    def save_sprites(self):
        if not self.image_path:
            messagebox.showerror("Erreur", "Aucune image chargée.")
            return
        path = self.image_path + ".sprites"
        with open(path, "w") as f:
            json.dump(self.sprites, f, indent=4)

    def import_sprites(self):
        path = filedialog.askopenfilename(filetypes=[("sprites files", "*.sprites")])
        if path:
            with open(path, "r") as f:
                data = json.load(f)
            image_path, _ = os.path.splitext(path)
            if not os.path.exists(image_path):
                messagebox.showerror("Erreur", f"Image introuvable : {image_path}")
                return
            self.image_path = image_path
            self.image = Image.open(image_path).convert("RGBA")
            self.update_scaled_image()
            self.sprites = data
            #enabling the save button
            self.file_menu.entryconfig("Sauvegarder sprites", state=tk.NORMAL)

    def snap_to_grid(self, screen_pos):
        return self.world_to_screen(self.screen_to_world(screen_pos))


if __name__ == "__main__":
    SpriteEditor().mainloop()