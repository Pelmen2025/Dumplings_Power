#include <windows.h>
#include <iostream>
#include <locale.h>
using namespace std;

// Структура узла бинарного дерева
struct Node {
    int data;           // данные узла
    Node* left;         // указатель на левого потомка
    Node* right;        // указатель на правого потомка
    int subtree_sum;    // сумма поддерева (результат работы потока)
};

// Структура для передачи параметров в поток
struct ThreadParam {
    Node* node;
};

// Функция потока для вычисления суммы поддерева
DWORD WINAPI calculate_sum(LPVOID param)
{
    ThreadParam* tp = (ThreadParam*)param;
    Node* node = tp->node;

    if (node == NULL)
    {
        return 0;
    }

    // Инициализируем сумму значением текущего узла
    node->subtree_sum = node->data;

    // Дескрипторы для дочерних потоков
    HANDLE hLeftThread = NULL;
    HANDLE hRightThread = NULL;
    DWORD dwLeftThreadID, dwRightThreadID;
    ThreadParam leftParam, rightParam;

    // Если есть левый потомок, создаём поток для него
    if (node->left != NULL)
    {
        leftParam.node = node->left;
        hLeftThread = CreateThread(
            NULL,
            0,
            calculate_sum,
            &leftParam,
            0,
            &dwLeftThreadID
        );
    }

    // Если есть правый потомок, создаём поток для него
    if (node->right != NULL)
    {
        rightParam.node = node->right;
        hRightThread = CreateThread(
            NULL,
            0,
            calculate_sum,
            &rightParam,
            0,
            &dwRightThreadID
        );
    }

    // Ожидаем завершения левого потока
    if (hLeftThread != NULL)
    {
        WaitForSingleObject(hLeftThread, INFINITE);
        node->subtree_sum += node->left->subtree_sum;
        CloseHandle(hLeftThread);
    }

    // Ожидаем завершения правого потока
    if (hRightThread != NULL)
    {
        WaitForSingleObject(hRightThread, INFINITE);
        node->subtree_sum += node->right->subtree_sum;
        CloseHandle(hRightThread);
    }

    return 0;
}

// Функция для создания нового узла
Node* createNode(int data)
{
    Node* newNode = new Node;
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->subtree_sum = 0;
    return newNode;
}

// Функция для вывода дерева (обход в ширину для наглядности)
void printTree(Node* root, int level = 0)
{
    if (root == NULL)
        return;

    printTree(root->right, level + 1);

    for (int i = 0; i < level; i++)
        cout << "    ";
    cout << root->data << endl;

    printTree(root->left, level + 1);
}

// Функция для освобождения памяти дерева
void deleteTree(Node* root)
{
    if (root == NULL)
        return;

    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

int main()
{
    setlocale(LC_ALL, "Russian");

    Node* root = createNode(10);
    root->left = createNode(5);
    root->right = createNode(15);
    root->left->left = createNode(3);
    root->left->right = createNode(7);
    root->right->left = createNode(12);
    root->right->right = createNode(20);
    root->left->left->left = createNode(1);
    root->right->right->right = createNode(25);

    cout << "=== Бинарное дерево ===" << endl;
    cout << "(повернуто на 90 градусов против часовой стрелки)" << endl << endl;
    printTree(root);

    cout << "\n=== Вычисление суммы элементов ===" << endl;

    // Создание параметра для главного потока
    ThreadParam mainParam;
    mainParam.node = root;

    // Создание главного потока для вычисления суммы
    HANDLE hMainThread;
    DWORD dwMainThreadID;

    hMainThread = CreateThread(
        NULL,
        0,
        calculate_sum,
        &mainParam,
        0,
        &dwMainThreadID
    );

    if (hMainThread == NULL)
    {
        cout << "Ошибка создания потока: " << GetLastError() << endl;
        deleteTree(root);
        return 1;
    }

    // Ожидание завершения вычислений
    WaitForSingleObject(hMainThread, INFINITE);
    CloseHandle(hMainThread);

    // Вывод результата
    cout << "\nСумма всех элементов дерева: " << root->subtree_sum << endl;

    // Вывод промежуточных результатов (сумм поддеревьев)
    cout << "\n=== Суммы поддеревьев ===" << endl;
    cout << "Корень (10): " << root->subtree_sum << endl;
    cout << "Левое поддерево (5): " << root->left->subtree_sum << endl;
    cout << "Правое поддерево (15): " << root->right->subtree_sum << endl;
    cout << "Левое-левое (3): " << root->left->left->subtree_sum << endl;
    cout << "Левое-правое (7): " << root->left->right->subtree_sum << endl;
    cout << "Правое-левое (12): " << root->right->left->subtree_sum << endl;
    cout << "Правое-правое (20): " << root->right->right->subtree_sum << endl;

    // Освобождение памяти
    deleteTree(root);

    return 0;
}