#include "main.h"

int FileOpen (wchar_t* lp_fileDir, FILE** lp_output) {
	FILE* lv_file;
	WORD lv_pe;
	
	//Open File
	wprintf(L"Open File: %ls\n", lp_fileDir);
	
	lv_file = _wfopen(lp_fileDir, L"r+b");
	
	if (lv_file == NULL) {
		perror("Could Not Open File. Error Code");
		fclose(lv_file);
		return 1;
	}
	
	//Check Format (File Name)
	if (wcsstr(lp_fileDir, L".exe") == NULL) {
		puts("File is not PE Format.");
		fclose(lv_file);
		return 1;
	}
	
	//Check Format (Header Character)
	fread(&lv_pe, sizeof(WORD), 1, lv_file);
	if (lv_pe != 0x5A4D) {
		puts("File is not PE Format.");
		fclose(lv_file);
		return 1;
	}
	puts("");

	*lp_output = lv_file;
	return 0;
}

int Read_NTHeader_Offset (FILE* lp_file, DWORD* lp_output) {
	DWORD lv_offset;
	
	//Get NT Header Offset
	fseek(lp_file, DOS_HEADER_OFFSET, SEEK_SET);
	fread(&lv_offset, sizeof(DWORD), 1, lp_file);
	
	//Print Result
	printf("NT Header Offset: 0x%X\n", lv_offset);
	
	*lp_output = lv_offset;
	return 0;
}

int Read_ArchitectureMachine (FILE* lp_file, DWORD lp_nt_offset, WORD* lp_output) {
	WORD lv_machine;
	
	//Get Architecture Machine
	fseek(lp_file, lp_nt_offset + sizeof(DWORD), SEEK_SET);
	fread(&lv_machine, sizeof(WORD), 1, lp_file);
	
	//Print Result
	printf("Process Machine: %X(", lv_machine);
	switch(lv_machine) {
		MachineCase(IMAGE_FILE_MACHINE_I386);
		MachineCase(IMAGE_FILE_MACHINE_IA64);
		MachineCase(IMAGE_FILE_MACHINE_AMD64);
		default: {
			printf("Unknown)");
			return 1;
		}
	}
	printf(" | %s)\n", Is32Processer(lv_machine) ? "32-bit" : "64-bit");
	*lp_output = lv_machine;
	return 0;
}

int Read_ImageOptionalHeader (FILE* lp_file, DWORD lp_nt_offset, WORD lp_machine, IMAGE_OPTIONAL_HEADER64* lp_output_header, int* lp_output_size) {
	int lv_header_size;
	
	//Decide Header Size
	lv_header_size = Is32Processer(lp_machine) ? sizeof(IMAGE_OPTIONAL_HEADER32) : sizeof(IMAGE_OPTIONAL_HEADER64);

	//Get Optional Header
	fseek(lp_file, lp_nt_offset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER), SEEK_SET);
	fread(lp_output_header, lv_header_size, 1, lp_file);
	
	*lp_output_size = lv_header_size;
	return 0;
}

int Get_ImageOptionalHeader_Subsystem (IMAGE_OPTIONAL_HEADER64* lp_header) {
	WORD* lv_optional_header_p = (WORD*)lp_header;
	
	//Print Subsystem
	printf("Subsystem: ");
	switch (lv_optional_header_p[SUBSYSTEM_OFFSET]) {
		SubsystemCase(IMAGE_SUBSYSTEM_UNKNOWN);
		SubsystemCase(IMAGE_SUBSYSTEM_NATIVE);
		SubsystemCase(IMAGE_SUBSYSTEM_WINDOWS_GUI);
		SubsystemCase(IMAGE_SUBSYSTEM_WINDOWS_CUI);
		SubsystemCase(IMAGE_SUBSYSTEM_OS2_CUI);
		SubsystemCase(IMAGE_SUBSYSTEM_POSIX_CUI);
		SubsystemCase(IMAGE_SUBSYSTEM_WINDOWS_CE_GUI);
		SubsystemCase(IMAGE_SUBSYSTEM_EFI_APPLICATION);
		SubsystemCase(IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER);
		SubsystemCase(IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER);
		SubsystemCase(IMAGE_SUBSYSTEM_EFI_ROM);
		SubsystemCase(IMAGE_SUBSYSTEM_XBOX);
		default: {
			printf("Not PE Format Subsystem.");
		}
	}
	
	return 0;
}

