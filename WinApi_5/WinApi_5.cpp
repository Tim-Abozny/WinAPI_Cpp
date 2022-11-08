/* TODO:
	1. enhance processing long input (like those buffer size constraints are not ok
*/

#include <Windows.h>
#include <tchar.h>  //for _tmain macro only
#include <cstdio>  
#include <cstring>  
#include <cstdint>


/*
	COMMAND object is created each time user inputs command.
	the command_info field is reserved; see comments below
*/
typedef struct __Command {
	int32_t command;
	void* command_info;
} COMMAND, * PCOMMAND, * const PCCOMMAND;


//command codes are valid values for COMMAND.command field
	//i don't like this INVALID. 
#define CMD_INVALID 1000
#define CMD_SENDMESSAGE 1007
#define CMD_EXIT 1009

//a row in a (string-command) association table
struct __AssociationTableRow {
	const char* str;
	int32_t command;
};

/*
	association table is used to determine appropriate
	command identifier using the users INPUT
*/
const __AssociationTableRow _association_table[] = {
	{"sendmessage", CMD_SENDMESSAGE},
	{"exit",		CMD_EXIT},
};

//size of association table
const int _association_table_size = sizeof(_association_table)
/ sizeof(__AssociationTableRow);


//one byte is taken by \0
#define LENGTH_COMMAND 64
#define LENGTH_MESSAGE 1024


//those names chosen in a random manner to make sure they identify 
// objects unambiguously
LPCTSTR const lpEvntMessageName = TEXT("EvntEventName199g45th");
LPCTSTR const lpFileMappingName = TEXT("FileMappingName8fug8210c");

//handles to objects for those processes to share data
HANDLE g_hEvntMessage;
char* g_lpSharedMem;  //contents of the shared memory are considered being chars


//model functions
DWORD WINAPI messageRecieverThread(LPVOID lpParam);
int getString(char* buffer);
int decodeCommandString(const char* cmdstring, PCCOMMAND cmd);
int processCommand(PCCOMMAND cmd);
int sendMessage();


int _tmain() {
	//creating or opening processes' common file mapping
	//whether it's opened or created depends on whether this process
	//is first or not
	HANDLE hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,  //use page file as backing file
		NULL,  //default security attributes
		PAGE_READWRITE,   //it's view are intended to be read and written to
		0,		//hiword of size of mapping. obviously zero
		LENGTH_MESSAGE,    //loword of size of mapping. this is desired size
		lpFileMappingName   //name of the mapping
	);
	if (hFileMapping == NULL) {

		printf("can't create/open file mapping %d\n", GetLastError());
		ExitProcess(GetLastError());
	}

	//mapping a file map object into virtual address space of this process
	g_lpSharedMem = (char*)MapViewOfFile(
		hFileMapping,  //handle to a file mapping object to map
		FILE_MAP_ALL_ACCESS, //desired access to the page
		0,  //hiword of offset in the file
		0,  //loword of offset in the file. those zeros mean that the whole file is mapped
		0  //number of bytes to map. this opt maps whole aval. in Mapping
	);
	if (g_lpSharedMem == NULL) {
		printf("can't create a file view\n%d\n", GetLastError());
		ExitProcess(GetLastError());
	}

	//creating a message-reciever thread
	HANDLE hIntThread = CreateThread(
		NULL,    //default security attributes
		0,		//default stack size
		messageRecieverThread,    //entry point for the thread
		NULL,		//no thread param is not needed
		0,			// no creatinon flags
		NULL     //no need to store thread id
	);
	if (hIntThread == NULL) {
		printf("can't create thread %d\n", GetLastError());
		ExitProcess(GetLastError());
	}

	//command loop of the application
	char cmd_str_buff[LENGTH_COMMAND];
	COMMAND cmd;
	while (true) {
		getString(cmd_str_buff);  //THIS SHOULD BE A MACRO
		decodeCommandString(cmd_str_buff, &cmd);
		processCommand(&cmd);
	}

	return 0;
}


/*
	now this thread also outputs messages which is not perfect
*/
DWORD WINAPI messageRecieverThread(LPVOID lpParam) {
	g_hEvntMessage = CreateEvent(
		NULL, //default security attributes
		TRUE, // this gon be a manual-reset event
		FALSE, //definitely not a TRUE, so the init state is non-signaled
		lpEvntMessageName
	);
	if (g_hEvntMessage == NULL) {
		//printf("can't create/open EvntEvent object\n%d\n", GetLastError());
		ExitProcess(GetLastError());
	}

	// message recieving loop.
	//WHEN THE EVENT IS SIGNALED THOSE threads ROTATE IN A LOOP
	while (true) {
		//wait for message to be posted
		WaitForSingleObject(g_hEvntMessage, INFINITE);
		printf("Got a message: ");
		puts((const char*)g_lpSharedMem);
		//unsignallig event
		ResetEvent(g_hEvntMessage);
	}

	return DWORD(0);
}


//current implementation of getCommandString
int getString(char* buffer) {
	gets_s(buffer, LENGTH_COMMAND);
	return 0;
}

int decodeCommandString(const char* cmdstring, PCCOMMAND cmd) {
	for (int i = 0; i < _association_table_size; i++) {
		if (!strcmp(_association_table[i].str, cmdstring)) {
			cmd->command = _association_table[i].command;
			break;
		}
		//otherwise the message was is not recognized
		cmd->command = CMD_INVALID;
	}
	// setting command_info to 0 (see COMMAND description)
	cmd->command_info = nullptr;

	return 0;
}

// command dispatcher. easily extendable
// to the case of using more fields in COMMAND
int processCommand(PCCOMMAND cmd) {
	switch (cmd->command) {
	case CMD_SENDMESSAGE:
		sendMessage();
		break;
	case CMD_EXIT:
		//consider using more complicated variant of leaving the process
		ExitProcess(0);
		break;
	case CMD_INVALID:
		printf("sorry, your message is not recognized(((\n");
		break;
	default:
		break;
		//nice fancy going away with saying things
	}
	return 0;
}

//blocking dialog function that performs sending (broadcasting message)
//should create a transaction (
int sendMessage() {
	printf("Enter the message: ");
	gets_s(g_lpSharedMem, LENGTH_MESSAGE);

	//signal event
	SetEvent(g_hEvntMessage);

	printf("Message sent!\n");

	return 0;
}