
## ci.yml

```yml
name: CI

on:
  push:
    branches: [main]
    tags: ['v*']
  pull_request:
    branches: [main]

jobs:
  build-gcc:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Install deps
        run: sudo apt-get update && sudo apt-get install -y build-essential cmake

      - name: Configure (GCC)
        run: cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

      - name: Build (GCC)
        run: cmake --build build --parallel

  build-clang:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Install deps
        run: sudo apt-get update && sudo apt-get install -y build-essential cmake clang

      - name: Configure (Clang)
        run: cmake -S . -B build-clang -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

      - name: Build (Clang)
        run: cmake --build build-clang --parallel

  release:
    if: startsWith(github.ref, 'refs/tags/v')
    needs: [build-gcc, build-clang]
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Build release
        run: cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel

      - name: Create GitHub Release
        uses: softprops/action-gh-release@v2
        with:
          name: Release ${{ github.ref_name }}
          files: bin/webserver    # 改为此项目的二进制名

```

## clang-format

```.clang-format
# ─── 基础 ─────────────────────────────────────────────────────
BasedOnStyle: Google
Standard: Latest

# ─── 缩进 ─────────────────────────────────────────────────────
IndentWidth: 2
TabWidth: 2
UseTab: Never

# ─── 行宽 ─────────────────────────────────────────────────────
ColumnLimit: 100

# ─── 函数 / 控制流 ─────────────────────────────────────────────
AllowShortFunctionsOnASingleLine: Inline
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
BreakBeforeBraces: Attach

# ─── 指针 / 引用 ──────────────────────────────────────────────
PointerAlignment: Left
ReferenceAlignment: Left

# ─── Include ──────────────────────────────────────────────────
SortIncludes: true
IncludeBlocks: Regroup
IncludeCategories:
  - Regex: '^<.*>'
    Priority: 1
  - Regex: '^".*"'
    Priority: 2

# ─── 空白 / 杂项 ──────────────────────────────────────────────
MaxEmptyLinesToKeep: 1
FixNamespaceComments: true

```

## clang-tidy

```.clang-tidy
# ─── 检查项 ───────────────────────────────────────────────────
Checks: >
  -*,
  bugprone-*,
  -bugprone-easily-swappable-parameters,
  modernize-*,
  -modernize-use-trailing-return-type,
  -modernize-avoid-c-arrays,
  performance-*,
  -performance-avoid-endl,
  cppcoreguidelines-pro-type-*,
  cppcoreguidelines-slicing,
  cppcoreguidelines-no-malloc,
  cppcoreguidelines-prefer-member-initializer,
  read*-container-size-empty,
  read*-implicit-bool-conversion,
  read*-redundant-*,
  misc-*,
  -misc-include-cleaner,
  -misc-const-correctness,
  -misc-no-recursion,
  -misc-non-private-member-variables-in-classes,
  google-explicit-constructor,
  google-readability-casting,

# ─── 检查选项 ─────────────────────────────────────────────────
CheckOptions:
  - key: bugprone-argument-comment.StrictMode
    value: true
  - key: modernize-use-nullptr.NullMacros
    value: "NULL"

```

## vscode-user-setting