int UserInput_SubsystemIndex (int* lp_output) {
	char lv_subsystem_c[100];
	int lv_subsystem_i;
	CONSOLE_SCREEN_BUFFER_INFO lv_csbi; //For Console Coordinate

	//Select Subsystem
	puts("\nSelect Subsystem Number.");
	PrintSubsystem(IMAGE_SUBSYSTEM_UNKNOWN, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_NATIVE, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_WINDOWS_GUI, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_WINDOWS_CUI, " (Console)");
	PrintSubsystem(IMAGE_SUBSYSTEM_OS2_CUI, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_POSIX_CUI, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_EFI_APPLICATION, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_EFI_ROM, "");
	PrintSubsystem(IMAGE_SUBSYSTEM_XBOX, "");
	
	printf("Number: ");
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &lv_csbi);
	
	INPUT:
	gets(lv_subsystem_c);
	lv_subsystem_i = atoi(lv_subsystem_c);
	
	//Check Valid
	switch (lv_subsystem_i) {
		SubsystemCheck(IMAGE_SUBSYSTEM_UNKNOWN);
		SubsystemCheck(IMAGE_SUBSYSTEM_NATIVE);
		SubsystemCheck(IMAGE_SUBSYSTEM_WINDOWS_GUI);
		SubsystemCheck(IMAGE_SUBSYSTEM_WINDOWS_CUI);
		SubsystemCheck(IMAGE_SUBSYSTEM_OS2_CUI);
		SubsystemCheck(IMAGE_SUBSYSTEM_POSIX_CUI);
		SubsystemCheck(IMAGE_SUBSYSTEM_WINDOWS_CE_GUI);
		SubsystemCheck(IMAGE_SUBSYSTEM_EFI_APPLICATION);
		SubsystemCheck(IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER);
		SubsystemCheck(IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER);
		SubsystemCheck(IMAGE_SUBSYSTEM_EFI_ROM);
		SubsystemCheck(IMAGE_SUBSYSTEM_XBOX);
		default: {
			printf("Invalid Number: %-10d", lv_subsystem_i);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), lv_csbi.dwCursorPosition);
			printf("                  "); //Empty Text
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), lv_csbi.dwCursorPosition);
			goto INPUT;
		}
	}
	
	*lp_output = lv_subsystem_i;
	return 0;
}

int Set_ImageOptionalHeader_Subsystem (IMAGE_OPTIONAL_HEADER64* lp_header, int lp_subsystem) {
	WORD *lv_optional_header_p = (WORD*)lp_header;
	
	lv_optional_header_p[SUBSYSTEM_OFFSET] = (WORD)lp_subsystem;
	return 0;
}
int Write_ImageOptionalHeader (FILE* lp_file, DWORD lp_nt_offset, IMAGE_OPTIONAL_HEADER64* lp_write_header, int lp_write_size) {
	//Write File
	fseek(lp_file, lp_nt_offset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER), SEEK_SET);
	if (fwrite(lp_write_header, lp_write_size, 1, lp_file) != 1) {
		perror("Could Not Write File. Error Code");
		return 1;
	}
	
	puts("\nOverwrite Success.");
	return 0;
}

int wmain (int argc, wchar_t* argv[]) {
	FILE *lv_file = NULL;
	DWORD lv_nt_offset;
	WORD lv_machine;
	IMAGE_OPTIONAL_HEADER64 lv_optional_header;
	int lv_header_size;
	int lv_input_subsystem;
	
	setlocale(LC_ALL, "");
	
	if (argc == 1) {
		wprintf(L"Usage: %ls [Path]\n", wcsrchr(argv[0], L'\\') + 1);
		goto END;
	}
	
	if (FileOpen(argv[1], &lv_file)) {
		goto END;
	}
	
	Read_NTHeader_Offset(lv_file, &lv_nt_offset);
	
	Read_ArchitectureMachine(lv_file, lv_nt_offset, &lv_machine);
	
	Read_ImageOptionalHeader(lv_file, lv_nt_offset, lv_machine ,&lv_optional_header, &lv_header_size);
	
	Get_ImageOptionalHeader_Subsystem(&lv_optional_header);
	
	UserInput_SubsystemIndex(&lv_input_subsystem);
	
	Set_ImageOptionalHeader_Subsystem(&lv_optional_header, lv_input_subsystem);

	if (Write_ImageOptionalHeader(lv_file, lv_nt_offset, &lv_optional_header, lv_header_size)) {
		goto END;
	}
	
	END:
	fclose(lv_file);
	system("pause");
	return 0;
}