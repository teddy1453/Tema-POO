#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

using namespace std;
void createFlow();
void viewFlows();
void viewSpecificFlow();
void runFlow();
void deleteFlow();


class Step {
public:
    bool wasSkipped = false;
    virtual void execute() = 0;
    virtual string getDescription() const = 0;
    virtual string getInfo() const { return ""; }
};



class TitleStep : public Step {
    string title;
    string subtitle;
public:
    TitleStep(const string& title, const string& subtitle) 
        : title(title), subtitle(subtitle) {}
    void execute() override {
        cout << "Title: " << title << '\n';
        cout << "Subtitle: " << subtitle << '\n';
    }
    string getDescription() const override {
        return  title + "\n" + subtitle;
    }
    string getInfo() const override {
        return "Title: " + title; 
    }
};

class TextStep : public Step {
    string title;
    string copy;
public:
    TextStep(const string& title, const string& copy) 
        : title(title), copy(copy) {}
    void execute() override {
        cout << "Title: " << title << '\n';
        cout << "Copy: " << copy << '\n';
    }
    string getDescription() const override {
        return title + "\n" + copy;
    }
};

class TextInputStep : public Step {
    string description;
    string input;
public:
    TextInputStep(const string& description) : description(description) {}
    void execute() override {
        cout << description << '\n';
        cout << "Give me a text: ";
        getline(cin, input);
    }
    string getDescription() const override {
        return description;
    }
    string getInfo() const override {
        return input; 
    }
};

class NumberInputStep : public Step {
    string description;
    float input;
public:
    NumberInputStep(const string& description) : description(description) {}
    void execute() override {
        cout << description << endl;
        cout << "Give me a number: ";
        cin >> input;
    }
    float getInput() const {
        return input;
    }
    string getInfo() const override {
        return to_string(input);
    }
    string getDescription() const override {
        return description;
    }
};

template <typename T>
class CalculusStep : public Step {
private:
    const Step* operand1;
    const Step* operand2;
    char operation;

public:
    CalculusStep(const Step* op1, const Step* op2, char op) : operand1(op1), operand2(op2), operation(op) {}

    void execute() override {
        T value1 = getValueFromStep(*operand1);
        T value2 = getValueFromStep(*operand2);
        T result;

        switch (operation) {
            case '+':
                result = value1 + value2;
                break;
            case '-':
                result = value1 - value2;
                break;
            case '*':
                result = value1 * value2;
                break;
            case '/':
                if (value2 != 0) {
                    result = value1 / value2;
                } else {
                    throw std::runtime_error("Division by zero");
                }
                break;
            case 'm': // 'm' for min
                result = std::min(value1, value2);
                break;
            case 'M': // 'M' for max
                result = std::max(value1, value2);
                break;
            default:
                throw std::runtime_error("Invalid operation");
        }

        std::cout << "Result: " << result << '\n';
    }

    std::string getDescription() const override {
        std::ostringstream oss;
        oss << "CalculusStep " << operation;
        return oss.str();
    }

private:
    T getValueFromStep(const Step& step) const {
        std::istringstream iss(step.getInfo());
        T value;
        iss >> value;

        return value;
    }
};

class DisplayStep : public Step {
    string filename;
public:
    DisplayStep(const string& filename) : filename(filename) {}
    void execute() override {
    const string directoryPath = "fisiere";
    string name = filename + ".txt";
    filesystem::path filePath = directoryPath + "/" + name;

    // Verifies if the file exists
    if (filesystem::exists(filePath)) {
        ifstream file(filePath);
        if (!file) {
            throw runtime_error("Could not open file: " + name);
        }

        string line;
        while (getline(file, line)) {
            cout << line << '\n';
        }
    } else {
        throw runtime_error("File does not exist: " + name);
    }
}
    string getDescription() const override {
        return filename;
    }
    string getInfo() const override {
        return filename;
    }
};

class TextFileInputStep : public Step {
    string description;
    string fileName;
    string fileContent;
public:
    TextFileInputStep(const string& description) 
        : description(description) {}
    void execute() override {
        cout << "Enter the text file name: \n";
        cin >> fileName;

        if(fileName.find(".txt") == std::string::npos)
            fileName += ".txt";

        ofstream file(fileName);
        if(file.is_open()){
            cout << "File " << fileName << " is create \n";
            cout << "Enter the content of the file. When finished, type 'STOP' \n";
            cin.ignore();
            while(getline(cin, fileContent)){
                if(fileContent == "STOP")
                {
                    cout << "End of story..";
                    break;
                }
            file << fileContent << endl;
            }
            cout << "The content is written in file. \n";
        }
        else 
            cout << "Error creating the file. \n";
    }
    string getFileContent() const {
        return fileContent;
    }
    string getDescription() const override {
        return description;
    }
    string getInfo() const override {
        return fileContent;
    }
    
};

