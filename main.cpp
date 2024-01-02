#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>

struct employee {
		int id;
		char name[10];
		double hours;
	};

employee emp;
std::ofstream os;
std::ifstream is;
std::fstream ios;
char name_of_binary_file[10];
int quantity_of_employees;

DWORD WINAPI processingThread(HANDLE named_pipe) {
	DWORD dwBytesWrite, dwBytesRead;
	char in_msg[80], out_msg[80];
	std::string str;
	int i;
	ConnectNamedPipe(named_pipe, (LPOVERLAPPED)NULL);
	while (true) {
		ReadFile(named_pipe, in_msg, sizeof(in_msg), &dwBytesRead, (LPOVERLAPPED)NULL);
		if (in_msg[0] == 'q')
			return 0;
		str = in_msg;
		i = stoi(str.substr(str.find(' ') + 1));
		if (in_msg[0] == 'm') {
			is.open(name_of_binary_file, std::ios::binary | std::ios::in);
			is.seekg(i * sizeof(employee));
			is.read((char*)&emp, sizeof(employee));
			is.close();
			strcpy_s(out_msg, ("id: " + std::to_string(emp.id) + ", name: " + emp.name + ", hours: " + std::to_string(emp.hours)).c_str());
			WriteFile(named_pipe, out_msg, sizeof(out_msg), &dwBytesWrite, (LPOVERLAPPED)NULL);
			ReadFile(named_pipe, in_msg, sizeof(in_msg), &dwBytesRead, (LPOVERLAPPED)NULL);
			str = in_msg;
			str = str.substr(str.find(' ') + 1);
			strcpy_s(emp.name, (str.substr(0, str.find(' '))).c_str());
			emp.hours = stod(str.substr(str.find(' ') + 1));
			ios.open(name_of_binary_file, std::ios::binary | std::ios::out | std::ios::in);
			ios.seekp(emp.id * sizeof(employee));
			ios.write((char*)&emp, sizeof(employee));
			ios.close();
		}
		else {
			is.open(name_of_binary_file, std::ios::binary | std::ios::in);
			is.seekg(i * sizeof(employee));
			is.read((char*)&emp, sizeof(employee));
			is.close();
			strcpy_s(out_msg, ("id: " + std::to_string(emp.id) + ", name: " + emp.name + ", hours: " + std::to_string(emp.hours)).c_str());
			WriteFile(named_pipe, out_msg, sizeof(out_msg), &dwBytesWrite, (LPOVERLAPPED)NULL);
			ReadFile(named_pipe, in_msg, sizeof(in_msg), &dwBytesRead, (LPOVERLAPPED)NULL);
		}
	}
	return 0;
}

int main() {
	std::cout << "enter the name of binary file: ";
	std::cin >> name_of_binary_file;

	std::cout << "enter the quantity of employees: ";
	std::cin >> quantity_of_employees;

	os.open(name_of_binary_file, std::ios::binary | std::ios::out | std::ios::trunc);
	for (int i = 0; i < quantity_of_employees; i++) {
		std::cout << "\nemployee's id: " << i;
		emp.id = i;
		std::cout << "\nenter employee's name: ";
		std::cin >> emp.name;
		std::cout << "enter employee's hours: ";
		std::cin >> emp.hours;
		os.write((char*)&emp, sizeof(employee));
	}
	os.close();

	is.open(name_of_binary_file, std::ios::binary | std::ios::in);
	std::cout << "\nbinary file contents:\n\n";
	for (int i = 0; i < quantity_of_employees; i++) {
		is.read((char*)&emp, sizeof(employee));
		std::cout << emp.id << " " << emp.name << " " << emp.hours << "\n";
	}
	is.close();

	HANDLE hNamedPipe;
	hNamedPipe = CreateNamedPipe(L"\\\\.\\pipe\\demo_pipe",
		PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, 0);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char client_path[100];
	wsprintfA(client_path, "Clientpr.exe");
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	CreateProcessA(nullptr, client_path, NULL, NULL, 0,
		CREATE_NEW_CONSOLE, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);

	DWORD IDThread;
	HANDLE hThread;
	hThread = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)processingThread, (LPVOID)hNamedPipe, 0, &IDThread);

	WaitForSingleObject(hThread, INFINITE);

	is.open(name_of_binary_file, std::ios::binary | std::ios::in);
	std::cout << "\nresulted binary file contents: \n\n";
	for (int i = 0; i < quantity_of_employees; i++) {
		is.read((char*)&emp, sizeof(employee));
		std::cout << emp.id << " " << emp.name << " " << emp.hours << "\n";
	}
	std::cout << "\n";
	is.close();

	CloseHandle(hNamedPipe);
	CloseHandle(hThread);

	std::string smth;
	std::cout << "enter something to finish: ";
	std::cin >> smth;
	return 0;
}