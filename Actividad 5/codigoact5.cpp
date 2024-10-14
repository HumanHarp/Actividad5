#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>

namespace fs = std::filesystem;

std::map<std::string, int> tokenizeFile(const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile) {
        std::cerr << "Error: No se pudo abrir el archivo " << filePath << std::endl;
        return {};
    }

    std::map<std::string, int> wordCount;
    std::string word;
    while (inFile >> word) {
        // Limpia caracteres no deseados (como etiquetas HTML)
        word.erase(std::remove_if(word.begin(), word.end(), [](char c) {
            return !std::isalnum(c);
        }), word.end());

        if (!word.empty()) {
            ++wordCount[word];
        }
    }

    inFile.close();
    return wordCount;
}

void saveAlphabetically(const std::map<std::string, int>& wordCount, const std::string& outputFile) {
    std::ofstream outFile(outputFile, std::ios::out | std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: No se pudo crear el archivo " << outputFile << std::endl;
        return;
    }

    outFile << "\xEF\xBB\xBF";  // BOM para UTF-8
    for (const auto& [word, freq] : wordCount) {
        outFile << word << ": " << freq << std::endl;
    }

    std::cout << "Archivo guardado correctamente: " << outputFile << std::endl;
    outFile.close();
}

void saveByFrequency(const std::map<std::string, int>& wordCount, const std::string& outputFile) {
    std::vector<std::pair<std::string, int>> sortedWords(wordCount.begin(), wordCount.end());

    std::sort(sortedWords.begin(), sortedWords.end(), [](const auto& a, const auto& b) {
        return b.second < a.second;
    });

    std::ofstream outFile(outputFile, std::ios::out | std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: No se pudo crear el archivo " << outputFile << std::endl;
        return;
    }

    outFile << "\xEF\xBB\xBF";  // BOM para UTF-8
    for (const auto& [word, freq] : sortedWords) {
        outFile << word << ": " << freq << std::endl;
    }

    std::cout << "Archivo guardado correctamente: " << outputFile << std::endl;
    outFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: Tokenize.exe <input-directory> <output-directory>" << std::endl;
        return 1;
    }

    std::string inputDir = argv[1];
    std::string outputDir = argv[2];

    std::map<std::string, int> consolidatedCount;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (entry.path().extension() == ".html") {
            auto wordCount = tokenizeFile(entry.path().string());
            for (const auto& [word, freq] : wordCount) {
                consolidatedCount[word] += freq;  // Acumula la frecuencia de cada palabra
            }
        }
    }

    saveAlphabetically(consolidatedCount, outputDir + "/consolidated_alphabetical.txt");
    saveByFrequency(consolidatedCount, outputDir + "/consolidated_frequency.txt");

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;

    std::ofstream logFile(outputDir + "/a5_matricula.txt");
    if (logFile) {
        logFile << "Tiempo total de ejecución: " << elapsed.count() << " segundos" << std::endl;
        std::cout << "Log guardado correctamente." << std::endl;
    } else {
        std::cerr << "Error: No se pudo crear el archivo de log." << std::endl;
    }

    return 0;
}