class CSVFileInputStep : public Step {
    string description;
    string fileName;
    string fileContent;

public:
    CSVFileInputStep(const string& description) 
        : description(description) {}

    void execute() override {
        cout << "Enter the csv file name: \n";
        cin >> fileName;

        if (fileName.find(".csv") == string::npos)
            fileName += ".csv";

        ofstream file(fileName.c_str());

        if (!file.is_open())
            cerr << "The file is not open. \n";

        int rows, cols; 
        cout << "Enter the number of rows: \n";
        cin >> rows;
        cout << "Enter the number of cols: \n";
        cin >> cols;

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j){
                double value;
                string input;
                cout << "Enter value in file or 'q' to exit : \n";
                cin >> input;

                if (input == "q") {
                    cout << "Exit \n" << endl;
                    file.close(); 
                    return;
                }

                istringstream iss(input);
                iss >> value;

                file << value;

                if (j < cols - 1)
                    file << ",";
            }
            file << " ";
            file << endl;
        }

        cout << "The CSV file is created: " << fileName << endl;
        file.close();
    }

    string getFileContent() const {
        return fileContent;
    }

    string getDescription() const override {
        return description;
    }

    string getInfo() const override {
        return fileContent;
    }
};

class OutputStep : public Step {
private:
    int step;
    string fileName;
    string title;
    string description;
    Step& previousStep;

public:
    OutputStep(int step, const string& fileName, const string& title, const string& description, Step& previousStep)
        : step(step), fileName(fileName), title(title), description(description), previousStep(previousStep) {}

    void execute() override {

        if(fileName.find(".txt") == std::string::npos)
            fileName += ".txt";

        ofstream file(fileName);
        if (!file.is_open()) {
            throw runtime_error("Could not open file: " + fileName);}
        
        file << "File Name: " << fileName << '\n';
        file << "Title: " << title << '\n';
        file << "Description: " << description << '\n';
        file << "Information from step " << step << ": " << previousStep.getInfo() << '\n';

        cout << "Open file for detail, file name: " << fileName << "\n";
        
    }

    string getDescription() const override {
        return to_string(step) + "\n" + fileName + "\n" + title + "\n" + description + "\n" + previousStep.getInfo();
    }
};

class EndStep : public Step {
public:
    void execute() override {
        cout << "End of flow.\n";
    }
    string getDescription() const override {
        return "End step";
    }
};

class Analytics {
    int timesStarted = 0;
    int timesCompleted = 0;
    int* skipCounts;
    int* errorCounts;
    int skipCountsSize;
public:
    Analytics(int maxSteps) : skipCountsSize(maxSteps) {
        skipCounts = new int[skipCountsSize]();
        errorCounts = new int[skipCountsSize]();
    }
    ~Analytics() {
        delete[] skipCounts;
        delete[] errorCounts;
    }
    void start() { timesStarted++; }
    void complete() { timesCompleted++; }
    void skip(int stepIndex) { skipCounts[stepIndex]++; }
    void error(int stepIndex) { errorCounts[stepIndex]++; } 
    void print() const {
        cout << "Times started: " << timesStarted << '\n';
        cout << "Times completed: " << timesCompleted << '\n';
        cout << "Skip counts:\n";
        for (int i = 0; i < skipCountsSize; i++) {
            cout << "  Step " << (i + 1) << ": " << skipCounts[i] << '\n';
        }
        cout << "Error counts:\n";
        for (int i = 0; i < skipCountsSize; i++) {
            cout << "  Step " << (i + 1) << ": " << errorCounts[i] << '\n';
        }
    }

};

class Flow {
private:
    Step** steps;
    int stepCount;
    int stepCapacity;
    string name;
    time_t timestamp;
    Analytics analytics;


public:
    Flow(const string& name, int maxSteps) : name(name), timestamp(time(nullptr)), stepCount(0), stepCapacity(maxSteps), analytics(maxSteps)  {
        steps = new Step*[stepCapacity];
    }

    ~Flow() {
        for (int i = 0; i < stepCount; i++) {
            delete steps[i];
        }
        delete[] steps;
    }

    int getStepCount() const {
        return stepCount;
    }

    string getName() const {
        return name;
    }

