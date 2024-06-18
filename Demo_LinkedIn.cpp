#include <iostream>
#include <string>
using namespace std;

class Node;
Node* front = nullptr;
Node* rear = nullptr;

class Node {
public:
    string sender;
    string receiver;
    string content;
    Node* next;

    Node(const string& sender, const string& receiver, const string& content) : sender(sender), receiver(receiver), content(content), next(nullptr) {}
};

class Queue {
public:
    void enqueue(const string& sender, const string& receiver, const string& content) {
        Node* newNode = new Node(sender, receiver, content);

        if (rear == nullptr) {
            front = newNode;
            rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
    }

    void dequeue() {
        if (front == nullptr) {
            cout << "QUEUE UNDERFLOW WARNING" << endl;
        } else {
            Node* tmp = front;
            front = front->next;
            delete tmp;
        }
    }

    void display() {
        Node* tmp2 = front;
        while (tmp2 != nullptr) {
            cout << "From: " << tmp2->sender << "\n";
            cout << "To: " << tmp2->receiver << "\n";
            cout << "Message: " << tmp2->content << "\n";
            cout << "------\n";
            tmp2 = tmp2->next;
        }
    }
};

// Constants
const int maxProfiles = 100;

// Structure to represent a user profile
struct UserProfile {
    string username;
    string password;
    string position;
    string role;
    string education;
    string skill;
    string about;
    string pendingRequests[maxProfiles]; // An array to store pending connection requests
};

// Structure to represent a message
struct Message {
    string sender;
    string receiver;
    string content;
};

// Global variables
UserProfile profiles[maxProfiles];
bool connections[maxProfiles][maxProfiles] = {false};
string loggedInUsername = "";

int findProfileIndex(const string& username, int profileCount) {
    for (int i = 0; i < profileCount; i++) {
        if (profiles[i].username == username) {
            return i;
        }
    }
    return -1; // Profile not found
}

void deleteProfile(const string& username, int& profileCount) {
    int profileIndex = -1;

    for (int i = 0; i < profileCount; i++) {
        if (profiles[i].username == username) {
            profileIndex = i;
            break;
        }
    }

    if (profileIndex != -1) {
        for (int i = profileIndex; i < profileCount - 1; i++) {
            profiles[i] = profiles[i + 1];
        }

        profileCount--;
        cout << "Profile '" << username << "' has been deleted.\n";
    } else {
        cout << "Profile with username '" << username << "' not found.\n";
    }
}

void connectWithUser(const string& username, const string& loggedInUsername, int& profileCount) {
    int senderIndex = findProfileIndex(loggedInUsername, profileCount);
    int receiverIndex = findProfileIndex(username, profileCount);

    if (senderIndex != -1 && receiverIndex != -1) {
        UserProfile& receiverProfile = profiles[receiverIndex];
        for (int i = 0; i < maxProfiles; i++) {
            if (receiverProfile.pendingRequests[i].empty()) {
                receiverProfile.pendingRequests[i] = loggedInUsername;
                cout << "Connection Request Sent. \n";
                break;
            }
        }
    } else {
        cout << "Invalid usernames. Connection request failed.\n";
    }
}

void approveConnectionRequest(const string& username, const string& loggedInUsername, int& profileCount) {
    int userIndex = findProfileIndex(loggedInUsername, profileCount);
    int senderIndex = findProfileIndex(username, profileCount);

    if (userIndex != -1 && senderIndex != -1) {
        UserProfile& userProfile = profiles[userIndex];
        for (int i = 0; i < maxProfiles; i++) {
            if (userProfile.pendingRequests[i] == username) {
                connections[userIndex][senderIndex] = true;
                connections[senderIndex][userIndex] = true;
                cout << "Connection with " << username << " approved.\n";
                userProfile.pendingRequests[i] = "";
                break;
            }
        }
    } else {
        cout << "Invalid usernames. Connection approval failed.\n";
    }
}

void disconnectFromUser(const string& username, const string& loggedInUsername, int& profileCount) {
    int senderIndex = findProfileIndex(loggedInUsername, profileCount);
    int receiverIndex = findProfileIndex(username, profileCount);

    if (senderIndex != -1 && receiverIndex != -1) {
        connections[senderIndex][receiverIndex] = false;
        connections[receiverIndex][senderIndex] = false;
        cout << "Disconnected from " << username << ".\n";
    } else {
        cout << "Invalid usernames. Disconnection failed.\n";
    }
}

void sendMessage(const string& sender, const string& receiver, Queue& messageQueue) {
    string content;
    cout << "Enter the message content: ";
    cin.ignore(); // Clear the newline character
    getline(cin, content);

    messageQueue.enqueue(sender, receiver, content);
    cout << "Message sent to " << receiver << ".\n";
}

void checkPendingRequests(const string& loggedInUsername, int& profileCount) {
    int userIndex = findProfileIndex(loggedInUsername, profileCount);

    cout << "Pending connection requests for " << loggedInUsername << ":\n";

    UserProfile& userProfile = profiles[userIndex];
    for (int i = 0; i < maxProfiles; i++) {
        if (!userProfile.pendingRequests[i].empty()) {
            cout << "Request from: " << userProfile.pendingRequests[i] << "\n";
            cout << "1. Accept\n";
            cout << "2. Reject\n";
            int choice;
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 1) {
                approveConnectionRequest(userProfile.pendingRequests[i], loggedInUsername, profileCount);
            } else {
                cout << "Connection request from " << userProfile.pendingRequests[i] << " rejected.\n";
                userProfile.pendingRequests[i] = "";
            }
        }
    }
}

