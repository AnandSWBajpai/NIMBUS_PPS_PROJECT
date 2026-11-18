
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DOCTORS 3
#define MAX_APPOINTMENTS 500
#define MAX_SLOTS 5

#define BOOKED 1
#define CANCELLED 0
#define COMPLETED 2
#define MISSED 3

/* ---------- STRUCTURES ---------- */

typedef struct {
    int id;
    char name[50];
    char specialization[50];
    int avgConsultTime;
    int slots[MAX_SLOTS]; /* 0 = free, 1 = occupied */
} Doctor;

typedef struct {
    int id;
    char name[50];
    char symptoms[100];
} Patient;

typedef struct {
    int id;
    int patientId;
    int doctorId;
    int slot;
    int status;
} Appointment;

typedef struct QueueNode {
    int patientId;
    int doctorId;
    int emergency;
    int estimatedWait;
    struct QueueNode *next;
} QueueNode;

/* ---------- GLOBAL DATA ---------- */

Doctor doctors[MAX_DOCTORS];
Patient *patients = NULL;
Appointment appointments[MAX_APPOINTMENTS];

int patientCount = 0;
int patientCapacity = 0;
int appointmentCount = 0;
int nextAppointmentId = 1;

QueueNode *front = NULL;
QueueNode *rear = NULL;

int totalWaitTime = 0;
int patientsServed = 0;

/* ---------- INPUT FUNCTIONS ---------- */

int readInt(const char *message) {
    char buffer[100];
    int value;
    char extra;

    while (1) {
        printf("%s", message);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Input ended. Exiting.\n");
            exit(0);
        }

        if (sscanf(buffer, " %d %c", &value, &extra) == 1)
            return value;

        printf("Invalid input. Enter a whole number.\n");
    }
}

void readString(const char *message, char *output, int size) {
    printf("%s", message);

    if (fgets(output, size, stdin) == NULL) {
        printf("Input ended. Exiting.\n");
        exit(0);
    }

    output[strcspn(output, "\n")] = '\0';
}

/* ---------- SEARCH FUNCTIONS ---------- */

int findPatient(int id) {
    for (int i = 0; i < patientCount; i++) {
        if (patients[i].id == id)
            return i;
    }
    return -1;
}

int findDoctor(int id) {
    for (int i = 0; i < MAX_DOCTORS; i++) {
        if (doctors[i].id == id)
            return i;
    }
    return -1;
}

int findAppointment(int id) {
    for (int i = 0; i < appointmentCount; i++) {
        if (appointments[i].id == id)
            return i;
    }
    return -1;
}

/* ---------- DOCTOR FUNCTIONS ---------- */

void initializeDoctors(void) {
    doctors[0].id = 1;
    strcpy(doctors[0].name, "Dr. Sharma");
    strcpy(doctors[0].specialization, "General Physician");
    doctors[0].avgConsultTime = 15;

    doctors[1].id = 2;
    strcpy(doctors[1].name, "Dr. Verma");
    strcpy(doctors[1].specialization, "Cardiologist");
    doctors[1].avgConsultTime = 20;

    doctors[2].id = 3;
    strcpy(doctors[2].name, "Dr. Khan");
    strcpy(doctors[2].specialization, "Dermatologist");
    doctors[2].avgConsultTime = 10;

    for (int i = 0; i < MAX_DOCTORS; i++) {
        for (int j = 0; j < MAX_SLOTS; j++)
            doctors[i].slots[j] = 0;
    }
}

void displayDoctors(void) {
    printf("\n========== DOCTOR PROFILES ==========\n");

    for (int i = 0; i < MAX_DOCTORS; i++) {
        printf("\nDoctor ID: %d\n", doctors[i].id);
        printf("Name: %s\n", doctors[i].name);
        printf("Specialization: %s\n",
               doctors[i].specialization);
        printf("Average consultation: %d minutes\n",
               doctors[i].avgConsultTime);

        printf("Available slots: ");

        int found = 0;
        for (int j = 0; j < MAX_SLOTS; j++) {
            if (doctors[i].slots[j] == 0) {
                printf("%d ", j + 1);
                found = 1;
            }
        }

        if (!found)
            printf("None");

        printf("\n");
    }
}

