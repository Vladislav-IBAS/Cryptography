#define _CRT_SECURE_NO_WARNINGS
#include <sstream>

#include "iostream"
#include <string>
#include <conio.h>
#include <string>
#include <vector>
#include <stdint.h>
#include <windows.h>
#include <math.h>
#include "..\shared\MyHash.h"
#include "..\shared\sha.h"
#include "..\shared\zip.h"
#include "..\shared\MyFile.h"

#pragma comment(lib,"..\\third-party\\lib\\my.lib")

void GenerateHash(std::vector<byte> data)
{
	srand(time(NULL));
	MyFile hash1;
	MyFile hash2;
	int iter = 0;
	int count = data.size() / 64;
	std::vector<byte> tmp;
	std::vector<byte> buf1;
	std::vector<byte> buf2;
	for (int i = 0; i < count; i++)
	{
		tmp.clear();
		buf1.clear();
		buf2.clear();

		int s = rand() % 8;
		int t = 0;
		if (iter + 64 + s < data.size())
		{
			t = s;
		}
		for (int j = iter; j < iter + 64 + t; j++)
		{
			tmp.push_back(data.at(j));
		}
		buf1 = My_hash(tmp);
		buf2 = SHA(tmp);
		for (int k = 0; k < buf1.size(); k++)
		{
			hash1.GetData().push_back(buf1.at(k));
			hash2.GetData().push_back(buf2.at(k));
		}
		iter += 64;
	}

	tmp.clear();
	buf1.clear();
	buf2.clear();
	for (int j = iter; j < data.size(); j++)
	{
		tmp.push_back(data.at(j));
	}

	buf1 = My_hash(tmp, data.size() - iter);
	buf2 = SHA(tmp);
	for (int k = 0; k < buf1.size(); k++)
	{
		hash1.GetData().push_back(buf1.at(k));
		hash2.GetData().push_back(buf2.at(k));
	}

	hash1.Write("my_hash.txt");
	hash2.Write("hash_sha.txt");
}

double Entropy(std::vector<byte> hash, int size = 8, int mode = 1)
{
	if (mode != 1 && mode != 2)
	{
		std::cout << "Use mode 1 or 2!";
		return -1;
	}
	int n = pow(2, size);
	int sum = 0;
	double E = 0;

	if (size == 8)
	{
		uint8_t* arr = new uint8_t[n];
		for (int i = 0; i < n; i++)
		{
			arr[i] = 0;
		}

		for (int i = 0; i < hash.size(); i++)
		{
			uint8_t tmp = hash.at(i);
			arr[tmp]++;
			sum++;
		}

		for (int i = 0; i < n; i++)
		{
			if (arr[i] > 0)
			{
				E += (arr[i] / (double)sum) * log2(arr[i] / (double)sum) / log(size);
			}
		}

		delete[] arr;
	}

	if (size == 16)
	{
		uint16_t* arr = new uint16_t[n];
		for (int i = 0; i < n; i++)
		{
			arr[i] = 0;
		}

		for (int i = 0; i < hash.size(); i += mode)
		{
			uint16_t tmp;
			try
			{
				tmp = (hash.at(i) << 8) + hash.at(i + 1);
				arr[tmp]++;
				sum++;
			}
			catch (...)
			{

			}
		}

		for (int i = 0; i < n; i++)
		{
			if (arr[i] > 0)
			{
				E += (arr[i] / (double)sum) * log2(arr[i] / (double)sum) / log(size);
			}
		}

		delete[] arr;
	}
	return -E;
}

double Compression(std::string file, std::string fileZIP)
{
	MyFile f1;
	MyFile f2;
	f1.Open(file);
	f2.Open(fileZIP);
	return 100 - (f2.GetData().size() / (double)f1.GetData().size()) * 100;
}

