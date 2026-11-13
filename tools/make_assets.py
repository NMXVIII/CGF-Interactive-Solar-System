import numpy as np
from PIL import Image, ImageDraw, ImageFont
import os

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.makedirs(os.path.join(root, "textures", "placeholder"), exist_ok=True)

def font_atlas():
    path = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
    cw, ch = 16, 32
    font = ImageFont.truetype(path, 24)
    img = Image.new("L", (cw * 16, ch * 8), 0)
    d = ImageDraw.Draw(img)
    for c in range(32, 127):
        x = (c % 16) * cw
        y = (c // 16) * ch
        d.text((x + 1, y + 2), chr(c), fill=255, font=font)
    img.save(os.path.join(root, "textures", "font.png"))

def noise(w, h, scale, seed):
    rng = np.random.default_rng(seed)
    gh, gw = int(h / scale) + 2, int(w / scale) + 2
    g = rng.random((gh, gw))
    ys = np.linspace(0, gh - 2, h)
    xs = np.linspace(0, gw - 2, w)
    y0 = ys.astype(int); x0 = xs.astype(int)
    fy = (ys - y0)[:, None]; fx = (xs - x0)[None, :]
    fy = fy * fy * (3 - 2 * fy); fx = fx * fx * (3 - 2 * fx)
    a = g[y0][:, x0]; b = g[y0][:, x0 + 1]
    c = g[y0 + 1][:, x0]; d = g[y0 + 1][:, x0 + 1]
    return a * (1 - fx) * (1 - fy) + b * fx * (1 - fy) + c * (1 - fx) * fy + d * fx * fy

def fbm(w, h, base, seed, octaves=5):
    out = np.zeros((h, w)); amp = 1.0; tot = 0.0
    for i in range(octaves):
        out += amp * noise(w, h, max(base / (2 ** i), 2), seed + i)
        tot += amp; amp *= 0.5
    return out / tot

def save(name, arr):
    arr = np.clip(arr, 0, 1)
    Image.fromarray((arr * 255).astype(np.uint8), "RGB").save(
        os.path.join(root, "textures", "placeholder", name + ".png"))

def tint(n, dark, light):
    dark = np.array(dark)[None, None, :]; light = np.array(light)[None, None, :]
    return dark + (light - dark) * n[:, :, None]

W, H = 512, 256
def rocky(name, dark, light, seed, base=48):
    n = fbm(W, H, base, seed)
    save(name, tint(n, dark, light))

def banded(name, c1, c2, c3, seed, strength=0.15):
    y = np.linspace(0, 1, H)[:, None] * np.ones((1, W))
    turb = fbm(W, H, 30, seed) - 0.5
    t = 0.5 + 0.5 * np.sin((y + turb * strength * 4) * 2 * np.pi * 7)
    img = tint(t, c1, c2)
    n = fbm(W, H, 20, seed + 9)
    img = img * (0.85 + 0.3 * n[:, :, None]) + (np.array(c3)[None, None, :] * 0.0)
    save(name, img)

rocky("mercury", (0.25, 0.24, 0.23), (0.62, 0.60, 0.57), 1)
rocky("venus", (0.62, 0.45, 0.22), (0.92, 0.80, 0.55), 2, 64)
rocky("mars", (0.45, 0.18, 0.09), (0.80, 0.42, 0.22), 3)
banded("jupiter", (0.55, 0.38, 0.25), (0.92, 0.82, 0.68), (0, 0, 0), 4, 0.12)
banded("saturn", (0.72, 0.60, 0.40), (0.93, 0.85, 0.65), (0, 0, 0), 5, 0.05)
rocky("uranus", (0.55, 0.78, 0.82), (0.72, 0.90, 0.92), 6, 96)
rocky("neptune", (0.15, 0.25, 0.65), (0.28, 0.45, 0.90), 7, 96)

font_atlas()
print("assets written")

def ring():
    w, h = 1024, 8
    x = np.linspace(0, 1, w)
    rng = np.random.default_rng(11)
    bands = 0.55 + 0.45 * np.sin(x * 90) * np.sin(x * 23 + 1.3)
    bands += 0.2 * rng.random(w)
    alpha = np.clip(bands, 0, 1)
    alpha[(x > 0.30) & (x < 0.36)] *= 0.12
    alpha[x < 0.05] *= 0.4
    alpha[x > 0.92] *= 0.5
    col = np.array([0.78, 0.70, 0.55])[None, :] * (0.8 + 0.2 * rng.random((w, 1)))
    img = np.zeros((h, w, 4))
    img[:, :, :3] = col[None, :, :]
    img[:, :, 3] = alpha[None, :]
    Image.fromarray((np.clip(img, 0, 1) * 255).astype(np.uint8), "RGBA").save(
        os.path.join(root, "textures", "placeholder", "saturn_ring.png"))

ring()
