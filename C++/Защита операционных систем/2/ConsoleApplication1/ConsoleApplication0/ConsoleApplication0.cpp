#include <windows.h>
#include <iostream>
#include <time.h>
#include <tchar.h>

using namespace std;

CRITICAL_SECTION cs;

int sum = 0;

// функция потока
DWORD WINAPI func(LPVOID param)
{
	// количесвто чисел, генерируемых потоком
	const int m = 10;
	int a;
	for (int i = 0; i<m; i++)
	{
		a = rand(); // генерация числа
		// Вход в критическую секцию
		EnterCriticalSection(&cs);
		sum += a;
		// Выход из критической секции
		LeaveCriticalSection(&cs);
	}
	return 0;
}


int main(int argc, TCHAR* argv[])
{
	const int n = 10;
	HANDLE hThread[n];	 //массив потоков
	DWORD dwThreadID[n]; //массив идентификаторов

	// Инициализация критической секции
	InitializeCriticalSection(&cs);
	sum = 0;
	srand((unsigned int) time(NULL));

	// Запуск потоков
	for (int i = 0; i<n; i++)
	{
		hThread[i] = CreateThread(NULL,0,
func,NULL,0,&(dwThreadID[i]));
		
		if (hThread[i] == NULL)
		{
		 cout << "Ошибка создания потока "
 << GetLastError() << '\n';
		}
	}

	// Ожидание завершения потоков
	WaitForMultipleObjects(n,hThread,
true,INFINITE);

	//Вывод результатов
	cout << "Сумма = " << sum << '\n';

	// Закрытие потоков
	for (int i = 0; i<n; i++)
		CloseHandle(hThread[i]);
	
	return 0;
}
