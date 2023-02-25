#ifndef TYPER_H
#define TYPER_H

// Speed Typer
// Dominic Gasperini

// includes
#include <QString>
#include <QFile>
#include <QStandardPaths>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>

// emums
enum PROMPT_LENGTH {
    SHORT = 0,
    MEDIUM = 1,
    LONG = 2,
};

/**
 * @brief The Typer class
 */
class Typer
{

public:
    // Constructor
    Typer();

    // Functions
    void getRandomPrompt();
    void readFile();
    void saveFile(QString text);
    void calculateAccuracy(QString text);
    void calculateWordsPerMinute(QString text);
    void clearKeystrokeData();
    void addKeystrokeData(QChar str, float time);

    // Getters & Setters
    QString getPromptText();
    void setPromptText(QString text);

    QString getUserInput();
    void setUserInput(QString text);

    float getDuration();
    void setDuration(float number);

    float getAccuracy();
    void setAccuracy(float number);

    float getWordsPerMinute();
    void setWordsPerMinute(float number);

    QString getPromptsFilename();
    void setPromptsFilename(QString text);

    PROMPT_LENGTH getPromptLength();
    void setPromptLength(PROMPT_LENGTH num);

    std::vector<QString> getPrompts();
    void setPrompts(std::vector<QString> vec);

    std::vector<QChar> getKeystrokeData();
    void setKeystrokeData(std::vector<QChar> vec);

    std::vector<float> getKeystrokeTime();
    void setKeystrokeTime(std::vector<float> vec);


private:
    // Test Options
    QString promptsFilename;
    std::vector<QString> prompts;
    PROMPT_LENGTH promptLength;

    // Test Data
    QString promptText;
    QString userInput;

    float duration;
    float accuracy;
    float wordsPerMinute;

    std::vector<QChar> keystrokeData;
    std::vector<float> keystrokeTime;
};

#endif // TYPER_H