/* ---------- PATIENT FUNCTIONS ---------- */

void registerPatient(void) {
    if (patientCount == patientCapacity) {
        int newCapacity = patientCapacity == 0
                              ? 5 : patientCapacity * 2;

        Patient *temp = realloc(
            patients, newCapacity * sizeof(Patient));

        if (temp == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }

        patients = temp;
        patientCapacity = newCapacity;
    }

    Patient p;
    p.id = 1001 + patientCount;

    readString("Enter patient name: ",
               p.name, sizeof(p.name));

    readString("Enter symptoms: ",
               p.symptoms, sizeof(p.symptoms));

    patients[patientCount] = p;
    patientCount++;

    printf("\nPatient registered successfully!\n");
    printf("Patient ID: %d\n", p.id);
}

void displayPatients(void) {
    printf("\n========== PATIENT RECORDS ==========\n");

    if (patientCount == 0) {
        printf("No patients registered.\n");
        return;
    }

    for (int i = 0; i < patientCount; i++) {
        printf("\nID: %d\n", patients[i].id);
        printf("Name: %s\n", patients[i].name);
        printf("Symptoms: %s\n", patients[i].symptoms);
    }
}

/* ---------- APPOINTMENT FUNCTIONS ---------- */

void bookAppointment(void) {
    if (appointmentCount >= MAX_APPOINTMENTS) {
        printf("Appointment storage is full.\n");
        return;
    }

    int pid = readInt("Enter patient ID: ");
    int pi = findPatient(pid);

    if (pi == -1) {
        printf("Patient not found. Register first.\n");
        return;
    }

    displayDoctors();

    int did = readInt("Enter doctor ID: ");
    int di = findDoctor(did);

    if (di == -1) {
        printf("Invalid doctor ID.\n");
        return;
    }

    int slot = readInt("Choose slot (1-5): ");

    if (slot < 1 || slot > MAX_SLOTS) {
        printf("Invalid slot.\n");
        return;
    }

    if (doctors[di].slots[slot - 1] == 1) {
        printf("This slot is already booked.\n");
        return;
    }

    Appointment a;
    a.id = nextAppointmentId++;
    a.patientId = pid;
    a.doctorId = did;
    a.slot = slot;
    a.status = BOOKED;

    appointments[appointmentCount++] = a;
    doctors[di].slots[slot - 1] = 1;

    printf("\nAppointment booked successfully!\n");
    printf("Appointment ID: %d\n", a.id);
    printf("Patient: %s\n", patients[pi].name);
    printf("Doctor: %s\n", doctors[di].name);
    printf("Slot: %d\n", slot);
}

void cancelAppointment(void) {
    int id = readInt("Enter appointment ID to cancel: ");
    int ai = findAppointment(id);

    if (ai == -1 ||
        appointments[ai].status != BOOKED) {
        printf("Active appointment not found.\n");
        return;
    }

    int di = findDoctor(appointments[ai].doctorId);

    if (di != -1)
        doctors[di].slots[appointments[ai].slot - 1] = 0;

    appointments[ai].status = CANCELLED;
    printf("Appointment cancelled successfully.\n");
}

void displayAppointments(void) {
    printf("\n========== APPOINTMENTS ==========\n");

    if (appointmentCount == 0) {
        printf("No appointments found.\n");
        return;
    }

    for (int i = 0; i < appointmentCount; i++) {
        Appointment a = appointments[i];

        printf("\nAppointment ID: %d\n", a.id);
        printf("Patient ID: %d\n", a.patientId);
        printf("Doctor ID: %d\n", a.doctorId);
        printf("Slot: %d\n", a.slot);

        if (a.status == BOOKED)
            printf("Status: Booked\n");
        else if (a.status == CANCELLED)
            printf("Status: Cancelled\n");
        else if (a.status == COMPLETED)
            printf("Status: Completed\n");
        else
            printf("Status: Missed\n");
    }
}