```json
{
    // ─── 代码运行器 ───────────────────────────────────────────────
    "code-runner.executorMap": {
        "javascript": "node",
        "java": "cd $dir && javac $fileName && java $fileNameWithoutExt",
        "c": "cd $dir && gcc $fileName -o $fileNameWithoutExt && $dir$fileNameWithoutExt",
        "zig": "zig run",
        "cpp": "cd $dir && g++ -std=c++17 -Wall -O2 $fileName -o $fileNameWithoutExt && $dir$fileNameWithoutExt",
        "objective-c": "cd $dir && gcc -framework Cocoa $fileName -o $fileNameWithoutExt && $dir$fileNameWithoutExt",
        "php": "php",
        "python": "python -u",
        "perl": "perl",
        "perl6": "perl6",
        "ruby": "ruby",
        "go": "go run",
        "lua": "lua",
        "groovy": "groovy",
        "powershell": "powershell -ExecutionPolicy ByPass -File",
        "bat": "cmd /c",
        "shellscript": "bash",
        "fsharp": "fsi",
        "csharp": "scriptcs",
        "vbscript": "cscript //Nologo",
        "typescript": "ts-node",
        "coffeescript": "coffee",
        "scala": "scala",
        "swift": "swift",
        "julia": "julia",
        "crystal": "crystal",
        "ocaml": "ocaml",
        "r": "Rscript",
        "applescript": "osascript",
        "clojure": "lein exec",
        "haxe": "haxe --cwd $dirWithoutTrailingSlash --run $fileNameWithoutExt",
        "rust": "cd $dir && rustc $fileName && $dir$fileNameWithoutExt",
        "racket": "racket",
        "scheme": "csi -script",
        "ahk": "autohotkey",
        "autoit": "autoit3",
        "dart": "dart",
        "pascal": "cd $dir && fpc $fileName && $dir$fileNameWithoutExt",
        "d": "cd $dir && dmd $fileName && $dir$fileNameWithoutExt",
        "haskell": "runghc",
        "nim": "nim compile --verbosity:0 --hints:off --run",
        "lisp": "sbcl --script",
        "kit": "kitc --run",
        "v": "v run",
        "sass": "sass --style expanded",
        "scss": "scss --style expanded",
        "less": "cd $dir && lessc $fileName $fileNameWithoutExt.css",
        "FortranFreeForm": "cd $dir && gfortran $fileName -o $fileNameWithoutExt && $dir$fileNameWithoutExt",
        "fortran-modern": "cd $dir && gfortran $fileName -o $fileNameWithoutExt && $dir$fileNameWithoutExt",
        "fortran_fixed-form": "cd $dir && gfortran $fileName -o $fileNameWithoutExt && $dir$fileNameWithoutExt",
        "fortran": "cd $dir && gfortran $fileName -o $fileNameWithoutExt && $dir$fileNameWithoutExt",
        "sml": "cd $dir && sml $fileName",
        "mojo": "mojo run",
        "erlang": "escript",
        "spwn": "spwn build",
        "pkl": "cd $dir && pkl eval -f yaml $fileName -o $fileNameWithoutExt.yaml",
        "gleam": "gleam run -m $fileNameWithoutExt"
    },
    "code-runner.runInTerminal": true,
    // ─── 竞赛插件 (cph) ────────────────────────────────────────────
    "cph.general.defaultLanguage": "cpp",
    "cph.general.hideStderrorWhenCompiledOK": false,
    // ─── 字体（JetBrains Mono 需手动安装）──────────────────────────
    "editor.fontFamily": "JetBrains Mono, Consolas, monospace",
    "editor.fontSize": 14,
    "editor.fontLigatures": true,
    "editor.lineHeight": 1.6,
    "window.zoomLevel": 0,

    // ─── 光标 ─────────────────────────────────────────────────────
    "editor.cursorStyle": "line",
    "editor.cursorBlinking": "smooth",
    "editor.cursorSmoothCaretAnimation": "on",
    // ─── 编辑器核心 ───────────────────────────────────────────────
    "editor.tabCompletion": "on",
    "editor.lineNumbers": "on",                           // 未来切 vim 模式改为 "relative"
    "editor.accessibilitySupport": "auto",                 // 关闭强制屏幕阅读器，避免拖慢性能
    "editor.rulers": [100, 120],                           // 100 = .clang-format 正式边界
    "editor.renderWhitespace": "trailing",
    "editor.bracketPairColorization.enabled": true,
    "editor.guides.bracketPairs": true,
    "editor.minimap.enabled": false,
    "editor.minimap.renderCharacters": false,
    "editor.minimap.maxColumn": 80,
    "editor.stickyScroll.enabled": true,                   // 函数/类名固定在顶部
    "editor.wordWrap": "off",
    "editor.suggest.preview": true,
    "files.autoSave": "onFocusChange",                     // 比 afterDelay 更可控
    // ─── 补全 & 建议 ──────────────────────────────────────────────
    "editor.quickSuggestions": {
        "other": true,
        "comments": false,
        "strings": true
    },
    "editor.suggestOnTriggerCharacters": true,
    "editor.acceptSuggestionOnCommitCharacter": false,     // 防止打 . / ; 时意外触发补全 accept
    "editor.suggest.snippetsPreventQuickSuggestions": false,
    "editor.suggestSelection": "recentlyUsed",
    "editor.parameterHints.enabled": true,
    "editor.parameterHints.delay": 100,
    // ─── C / C++ ─────────────────────────────────────────────────
    "C_Cpp.autocomplete": "default",
    "C_Cpp.intelliSenseEngine": "disabled",
    "C_Cpp.errorSquiggles": "enabled",
    "C_Cpp.autoAddFileAssociations": true,
    "[cpp]": {
        "editor.defaultFormatter": "xaver.clang-format",
        "editor.formatOnSave": true,
        "editor.wordBasedSuggestions": "matchingDocuments",
        "editor.suggestOnTriggerCharacters": true,
        "editor.quickSuggestions": {
            "other": true,
            "comments": false,
            "strings": true
        }
    },
    "[c]": {
        "editor.defaultFormatter": "xaver.clang-format",
        "editor.formatOnSave": true,
        "editor.wordBasedSuggestions": "matchingDocuments",
        "editor.suggestOnTriggerCharacters": true,
        "editor.quickSuggestions": {
            "other": true,
            "comments": false,
            "strings": true
        }
    },
    // ─── 终端 ─────────────────────────────────────────────────────
    "terminal.integrated.fontSize": 13,
    "terminal.integrated.fontFamily": "JetBrains Mono, Consolas, monospace",
    "terminal.integrated.cursorStyle": "line",
    "terminal.integrated.scrollback": 5000,

    // ─── 文件保存规范 ─────────────────────────────────────────────
    "files.trimTrailingWhitespace": true,
    "files.insertFinalNewline": true,
    "files.trimFinalNewlines": true,

    // ─── 搜索 ─────────────────────────────────────────────────────
    "search.exclude": {
        "**/build": true,
        "**/node_modules": true,
        "**/.git": true
    },
    // ─── Git ──────────────────────────────────────────────────────
    "git.enableSmartCommit": true,
    "git.autofetch": true,
    "git.openRepositoryInParentFolders": "never",
    "git.blame.statusBarItem.enabled": false,
    "diffEditor.ignoreTrimWhitespace": true,               // 忽略空格差异，避免 diff 噪音
    // ─── GitLens ──────────────────────────────────────────────────
    "gitlens.ai.model": "vscode",
    "gitlens.ai.vscode.model": "copilot:gpt-4.1",
    "gitlens.gitCommands.skipConfirmations": [
        "fetch:command",
        "stash-push:command",
        "switch:command"
    ],
    "gitlens.codeLens.enabled": false,
    // ─── GitHub Copilot（训练手写阶段关闭 inline，保留 Chat）───────
    "github.copilot.enable": {
        "*": false,
        "plaintext": false,
        "markdown": false,
        "scminput": false,
        "cpp": false
    },
    "github.copilot.nextEditSuggestions.enabled": false,
    "github.copilot.nextEditSuggestions.eagerness": "auto",
    // ─── Remote SSH ───────────────────────────────────────────────
    "remote.SSH.remotePlatform": {
        "192.168.43.100": "linux",
        "ubuntu-vm": "linux"
    },
    // ─── 调试 & 文件管理 ───────────────────────────────────────────
    "debug.onTaskErrors": "debugAnyway",
    "explorer.confirmDelete": true,
    "explorer.confirmDragAndDrop": false,
    "security.workspace.trust.untrustedFiles": "open",
    // ─── 工作区体验 ───────────────────────────────────────────────
    "window.menuBarVisibility": "compact",
    "workbench.startupEditor": "none",
    "workbench.editor.highlightModifiedTabs": true,
    "workbench.editor.tabSizing": "shrink",
    "breadcrumbs.enabled": true,
    "workbench.iconTheme": "material-icon-theme",
    "claudeCode.preferredLocation": "panel",
    "chat.viewSessions.orientation": "stacked",
    "workbench.secondarySideBar.defaultVisibility": "visible"
}

```