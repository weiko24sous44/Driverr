# VirtualDriver - Windows KMDF Kernel Driver

Un driver kernel virtuel Windows KMDF (Kernel-Mode Driver Framework) écrit en C++ pour l'apprentissage du développement de drivers noyau Windows.

## 📋 Description

Ce projet est un driver kernel virtuel qui démontre les concepts fondamentaux du développement de drivers Windows avec KMDF :
- Initialisation du driver
- Gestion des périphériques (PnP)
- Gestion de l'alimentation (Power Management)
- Communication I/O (Read/Write/IOCTL)
- Contexte de périphérique

## 🏗️ Structure du projet

```
VirtualDriver/
├── driver.cpp          # Point d'entrée du driver et callbacks principaux
├── device.cpp          # Gestion du contexte du périphérique
├── queue.cpp           # Gestion des files d'attente I/O
├── driver.h            # En-têtes et définitions
├── VirtualDriver.inf   # Fichier d'installation
├── VirtualDriver.vcxproj # Fichier de projet Visual Studio
└── README.md           # Ce fichier
```

## 🔧 Prérequis

### Outils nécessaires
- **Windows Driver Kit (WDK)** 10 ou supérieur
- **Visual Studio** 2019 ou supérieur avec la charge de travail "Développement de pilotes Windows"
- **Windows SDK** correspondant à votre version de Windows

### Système d'exploitation
- Windows 10 ou Windows 11 (x64)
- Mode test activé ou signature de driver désactivée pour le développement

## 📦 Compilation

### Avec Visual Studio

1. Ouvrez `VirtualDriver.vcxproj` dans Visual Studio
2. Sélectionnez la configuration `Debug` ou `Release`
3. Sélectionnez la plateforme `x64`
4. Build → Build Solution (Ctrl+Shift+B)

Le driver compilé sera dans : `x64\Debug\VirtualDriver.sys` ou `x64\Release\VirtualDriver.sys`

### Avec MSBuild en ligne de commande

```cmd
msbuild VirtualDriver.vcxproj /p:Configuration=Debug /p:Platform=x64
```

## 🚀 Installation

### Activer le mode test (Test Mode)

Pour charger un driver non signé en développement :

```cmd
bcdedit /set testsigning on
bcdedit /set nointegritychecks on
```

**Redémarrez votre ordinateur après avoir exécuté ces commandes.**

### Installation manuelle

```cmd
# Copier le driver
copy x64\Debug\VirtualDriver.sys C:\Windows\System32\drivers\

# Créer le service
sc create VirtualDriver type= kernel binPath= C:\Windows\System32\drivers\VirtualDriver.sys

# Démarrer le driver
sc start VirtualDriver
```

### Désinstallation

```cmd
# Arrêter le driver
sc stop VirtualDriver

# Supprimer le service
sc delete VirtualDriver

# Supprimer le fichier
del C:\Windows\System32\drivers\VirtualDriver.sys
```

## 📡 IOCTL Supportés

Le driver supporte les IOCTL suivants (définis dans `driver.h`) :

| IOCTL | Code | Description |
|-------|------|-------------|
| `IOCTL_VIRTUALDRIVER_GET_INFO` | 0x800 | Récupérer les informations du périphérique |
| `IOCTL_VIRTUALDRIVER_SET_INFO` | 0x801 | Définir les informations du périphérique |
| `IOCTL_VIRTUALDRIVER_RESET_STATS` | 0x802 | Réinitialiser les statistiques |
| `IOCTL_VIRTUALDRIVER_PING` | 0x803 | Test de ping (retourne 0xDEADBEEF) |

## 🧪 Test du driver

### Application de test (C++)

Voici un exemple simple pour tester le driver :

```cpp
#include <windows.h>
#include <iostream>

// IOCTL definitions (doivent correspondre à driver.h)
#define IOCTL_VIRTUALDRIVER_GET_INFO \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIRTUALDRIVER_PING \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _DEVICE_INFO {
    ULONG DeviceId;
    ULONG ReadCount;
    ULONG WriteCount;
    CHAR DeviceName[32];
} DEVICE_INFO;

int main() {
    HANDLE hDevice = CreateFile(
        L"\\\\.\\VirtualDriver",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hDevice == INVALID_HANDLE_VALUE) {
        std::cerr << "Erreur: Impossible d'ouvrir le device" << std::endl;
        return 1;
    }

    // Test IOCTL PING
    ULONG pingResult = 0;
    DWORD bytesReturned;
    BOOL result = DeviceIoControl(
        hDevice,
        IOCTL_VIRTUALDRIVER_PING,
        NULL, 0,
        &pingResult, sizeof(pingResult),
        &bytesReturned,
        NULL
    );

    if (result) {
        std::cout << "PING result: 0x" << std::hex << pingResult << std::endl;
    }

    // Test GET_INFO
    DEVICE_INFO info;
    result = DeviceIoControl(
        hDevice,
        IOCTL_VIRTUALDRIVER_GET_INFO,
        NULL, 0,
        &info, sizeof(info),
        &bytesReturned,
        NULL
    );

    if (result) {
        std::cout << "Device ID: " << info.DeviceId << std::endl;
        std::cout << "Read Count: " << info.ReadCount << std::endl;
        std::cout << "Write Count: " << info.WriteCount << std::endl;
        std::cout << "Device Name: " << info.DeviceName << std::endl;
    }

    CloseHandle(hDevice);
    return 0;
}
```

## 🔍 Débogage

### DebugView

Utilisez DebugView pour voir les messages `KdPrint()` du driver :
1. Téléchargez DebugView de Sysinternals
2. Lancez-le en tant qu'administrateur
3. Activez "Capture Kernel"
4. Vous verrez les messages de debug du driver

### WinDbg

Pour un débogage kernel complet :
1. Configurez WinDbg pour le débogage kernel
2. Définissez des points d'arrêt dans votre code
3. Utilisez les commandes WinDbg pour analyser le driver

## ⚠️ Avertissements

- Ce driver est destiné **uniquement à l'apprentissage**
- Ne l'utilisez pas en environnement de production
- Les erreurs dans les drivers kernel peuvent causer des BSOD (Blue Screen of Death)
- Testez toujours dans une machine virtuelle
- Sauvegardez vos données avant de tester

## 📚 Ressources d'apprentissage

- [Windows Driver Kit Documentation](https://docs.microsoft.com/en-us/windows-hardware/drivers/)
- [KMDF Overview](https://docs.microsoft.com/en-us/windows-hardware/drivers/wdf/kernel-mode-driver-framework-overview)
- [Windows Driver Samples](https://github.com/microsoft/Windows-driver-samples)

## 📝 License

Ce projet est fourni à des fins éducatives uniquement.

## 👨‍💻 Auteur

Projet créé pour l'apprentissage du développement de drivers kernel Windows.