void main()
{
	MyFile plaintext;
	MyFile my_hash;
	MyFile sha;
	plaintext.Open("..\\resources\\plaintext.txt");
	GenerateHash(plaintext.GetData());
	my_hash.Open("my_hash.txt");
	sha.Open("hash_sha.txt");
	bool flag = true;

	CreateDirectory("Results", NULL);

	while (flag)
	{
		system("cls");
		std::cout << "Calculate entropy:\n1. 8 bit\n2. 16 bit\n3. ZIP\n4. All\n5. Exit\nChoose action: ";
		int size;
		std::cin >> size;
		if (size == 1)
		{
			FILE* _file(NULL);
			_file = fopen("Results\\Entropy 8 bit.txt", "w");
			system("cls");
			double plain = Entropy(plaintext.GetData());
			double myhash = Entropy(my_hash.GetData());
			double sha_hash = Entropy(sha.GetData());
			std::cout << "Entropy 8 bit" << std::endl;
			std::cout << "for plaintext: " << plain << std::endl;
			std::cout << "for my hash: " << myhash << std::endl;
			std::cout << "for sha: " << sha_hash << std::endl << std::endl;
			std::cout << "You can find results in \"Results\\Entropy 8 bit.txt\"" << std::endl;

			fprintf(_file, "Entropy 8 bit:\nfor plaintext: %f\nfor my hash: %f\nfor sha: %f\n\n", plain, myhash, sha_hash);
			fclose(_file);

			system("pause");
		}
		if (size == 2)
		{
			FILE* _file(NULL);

			system("cls");
			std::cout << "1. With overlay\n2. Without overlay\n";
			int mode;
			std::cin >> mode;
			system("cls");
			std::string name = "";
			if (mode == 1)
			{
				name = "Entropy 16 bit with overlay.txt";
				_file = fopen(("Results\\" + name).c_str(), "w");
				std::cout << "With overlay" << std::endl;
			}
			if (mode == 2)
			{
				name = "Entropy 16 bit without overlay.txt";
				_file = fopen(("Results\\" + name).c_str(), "w");
				std::cout << "Without overlay" << std::endl;
			}
			std::cout << "Entropy 16 bit" << std::endl;
			std::cout << "for plaintext: " << Entropy(plaintext.GetData(), 16, mode) << std::endl;
			std::cout << "for my hash: " << Entropy(my_hash.GetData(), 16, mode) << std::endl;
			std::cout << "for sha: " << Entropy(sha.GetData(), 16, mode) << std::endl << std::endl;
			std::cout << "You can find results in \"Results\\" << name << "\"" << std::endl;

			fprintf(_file, "%s:\nfor plaintext: %f\nfor my hash: %f\nfor sha: %f\n\n", name.substr(0, name.size() - 4).c_str(), Entropy(plaintext.GetData(), 16, mode),
				Entropy(my_hash.GetData(), 16, mode), Entropy(sha.GetData(), 16, mode));
			fclose(_file);

			system("pause");

		}
		if (size == 4)
		{
			FILE* _file(NULL);
			_file = fopen("Results\\All results.txt", "w");

			system("cls");
			std::cout << "Entropy 8 bit" << std::endl;
			std::cout << "for plaintext: " << Entropy(plaintext.GetData()) << std::endl;
			std::cout << "for my hash: " << Entropy(my_hash.GetData()) << std::endl;
			std::cout << "for sha: " << Entropy(sha.GetData()) << std::endl << std::endl;

			fprintf(_file, "Entropy 8 bit:\nfor plaintext: %f\nfor my hash: %f\nfor sha: %f\n\n",
				Entropy(plaintext.GetData()), Entropy(my_hash.GetData()), Entropy(sha.GetData()));

			std::cout << "Entropy 16 bit" << std::endl;
			std::cout << "With overlay" << std::endl;
			std::cout << "for plaintext: " << Entropy(plaintext.GetData(), 16, 1) << std::endl;
			std::cout << "for my hash: " << Entropy(my_hash.GetData(), 16, 1) << std::endl;
			std::cout << "for sha: " << Entropy(sha.GetData(), 16, 1) << std::endl << std::endl;

			fprintf(_file, "Entropy 16 bit with overlay:\nfor plaintext: %f\nfor my hash: %f\nfor sha: %f\n\n",
				Entropy(plaintext.GetData(), 16, 1),
				Entropy(my_hash.GetData(), 16, 1), Entropy(sha.GetData(), 16, 1));

			std::cout << "Entropy 16 bit" << std::endl;
			std::cout << "Without overlay" << std::endl;
			std::cout << "for plaintext: " << Entropy(plaintext.GetData(), 16, 2) << std::endl;
			std::cout << "for my hash: " << Entropy(my_hash.GetData(), 16, 2) << std::endl;
			std::cout << "for sha: " << Entropy(sha.GetData(), 16, 2) << std::endl << std::endl;

			fprintf(_file, "Entropy 16 bit without overlay:\nfor plaintext: %f\nfor my hash: %f\nfor sha: %f\n\n",
				Entropy(plaintext.GetData(), 16, 2),
				Entropy(my_hash.GetData(), 16, 2), Entropy(sha.GetData(), 16, 2));

			HZIP plain = CreateZip("plaintext.zip", 0);
			ZipAdd(plain, "plaintextZIP.txt", "..\\resources\\plaintext.txt");
			CloseZip(plain);
			HZIP myHash = CreateZip("myHash.zip", 0);
			ZipAdd(myHash, "my_hashZIP.txt", "my_hash.txt");
			CloseZip(myHash);
			HZIP shaHash = CreateZip("shaHash.zip", 0);
			ZipAdd(shaHash, "hash_sha.txt", "hash_sha.txt");
			CloseZip(shaHash);
			std::cout << "Compression:" << std::endl;
			std::cout << "for plaintext: " << Compression("..\\resources\\plaintext.txt", "plaintext.zip") << "%" << std::endl;
			std::cout << "for my hash: " << Compression("my_hash.txt", "myHash.zip") << "%" << std::endl;
			std::cout << "for sha: " << Compression("hash_sha.txt", "shaHash.zip") << "%" << std::endl << std::endl;

			fprintf(_file, "Compression:\nfor plaintext: %f%%\nfor my hash: %f%%\nfor sha: %f%%\n\n",
				Compression("..\\resources\\plaintext.txt", "plaintext.zip"), Compression("my_hash.txt", "myHash.zip"),
				Compression("hash_sha.txt", "shaHash.zip"));

			std::cout << "You can find results in \"Results\\All results.txt\"" << std::endl;

			fclose(_file);

			system("pause");
		}
		if (size == 3)
		{
			FILE* _file(NULL);
			_file = fopen("Results\\Compression.txt", "w");
			system("cls");
			HZIP plain = CreateZip("plaintext.zip", 0);
			ZipAdd(plain, "plaintextZIP.txt", "..\\resources\\plaintext.txt");
			CloseZip(plain);
			HZIP myHash = CreateZip("myHash.zip", 0);
			ZipAdd(myHash, "my_hashZIP.txt", "my_hash.txt");
			CloseZip(myHash);
			HZIP shaHash = CreateZip("shaHash.zip", 0);
			ZipAdd(shaHash, "hash_sha.txt", "hash_sha.txt");
			CloseZip(shaHash);
			std::cout << "Compression:" << std::endl;
			std::cout << "for plaintext: " << Compression("..\\resources\\plaintext.txt", "plaintext.zip") << "%" << std::endl;
			std::cout << "for my hash: " << Compression("my_hash.txt", "myHash.zip") << "%" << std::endl;
			std::cout << "for sha: " << Compression("hash_sha.txt", "shaHash.zip") << "%" << std::endl << std::endl;
			std::cout << "You can find results in \"Results\\Compression.txt\"" << std::endl;

			fprintf(_file, "Compression:\nfor plaintext: %f%%\nfor my hash: %f%%\nfor sha: %f%%\n\n",
				Compression("..\\resources\\plaintext.txt", "plaintext.zip"), Compression("my_hash.txt", "myHash.zip"),
				Compression("hash_sha.txt", "shaHash.zip"));
			fclose(_file);

			system("pause");
		}
		if (size == 5)
		{
			flag = false;
		}
	}
}