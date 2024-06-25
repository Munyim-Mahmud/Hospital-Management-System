
#include <bits/stdc++.h>
#include <fstream> // For file handling
#include <string>
#include <ctime>
#include <queue>
#include <vector>

using namespace std;

// Structure for patient details
struct Patient {
    int id;
    string patientName;
    string patientAddress;
    string disease;
    string date;
    string admissionTime; // New field for admission time
    time_t admissionTimestamp; // Timestamp for priority comparison
    string doctorName; // New field for doctor name

    // Constructor to initialize date and time automatically
    Patient() {
        time_t now = time(nullptr);
        tm* t = localtime(&now);
        char myDate[12];
        char myTime[12];
        strftime(myDate, sizeof(myDate), "%d/%m/%Y", t);
        strftime(myTime, sizeof(myTime), "%I:%M %p", t); // 12-hour format with AM/PM
        date = myDate;
        admissionTime = myTime;
        admissionTimestamp = now; // Save the timestamp
    }

    bool operator<(const Patient& other) const {
        // Custom comparator for priority queue (earlier admission gets higher priority)
        return admissionTimestamp > other.admissionTimestamp;
    }
};

// Node structure for linked list
struct Node {
    Patient data;
    Node* next;

    // Constructor
    Node(const Patient& p) : data(p), next(nullptr) {}
};

// Class for managing patient list with priority queue and linked list
class PatientList {
private:
    priority_queue<Patient> patientQueue;
    Node* head;
    Node* tail;

public:
    // Constructor
    PatientList() : head(nullptr), tail(nullptr) {}

    // Destructor
    ~PatientList() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Function to add patient to the list
    void addPatient(const Patient& p) {
        Node* newNode = new Node(p);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        patientQueue.push(p);
    }

    // Function to display all patients
    void displayPatients() {
        if (!head) {
            cout << "\n\t\t\t\t\t\tPatient List is empty.\n";
            return;
        }

        cout << "\n\n\t\t\t\t\t\t<== Patient List ==>\n\n";
        cout << "ID\tName\t\tAddress\t\tDisease\t\tDate\t\tAdmission Time\tDoctor Name\n";
        Node* temp = head;
        while (temp) {
            Patient& p = temp->data;
            cout << p.id << "\t" << p.patientName << "\t\t" << p.patientAddress << "\t\t" << p.disease << "\t\t" << p.date << "\t" << p.admissionTime << "\t" << p.doctorName << "\n";
            temp = temp->next;
        }
        cout << endl;
    }

    // Function to find and remove patient by ID
    void removePatient(int id) {
        if (!head) {
            cout << "\n\t\t\t\t\t\tPatient List is empty.\n";
            return;
        }

        Node* current = head;
        Node* prev = nullptr;

        while (current) {
            if (current->data.id == id) {
                // Confirm discharge with the doctor
                string confirmation;
                cout << "\n\t\t\t\t\t\tDoctor " << current->data.doctorName << " needs to confirm discharge.\n";
                cout << "\t\t\t\t\t\tEnter 'yes' to confirm: ";
                cin >> confirmation;

                if (confirmation != "yes") {
                    cout << "\n\t\t\t\t\t\tDischarge cancelled.\n";
                    return;
                }

                if (current == head) {
                    head = head->next;
                } else {
                    prev->next = current->next;
                    if (current == tail) {
                        tail = prev;
                    }
                }
                cout << "\n\t\t\t\t\t\tPatient discharged successfully by Dr. " << current->data.doctorName << ".\n";
                delete current;
                return;
            }
            prev = current;
            current = current->next;
        }

        cout << "\n\t\t\t\t\t\tPatient not found.\n";
    }

