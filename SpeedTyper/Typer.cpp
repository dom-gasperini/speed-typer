// Speed Typer
// Dominic Gasperini

// includes
#include "Typer.h"

/**
 * @brief Typer::Typer
 */
Typer::Typer()
{
    // Init Test Data
    duration = 0.0f;
    accuracy = 0.0f;
    wordsPerMinute = 0.0f;
    promptLength = SHORT;

    // use default filename
    setPromptsFilename("default_prompts.txt");

    // read in prompts
    readFile();
    qDebug() << "prompts: " << prompts;

    // set prompt to nothing on init
    setPromptText(" ");

    // set a random prompt
    getRandomPrompt();
}


/**
 * @brief Typer::readFile
 */
void Typer::readFile()
{
    // clear current prompts
    prompts.clear();

    // read file
    QFile promptsFile(getPromptsFilename());
    if (promptsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!promptsFile.atEnd()) {
            // read prompt
            QString line = promptsFile.readLine();

            // add to prompts vector
            prompts.push_back(line.trimmed());      // add to vector and remove trailing whitespace
        }
    }

    promptsFile.close();
}


/**
 * @brief Typer::calculateAccuracy
 * @param text
 */
void Typer::calculateAccuracy(QString text) {
    // inits
    QString part;
    qreal hits = 0;
    int n = 2;
    Qt::CaseSensitivity caseSense = Qt::CaseSensitivity::CaseSensitive;
    QString tmpPrompt = getPromptText();
    float tmpAccuracy;

    text = QString(" ").repeated(n - 1) + text + QString(" ").repeated(n - 1);
    tmpPrompt = QString(" ").repeated(n - 1) + tmpPrompt + QString(" ").repeated(n - 1);

    for (int i = 0; i < text.count() - (n - 1); ++i) {
        part = text.mid(i, n);
        if (tmpPrompt.contains(part, caseSense)){
            hits++;
        }
    }


    if (text.length() < tmpPrompt.length()) {
        tmpAccuracy = 100 * hits / (text.length() - (n - 1));
    }

    else {
        tmpAccuracy = 100 * hits / (tmpPrompt.length() - (n - 1));
    }

    // set accuracy
    setAccuracy(tmpAccuracy);
}


/**
 * @brief Typer::calculateWordsPerMinute
 */
void Typer::calculateWordsPerMinute(QString text) {
    float keysPressed = text.length();
    qDebug() << "length of user input: " << keysPressed;

    float wpm = ((keysPressed / 5) / (getDuration() / 60));

    float adjustedWpm = wpm * (getAccuracy() / 100);

    setWordsPerMinute(adjustedWpm);
}


/**
 * @brief Typer::getRandomPrompt
 */
void Typer::getRandomPrompt() {
    // inits
    int lengthMax = 0;
    int lengthMin = 200;
    bool valid = false;

    switch (promptLength) {
    case SHORT:
        lengthMax = 50;
        lengthMin = 0;
        break;

    case MEDIUM:
        lengthMax = 99;
        lengthMin = 51;
        break;

    case LONG:
        lengthMax = 500;
        lengthMin = 100;
        break;

    default:
        qDebug() << "ERROR: prompt length switch exited determined control flow";
        break;
    }

    // find a prompt!
    srand(time(NULL));

    int counter = 0;

    while (!valid) {
        int num = rand() % prompts.size();
        QString tmp = prompts[num];

        counter++;

        if (tmp.length() >= lengthMin && tmp.length() <= lengthMax) {
            if (tmp != getPromptText()) {
                // set the prompt for the UI
                setPromptText(tmp);
                valid = true;
            }
        }

        // catch to prevent infinite while loop if there is only one option in a prompt length range
        if (counter > 100) {
            valid = true;
        }
    }
}


/**
 * @brief Typer::saveFile
 * @param text
 */
