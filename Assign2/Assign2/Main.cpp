///////////////////// System Includes /////////////////////
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <Ws2tcpip.h>
#include <windows.h>
#include <string>
#include <tchar.h>
#include "resource1.h"


#pragma comment(lib, "comctl32.lib")

///////////////////// Function Prototypes /////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam);
BOOL DialogProcInst(HWND hDlg, UINT msg,
	WPARAM wParam, LPARAM lParam);
BOOL DialogProcForms(HWND hDlg, UINT msg,
	WPARAM wParam, LPARAM lParam);
void ShowLastErr(bool wsa);
int GetBitSize(char *word);

///////////////////// Macros //////////////////////////////
// Buffer size for reading instructions from text file
#define INSTBUFFERSIZE	512
// Kilobyte names for bit amounts
#define KBYTES1			1024
#define KBYTES4			4098
#define KBYTES20		163840
#define KBYTES60		491520
#define KBYTES180		1474560

#include "Globals.h"

///////////// Program-wide Global Variables ///////////////
char address[100];
char errmsg[ERRORSIZE];
bool clientrunning;
bool tcp;

///////////// File Global Variables ///////////////////////
char instbuff[INSTBUFFERSIZE] = { 0 };
char filenameclient[FILENAMESIZE];
bool sizepicked;
HWND hMainWin;
OPENFILENAME ofncl;
HANDLE clientthread, serverthread;
DWORD ThreadId;
struct ClientParams *clientparam, *serverparm;
// Main window name, title & handle
static TCHAR MainWinName[] = TEXT("client_server");
static TCHAR MainWinTitle[] = TEXT("Client/Server Transfer program");

#include "Client.h"
#include "Server.h"

