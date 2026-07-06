# Guide de Publication de Release - BankVision

## Étapes pour créer une release avec votre executable

### 1. Préparez votre fichier (sur votre machine locale)

Compressez votre `BankVision.exe` avec ses dépendances :

```bash
# PowerShell Windows
Compress-Archive -Path "C:\chemin\vers\BankVision.exe" -DestinationPath "BankVision-v1.0.0.zip"

# Ou utilisez 7-Zip / WinRAR graphiquement
```

### 2. Créez une release sur GitHub

1. Visitez : https://github.com/Adama-Faye-tech/BankVision
2. Cliquez sur l'onglet **"Releases"** (à droite)
3. Cliquez sur **"Create a new release"**

### 3. Remplissez les informations

- **Tag version** : `v1.0.0`
- **Release title** : `Version 1.0.0 - Initial Release`
- **Description** : 
  ```
  Premier release de BankVision
  - Fonctionnalités principales implémentées
  - Prêt pour utilisation
  ```

### 4. Téléchargez votre fichier

- Glissez-déposez votre `BankVision-v1.0.0.zip` dans la zone **"Attach binaries by dropping them here"**

### 5. Publiez

- Cliquez sur **"Publish release"**

---

**Voilà ! Votre release est en ligne et accessible pour les utilisateurs.**