    int getMaxSteps() const {
        return stepCapacity;
    }

    void addStep(Step* step) {
        if (stepCount >= stepCapacity) {
            throw runtime_error("Cannot add more steps: capacity reached");
        }
        steps[stepCount++] = step;
    }

   

  void run(Step& step, int stepIndex) {
    try {
        analytics.start();
        cout << "Do you want to skip this step? (Press 's' to skip, Enter to continue): ";
        char choice;
        cin.get(choice);

        if (choice == 's' || choice == 'S') {
            cout << "Step skipped ! " << '\n';
            analytics.skip(stepIndex);
            return;
        } else if (choice == '\n') {
            step.execute();
        }
    } catch (const std::exception& e) {
        cout << "Error executing step: " << e.what() << '\n';
        analytics.error(stepIndex);
    }
    analytics.complete();
}
    
    
    Step* getStep(int index) const {
        if (index >= 0 && index < stepCount) {
            return steps[index];
        } else {
            return nullptr;
        }
    }

    void printSteps() const {
        for (int i = 0; i < stepCount; i++) {
            cout << steps[i]->getDescription() << '\n';
        }
    }

    void viewAnalytics()
    {
        analytics.print();
    }

};


class ProcessBuilderMenu {
public:
void showMenu() {
    while (true) {
        cout << "----- Process Builder Menu -----" << endl;
        cout << "1. Create a new flow" << endl;
        cout << "2. View existing flows" << endl;
        cout << "3. Run a flow" << endl;
        cout << "4. View Analystics " <<endl;
        cout << "5. Delete a flow" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1:
                createFlow();
                break;
            case 2:
                viewFlows();
                break;
            case 3:
                runFlow();
                break;
            case 4:
                viewAnalytics();
                break;
            case 5:
                deleteFlow();
                break;
            case 6:
                return;
            default:
                cout << "Invalid choice" << endl;
        }
    }
}

void saveFlowToFile(const Flow& flow) {
    ofstream file("flows/" + flow.getName() + ".txt");
    if (!file.is_open()) {
        throw runtime_error("Could not open file for writing: " + flow.getName() + ".txt");
    }

    file << flow.getName() << '\n';
    file << flow.getMaxSteps() << '\n';

    for (int i = 0; i < flow.getStepCount(); i++) {
        Step* step = flow.getStep(i);

        // Verificăm tipul de pas și scriem eticheta corespunzătoare în fișier
        if (dynamic_cast<TitleStep*>(step)) {
            file << "TitleStep\n";
        } 
        else if (dynamic_cast<TextStep*>(step)) {
            file << "TextStep\n";
        } 
        else if (dynamic_cast<TextInputStep*>(step)) {
            file << "TextInputStep\n";
        } 
         else if (dynamic_cast<NumberInputStep*>(step)) {
            file << "NumberInputStep\n";}

        else if (dynamic_cast<CalculusStep<float>*>(step)) {
            file << "CalculusStep\n";
        }

        else if (dynamic_cast<DisplayStep*>(step)) {
            file << "DisplayStep\n";}
            
        else if (dynamic_cast<TextFileInputStep*>(step)) {
            file << "TextFileInputStep\n";}
        
        else if (dynamic_cast<CSVFileInputStep*>(step)) {
            file << "CSVFileInputStep\n";}

        else if (dynamic_cast<OutputStep*>(step)) {
            file << "OutputStep\n";}

        else if (dynamic_cast<EndStep*>(step)) {
            file << "EndStep\n";
        }
        else {
            throw runtime_error("Unknown step type");
        }

        // Scriem detaliile specifice pasului în fișier
        file << step->getDescription() << '\n';
    }
}





void viewAllFlows() {
    const string directoryPath = "flows";
    
    int index = 1; // Index pentru numerotare

    for (const auto& entry : filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            // Obținem numele fișierului fără extensie
            string fileName = entry.path().filename().stem().string();

            // Afișăm numărul și numele fișierului
            cout << index++ << ". " << fileName << '\n';
        }
    }
}