    // Function to load patients from file
    void loadPatientsFromFile(const string& filename) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cout << "\n\t\t\t\t\t\tError opening file: " << filename << endl;
            return;
        }

        while (!inFile.eof()) {
            Patient p;
            inFile >> p.id;
            inFile.ignore(); // Ignore newline after ID
            getline(inFile, p.patientName);
            getline(inFile, p.patientAddress);
            getline(inFile, p.disease);
            getline(inFile, p.date);
            getline(inFile, p.admissionTime);
            getline(inFile, p.doctorName); // Read doctor name

            if (inFile.fail()) break;

            // Compute admissionTimestamp based on the read date and time
            struct tm t = {};
            sscanf(p.date.c_str(), "%d/%m/%Y", &t.tm_mday, &t.tm_mon, &t.tm_year);
            t.tm_year -= 1900; // tm_year is the number of years since 1900
            t.tm_mon -= 1; // tm_mon is 0-based

            int hour, minute;
            char ampm[3];
            sscanf(p.admissionTime.c_str(), "%d:%d %s", &hour, &minute, ampm);
            if (strcmp(ampm, "PM") == 0 && hour != 12) {
                hour += 12;
            }
            if (strcmp(ampm, "AM") == 0 && hour == 12) {
                hour = 0;
            }
            t.tm_hour = hour;
            t.tm_min = minute;
            t.tm_sec = 0;

            p.admissionTimestamp = mktime(&t);

            addPatient(p);
        }

        inFile.close();
    }

    // Function to save patients to file
    void savePatientsToFile(const string& filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cout << "\n\t\t\t\t\t\tError opening file: " << filename << endl;
            return;
        }

        Node* temp = head;
        while (temp) {
            Patient& p = temp->data;
            outFile << p.id << "\n" << p.patientName << "\n" << p.patientAddress << "\n" << p.disease << "\n" << p.date << "\n" << p.admissionTime << "\n" << p.doctorName << "\n";
            temp = temp->next;
        }

        outFile.close();
    }
};

// Main function to run the hospital management system
int main() {
    PatientList patientList;
    const string patientFileName = "patients.txt";

    // Load patients from file on startup
    patientList.loadPatientsFromFile(patientFileName);

    vector<string> doctorNames = {"Dr. Debi Sheti(Cardiologist)", "Dr. Hamid Hasan(Neurologist)", "Dr. Anisul Awal(Medicine Specialist)", "Dr. Abdul Karim(Internal Medicine)", "Dr. Rasel Viper(Poison Specialist)"};

    int choice;

    while (true) {
        cout << "\n\n\t\t\t\t\t\t<== Hospital Management System ==>\n";
        cout << "\n\t\t\t\t\t\t1. Admit Patient\n";
        cout << "\t\t\t\t\t\t2. Display Patient List\n";
        cout << "\t\t\t\t\t\t3. Discharge Patient\n";
        cout << "\t\t\t\t\t\t0. Exit\n\n";
        cout << "\t\t\t\t\t\tEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 0:
                cout << "\n\t\t\t\t\t\tExiting... Thank you!\n\n";
                // Save patients to file before exit
                patientList.savePatientsToFile(patientFileName);
                return 0;

            case 1: {
                Patient p;
                cout << "\n\t\t\t\t\t\tEnter Patient ID: ";
                cin >> p.id;
                cin.ignore(); // Clear the newline character from buffer
                cout << "\t\t\t\t\t\tEnter Patient Name: ";
                getline(cin, p.patientName);
                cout << "\t\t\t\t\t\tEnter Patient Address: ";
                getline(cin, p.patientAddress);
                cout << "\t\t\t\t\t\tEnter Patient Disease: ";
                getline(cin, p.disease);

                // Display doctor list
                cout << "\n\t\t\t\t\t\tSelect a Doctor from the list below:\n";
                for (size_t i = 0; i < doctorNames.size(); ++i) {
                    cout << "\t\t\t\t\t\t" << i + 1 << ". " << doctorNames[i] << "\n";
                }
                int doctorChoice;
                cout << "\t\t\t\t\t\tEnter your choice: ";
                cin >> doctorChoice;

                // Validate doctor choice
                if (doctorChoice < 1 || doctorChoice > doctorNames.size()) {
                    cout << "\n\t\t\t\t\t\tInvalid choice. Please try again.\n";
                    break;
                }

                p.doctorName = doctorNames[doctorChoice - 1];

                patientList.addPatient(p); // Add patient to the list
                cout << "\n\t\t\t\t\t\tPatient Admitted Successfully.\n";
                break;
            }

            case 2:
                patientList.displayPatients();
                break;

            case 3: {
                int id;
                cout << "\n\t\t\t\t\t\tEnter Patient ID to discharge: ";
                cin >> id;
                patientList.removePatient(id); // Remove patient from the list
                break;
            }

            default:
                cout << "\n\t\t\t\t\t\tInvalid Choice...\n";
                break;
        }
    }

    return 0;
}

