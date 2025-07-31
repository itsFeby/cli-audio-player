#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <filesystem> // butuh -std=c++17
#include <string>

namespace fs = std::filesystem;

int main() {
    std::string folderPath = "music";
    std::vector<fs::path> laguList;

    // Scan folder
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension().string();
            if (ext == ".wav" || ext == ".WAV") {
                laguList.push_back(entry.path());
            }
        }
    }

    if (laguList.empty()) {
        std::cout << "Gak ada lagu yang bisa diputar.\n";
        return 1;
    }

    // Tampilkan daftar lagu
    std::cout << "Daftar lagu:\n";
    for (size_t i = 0; i < laguList.size(); ++i) {
        std::cout << i + 1 << ". " << laguList[i].filename().string() << "\n";
    }

    // Pilih lagu
    std::cout << "\nMasukkan nomor lagu yang mau diputar: ";
    int pilihan;
    std::cin >> pilihan;

    if (pilihan < 1 || pilihan > laguList.size()) {
        std::cout << "Nomor gak valid.\n";
        return 1;
    }

    // Putar lagu
    sf::Music music;
    if (!music.openFromFile(laguList[pilihan - 1].string())) {
        std::cout << "Gagal buka file.\n";
        return 1;
    }

    music.play();
    std::cout << "Memutar: " << laguList[pilihan - 1].filename().string() << "\n";
    std::cout << "Tekan ENTER buat berhenti...\n";
    std::cin.ignore();
    std::cin.get();


    return 0;
}