void viewFlows() {
    cout << "List with name of flows: " << endl;
    viewAllFlows();
    const string directoryPath = "flows";

    // Solicită utilizatorului să introducă numele flowului
    cout << "Enter the name of the flow: ";
    string name;
    getline(cin, name);

    // Adaugă extensia ".txt" la numele flowului
    string fileName = name + ".txt";

    // Construiește calea către fișierul specificat
    filesystem::path filePath = directoryPath + "/" + fileName;

    // Verifică dacă fișierul există
    if (filesystem::exists(filePath)) {
        ifstream file(filePath);
        if (!file) {
            throw runtime_error("Could not open file: " + fileName);
        }
        
        cout << "Flow Name: " << name << '\n';
        string line;
        getline(file, line);
        getline(file, line);
        
        while (getline(file, line)) {
            if (!line.empty()) {
                // Read only the first two words from each line
                istringstream iss(line);
                cout << line << '\n';
            }
            
        }
    } else {
        cout << "Flow not found\n";
    }

    cout << "Press enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void createFlow() {
    cout << "Enter the name of the flow: ";
    string name;
    getline(cin, name);
    cout << "Enter the maximum number of steps: ";
    int maxSteps;
    cin >> maxSteps;
    cin.ignore();
    Flow flow(name, maxSteps);
    while (true) {
        cout << "----- Create Flow Menu -----" << endl;
        cout << "1. Add a title step" << endl;
        cout << "2. Add a text step" << endl;
        cout << "3. Add a text input step" << endl;
        cout << "4. Add a number input step" << endl;
        cout << "5. Add a calculus step" << endl;
        cout << "6. Add a display step" << endl;
        cout << "7. Add a text file input step" << endl;
        cout << "8. Add a CSV file input step" << endl;
        cout << "9. Add an output step" << endl;
        cout << "10. Add an end step" << endl;
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1: {
                cout << "TITLE STEP\n";
                cout << "Enter the title: ";
                string title;
                getline(cin, title);
                cout << "Enter the subtitle: ";
                string subtitle;
                getline(cin, subtitle);
                flow.addStep(new TitleStep(title, subtitle));
                break;
            }
            case 2: {
                cout << "TEXT STEP\n";
                cout << "Enter the title: ";
                string title;
                getline(cin, title);
                cout << "Enter the copy: ";
                string copy;
                getline(cin, copy);
                flow.addStep(new TextStep(title, copy));
                break;
            }
            case 3: {
                cout << "TEXT INPUT STEP\n";
                cout << "Enter the description: ";
                string description;
                getline(cin, description);
                flow.addStep(new TextInputStep(description));
                break;
            }
            case 4: {
                cout<< "NUMBER INPUT STEP\n";	
                cout << "Enter the description: ";
                string description;
                getline(cin, description);
                flow.addStep(new NumberInputStep(description));
                break;
            }
            case 5: {
                // Adaugă un pas de tip CalculusStep la flux
                cout << "CALCULUS STEP\n";
                cout << "Enter the operation (+, -, *, /, min, max): ";
                char operation;
                cin >> operation;
                cin.ignore();

                int operand1Index, operand2Index;
                cout << "Enter the index of the first operand step: ";
                cin >> operand1Index;
                cin.ignore();

                cout << "Enter the index of the second operand step: ";
                cin >> operand2Index;
                cin.ignore();

                // Verifică dacă indicii operanzilor sunt valizi
                if (operand1Index >= 1 && operand1Index <= flow.getStepCount() &&
                    operand2Index >= 1 && operand2Index <= flow.getStepCount()) {
                    // Adaugă un CalculusStep la flux
                    Step* operand1 = flow.getStep(operand1Index - 1);
                    Step* operand2 = flow.getStep(operand2Index - 1);
                    flow.addStep(new CalculusStep<float>(operand1, operand2, operation));

                    // Afișează informațiile în fișier
                    ofstream file("flows/" + name + ".txt", ios::app);
                    file << "CalculusStep " << operation << " " << operand1Index << " " << operand2Index << "\n";
                    file.close();
                } else {
                    cout << "Invalid operand indices" << endl;
                }

                break;
            }


            case 6: {
                cout << "DISPLAY STEP\n";
                cout << "Enter the filename: ";
                string filename;
                getline(cin, filename);
                flow.addStep(new DisplayStep(filename));
                break;
            }

            case 7: {
                cout << "TEXT FILE INPUT STEP\n";
                cout << "Enter the description of text filename: ";
                string description;
                getline(cin, description);
                flow.addStep(new TextFileInputStep(description));
                break;
            }

            case 8: {
                cout << "CSV FILE INPUT STEP\n";
                cout << "Enter the description of CSV filename: ";
                string description;
                getline(cin, description);
                flow.addStep(new CSVFileInputStep(description));
                break;
            }

            case 9: {
                cout << "OUTPUT STEP\n";
                cout << "Enter the step number: ";
                int step;
                cin >> step;
                cin.ignore();
                cout << "Enter the filename: ";
                string filename;
                getline(cin, filename);
                cout << "Enter the title: ";
                string title;
                getline(cin, title);
                cout << "Enter the description: ";
                string description;
                getline(cin, description);
                flow.addStep(new OutputStep(step, filename, title, description, *flow.getStep(step - 1)));
                break;
            }

            case 10:
                cout << "END STEP\n";
                flow.addStep(new EndStep());
                saveFlowToFile(flow);  
                return;
            default:
                cout << "Invalid choice" << endl;
        }
    }
    saveFlowToFile(flow);

}

