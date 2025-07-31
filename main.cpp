#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <atomic>
#include <chrono>

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

    sf::Music music;
    if (!music.openFromFile(laguList[pilihan - 1].string())) {
        std::cout << "Gagal buka file.\n";
        return 1;
    }

    std::atomic<bool> stop(false);
    std::atomic<bool> paused(false);

    // Thread buat input kontrol
    std::thread inputThread([&]() {
        std::cout << "\nKontrol: [p]ause/resume, [s]top, [q]uit\n";
        char cmd;
        while (!stop) {
            std::cin >> cmd;
            if (cmd == 'p') {
                if (!paused) {
                    music.pause();
                    std::cout << "⏸ Paused\n";
                    paused = true;
                } else {
                    music.play();
                    std::cout << "▶️ Resume\n";
                    paused = false;
                }
            } else if (cmd == 's') {
                music.stop();
                std::cout << "⏹ Stopped\n";
                paused = false;
            } else if (cmd == 'q') {
                stop = true;
                music.stop();
                std::cout << "👋 Keluar\n";
                break;
            }
        }
    });

    // Main thread muter lagu
    music.play();
    std::cout << "\n🎶 Memutar: " << laguList[pilihan - 1].filename().string() << "\n";

    // Tunggu sampai lagu selesai atau user quit
    while (!stop) {
        auto status = music.getStatus();
        if (status == sf::SoundSource::Status::Stopped && !paused) {
            break; // lagu selesai dan gak dalam keadaan pause
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    stop = true;
    inputThread.join();

    return 0;
}
