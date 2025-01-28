#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <string>
#include <cmath>
#include <sstream>

using namespace std;

class Calculator {
public:
    Calculator();
    void run();

private:
    Fl_Window *window;
    Fl_Output *display;
    string expression;

    void appendToExpression(const string& value);
    void calculateResult();
    void clearExpression();
    void createButton(const char* label, int x, int y, void (*callback)(Fl_Widget*, void*));

    static void buttonCallback(Fl_Widget* widget, void* data);
    double evaluateExpression(const string& expr);
};

// Constructor: Setup GUI
Calculator::Calculator() {
    window = new Fl_Window(300, 400, "Calculator");
    display = new Fl_Output(10, 10, 280, 50);
    display->textsize(20);
    display->value("");

    // Corrected button labels
    const char* buttons[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"C", "0", "=", "+"}
    };

    int x_offset = 10, y_offset = 70, button_size = 60;

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            createButton(buttons[row][col], x_offset + col * button_size, y_offset + row * button_size, buttonCallback);
        }
    }

    window->end();
}

// Append input to expression
void Calculator::appendToExpression(const string& value) {
    expression += value;
    display->value(expression.c_str());
}

// Perform calculation
void Calculator::calculateResult() {
    try {
        double result = evaluateExpression(expression);
        expression = to_string(result);
        display->value(expression.c_str());
    } catch (...) {
        display->value("Error");
        expression.clear();
    }
}

// Simple expression evaluator
double Calculator::evaluateExpression(const string& expr) {
    stringstream ss(expr);
    double num1, num2;
    char op;
    ss >> num1;

    while (ss >> op) {
        ss >> num2;
        switch (op) {
            case '+': num1 += num2; break;
            case '-': num1 -= num2; break;
            case '*': num1 *= num2; break;
            case '/': 
                if (num2 != 0) num1 /= num2;
                else throw runtime_error("Division by zero");
                break;
            default: throw runtime_error("Invalid operator");
        }
    }

    return num1;
}

// Clear expression
void Calculator::clearExpression() {
    expression.clear();
    display->value("");
}

// Create a button with proper labeling
void Calculator::createButton(const char* label, int x, int y, void (*callback)(Fl_Widget*, void*)) {
    Fl_Button* button = new Fl_Button(x, y, 60, 60, label);
    button->labelsize(20);
    button->callback(callback, this);
}

// Button callback function
void Calculator::buttonCallback(Fl_Widget* widget, void* data) {
    Calculator* calc = static_cast<Calculator*>(data);
    string label = widget->label();

    if (label == "=") {
        calc->calculateResult();
    } else if (label == "C") {
        calc->clearExpression();
    } else {
        calc->appendToExpression(label);
    }
}

// Run the application
void Calculator::run() {
    window->show();
    Fl::run();
}

// Main function
int main() {
    Calculator calc;
    calc.run();
    return 0;
}
