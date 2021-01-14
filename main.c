#include <hal/debug.h>
#include <hal/video.h>
#include <nxdk/mount.h>
#include <windows.h>
#include <string.h>

#include "kafl_user.h"
char target_file[MAX_PATH] = { 0 };

uint32_t resolve_KeBugCheck() {
    // TODO: return actual address or use some scanning method
    return 0xDEADBEEF;
}

uint32_t resolve_KeBugCheckEx() {
    // TODO: return actual address or use some scanning method
    return 0xCAFEBABE;
}

/*
static inline void run_program(void* target){
    PROCESS_INFORMATION p1;
    STARTUPINFOA s1;

        ZeroMemory(&p1, sizeof(p1));
        ZeroMemory(&s1, sizeof(s1));
        s1.cb = sizeof(s1);

        debugPrint("[+] LOADER: Starting fuzzing target\n");
        BOOL success = CreateProcessA(NULL, target, NULL, NULL, FALSE,
            0, NULL, NULL, &s1, &p1);
        if (!success){
            debugPrint("[-] LOADER: cannot start fuzzing target\n");
            getchar();
            ExitProcess(0);
        }
        TerminateProcess((HANDLE)-1,0x41);
}
*/
/*
    // Mount C:
    BOOL ret = nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
    if (!ret) {
        debugPrint("Failed to mount E: drive!\n");
        Sleep(5000);
        return -1;
    }
    debugPrint("Mounted E: drive!\n");
    Sleep(5000);
    return 0;
*/
/*
static inline void load_program(void* buf){
    HANDLE payload_file_handle = NULL;
    DWORD dwWritten;

    memset(target_file, 0x00, MAX_PATH);
    // TODO: equivalent API?
    CONST char* temp_path = "";
    DWORD tmp_path_len = strlen(temp_path) + 0x01;
    strcpy(target_file, temp_path);
    //DWORD tmp_path_len = GetTempPathA(MAX_PATH, target_file);
    memcpy(target_file + tmp_path_len, "\x5c", 1);
    memcpy(target_file + tmp_path_len + 1, TARGET_FILE_WIN, strlen(TARGET_FILE_WIN));

    payload_file_handle = CreateFile((LPCSTR)target_file,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    BOOL result = WriteFile(
        payload_file_handle,
        buf,
        PROGRAM_SIZE,
        &dwWritten,
        NULL
    );
    if (result == 0){
        debugPrint("[+] Cannot write usermode fuzzer (%ld)\n", GetLastError());
        // blocks
        getchar();
    }

    debugPrint("[+] LOADER: Executing target: %s\n", target_file);
    CloseHandle(payload_file_handle);
    //run_program(target_file);
}
*/

int main(void)
{
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    uint32_t panic_handler1 = 0x00000000;
    uint32_t panic_handler2 = 0x00000000;
    void* program_buffer;

    // TODO: AddVectoredExceptionHandler

    // TODO: Error handling
    debugPrint("Resolving KeBugCheck...\n");
    panic_handler1 = resolve_KeBugCheck();
    debugPrint("Resolving KeBugCheckEx...\n");
    panic_handler2 = resolve_KeBugCheckEx();

    /* allocate 4MB contiguous virtual memory to hold fuzzer program; data is provided by the fuzzer */
    //program_buffer = (void*)VirtualAlloc(0, PROGRAM_SIZE, MEM_COMMIT, PAGE_READWRITE);
    /* ensure that the virtual memory is *really* present in physical memory... */
    //memset(program_buffer, 0xff, PROGRAM_SIZE);

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /* this hypercall will generate a VM snapshot for the fuzzer and subsequently terminate QEMU */
    debugPrint("Acquiring kAFL lock...\n");

    // TODO return values from hypercall
    // PATCH HERE -- I was calling this with 0 rather than HYPERCALL_KAFL_LOCK
    kAFL_hypercall(HYPERCALL_KAFL_LOCK, 0);
    debugPrint("kAFL lock acquired!\n");

    /***** Fuzzer Entrypoint *****/


    /* initial fuzzer handshake */
    kAFL_hypercall(HYPERCALL_KAFL_ACQUIRE, 0);
    debugPrint("HYPERCALL_KAFL_ACQUIRE Complete\n");
    kAFL_hypercall(HYPERCALL_KAFL_RELEASE, 0);
    debugPrint("HYPERCALL_KAFL_RELEASE Complete\n");
    /* submit panic address */
    kAFL_hypercall(HYPERCALL_KAFL_SUBMIT_PANIC, panic_handler1);
    debugPrint("HYPERCALL_KAFL_SUBMIT_PANIC Complete\n");
    kAFL_hypercall(HYPERCALL_KAFL_SUBMIT_PANIC, panic_handler2);
    debugPrint("HYPERCALL_KAFL_SUBMIT_PANIC Complete\n");

    // The fuzzer program will be baked in...
    // This makes setup slightly more annoying
    /* submit virtual address of program buffer and wait for data (*blocking*) */
    // kAFL_hypercall(HYPERCALL_KAFL_GET_PROGRAM, (uint32_t)program_buffer);
    /* execute fuzzer program */
    // load_program(program_buffer);

    /* bye */
    return 0;
}