void Typer::saveFile(QString text) {
    // create unique filename
    QDateTime currentDate = QDateTime::currentDateTime();
    QString formattedTime = currentDate.toString("dd.MM.yyyy-hh:mm");
    QString name = "typing-test-" + formattedTime + "-results";

    // create full file path and name
    QString filename = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + name + ".txt";
    qDebug() << "new filename: " << filename;
    QFile saveFile(filename);

    // open new file
    if (saveFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        // open text stream
        QTextStream saveFileData(&saveFile);

        // write encrypted content
        saveFileData << "Test Information: \n";
        saveFileData << "Date: " + currentDate.toString("MM/dd/yyyy hh:mm") + "\n";
        saveFileData << "Prompt: " + getPromptText() + "\n";
        saveFileData << "Input: " + text + "\n\n";
        saveFileData << "Results: \n";
        saveFileData << "Accuracy: " + QString::number(getAccuracy()) + "%\n";
        saveFileData << "Duration: " + QString::number(getDuration()) + " seconds \n";
        saveFileData << "Words per Minute: " + QString::number(getWordsPerMinute()) + "\n";
    }

    saveFile.close();

    // Show pop-up in on save file completion
    QMessageBox messageBox;
    messageBox.setText("Successfully saved your results!\n" + filename);
    messageBox.exec();
}


/**
 * @brief Typer::addKeystrokeData
 * @param str
 * @param time
 */
void Typer::addKeystrokeData(QChar ch, float time) {
    // add data to each vector
    keystrokeData.push_back(ch);
    keystrokeTime.push_back(time);
}


void Typer::clearKeystrokeData() {
    // clear data
    keystrokeData.clear();
    keystrokeTime.clear();
}


// Getters & Setters
/**
 * @brief Typer::getPromptText
 * @return
 */
QString Typer::getPromptText() {
    return promptText;
}


/**
 * @brief Typer::setPromptText
 * @param text
 */
void Typer::setPromptText(QString text) {
    promptText = text;
}


/**
 * @brief Typer::getUserInput
 * @return
 */
QString Typer::getUserInput() {
    return userInput;
}


/**
 * @brief Typer::setUserInput
 * @param text
 */
void Typer::setUserInput(QString text) {
    userInput = text;
}


/**
 * @brief Typer::getDuration
 * @return
 */
float Typer::getDuration() {
    return duration;
}


/**
 * @brief Typer::setDuration
 * @param number
 */
void Typer::setDuration(float number) {
    duration = number;
}


/**
 * @brief Typer::getAccuracy
 * @return
 */
float Typer::getAccuracy() {
    return accuracy;
}


/**
 * @brief Typer::setAccuracy
 * @param number
 */
void Typer::setAccuracy(float number) {
    accuracy = number;
}


/**
 * @brief Typer::getWordsPerMinute
 * @return
 */
float Typer::getWordsPerMinute() {
    return wordsPerMinute;
}


/**
 * @brief Typer::setWordsPerMinute
 * @param number
 */
void Typer::setWordsPerMinute(float number) {
    wordsPerMinute = number;
}


/**
 * @brief Typer::getPromptsFilename
 * @return
 */
QString Typer::getPromptsFilename() {
    return promptsFilename;
}


/**
 * @brief Typer::setPromptsFilename
 * @param text
 */
void Typer::setPromptsFilename(QString text) {
    promptsFilename = text;
}


/**
 * @brief Typer::getPromptLength
 * @return
 */
PROMPT_LENGTH Typer::getPromptLength() {
    return promptLength;
}


/**
 * @brief Typer::setPromptLength
 */
void Typer::setPromptLength(PROMPT_LENGTH num) {
    promptLength = num;
}


/**
 * @brief Typer::getPrompts
 * @return
 */
std::vector<QString> Typer::getPrompts() {
    return prompts;
}


/**
 * @brief Typer::setPrompts
 */
void Typer::setPrompts(std::vector<QString> vec) {
    prompts = vec;
}


/**
 * @brief Typer::getKeystrokeData
 * @return
 */
std::vector<QChar> Typer::getKeystrokeData() {
    return keystrokeData;
}


/**
 * @brief Typer::setKeystrokeData
 * @param vec
 */
void Typer::setKeystrokeData(std::vector<QChar> vec) {
    keystrokeData = vec;
}


/**
 * @brief Typer::getKeystrokeTime
 * @return
 */
std::vector<float> Typer::getKeystrokeTime() {
    return keystrokeTime;
}


/**
 * @brief Typer::setKeystrokeTime
 * @param vec
 */
void Typer::setKeystrokeTime(std::vector<float> vec) {
    keystrokeTime = vec;
}
