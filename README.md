# 🛡️ DoomsDay POC Checker (Minecraft)

**Proof-of-Concept** project for detecting loaded Java classes in a Minecraft process, intended to serve as a foundation for a DoomsDay cheat detector in **legit Minecraft clients**.

## ⚡ Overview

This project demonstrates how to attach to a running JVM process (Minecraft), enumerate all loaded Java classes via **JVMTI**, and dump class signatures to a text file on Desktop. This is **POC only** — not a full anti-cheat, intended for research, testing, or educational purposes.

## 🧩 Features

* Enumerates all loaded Java classes in the target JVM
* Supports attachment to legit Minecraft clients
* Outputs results to `checker.txt` on Desktop
* Lightweight DLL injection-based proof-of-concept

## 💻 Usage

1. Build the DLL in Visual Studio (x64)
2. Inject the DLL into a running Minecraft process
3. Check `checker.txt` on your Desktop — it will contain all loaded class signatures

> ⚠️ This project **does not block cheats**. It only lists loaded classes and can be used as a foundation for detection logic.

## 🔧 Technical Details

* Written in **C++** using **JVMTI** and **JNI**
* Uses `xorstr.hpp` for string obfuscation
* Retrieves `JavaVM*` via `JNI_GetCreatedJavaVMs`
* Dumps class signatures to file

## 📌 Notes

* Tested on **Minecraft 1.16.5 (Java Edition)**
* Only works with **legit clients**
* Intended for developers/educators to study cheat detection mechanisms

## 📝 License

MIT License — free to use for educational purposes
