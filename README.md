# Snake Raylib Android

Jogo Snake em C puro com Raylib, controles touch e suporte a teclado.

## Estrutura

```text
.
├── main.c
├── CMakeLists.txt
├── AndroidManifest.xml
└── .github/workflows/build.yml
```

## Build na nuvem

1. Crie um repositório no GitHub.
2. Envie estes arquivos mantendo a pasta `.github/workflows`.
3. Abra **Actions**.
4. Execute **Build Android APK** ou faça um push.
5. Baixe o artefato **Snake-Raylib-Android-arm64-v8a**.

O workflow usa o tarball da Raylib em vez de `git clone`/`FetchContent` por Git, evitando falhas `exit code 128` durante o download da dependência. Ele também usa versões das Actions compatíveis com Node.js 24.

O APK é assinado com uma chave debug criada no próprio runner, adequada para testes e instalação local.
