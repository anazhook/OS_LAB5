#include <windows.h>
#include <iostream>
#include <string>

struct employee {
	int id;
	char name[10];
	double hours;
};

int main(int argc, char* argv[]) {
	employee emp;
	DWORD dwBytesWritten, dwBytesRead;
	char out_msg[80], in_msg[80];
	std::string request, smth;
	HANDLE hNamedPipe = CreateFile(L"\\\\.\\pipe\\demo_pipe",
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		(LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, (HANDLE)NULL);
	while (true) {
		std::cout << "write m to modify, r to read or q to quit: ";
		std::cin >> request;
		if (request == "q") {
			strcpy_s(out_msg, request.c_str());
			WriteFile(hNamedPipe, out_msg, sizeof(out_msg), &dwBytesWritten, (LPOVERLAPPED)NULL);
			CloseHandle(hNamedPipe);
			return 0;
		}
		if (request == "m") {
			std::cout << "employee's id: "; std::cin >> emp.id;
			strcpy_s(out_msg, (request + " " + std::to_string(emp.id)).c_str());
			WriteFile(hNamedPipe, out_msg, sizeof(out_msg), &dwBytesWritten, (LPOVERLAPPED)NULL);
			ReadFile(hNamedPipe, in_msg, sizeof(in_msg), &dwBytesRead, (LPOVERLAPPED)NULL);
			std::cout << "\n employee info: " << in_msg << "\n";
			std::cout << "\n\n new name: ";
			std::cin >> emp.name;
			std::cout << "new hours: ";
			std::cin >> emp.hours;
			strcpy_s(out_msg, (std::to_string(emp.id) + " " + emp.name + " " + std::to_string(emp.hours)).c_str());
			WriteFile(hNamedPipe, out_msg, sizeof(out_msg), &dwBytesWritten, (LPOVERLAPPED)NULL);
		}
		else if (request == "r") {
			std::cout << "employee's id: ";
			std::cin >> emp.id;
			strcpy_s(out_msg, (request + " " + std::to_string(emp.id)).c_str());
			WriteFile(hNamedPipe, out_msg, sizeof(out_msg), &dwBytesWritten, (LPOVERLAPPED)NULL);
			ReadFile(hNamedPipe, in_msg, sizeof(in_msg), &dwBytesRead, (LPOVERLAPPED)NULL);
			std::cout << "\n employee info: " << in_msg << "\n";
			WriteFile(hNamedPipe, out_msg, sizeof(out_msg), &dwBytesWritten, (LPOVERLAPPED)NULL);
		}
	}
}