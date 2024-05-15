// Musikdatei Hex,Bin,Char-Editor.cpp :
// -------------------------------------

// --------------------------------------
// Einbinden der benötigten Bibliotheken 
// --------------------------------------
#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <bitset>
#include <iomanip>
#include <shlobj.h>

// ---------------------------------------------------------------------
// Nachfolgend werden Funktionen erstellet,
// welche für die weitere Verarbeitung im Hauptprogramm benötigt werden
// ---------------------------------------------------------------------

// -----------------------------------------------------------------
// Erstellen einer Funktion zum Ändern der Textfarbe in der Konsole
// -----------------------------------------------------------------
void setConsoleColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// -------------------------------------------------------------
// Erstellen einer Funktion zum Öffnen des Ordnerauswahldialogs
// -------------------------------------------------------------
std::wstring openFolderDialog(HWND hwndOwner) {
    wchar_t path[MAX_PATH] = L"";
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = L"Bitte wählen Sie einen Zielordner aus:";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.hwndOwner = hwndOwner;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != NULL) {
        SHGetPathFromIDList(pidl, path);
        CoTaskMemFree(pidl);
    }
    return path;
}

// -------------------------------------------------------------------------
// Erstellen einer Funktion zum Überprüfen, ob eine Datei bereits existiert
// -------------------------------------------------------------------------
bool fileExists(const std::wstring& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

// ------------------------------
// Nun beginnt das Hauptprogramm
// ------------------------------
int main() {
    bool continueProcessing = true;

    // -------------------------------------------------------------------------------------------------------------------------------
    // Das Programm läuft in einer Schleife, damit man die Musikdatei so oft wie man möchte in das gewünschte Format konvertiern kann
    // -------------------------------------------------------------------------------------------------------------------------------
    while (continueProcessing) { 
        // -------------------------------------------------
        // Ausgabe einer Begrüßungsnachricht in der Konsole
        // -------------------------------------------------
        const char* greeting = "Herzlich Willkommen! Bitte waehlen Sie eine MP3-Datei aus.";
        int len = MultiByteToWideChar(CP_ACP, 0, greeting, -1, NULL, 0);
        wchar_t* wideGreeting = new wchar_t[len];
        MultiByteToWideChar(CP_ACP, 0, greeting, -1, wideGreeting, len);
        std::wcout << wideGreeting << std::endl;
        Sleep(2000); // Kurze Verweilzeit, damit der Text in der Konsole gelesen werden kann (2 Sekunden)

        // --------------------------------------
        // Buffer für den ausgewählten Dateipfad
        // --------------------------------------
        wchar_t file_path[MAX_PATH] = L"";
        OPENFILENAMEW ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = L"MP3-Dateien\0*.mp3\0Alle Dateien\0*.*\0";
        ofn.lpstrFile = file_path;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

        // ----------------------------------------------------------------------------------------------------------------
        // Hier muss nun das Format Hex, Bin oder Char ausgewählt werden, in welches die MP3-Datei konvertiert werden soll
        // ----------------------------------------------------------------------------------------------------------------
        if (GetOpenFileNameW(&ofn) == TRUE) {
            std::cout << "Bitte waehlen Sie ein Format, in welches die MP3-Datei konvertiert werden soll:" << std::endl;
            std::cout << "1. Hexadezimal" << std::endl;
            std::cout << "2. Binaer" << std::endl;
            std::cout << "3. Char" << std::endl;

            int option;
            std::cin >> option;
            
            // ------------------------------------------------------------------------------------------------------
            // Eine der drei Formate wählen, wenn ein ungültiges Format gewählt wird, erscheint in der Farbe rot ein
            // Fehlertext, der darauf hinweist, dass ein ungültiges Format ausgewählt wurde.
            // ------------------------------------------------------------------------------------------------------
            std::wstring optionText;
            if (option == 1) {
                optionText = L"Hexadezimal";
            }
            else if (option == 2) {
                optionText = L"Binaer";
            }
            else if (option == 3) {
                optionText = L"Char";
            }
            else {
                setConsoleColor(FOREGROUND_RED);
                std::cout << "Ungueltiges Format ausgewaehlt. Versuchen Sie es bitte erneut." << std::endl;
                setConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                delete[] wideGreeting;
                return 1;
            }

            // --------------------------------------------------------------------------------------------------
            // Der Zielordner, wohin die konvertierte Datei gespeichert werden soll, muss nun ausgewählt werden.
            // Der Hinweis wird nun in der Konsole ausgegeben.
            // --------------------------------------------------------------------------------------------------
            std::cout << "Bitte waehlen Sie einen Zielordner zum Speichern der Datei aus." << std::endl;
            Sleep(2000); // Kurze Verweilzeit, damit der Text in der Konsole gelesen werden kann (2 Sekunden)

            // ------------------------
            // Auswahl des Zielordners
            // ------------------------
            std::wstring outputFolderPath = openFolderDialog(NULL);
            if (!outputFolderPath.empty()) {
                std::wstring outputFile;
                bool overwrite = false;

                // ------------------------
                // Einen Dateinamen wählen
                // ------------------------
                do {
                    std::wcout << L"Bitte geben Sie den Dateinamen für die Ausgabedatei an (zusammenhaengendes Wort waehlen): ";
                    std::wcin >> outputFile;
                    outputFile = outputFolderPath + L"\\" + outputFile + L".txt";

                    // -------------------------------------------------------------------------------------------------------
                    // Wenn der Dateiname schon vorhanden ist, wird gefragt, ob die Datei überschrieben werden soll (Ja/Nein)
                        // Ja: -> Dateiname überschreiben
                        // Nein: -> Es wird gefragt, ob ein neuer Dateiname gewählt werden soll (Ja/Nein)
                            // Ja: -> Neuen Dateinamen wählen
                            // Nein: -> Verarbeitungsabbruch (Ausgabe in der Farbe rot)
                    // --------------------------------------------------------------------------------------------------------
                    if (fileExists(outputFile)) {
                        std::cout << "Eine Datei mit diesem Namen existiert bereits." << std::endl;
                        std::cout << "Moechten Sie die Datei ueberschreiben? (Ja/Nein): ";
                        std::string overwriteChoice;
                        std::cin >> overwriteChoice;

                        if (overwriteChoice.compare("Ja") == 0 || overwriteChoice.compare("ja") == 0) {
                            overwrite = true;
                        }
                        else {
                            std::cout << "Moechten Sie einen neuen Dateinamen waehlen? (Ja/Nein): ";
                            std::cin >> overwriteChoice;
                            if (overwriteChoice.compare("Nein") == 0 || overwriteChoice.compare("nein") == 0) {
                                std::cout << "Moechten Sie die Datei ersetzen? (Ja/Nein): ";
                                std::cin >> overwriteChoice;
                                if (overwriteChoice.compare("Ja") == 0 || overwriteChoice.compare("ja") == 0) {
                                    overwrite = true;
                                }
                                else {
                                    setConsoleColor(FOREGROUND_RED);
                                    std::cout << "Verarbeitung abgebrochen." << std::endl;
                                    setConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                                    continueProcessing = false;
                                    break;
                                }
                            }
                        }
                    }
                    else {
                        overwrite = true;
                    }

                } while (!overwrite);

                if (!continueProcessing) {
                    delete[] wideGreeting;
                    break;
                }

                // -------------------------------------------------------------------------------------------------------------------------
                // Während der Prozess der Speicherung durchgeführt wird, erscheint in der Konsole eine Warte-Nachricht (in der Farbe grün)
                // -------------------------------------------------------------------------------------------------------------------------
                setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                std::cout << "Bitte haben Sie einen Moment Geduld, die Datei wird soeben gespeichert..." << std::endl;
                setConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

                // ------------------------------------------------------
                // Textdatei für die Ausgabe öffnen (oder überschreiben)
                //  // --------------------------------------------------
                std::wofstream outputFileStream(outputFile);
                if (outputFileStream.is_open()) {
                    outputFileStream << L"Ausgewaehlte Datei: " << file_path << std::endl;
                    outputFileStream << L"Option: " << optionText << std::endl;

                    std::ifstream inputFileStream(file_path, std::ios::binary);
                    char ch;
                    const int bytesPerRow = 16; // Anzahl der Bytes pro Zeile
                    int byteCount = 0;

                    // -------------------
                    // Format Hexadezimal
                    // -------------------
                    if (option == 1) { 
                        outputFileStream << L"Offset\t";
                        for (int i = 0; i < bytesPerRow; ++i) {
                            outputFileStream << std::setw(3) << std::hex << i;
                        }
                        outputFileStream << std::endl << L"------\t";
                        for (int i = 0; i < bytesPerRow; ++i) {
                            outputFileStream << L"---";
                        }
                        outputFileStream << std::endl;

                        while (inputFileStream.get(ch)) {
                            if (byteCount % bytesPerRow == 0) {
                                outputFileStream << std::setw(4) << std::setfill(L'0') << std::hex << byteCount << L"\t";
                            }
                            outputFileStream << std::setw(2) << std::setfill(L'0') << std::hex << static_cast<int>(static_cast<unsigned char>(ch)) << L" ";
                            byteCount++;
                            if (byteCount % bytesPerRow == 0) {
                                outputFileStream << std::endl;
                            }
                        }
                        if (byteCount % bytesPerRow != 0) {
                            outputFileStream << std::endl;
                        }
                    }
                    // -------------
                    // Format Binär
                    // -------------
                    else if (option == 2) {
                        outputFileStream << L"Offset\t";
                        for (int i = 0; i < bytesPerRow; ++i) {
                            outputFileStream << std::setw(9) << std::left << std::bitset<8>(0).to_string().c_str();
                        }
                        outputFileStream << std::endl << L"------\t";
                        for (int i = 0; i < bytesPerRow; ++i) {
                            outputFileStream << L"---------";
                        }
                        outputFileStream << std::endl;

                        while (inputFileStream.get(ch)) {
                            if (byteCount % bytesPerRow == 0) {
                                outputFileStream << std::setw(4) << std::setfill(L'0') << std::hex << byteCount << L"\t";
                            }
                            outputFileStream << std::setw(8) << std::bitset<8>(static_cast<unsigned char>(ch)).to_string().c_str() << L" ";
                            byteCount++;
                            if (byteCount % bytesPerRow == 0) {
                                outputFileStream << std::endl;
                            }
                        }
                        if (byteCount % bytesPerRow != 0) {
                            outputFileStream << std::endl;
                        }
                    }
                    // ------------
                    // Format Char
                    // ------------
                    else if (option == 3) { // Char
                        while (inputFileStream.get(ch)) {
                            outputFileStream << ch;
                        }
                    }

                    inputFileStream.close();
                    outputFileStream.close();

                    // ---------------------------------------------------------------------------------------------------------
                    // Nach erfolgreicher Speicherung in der Textdatei, wird dies in der Konsole ausgegeben (in der Farbe grün)
                    // ---------------------------------------------------------------------------------------------------------
                    setConsoleColor(FOREGROUND_GREEN);
                    std::wcout << L"Auswertung erfolgreich gespeichert." << std::endl;
                    setConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
                // ------------------------------------------------------------------------------------------
                // Fehlermeldung ausgeben, wenn ein Fehler beim öffnen der Datei auftritt (in der Farbe rot)
                // ------------------------------------------------------------------------------------------
                else {
                    setConsoleColor(FOREGROUND_RED);
                    std::cout << "Fehler beim Oeffnen der Ausgabedatei." << std::endl;
                    setConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
            }
            // ------------------------------------------------------------------------------------------------------------
            // Fehlermeldung ausgeben, wenn die Ordnerauswahl abgebrochen wurde oder fehlgeschlagen ist (in der Farbe rot)
            // ------------------------------------------------------------------------------------------------------------
            else {
                setConsoleColor(FOREGROUND_RED);
                std::cout << "Ordnerauswahl abgebrochen oder fehlgeschlagen." << std::endl;
                setConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                continueProcessing = false;
                break;
            }
        }
        // -----------------------------------------------------------------------------------------------------------
        // Fehlermeldung ausgeben, wenn die Dateiauswahl abgebrochen wurde oder fehlgeschlagen ist (in der Farbe rot)
        // -----------------------------------------------------------------------------------------------------------
        else {
            setConsoleColor(FOREGROUND_RED);
            std::cout << "Dateiauswahl abgebrochen oder fehlgeschlagen." << std::endl;
            setConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            continueProcessing = false;
            break;
        }

        if (!continueProcessing) {
            delete[] wideGreeting;
            break;
        }
        
        // --------------------------------------------------------------------------------------------------------------------------------
        // Nach der Konvertierung in das gewünschte Format, wird nochmals gefragt, ob eine weitere Datei verarbeitet werden soll (Ja/Nein)
            // Ja: -> Rücksprung an den Schleifenanfang
            // Nein: -> Programmablauf beenden und eine Abschiedsbenachrichtigung an den Benutzer ausgeben
        // --------------------------------------------------------------------------------------------------------------------------------
        std::cout << "Moechten Sie eine weitere Datei verarbeiten? (Ja/Nein): ";
        std::string userInput;
        std::cin >> userInput;

        if (userInput.compare("Nein") == 0 || userInput.compare("nein") == 0) {
            std::cout << "Vielen Dank und auf Wiedersehen!" << std::endl;
            Sleep(2000);
            break;
        }

        delete[] wideGreeting;
    }

    return 0;
}