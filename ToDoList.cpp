#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <ctime> // Added for handling date and time

using namespace std;

// Task class representing a to-do item
class Task {
public:
    Task(const string& description) : description(description), completed(false) {}

    void markCompleted() {
        completed = true;
        // Record the completion date as the current system date and time
        time_t currentTime = time(nullptr);
        completionDate = *localtime(&currentTime);
    }

    void markPending() {
        completed = false;
        // Clear the completion date when marking as pending
        completionDate = {};
    }

    bool isCompleted() const {
        return completed;
    }

    const string& getDescription() const {
        return description;
    }

    const string& getDueDate() const {
        return dueDate;
    }

    void setDueDate(const string& date) {
        dueDate = date;
    }

    const vector<string>& getTags() const {
        return tags;
    }

    void addTag(const string& tag) {
        tags.push_back(tag);
    }

    const tm& getCompletionDate() const {
        return completionDate;
    }

private:
    string description;
    bool completed;
    string dueDate;
    vector<string> tags;
    tm completionDate; // Date and time of completion
};

// TaskBuilder class for building tasks with optional attributes
class TaskBuilder {
public:
    TaskBuilder(const string& description) : task(new Task(description)) {}

    TaskBuilder& setDueDate(const string& dueDate) {
        task->setDueDate(dueDate);
        return *this;
    }

    TaskBuilder& addTag(const string& tag) {
        task->addTag(tag);
        return *this;
    }

    Task* build() {
        return task;
    }

private:
    Task* task;
};

// TaskManager class for managing tasks and supporting undo and redo
class TaskManager {
public:
    void addTask(Task* task) {
        tasks.push_back(task);
        undoStack.push(tasks);
        redoStack = stack<vector<Task*> >(); // Clear redo stack
    }

    void markCompleted() {
        if (tasks.empty()) {
            cout << "No tasks to mark as completed. Task list is empty." << endl;
            return;
        }

        cout << "Select a task to mark as completed:" << endl;
        displayNumberedTasks();

        int taskNumber;
        cout << "Enter the task number to mark as completed: ";
        cin >> taskNumber;

        if (taskNumber >= 1 && taskNumber <= tasks.size()) {
            string description = tasks[taskNumber - 1]->getDescription();
            tasks[taskNumber - 1]->markCompleted();
            undoStack.push(tasks);
            redoStack = stack<vector<Task*> >(); // Clear redo stack
            cout << "Task '" << description << "' marked as completed successfully!" << endl;
        } else {
            cout << "Invalid task number. No task marked as completed." << endl;
        }
    }

    void deleteTask() {
        if (tasks.empty()) {
            cout << "No tasks to delete. Task list is empty." << endl;
            return;
        }

        cout << "Select a task to delete:" << endl;
        displayNumberedTasks();
        
        int taskNumber;
        cout << "Enter the task number to delete: ";
        cin >> taskNumber;

        if (taskNumber >= 1 && taskNumber <= tasks.size()) {
            string description = tasks[taskNumber - 1]->getDescription();
            tasks.erase(remove_if(tasks.begin(), tasks.end(),
                [&description](Task* task) { return task->getDescription() == description; }),
                tasks.end());
            undoStack.push(tasks);
            redoStack = stack<vector<Task*> >(); // Clear redo stack
            cout << "Task '" << description << "' deleted successfully!" << endl;
        } else {
            cout << "Invalid task number. No task deleted." << endl;
        }
    }

    void undo() {
        if (undoStack.size() > 1) {
            redoStack.push(undoStack.top());
            undoStack.pop();
            tasks = undoStack.top();
        }
        cout << "Undo successful!" << endl;
    }

    void redo() {
        if (!redoStack.empty()) {
            undoStack.push(redoStack.top());
            redoStack.pop();
            tasks = undoStack.top();
        }
        cout << "Redo successful!" << endl;
    }

    void viewTasks(const string& filter = "Show all") const {
        cout << "Task List:" << endl;
        if (tasks.empty()) {
            cout << "EMPTY" << endl;
        } else {
            for (const auto& task : tasks) {
                if ((filter == "Show completed" && task->isCompleted()) ||
                    (filter == "Show pending" && !task->isCompleted()) ||
                    filter == "Show all") {
                    cout << task->getDescription() << " - " << (task->isCompleted() ? "Completed" : "Pending");
                    if (!task->getDueDate().empty()) {
                        cout << ", Due: " << task->getDueDate();
                    }
                    if (task->isCompleted()) {
                        cout << ", Completed On: " << asctime(&task->getCompletionDate());
                    }
                    const auto& tags = task->getTags();
                    if (!tags.empty()) {
                        cout << ", Tags: ";
                        for (const auto& tag : tags) {
                            cout << tag << " ";
                        }
                    }
                    cout << endl;
                }
            }
        }
        pauseExecution();
    }

private:
    vector<Task*> tasks;
    stack<vector<Task*> > undoStack;
    stack<vector<Task*> > redoStack;

    // Function to pause execution until the user presses Enter
    void pauseExecution() const {
        cout << "\nPress any key to continue...";
        cin.ignore();  // Clear newline character from the buffer
        cin.get();     // Wait for the user to press Enter
    }

    // Function to display a numbered list of tasks
    void displayNumberedTasks() const {
        for (size_t i = 0; i < tasks.size(); ++i) {
            cout << i + 1 << ". " << tasks[i]->getDescription() << endl;
        }
    }
};

// Function to display the menu
void displayMenu() {
    cout << "\n===== TO-DO LIST MANAGER =====" << endl;
    cout << "1. Add Task" << endl;
    cout << "2. Mark Task as Completed" << endl;
    cout << "3. Delete Task" << endl;
    cout << "4. Undo" << endl;
    cout << "5. Redo" << endl;
    cout << "6. View Tasks" << endl;
    cout << "0. Exit" << endl;
    cout << "==============================\n" << endl;
    cout << "Enter your choice: ";
}

int main() {
    TaskManager taskManager;

    int choice;

    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << "Enter task description: ";
                string description;
                cin.ignore();  // Clear newline character from the buffer
                getline(cin, description);

                cout << "Enter due date (or leave empty): ";
                string dueDate;
                getline(cin, dueDate);

                Task* newTask = TaskBuilder(description).setDueDate(dueDate).build();
                taskManager.addTask(newTask);
                cout << "Task added successfully!" << endl;
                break;
            }
            case 2:
                taskManager.markCompleted();
                break;
            case 3:
                taskManager.deleteTask();
                break;
            case 4:
                taskManager.undo();
                break;
            case 5:
                taskManager.redo();
                break;
            case 6: {
                cout << "Select filter option:" << endl;
                cout << "1. Show all" << endl;
                cout << "2. Show completed" << endl;
                cout << "3. Show pending" << endl;
                cout << "Enter your choice: ";
                int filterChoice;
                cin >> filterChoice;

                switch (filterChoice) {
                    case 1:
                        taskManager.viewTasks("Show all");
                        break;
                    case 2:
                        taskManager.viewTasks("Show completed");
                        break;
                    case 3:
                        taskManager.viewTasks("Show pending");
                        break;
                    default:
                        cout << "Invalid choice!" << endl;
                }
                break;
            }
            case 0:
                cout << "Exiting the program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice! Please enter a number between 0 and 6." << endl;
        }
    } while (choice != 0);

    return 0;
}