void runFlow() {
    // Afișează toate flowurile disponibile
    viewAllFlows();

    // Solicită utilizatorului să aleagă un nume pentru flow
    cout << "Choose a name for the flow:\n";
    string flowName;
    getline(cin, flowName);

    // Definirea căii și a numelui de fișier pentru flow
    const string directoryPath = "flows";
    string fileName = flowName + ".txt";
    filesystem::path filePath = directoryPath + "/" + fileName;

    // Verifică dacă fișierul există
    if (filesystem::exists(filePath)) {
        ifstream file(filePath);
        if (!file) {
            // Aruncă o excepție dacă nu se poate deschide fișierul
            throw runtime_error("Could not open file: " + fileName);
        }

        cout << "Executing the flow: " << flowName << endl;

        // Citește numele flowului și numărul maxim de pași
        file >> flowName;
        int maxSteps;
        file >> maxSteps;
        file.ignore(); // Consumă caracterul newline

        // Creează flowul
        Flow flow(flowName, maxSteps);

        // Citirea și adăugarea fiecărui pas în flow
        while (!file.eof()) {
            string stepType;
            file >> stepType;
            file.ignore(); // Consumă caracterul newline

            if (stepType == "TitleStep") {
                // Citirea datelor pentru pasul TitleStep și adăugarea la flow
                string title, subtitle;
                getline(file, title);
                getline(file, subtitle);
                TitleStep* step = new TitleStep(title, subtitle);
                flow.addStep(step);

            } else if (stepType == "TextStep") {
                // Citirea datelor pentru pasul TextStep și adăugarea la flow
                string title, copy;
                getline(file, title);
                getline(file, copy);
                TextStep* step = new TextStep(title, copy);
                flow.addStep(step);

            } else if (stepType == "TextInputStep") {
                // Citirea datelor pentru pasul TextInputStep și adăugarea la flow
                string description;
                getline(file, description);
                TextInputStep* step = new TextInputStep(description);
                flow.addStep(step);

            } else if (stepType == "NumberInputStep") {
                // Citirea datelor pentru pasul NumberInputStep și adăugarea la flow
                string description;
                getline(file, description);
                NumberInputStep* step = new NumberInputStep(description);
                flow.addStep(step);

            } else if (stepType == "CalculusStep") {
                // Citirea datelor pentru pasul CalculusStep și adăugarea la flow
                char operation;
                file >> operation;
                file.ignore();

                int operand1Index, operand2Index;
                file >> operand1Index >> operand2Index;
                file.ignore();

                // Verifică dacă indicii operanzilor sunt valizi
                if (operand1Index >= 1 && operand1Index <= flow.getStepCount() &&
                    operand2Index >= 1 && operand2Index <= flow.getStepCount()) {
                    Step* operand1 = flow.getStep(operand1Index - 1);
                    Step* operand2 = flow.getStep(operand2Index - 1);

                    flow.addStep(new CalculusStep<float>(operand1, operand2, operation));

                } else {
                    cout << "Invalid operand indices in CalculusStep" << endl;
                }
            } else if (stepType == "DisplayStep") {
                // Citirea datelor pentru pasul DisplayStep și adăugarea la flow
                string filename;
                getline(file, filename);
                DisplayStep* step = new DisplayStep(filename);
                flow.addStep(step);

            } else if (stepType == "TextFileInputStep") {
                // Citirea datelor pentru pasul TextFileInputStep și adăugarea la flow
                string description;
                getline(file, description);
                TextFileInputStep* step = new TextFileInputStep(description);
                flow.addStep(step);

            } else if (stepType == "CSVFileInputStep") {
                // Citirea datelor pentru pasul CSVFileInputStep și adăugarea la flow
                string description;
                getline(file, description);
                CSVFileInputStep* step = new CSVFileInputStep(description);
                flow.addStep(step);

            } else if (stepType == "OutputStep") {
                // Citirea datelor pentru pasul OutputStep și adăugarea la flow
                int s;
                string filename1, title, description;
                file >> s;
                file.ignore();
                getline(file, filename1);
                getline(file, title);
                getline(file, description);
                OutputStep* step = new OutputStep(s, filename1, title, description, *flow.getStep(s-1));
                flow.addStep(step);

            } else if (stepType == "EndStep") {
                // Nu există conținut după EndStep, deci nu este nevoie să se citească nimic
                flow.addStep(new EndStep());
            }
        }

        // Rulează fiecare pas din flow
        for (int i = 0; i < flow.getStepCount(); i++) {
            Step* step = flow.getStep(i);
            cout << "Press Enter to execute Step " << i + 1 << "...";
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            flow.run(*step, i);
        }

        // Afișează analiticele flowului
        flow.viewAnalytics();
     } else {
        cout << "Flow not found\n";
    }
}

