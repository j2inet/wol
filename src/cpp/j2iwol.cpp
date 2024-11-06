// j2iwol.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN
//#include <xmllite.h>
#include <shlwapi.h>
#include <iostream>
#include <vector>
#include <string>
#include <WinSock2.h>
#include <cstddef>
#include <regex>
#include "resource.h"



#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "xmllite.lib")
#pragma comment(lib, "shlwapi.lib")


std::wregex pattern(L"(([0-9a-fA-F]{2}):){5}([0-9a-fA-F]{2})");

enum APP_SWITCHES
{
	SHOW_HELP = 1,
	QUITE_MODE = 2
};

bool IsWellFormedMACAddress(std::wstring address)
{
	std::smatch matchList;
	bool isMatch = std::regex_search(address, pattern);
	return isMatch;
}
bool BroadcastMessage(std::vector<BYTE> message)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		printf("Socket creation failed.\n");
		WSACleanup();
		return false;
	}

	BOOL broadcast = TRUE;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) < 0) {
		printf("Error in setting Broadcast option.\n");
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	sockaddr_in broadcastAddr;
	broadcastAddr.sin_family = AF_INET;
	broadcastAddr.sin_port = htons(9); // Use your desired port
	broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

	if (sendto(sock, (char*)(message.data()), message.size(), 0, (sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
		printf("Broadcast message send failed.\n");
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	closesocket(sock);
	return true;
}


std::vector<BYTE> MacAddressToByteArray(std::wstring macAddress)
{
	std::vector<BYTE> macAddressBytes;
	std::wstring macAddressPart;
	for (size_t i = 0; i < macAddress.size(); i++)
	{
		if (macAddress[i] == L':')
		{
			macAddressBytes.push_back((BYTE)std::stoi(macAddressPart, nullptr, 16));
			macAddressPart.clear();
		}
		else
		{
			macAddressPart.push_back(macAddress[i]);
		}
	}
	macAddressBytes.push_back((BYTE)std::stoi(macAddressPart, nullptr, 16));
	return macAddressBytes;
}


void SendWOL(std::vector<BYTE> macAddress)
{
	std::vector<BYTE> magicPacket;
	for (size_t i = 0; i < 6; i++)
	{
		magicPacket.push_back(0xFF);
	}
	for (size_t i = 0; i < 16; i++)
	{
		for (size_t j = 0; j < macAddress.size(); j++)
		{
			magicPacket.push_back(macAddress[j]);
		}
	}
	BroadcastMessage(magicPacket);
}

void SendWOL(std::wstring macAddress) {
	auto bytes = MacAddressToByteArray(macAddress);
	SendWOL(bytes);
}

void ShowResourceString(UINT resourceId) 
{
	auto messageBuffer = std::vector<WCHAR>(1024);
	auto messageLength = LoadStringW(GetModuleHandleW(NULL), resourceId, messageBuffer.data(), messageBuffer.size());
	std::wstring message(messageBuffer.data(), messageLength);
	std::wcout << message << std::endl;
}
void showApplicationInfo() 
{
	ShowResourceString(IDS_ABOUT);
}

void showUsage()
{
	ShowResourceString(IDS_USAGE);
}

void showSite()
{
	ShowResourceString(IDS_SITE);
}

int GetSwitches(std::vector<std::wstring> args)
{
	int retVal = 0;
	for (auto i = 0; i < args.size(); ++i)
	{
		if (args[i] == L"-h" || args[i] == L"--help" || args[i] == L"/h")
		{
			retVal |= APP_SWITCHES::SHOW_HELP;
		}
		else if (args[i] == L"-q" || args[i] == L"--quite" || args[i] == L"/q")
		{
			retVal |= APP_SWITCHES::QUITE_MODE;
		}
	}
	return retVal;
}

std::vector<std::wstring> GetMACAddresses(std::vector<std::wstring> args)
{
	std::vector<std::wstring> macAddresses;
	for (auto i = 0; i < args.size(); ++i)
	{
		if (IsWellFormedMACAddress(args[i]))
		{
			macAddresses.push_back(args[i]);
		}
	}
	return macAddresses;
}

int wmain(int argc, WCHAR** argv)
{
	bool quiteMode = false;
	std::vector<std::wstring> argList;
	for (auto i = 0; i < argc; ++i)
	{
		argList.push_back(argv[i]);
	}

	auto switches = GetSwitches(argList);
	if (switches & APP_SWITCHES::SHOW_HELP)
	{
		showUsage();
		return 0;
	}
	if (switches & APP_SWITCHES::QUITE_MODE)
	{
		quiteMode = true;
	}

	if (!quiteMode)
		showApplicationInfo();

	auto addressList = GetMACAddresses(argList);
	if (addressList.size() == 0)
	{
		// note that even if the exe is run in quite mode, 
		// the usage will be shown if 
		// no valid MAC address is provided.
		showUsage();
	}
	else
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			std::cerr << "WSAStartup failed." << std::endl;
			return 1;
		}

		for (auto i = 0; i < addressList.size(); ++i)
		{
			SendWOL(addressList[i]);
		}
		WSACleanup();
	}
	if (!quiteMode)
		showSite();
	return 0;
}

