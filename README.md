# CLI Audio Player 🎧

Pemutar audio minimalis via terminal, ditulis dengan C++.  
Gak ada UI, gak ada warna, cuma suara.

Saat ini hanya support file dengan ekstensi .wav atau .ogg


## Fitur

- Memindai folder `music/` untuk file `.wav`
- Menampilkan daftar lagu
- Memutar lagu berdasarkan pilihan user
- Dibuat untuk belajar, bukan untuk memanjakan

## Requirements

- C++17 atau lebih baru
- [SFML 2.6+](https://www.sfml-dev.org/) (hanya audio module)

### MacOS (via Homebrew)

```bash
brew install sfml
