#include <Windows.h>
#include <ShlObj.h>
#include <jni.h>
#include <jvmti.h>
#include <string>
#include <fstream>
#include "xorstr.hpp"

DWORD Init(LPVOID lpParam)
{
    HMODULE jvm = GetModuleHandleA(xorstr_("jvm"));
    if (!jvm) {
        MessageBoxA(nullptr, xorstr_("Code: #1"), xorstr_("Error"), MB_ICONERROR | MB_OK);
        return 1;
    }

    using JNI_GetCreatedJavaVMs_t = jint(JNICALL*)(JavaVM**, jsize, jsize*);
    auto JNI_GetCreatedJavaVMs = reinterpret_cast<JNI_GetCreatedJavaVMs_t>(
        GetProcAddress(jvm, "JNI_GetCreatedJavaVMs")
        );

    if (!JNI_GetCreatedJavaVMs) {
        MessageBoxA(nullptr, xorstr_("Code: #2"), xorstr_("Error"), MB_ICONERROR | MB_OK);
        return 2;
    }

    JavaVM* jvm_ptr = nullptr;
    jsize vmCount = 0;
    if (JNI_GetCreatedJavaVMs(&jvm_ptr, 1, &vmCount) != JNI_OK || vmCount == 0) {
        MessageBoxA(nullptr, xorstr_("Code: #3"), xorstr_("Error"), MB_ICONERROR | MB_OK);
        return 3;
    }

    JNIEnv* env = nullptr;
    jint res = jvm_ptr->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8);

    if (res == JNI_EDETACHED) {
        if (jvm_ptr->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK || !env) {
            MessageBoxA(nullptr, xorstr_("Code: #5"), xorstr_("Error"), MB_ICONERROR | MB_OK);
            return 5;
        }
    }
    else if (res != JNI_OK || !env) {
        MessageBoxA(nullptr, xorstr_("Code: #4"), xorstr_("Error"), MB_ICONERROR | MB_OK);
        return 4;
    }

    jvmtiEnv* jvmti = nullptr;
    if (jvm_ptr->GetEnv(reinterpret_cast<void**>(&jvmti), JVMTI_VERSION_1_2) != JNI_OK || !jvmti) {
        MessageBoxA(nullptr, xorstr_("Code: #6"), xorstr_("Error"), MB_ICONERROR | MB_OK);
        return 6;
    }

    jint classesCount = 0;
    jclass* loadedClasses = nullptr;

    jvmtiError err = jvmti->GetLoadedClasses(&classesCount, &loadedClasses);
    if (err != JVMTI_ERROR_NONE || !loadedClasses) {
        MessageBoxA(nullptr, xorstr_("Code: #7"), xorstr_("Error"), MB_ICONERROR | MB_OK);
        return 7;
    }

    char desktopPath[MAX_PATH];
    if (!SHGetSpecialFolderPathA(nullptr, desktopPath, CSIDL_DESKTOP, FALSE)) {
        MessageBoxA(nullptr, xorstr_("Code: #8 (Desktop path)"), xorstr_("Error"), MB_ICONERROR | MB_OK);
        return 8;
    }

    std::string filePath = std::string(desktopPath) + "\\checker.txt";
    std::ofstream file(filePath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        MessageBoxA(nullptr, xorstr_("Code: #9 (Open file)"), xorstr_("Error"), MB_ICONERROR | MB_OK);
        return 9;
    }

    jmethodID midGetName;

    for (int i = 0; i < classesCount; i++) {
        jclass currentClass = loadedClasses[i];
        if (!currentClass) continue;

        char* signature = nullptr;
        char* generic = nullptr;
        err = jvmti->GetClassSignature(currentClass, &signature, &generic);
        if (err == JVMTI_ERROR_NONE && signature) {
            file << signature << "\n";

            jvmti->Deallocate(reinterpret_cast<unsigned char*>(signature));
            if (generic) jvmti->Deallocate(reinterpret_cast<unsigned char*>(generic));
        }
        else {
            jclass clsClass = env->GetObjectClass(currentClass);
            if (clsClass) {
                midGetName = env->GetMethodID(clsClass, "getName", "()Ljava/lang/String;");
                if (midGetName) {
                    jstring name = (jstring)env->CallObjectMethod(currentClass, midGetName);
                    if (name) {
                        const char* cname = env->GetStringUTFChars(name, nullptr);
                        if (cname) {
                            file << cname << "\n";
                            env->ReleaseStringUTFChars(name, cname);
                        }
                    }
                }
            }
        }
    }
    file.close();

    jvmti->Deallocate((unsigned char*)loadedClasses);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
        HANDLE hThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Init), hModule, 0, nullptr);

        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}