void markMissed(void) {
    int id = readInt("Enter appointment ID marked as missed: ");
    int ai = findAppointment(id);

    if (ai == -1 || appointments[ai].status != BOOKED) {
        printf("Active appointment not found.\n");
        return;
    }

    int di = findDoctor(appointments[ai].doctorId);

    if (di != -1)
        doctors[di].slots[appointments[ai].slot - 1] = 0;

    appointments[ai].status = MISSED;
    printf("Appointment marked as missed.\n");
}

/* ---------- QUEUE FUNCTIONS ---------- */

int countAhead(int doctorId, int emergency) {
    int count = 0;
    QueueNode *current = front;

    while (current != NULL) {
        if (current->doctorId == doctorId) {
            if (!emergency || current->emergency)
                count++;
        }
        current = current->next;
    }

    return count;
}

void addWalkIn(void) {
    int pid = readInt("Enter patient ID: ");

    if (findPatient(pid) == -1) {
        printf("Patient not found. Register first.\n");
        return;
    }

    displayDoctors();

    int did = readInt("Enter doctor ID: ");

    if (findDoctor(did) == -1) {
        printf("Invalid doctor ID.\n");
        return;
    }

    int emergency = readInt(
        "Emergency? Enter 1 for yes, 0 for no: ");

    if (emergency != 0 && emergency != 1) {
        printf("Invalid emergency flag.\n");
        return;
    }

    QueueNode *node = malloc(sizeof(QueueNode));

    if (node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    node->patientId = pid;
    node->doctorId = did;
    node->emergency = emergency;
    node->estimatedWait =
        countAhead(did, emergency) *
        doctors[findDoctor(did)].avgConsultTime;
    node->next = NULL;

    /* Emergency patients go before ordinary patients.
       Patients with equal priority retain arrival order. */

    if (front == NULL) {
        front = rear = node;
    } else if (emergency == 1) {
        if (front->emergency == 0) {
            node->next = front;
            front = node;
        } else {
            QueueNode *current = front;

            while (current->next != NULL &&
                   current->next->emergency == 1) {
                current = current->next;
            }

            node->next = current->next;
            current->next = node;

            if (node->next == NULL)
                rear = node;
        }
    } else {
        rear->next = node;
        rear = node;
    }

    printf("Patient added to queue successfully.\n");
    printf("Estimated waiting time: %d minutes\n",
           node->estimatedWait);

    if (emergency)
        printf("Priority: Emergency\n");
    else
        printf("Priority: Normal\n");
}

void serveNextPatient(void) {
    if (front == NULL) {
        printf("The queue is empty.\n");
        return;
    }

    QueueNode *temp = front;
    int pi = findPatient(temp->patientId);
    int di = findDoctor(temp->doctorId);

    printf("\n========== SERVING PATIENT ==========\n");

    if (pi != -1)
        printf("Patient: %s\n", patients[pi].name);

    printf("Patient ID: %d\n", temp->patientId);

    if (di != -1)
        printf("Doctor: %s\n", doctors[di].name);

    printf("Priority: %s\n",
           temp->emergency ? "Emergency" : "Normal");

    totalWaitTime += temp->estimatedWait;
    patientsServed++;

    front = front->next;

    if (front == NULL)
        rear = NULL;

    free(temp);

    printf("Patient served and removed from queue.\n");
}

void displayQueue(void) {
    QueueNode *current = front;

    printf("\n========== WAITING QUEUE ==========\n");

    if (current == NULL) {
        printf("Queue is empty.\n");
        return;
    }

    while (current != NULL) {
        int pi = findPatient(current->patientId);

        printf("\nPatient ID: %d\n", current->patientId);

        if (pi != -1)
            printf("Name: %s\n", patients[pi].name);

        printf("Doctor ID: %d\n", current->doctorId);
        printf("Priority: %s\n",
               current->emergency ? "Emergency" : "Normal");
        printf("Estimated wait at entry: %d minutes\n",
               current->estimatedWait);

        current = current->next;
    }
}

void estimateWaitingTime(void) {
    int did = readInt("Enter doctor ID: ");
    int di = findDoctor(did);

    if (di == -1) {
        printf("Invalid doctor ID.\n");
        return;
    }

    int count = 0;
    QueueNode *current = front;

    while (current != NULL) {
        if (current->doctorId == did)
            count++;

        current = current->next;
    }

    printf("Patients waiting for %s: %d\n",
           doctors[di].name, count);

    printf("Estimated waiting time: %d minutes\n",
           count * doctors[di].avgConsultTime);
}

/* ---------- REPORTS ---------- */

void generateReports(void) {
    int missed = 0;

    printf("\n========== CLINIC REPORTS ==========\n");

    for (int d = 0; d < MAX_DOCTORS; d++) {
        int count = 0;

        for (int i = 0; i < appointmentCount; i++) {
            if (appointments[i].doctorId == doctors[d].id &&
                appointments[i].status != CANCELLED) {
                count++;
            }
        }

        printf("\nDoctor: %s\n", doctors[d].name);
        printf("Appointments (excluding cancelled): %d\n",
               count);
    }

    for (int i = 0; i < appointmentCount; i++) {
        if (appointments[i].status == MISSED)
            missed++;
    }

    printf("\nTotal registered patients: %d\n", patientCount);
    printf("Total appointments recorded: %d\n", appointmentCount);
    printf("Missed appointments: %d\n", missed);
    printf("Patients served from walk-in queue: %d\n",
           patientsServed);

    if (patientsServed > 0) {
        printf("Average waiting time of served queue patients: "
               "%.2f minutes\n",
               (double)totalWaitTime / patientsServed);
    } else {
        printf("Average waiting time: No patients served yet.\n");
    }
}

/* ---------- MAIN MENU ---------- */

int main(void) {
    initializeDoctors();

    int choice;

    do {
        printf("\n\n====================================\n");
        printf(" CLINIC APPOINTMENT & QUEUE SYSTEM\n");
        printf("====================================\n");
        printf("1. Register Patient\n");
        printf("2. Display Patients\n");
        printf("3. Display Doctors\n");
        printf("4. Book Appointment\n");
        printf("5. Cancel Appointment\n");
        printf("6. Add Walk-in Patient\n");
        printf("7. Serve Next Patient\n");
        printf("8. Estimate Waiting Time\n");
        printf("9. Generate Reports\n");
        printf("10. Mark Appointment as Missed\n");
        printf("11. Display Appointments\n");
        printf("12. Display Waiting Queue\n");
        printf("0. Exit\n");

        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1:
                registerPatient();
                break;

            case 2:
                displayPatients();
                break;

            case 3:
                displayDoctors();
                break;

            case 4:
                bookAppointment();
                break;

            case 5:
                cancelAppointment();
                break;

            case 6:
                addWalkIn();
                break;

            case 7:
                serveNextPatient();
                break;

            case 8:
                estimateWaitingTime();
                break;

            case 9:
                generateReports();
                break;

            case 10:
                markMissed();
                break;

            case 11:
                displayAppointments();
                break;

            case 12:
                displayQueue();
                break;

            case 0:
                printf("Closing clinic system...\n");
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }

    } while (choice != 0);

    /* Release dynamically allocated memory */

    while (front != NULL) {
        QueueNode *temp = front;
        front = front->next;
        free(temp);
    }

    free(patients);

    printf("Memory released. Goodbye!\n");

    return 0;
}