/*---------------------------------------------------------------------------------------
--	FUNCTION:		int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
--						LPSTR lspszCmdParam, int nCmdShow)
--
--	DATE:			January 19, 2016
--
--	REVISIONS:		February 13, 2016
--
--	DESIGNERS:		Micah Willems
--
--	PROGRAMMER:		Micah Willems
--
--	NOTES:
--      This is the main function of the program, of typical win32 design. Two incidents
--		of note are 1: the createwindow call is used to add the text edit control to the
--		main window and 2: A file is read in and stored in a buffer for later use which
--		contains a large paragraph about how to use the program, for use in the help
--		menu.
---------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	WNDCLASSEX Wcl;
	HINSTANCE hinst;

	hinst = hInst;

	HANDLE hFile;
	DWORD  dwBytesRead = 0;

	clientparam = (struct ClientParams*)malloc(sizeof(struct ClientParams));
	serverparm = (struct ClientParams*)malloc(sizeof(struct ClientParams));

	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL; // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //white background
	Wcl.lpszClassName = MainWinName;

	Wcl.lpszMenuName = "IDR_MENU"; // The menu Class
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl)) {
		OutputDebugString("Failed to register window\n");
		return 1;
	}

	hMainWin = CreateWindow(MainWinName,
		MainWinTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		700, 400,
		NULL,
		NULL,
		hInst,
		NULL
		);

	ShowWindow(hMainWin, nCmdShow);
	UpdateWindow(hMainWin);

	hFile = CreateFile("instructions.txt", // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if (ReadFile(hFile, instbuff, INSTBUFFERSIZE - 1, &dwBytesRead, NULL) == FALSE)
	{
		OutputDebugString("Couldn't read instructions\n");
	}

	CloseHandle(hFile);

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

/*---------------------------------------------------------------------------------------
--	FUNCTION:		LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
--						WPARAM wParam, LPARAM lParam)

--
--	DATE:			January 19, 2016
--
--	REVISIONS:		February 13, 2016
--
--	DESIGNERS:		Micah Willems
--
--	PROGRAMMER:		Micah Willems
--
--	NOTES:
--      This is the main message handling loop. All 5 menu items, the 3 service calls
--		plus the 2 modes, are handled and pop open the appropriate dialog. When the
--		Dialog opens, using DialogBox (which utilizes the pre-existing resources-created
--		dialogs), they are assigned a blocking function to handle all interior events.
--		On closure, the send file option and connect option trigger the start of the
--		client and server threads, respectively.
---------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
	static HMENU hmenu;
	UINT menuFlags;
	int ret;
	switch (Message)
	{
	case WM_CREATE:
		hmenu = GetMenu(hwnd);
		ModifyMenu(hmenu, ID_FILE_CONNECT, MF_BYCOMMAND | MF_DISABLED, ID_FILE_CONNECT, "Connect");
		ModifyMenu(hmenu, ID_FILE_SENDFILE, MF_BYCOMMAND | MF_DISABLED, ID_FILE_SENDFILE, "Send File");
		ModifyMenu(hmenu, ID_FILE_DISCONNECT, MF_BYCOMMAND | MF_DISABLED, ID_FILE_DISCONNECT, "Disconnect");
	case WM_COMMAND:
		hmenu = GetMenu(hwnd);
		switch (LOWORD(wParam)) {
		case ID_HELP_INSTRUCTIONS:
			DialogBox(GetModuleHandle(NULL),
				MAKEINTRESOURCE(IDD_DIALOGINST),
				hwnd,
				(DLGPROC)DialogProcInst);
			break;
		case ID_OPERATINGMODE_CLIENT:
			ModifyMenu(hmenu, ID_FILE_CONNECT, MF_BYCOMMAND | MF_ENABLED, ID_FILE_CONNECT, "Connect");
			ModifyMenu(hmenu, ID_OPERATINGMODE_SERVER, MF_BYCOMMAND | MF_UNCHECKED, ID_OPERATINGMODE_SERVER, "Server");
			ModifyMenu(hmenu, ID_OPERATINGMODE_CLIENT, MF_BYCOMMAND | MF_CHECKED, ID_OPERATINGMODE_CLIENT, "Client");
			break;
		case ID_OPERATINGMODE_SERVER:
			ModifyMenu(hmenu, ID_FILE_CONNECT, MF_BYCOMMAND | MF_ENABLED, ID_FILE_CONNECT, "Connect");
			ModifyMenu(hmenu, ID_OPERATINGMODE_CLIENT, MF_BYCOMMAND | MF_UNCHECKED, ID_OPERATINGMODE_CLIENT, "Client");
			ModifyMenu(hmenu, ID_OPERATINGMODE_SERVER, MF_BYCOMMAND | MF_CHECKED, ID_OPERATINGMODE_SERVER, "Server");
			break;
		case ID_FILE_CONNECT:
			menuFlags = GetMenuState(hmenu, ID_OPERATINGMODE_CLIENT, MF_BYCOMMAND);
			if (menuFlags & MF_CHECKED) {
				ModifyMenu(hmenu, ID_FILE_SENDFILE, MF_BYCOMMAND | MF_ENABLED, ID_FILE_SENDFILE, "Send File");
			}
			ModifyMenu(hmenu, ID_FILE_CONNECT, MF_BYCOMMAND | MF_DISABLED, ID_FILE_CONNECT, "Connect");
			ModifyMenu(hmenu, ID_FILE_DISCONNECT, MF_BYCOMMAND | MF_ENABLED, ID_FILE_DISCONNECT, "Disconnect");
			if (menuFlags & MF_CHECKED) {
				DialogBox(GetModuleHandle(NULL),
					MAKEINTRESOURCE(IDD_DIALOGCONN),
					hwnd,
					(DLGPROC)DialogProcForms);
			}
			else {
				DialogBox(GetModuleHandle(NULL),
					MAKEINTRESOURCE(IDD_DIALOGCONNSER),
					hwnd,
					(DLGPROC)DialogProcForms);
				serverparm->tcp = tcp;
				serverparm->sock = listensock;
				serverparm->accept = AcceptEvent;
				if ((serverthread = CreateThread(NULL, 0, ServerListen, (LPVOID)serverparm, 0, &ThreadId)) == NULL)
				{
					sprintf_s(errmsg, "CreateThread failed with error %d\n", GetLastError());
					OutputDebugString(errmsg);
					break;
				}
			}
			break;
		case ID_FILE_SENDFILE:
			ret = DialogBox(GetModuleHandle(NULL),
				MAKEINTRESOURCE(IDD_DIALOGSEND),
				hwnd,
				(DLGPROC)DialogProcForms);
			if (ret == 1) {
				ModifyMenu(hmenu, ID_FILE_SENDFILE, MF_BYCOMMAND | MF_DISABLED, ID_FILE_SENDFILE, "Send File");
				strcpy_s(clientparam->filename, FILENAMESIZE, filenameclient);
				clientparam->tcp = tcp;
				clientparam->sock = sClient;
				clientparam->server = server;
				if ((clientthread = CreateThread(NULL, 0, ClientSend, (LPVOID)clientparam, 0, &ThreadId)) == NULL)
				{
					sprintf_s(errmsg, "CreateThread failed with error %d\n", GetLastError());
					OutputDebugString(errmsg);
					break;
				}
			}
			break;
		case ID_FILE_DISCONNECT:
			menuFlags = GetMenuState(hmenu, ID_OPERATINGMODE_CLIENT, MF_BYCOMMAND);
			if (menuFlags & MF_CHECKED) {
				if (clientrunning) {
					ClientCleanup(clientparam->sock);
				}
				ModifyMenu(hmenu, ID_OPERATINGMODE_CLIENT, MF_BYCOMMAND | MF_UNCHECKED, ID_OPERATINGMODE_CLIENT, "Client");
				ModifyMenu(hmenu, ID_FILE_CONNECT, MF_BYCOMMAND | MF_DISABLED, ID_FILE_CONNECT, "Connect");
				ModifyMenu(hmenu, ID_FILE_SENDFILE, MF_BYCOMMAND | MF_DISABLED, ID_FILE_SENDFILE, "Send File");
				ModifyMenu(hmenu, ID_FILE_DISCONNECT, MF_BYCOMMAND | MF_DISABLED, ID_FILE_DISCONNECT, "Disconnect");
			}
			else {
				ServerCleanup();
				ModifyMenu(hmenu, ID_OPERATINGMODE_SERVER, MF_BYCOMMAND | MF_UNCHECKED, ID_OPERATINGMODE_SERVER, "Server");
				ModifyMenu(hmenu, ID_FILE_CONNECT, MF_BYCOMMAND | MF_DISABLED, ID_FILE_CONNECT, "Connect");
				ModifyMenu(hmenu, ID_FILE_SENDFILE, MF_BYCOMMAND | MF_DISABLED, ID_FILE_SENDFILE, "Send File");
				ModifyMenu(hmenu, ID_FILE_DISCONNECT, MF_BYCOMMAND | MF_DISABLED, ID_FILE_DISCONNECT, "Disconnect");
			}
			break;
		}
		break;
	case WM_DESTROY:	// Terminate program
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/*---------------------------------------------------------------------------------------
--	FUNCTION:		BOOL DialogProcInst(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)

--
--	DATE:			January 19, 2016
--
--	REVISIONS:		February 13, 2016
--
--	DESIGNERS:		Micah Willems
--
--	PROGRAMMER:		Micah Willems
--
--	NOTES:
--      This is the event processing loop for the instructions dialog, which is much
--		simpler than all the others. It puts the data in the buffer read from a file 
--		containing the instruction text, and puts it into the readonly edittext control.
---------------------------------------------------------------------------------------*/
BOOL DialogProcInst(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hDlg, IDC_EDIT_INST), EM_SETSEL, 0, -1);
		SendMessage(GetDlgItem(hDlg, IDC_EDIT_INST), EM_REPLACESEL, FALSE, (LPARAM)instbuff);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK_INST:
		case IDCANCEL_INST:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}
	return FALSE;
}