void viewAndReplyMessages(const string& loggedInUsername, int& profileCount, Queue& messageQueue) {
    int userIndex = findProfileIndex(loggedInUsername, profileCount);
    cout << "Messages for " << loggedInUsername << ":\n";
    messageQueue.display();

    cout << "Enter username of the sender to reply to (or enter 0 to return): ";
    string senderToReply;
    cin >> senderToReply;

    if (senderToReply == "0") {
        return;
    }

    int senderIndex = findProfileIndex(senderToReply, profileCount);
    if (senderIndex == -1) {
        cout << "User not found.\n";
        return;
    }

    sendMessage(loggedInUsername, senderToReply, messageQueue);
}

int main() {
    cout << "User Profile Creation and Management System\n";
    char choice;
    bool isLoggedIn = false;
    int profileCount = 0; // Local variable for profile count

    Queue messageQueue; // Initialize the message queue

    do {
        if (isLoggedIn) {
            cout << "Logged in as " << loggedInUsername << ".\n";
            cout << "1. Connect with another user\n";
            cout << "2. Disconnect from another user\n";
            cout << "3. Send a message\n";
            cout << "4. Check pending connection requests\n";
            cout << "7. View and reply to messages\n";
            cout << "8. Logout\n";
            cout << "Enter your choice: ";
            cin >> choice;
            string usernameToConnect;
            string usernameToDisconnect;
            switch (choice) {
                case '1': // Connect with another user
                    cout << "Enter the username of the user you want to connect with: ";
                    cin >> usernameToConnect;
                    connectWithUser(usernameToConnect, loggedInUsername, profileCount);
                    break;

                case '2': // Disconnect from another user
                    cout << "Enter the username of the user you want to disconnect from: ";
                    cin >> usernameToDisconnect;
                    disconnectFromUser(usernameToDisconnect, loggedInUsername, profileCount);
                    break;

                case '3': // Send a message
                    cout << "Enter the username of the user you want to message: ";
                    cin >> usernameToConnect;
                    sendMessage(loggedInUsername, usernameToConnect, messageQueue);
                    break;

                case '4': // Check pending connection requests
                    checkPendingRequests(loggedInUsername, profileCount);
                    break;

                case '7': // View and reply to messages
                    viewAndReplyMessages(loggedInUsername, profileCount, messageQueue);
                    break;

                case '8': // Logout
                    isLoggedIn = false;
                    loggedInUsername = "";
                    cout << "Logged out successfully.\n";
                    break;

                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } else {
            cout << "1. Create a new profile\n";
            cout << "2. Delete a profile\n";
            cout << "3. Login\n";
            cout << "4. Display all profiles\n";
            cout << "6. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case '1': // Create a new profile
                    if (profileCount < maxProfiles) {
                        UserProfile newUserProfile;

                        cout << "Enter your username: ";
                        cin >> newUserProfile.username;
                        cout << "Enter your password: ";
                        cin >> newUserProfile.password;
                        cout << "Enter your position: ";
                        cin >> newUserProfile.position;
                        cout << "Enter your role: ";
                        cin >> newUserProfile.role;
                        cout << "Enter your education: ";
                        cin >> newUserProfile.education;
                        cout << "Enter your skills: ";
                        cin >> newUserProfile.skill;
                        cin.ignore(); // Consume the newline character
                        cout << "Tell us about yourself: ";
                        getline(cin, newUserProfile.about);

                        profiles[profileCount] = newUserProfile;
                        profileCount++;

                        cout << "Profile created successfully!\n";
                    } else {
                        cout << "Maximum number of profiles reached. Cannot create more.\n";
                    }
                    break;

                case '2': // Delete a profile
                    if (profileCount > 0) {
                        cout << "Enter the username of the profile you want to delete: ";
                        string usernameToDelete;
                        cin >> usernameToDelete;
                        deleteProfile(usernameToDelete, profileCount);
                    } else {
                        cout << "No profiles to delete.\n";
                    }
                    break;

                case '3': // Login
                    if (!isLoggedIn) {
                        string username, password;
                        cout << "Enter your username: ";
                        cin >> username;
                        cout << "Enter your password: ";
                        cin >> password;
                        int index = findProfileIndex(username, profileCount);
                        if (index != -1 && profiles[index].password == password) {
                            isLoggedIn = true;
                            loggedInUsername = username;
                            cout << "Logged in as " << username << ".\n";
                        } else {
                            cout << "Invalid username or password.\n";
                        }
                    } else {
                        cout << "You are already logged in as " << loggedInUsername << ". Please logout first.\n";
                    }
                    break;

                case '4': // Display all profiles
                    cout << "All created profiles:\n";
                    if (profileCount == 0) {
                        cout << "No profiles to display.\n";
                    } else {
                        for (int i = 0; i < profileCount; i++) {
                            cout << "Profile " << (i + 1) << ":\n";
                            cout << "Username: " << profiles[i].username << "\n";
                            cout << "Position: " << profiles[i].position << "\n";
                            cout << "Role: " << profiles[i].role << "\n";
                            cout << "Education: " << profiles[i].education << "\n";
                            cout << "Skills: " << profiles[i].skill << "\n";
                            cout << "About: " << profiles[i].about << "\n\n";
                        }
                    }
                    break;

                case '6': // Exit
                    cout << "Exiting the program.\n";
                    break;

                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        }
    } while (choice != '6');

    return 0;
}