void viewAnalytics() {
    viewAllFlows();

    cout << "Choose a name for the flow: \n";
    string flowName;
    getline(cin, flowName);

    const string directoryPath = "flows";
    string fileName = flowName + ".txt";
    filesystem::path filePath = directoryPath + "/" + fileName;

    if (filesystem::exists(filePath)) {
        ifstream file(filePath);
        if (!file) {
            throw runtime_error("Could not open file: " + fileName);
        }

        // Read the flowName and maxSteps, but we don't need them for viewing analytics
        file >> flowName;
        int maxSteps;
        file >> maxSteps;
        file.ignore(); // Consume the newline character

        Flow flow(flowName, maxSteps);

        
        while (!file.eof()) {
            string stepType;
            file >> stepType;
            file.ignore(); // Consume the newline character

            if (stepType == "TitleStep") {
                string title;
                getline(file, title);
                string subtitle;
                getline(file, subtitle);
                TitleStep* step = new TitleStep(title, subtitle);
                flow.addStep(step);

            } else if (stepType == "TextStep") {
                string title1;
                getline(file, title1);
                string copy;
                getline(file, copy);
                TextStep* step = new TextStep(title1, copy);
                flow.addStep(step);

            } else if (stepType == "TextInputStep") {
                string description;
                getline(file, description);
                TextInputStep* step = new TextInputStep(description);
                flow.addStep(step);

            }else if (stepType == "NumberInputStep") {
                string description;
                getline(file, description);
                NumberInputStep* step = new NumberInputStep(description);
                flow.addStep(step);

            }else if (stepType == "DisplayStep") {
                string filename;
                getline(file, filename);
                DisplayStep* step = new DisplayStep(filename);
                flow.addStep(step);

            }else if (stepType == "TextFileInputStep") {
                string description;
                getline(file, description);
                TextFileInputStep* step = new TextFileInputStep(description);
                flow.addStep(step);

            }else if (stepType == "CSVFileInputStep") {
                string description;
                getline(file, description);
                CSVFileInputStep* step = new CSVFileInputStep(description);
                flow.addStep(step);

            }else if (stepType == "OutputStep") {
                int s;
                string filename1, title, description;
                file >> s;
                file.ignore();
                getline(file, filename1);
                getline(file, title);
                getline(file, description);
                OutputStep* step = new OutputStep(s, filename1, title, description, *flow.getStep(s-1));
                flow.addStep(step);

            } else if (stepType == "EndStep") {
                // No content after EndStep, so no need to read anything
                flow.addStep(new EndStep());}
            
        }

        // Run each step, but don't execute the actual logic
      
        
        // View the analytics for the flow
        flow.viewAnalytics();
    } else {
        cout << "Flow not found\n";
    }
}

void deleteFlow() {
    cout << "Enter the name of the flow to delete: ";
    string name;
    getline(cin, name);

    // Combinăm directorul curent cu subdirectorul "flows"
    string flowDirectory = "flows/";
    string filePath = flowDirectory + name + ".txt";

    // Folosim std::filesystem pentru a verifica dacă fișierul există
    if (filesystem::exists(filePath)) {
        // Ștergem fișierul dacă există
        try {
            filesystem::remove(filePath);
            cout << "Flow '" << name << "' has been successfully deleted.\n";
        } catch (const filesystem::filesystem_error& e) {
            cerr << "Error deleting the flow: " << e.what() << '\n';
        }
    } else {
        // Afisăm un mesaj în cazul în care fișierul nu a fost găsit
        cout << "Flow '" << name << "' not found.\n";
    }
    cout << "Press enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

};

int main() {
    ProcessBuilderMenu menu;
    menu.showMenu();

    return 0;
}