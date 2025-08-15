import customtkinter as ctk
from typing import Literal, Optional

class ResizableScrollableFrame(ctk.CTkScrollableFrame):
    def __init__(self, parent, **kwargs):
        super().__init__(parent, **kwargs)
        self._grip = ctk.CTkLabel(self._parent_frame, text="⤡", width=18, height=18, corner_radius=4)
        self._grip.place(relx=1.0, rely=1.0, anchor="se", x=0, y=5)
        self._drag_start_x = 0
        self._drag_start_y = 0
        self._start_w = 0
        self._start_h = 0

        # curseur "both"
        try:
            self._grip.configure(cursor="size_nw_se")
        except Exception:
            pass

        # bindings
        self._grip.bind("<Button-1>", self._on_grip_press)
        self._grip.bind("<B1-Motion>", self._on_grip_drag)
        self._grip.bind("<ButtonRelease-1>", self._on_grip_release)

    # --- handlers pour le drag (both) ---
    def _on_grip_press(self, event):
        self.update_idletasks()
        self._drag_start_x = event.x_root
        self._drag_start_y = event.y_root
        # tailles actuelles du CTkScrollableFrame (ce widget)
        self._start_w = max(self._parent_canvas.winfo_width(), 1)
        self._start_h = max(self._parent_canvas.winfo_height(), 1)

    def _on_grip_drag(self, event):
        dx = event.x_root - self._drag_start_x
        dy = event.y_root - self._drag_start_y
        new_w = max(1, int(self._start_w + dx))
        new_h = max(1, int(self._start_h + dy))
        # redimensionner via l'API fournie
        #print(f"Resizing to {new_w}x{new_h}")
        self._parent_canvas.configure(width=new_w, height=new_h)

    def _on_grip_release(self, event):
        # rien de spécial pour le moment
        pass