#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <openssl/sha.h>

using namespace std;

void welcome() {
    cout << "\033[1;35m";
    cout << "Лабораторна робота 4. Дослідження стійкості парольного захисту\n";
    cout << "\033[1;36m";
    cout << "Управління інформаційною безпекою\n";
    cout << "\033[1;35m";
    cout << "Алєксєєва Аліна КБ-21. Варіант 1\n";
    cout << "\033[0m";
}

// Функція для обчислення SHA-256 хешу
string sha256(const string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Функція для запису хешу в файл
void writeHashToFile(const string& filename, const string& hash) {
    ofstream file(filename);
    if (file.is_open()) {
        file << hash << endl;
        file.close();
        cout << "Хеш записано в файл: " << filename << endl;
    } else {
        cout << "Не вдалося відкрити файл для запису." << endl;
    }
}

// Функція для читання хешу з файлу
string readHashFromFile(const string& filename) {
    ifstream file(filename);
    string hash;
    if (file.is_open()) {
        getline(file, hash);
        file.close();
    } else {
        cout << "Не вдалося відкрити файл." << endl;
        exit(1);
    }
    return hash;
}

// Функція для генерації словника
vector<string> loadDictionary(const string& filename) {
    vector<string> dictionary;
    ifstream file(filename);
    string word;
    if (file.is_open()) {
        while (getline(file, word)) {
            dictionary.push_back(word);
        }
        file.close();
    } else {
        cout << "Не вдалося відкрити файл словника." << endl;
        exit(1);
    }
    return dictionary;
}

// Брутфорсова атака
void bruteForceAttack(const string& targetHash, const string& charset, int maxLength) {
    auto start = chrono::high_resolution_clock::now();
    string password;

    for (int length = 1; length <= maxLength; length++) {
        vector<int> indices(length, 0);
        while (true) {
            password.clear();
            for (int i : indices) {
                password += charset[i];
            }
            if (sha256(password) == targetHash) {
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double> elapsed = end - start;
                cout << "Пароль знайдено: " << password << " | Час: " << elapsed.count() << " сек" << endl;
                return;
            }

            int i = length - 1;
            while (i >= 0 && ++indices[i] == charset.size()) {
                indices[i] = 0;
                i--;
            }
            if (i < 0) break;
        }
    }
    cout << "Пароль не знайдено." << endl;
}

// Словникова атака
void dictionaryAttack(const string& targetHash, const vector<string>& dictionary) {
    auto start = chrono::high_resolution_clock::now();
    for (const string& password : dictionary) {
        if (sha256(password) == targetHash) {
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;
            cout << "Пароль знайдено: " << password << " | Час: " << elapsed.count() << " сек" << endl;
            return;
        }
    }
    cout << "Пароль не знайдено." << endl;
}

int main() {
    welcome();
    string option;

    cout << "\nОберіть дію:\n";
    cout << "1. Згенерувати хеш з пароля та записати в файл\n";
    cout << "2. Перевірити хеш пароля\n";
    cout << "3. Вихід\n";

    while (true) {
        cout << "Оберіть опцію: ";
        cin >> option;

        if (option == "1") {
            string password, hashFile;

            // Генерація хешу з пароля та запис у файл
            cout << "Введіть пароль для генерації хешу: ";
            cin >> password;
            string hash = sha256(password);

            cout << "Введіть шлях до файлу для збереження хешу: ";
            cin >> hashFile;
            writeHashToFile(hashFile, hash);

        } else if (option == "2") {
            string hashFile, attackMode;
            cout << "Введіть шлях до файлу з хешем: ";
            cin >> hashFile;
            string targetHash = readHashFromFile(hashFile);

            cout << "Оберіть метод атаки (1 - brute-force, 2 - dictionary): ";
            cin >> attackMode;

            if (attackMode == "1") {
                string charsetChoice;
                cout << "Оберіть символи (1 - малі, 2 - великі, 3 - цифри, 4 - спецсимволи, 5 - всі): ";
                cin >> charsetChoice;

                string charset = "";
                if (charsetChoice.find('1') != string::npos) charset += "abcdefghijklmnopqrstuvwxyz";
                if (charsetChoice.find('2') != string::npos) charset += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                if (charsetChoice.find('3') != string::npos) charset += "0123456789";
                if (charsetChoice.find('4') != string::npos) charset += "!@#$%^&*()_+-=[]{}|;:',.<>?/";
                if (charsetChoice.find('5') != string::npos) charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:',.<>?/";

                int maxLength;
                cout << "Введіть максимальну довжину пароля для перебору: ";
                cin >> maxLength;
                bruteForceAttack(targetHash, charset, maxLength);
            } else if (attackMode == "2") {
                string dictionaryFile;
                cout << "Введіть шлях до словника: ";
                cin >> dictionaryFile;
                vector<string> dictionary = loadDictionary(dictionaryFile);
                dictionaryAttack(targetHash, dictionary);
            } else {
                cout << "Невірний вибір! Будь ласка, виберіть 1 або 2." << endl;
            }

        } else  {
            cout << "\033[1;36m" << "Дякую за використання програми. Алєксєєва Аліна КБ-21 \nЗавершення роботи..." << "\033[0m" << endl;
        }
    }

    return 0;
}