/*---------------------------------------------------------------------------------------
--	FUNCTION:		BOOL DialogProcForms(HWND hDlg, UINT msg,
--						WPARAM wParam, LPARAM lParam)
--
--
--	DATE:			January 19, 2016
--
--	REVISIONS:		February 13, 2016
--
--	DESIGNERS:		Micah Willems
--
--	PROGRAMMER:		Micah Willems
--
--	NOTES:
--      This is the main dialog message processing loop. Each dialog with input required
--		checks to make sure that the data fields have been filled. Radio buttons are 
--		selected by default and the combo box is initialized upon creation.
--
---------------------------------------------------------------------------------------*/
BOOL DialogProcForms(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hCtl;
	int id;
	char charsize[100];
	switch (msg) {
	case WM_INITDIALOG:
		sizepicked = false;
		id = GetDlgCtrlID((HWND)wParam);
		CheckRadioButton(hDlg, IDC_RADIOTCP, IDC_RADIOUDP, IDC_RADIOTCP);
		CheckRadioButton(hDlg, IDC_RADIOTCPSV, IDC_RADIOUDPSV, IDC_RADIOTCPSV);
		CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO100, IDC_RADIO1);
		if (id == IDC_EDIT_CONN) {
			hCtl = GetDlgItem(hDlg, IDC_COMBO1);
			SendMessage(hCtl, CB_ADDSTRING, NULL, (LPARAM)"1024B");
			SendMessage(hCtl, CB_ADDSTRING, NULL, (LPARAM)"4096B");
			SendMessage(hCtl, CB_ADDSTRING, NULL, (LPARAM)"20KB");
			SendMessage(hCtl, CB_ADDSTRING, NULL, (LPARAM)"60KB");
			SendMessage(hCtl, CB_ADDSTRING, NULL, (LPARAM)"180KB");
			SendMessage(hCtl, CB_ADDSTRING, NULL, (LPARAM)"540KB");
		}
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == CBN_SELCHANGE)
			// If the user makes a selection from the list:
			//   Send CB_GETCURSEL message to get the index of the selected list item.
			//   Send CB_GETLBTEXT message to get the item at the index.
			//   Store the item in a TCHAR.
		{
			int id = LOWORD(wParam);
			int ItemIndex = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
			SendMessage((HWND)lParam, CB_GETLBTEXT, ItemIndex, (LPARAM)charsize);
			clientparam->size = GetBitSize(charsize);
			sizepicked = true;
			break;
		}
		switch (LOWORD(wParam)) {
		case IDOK_CONNSER:
			hCtl = GetDlgItem(hDlg, IDC_RADIOTCPSV);
			if (SendMessage(hCtl, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				tcp = true;
			}
			if (ServerSetup(tcp) == -1) {
				MessageBox(hDlg, "Error opening connection", "Warning", MB_OK);
				break;
			}
			EndDialog(hDlg, 0);
			break;
		case IDOK_CONN:
			hCtl = GetDlgItem(hDlg, IDC_EDIT_CONN);
			if (SendMessage(hCtl, WM_GETTEXTLENGTH, 0, 0) == 0 || !sizepicked) {
				MessageBox(hDlg, "Please enter an address and packet size", "Warning", MB_OK);
				break;
			}
			hCtl = GetDlgItem(hDlg, IDC_RADIOTCP);
			if (SendMessage(hCtl, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				tcp = true;
			}
			if (ClientSetup(tcp) == -1) {
				MessageBox(hDlg, "Error opening connection", "Warning", MB_OK);
				break;
			}
			EndDialog(hDlg, 0);
			break;
		case IDC_BUTTONOPENFILE:
			ZeroMemory(&ofncl, sizeof(ofncl));
			ofncl.lStructSize = sizeof(ofncl);
			ofncl.hwndOwner = hDlg;
			ofncl.lpstrFile = filenameclient;
			ofncl.lpstrFile[0] = '\0';
			ofncl.nMaxFile = sizeof(filenameclient);
			ofncl.lpstrFilter = TEXT("Text files (.txt)\0*.TXT\0\0");
			ofncl.nFilterIndex = 1;
			ofncl.lpstrFileTitle = NULL;
			ofncl.nMaxFileTitle = 0;
			ofncl.lpstrInitialDir = NULL;
			ofncl.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofncl) == 0) {
				break;
			}
			hCtl = GetDlgItem(hDlg, IDC_RADIO1);
			if (SendMessage(hCtl, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				clientparam->numpackets = 1;
			}
			else {
				hCtl = GetDlgItem(hDlg, IDC_RADIO10);
				if (SendMessage(hCtl, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					clientparam->numpackets = 10;
				}
				else {
					clientparam->numpackets = 100;
				}
			}
			OutputDebugString(filenameclient);
			EndDialog(hDlg, 1);
			break;
		case IDCANCEL_CONN:
		case IDCANCEL_CONNSER:
		case IDCANCEL_SEND:
			EndDialog(hDlg, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}
	return FALSE;
}

/*---------------------------------------------------------------------------------------
--	FUNCTION:		int GetBitSize()

--
--	DATE:			February 13, 2016
--
--	REVISIONS:		
--
--	DESIGNERS:		Micah Willems
--
--	PROGRAMMER:		Micah Willems
--
--	NOTES:
--      This function translates the words picked from the combobox to their respective
--		numeric values.
---------------------------------------------------------------------------------------*/
int GetBitSize(char *word) {
	if (strcmp(word, "1024B") == 0) {
		return KBYTES1;
	}
	else if (strcmp(word, "1024B") == 0) {
		return KBYTES4;
	}
	else if(strcmp(word, "1024B") == 0) {
		return KBYTES20;
	}
	else if(strcmp(word, "1024B") == 0) {
		return KBYTES60;
	}
	else if(strcmp(word, "1024B") == 0) {
		return KBYTES180;
	}
	return KBYTES540;
}

/*---------------------------------------------------------------------------------------
--	FUNCTION:		void ShowLastErr()

--
--	DATE:			January 19, 2016
--
--	REVISIONS:		February 13, 2016
--
--	DESIGNERS:		Micah Willems
--
--	PROGRAMMER:		Micah Willems
--
--	NOTES:
--      This is a universal-purpose error reporting function for functions that can be
--		error checked using GetLastError. It uses FormatMessage to get an actual
--		human-readable, understandable string from the error ID and outputs it to the
--		Debug output console in the IDE.
---------------------------------------------------------------------------------------*/
void ShowLastErr(bool wsa) {
	DWORD dlasterr;
	LPCTSTR errmsg = NULL;
	char errnum[100];
	if (wsa = true) {
		dlasterr = WSAGetLastError();
	}
	else {
		dlasterr = GetLastError();
	}
	sprintf_s(errnum, "Error number: %d\n", dlasterr);
	OutputDebugString(errnum);
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, dlasterr, 0, (LPSTR)&errmsg, 0, NULL);
	OutputDebugString(errmsg);
}