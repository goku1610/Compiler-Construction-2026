#include "lexer/stream_lexer.h"
#include "parser/ll1_frontend.h"
#include "core/grammar_model.h"
#include "parser/first_follow.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static const char *MAIN_MENU =
    "What do you want to do:\n"
    "\t0. Exit\n"
    "\t1. Remove Comments\n"
    "\t2. Print Lexer Token List\n"
    "\t3. Parse Code and Generate Parse Tree Visualization\n"
    "\t4. Parse Code and Print Time Taken\n";

static int hasTxtExtension(const char *fileName) {
  size_t len = strlen(fileName);
  return len >= 4 && strcmp(fileName + len - 4, ".txt") == 0;
}

static int compareNames(const void *a, const void *b) {
  const char *nameA = (const char *)a;
  const char *nameB = (const char *)b;
  return strcmp(nameA, nameB);
}

static int collectTestFiles(char files[][256], int maxFiles) {
  DIR *dir = opendir("tests");
  if (dir == NULL) {
    return 0;
  }

  int count = 0;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL && count < maxFiles) {
    if (entry->d_name[0] == '.') {
      continue;
    }
    if (!hasTxtExtension(entry->d_name)) {
      continue;
    }
    if (strncmp(entry->d_name, "lexemesandtokens_", 17) == 0) {
      continue;
    }
    strncpy(files[count], entry->d_name, 255);
    files[count][255] = '\0';
    count++;
  }
  closedir(dir);

  if (count > 1) {
    qsort(files, count, sizeof(files[0]), compareNames);
  }
  return count;
}

static int chooseInputInteractively(char *inputPath, size_t inputPathSize) {
  char files[256][256];
  int fileCount = collectTestFiles(files, 256);

  printf("No input file provided via command line.\n");
  if (fileCount > 0) {
    printf("Select a testcase from ./tests:\n");
    for (int i = 0; i < fileCount; i++) {
      printf("%d. %s\n", i + 1, files[i]);
    }
    printf("0. Enter custom input file path\n");
  } else {
    printf("No testcase files found under ./tests.\n");
    printf("Enter custom input file path:\n");
  }

  int choice = 0;
  if (fileCount > 0) {
    printf("==> ");
    if (scanf("%d", &choice) != 1) {
      return 0;
    }
  }

  if (fileCount > 0 && choice > 0 && choice <= fileCount) {
    return snprintf(inputPath, inputPathSize, "tests/%s", files[choice - 1]) <
           (int)inputPathSize;
  }

  printf("Input file path: ");
  if (scanf("%511s", inputPath) != 1) {
    return 0;
  }
  return 1;
}

static void deriveOutputName(const char *inputPath, char *outputName,
                             size_t outputNameSize) {
  const char *base = strrchr(inputPath, '/');
  if (base != NULL) {
    base++;
  } else {
    base = inputPath;
  }

  char stem[256];
  strncpy(stem, base, sizeof(stem) - 1);
  stem[sizeof(stem) - 1] = '\0';

  char *dot = strrchr(stem, '.');
  if (dot != NULL) {
    *dot = '\0';
  }
  if (stem[0] == '\0') {
    strncpy(stem, "output", sizeof(stem) - 1);
    stem[sizeof(stem) - 1] = '\0';
  }

  if (snprintf(outputName, outputNameSize, "%s_out.txt", stem) >=
      (int)outputNameSize) {
    strncpy(outputName, "output.txt", outputNameSize - 1);
    outputName[outputNameSize - 1] = '\0';
  }
}

static int ensureOutputDir(void) {
  if (mkdir("outputs", 0775) == 0) {
    return 1;
  }
  return errno == EEXIST;
}

static int removePathRecursive(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return 0;
  }

  if (S_ISDIR(st.st_mode)) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
      return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }

      char childPath[1024];
      if (snprintf(childPath, sizeof(childPath), "%s/%s", path, entry->d_name) >=
          (int)sizeof(childPath)) {
        closedir(dir);
        return 0;
      }
      if (!removePathRecursive(childPath)) {
        closedir(dir);
        return 0;
      }
    }

    closedir(dir);
    return rmdir(path) == 0;
  }

  return remove(path) == 0;
}

static int clearDirectoryContents(const char *dirPath) {
  DIR *dir = opendir(dirPath);
  if (dir == NULL) {
    return 0;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    if (strcmp(entry->d_name, ".gitkeep") == 0) {
      continue;
    }

    char path[1024];
    if (snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name) >=
        (int)sizeof(path)) {
      closedir(dir);
      return 0;
    }
    if (!removePathRecursive(path)) {
      closedir(dir);
      return 0;
    }
  }

  closedir(dir);
  FILE *keep = fopen("outputs/.gitkeep", "w");
  if (keep != NULL) {
    fclose(keep);
  }
  return 1;
}

static int buildOutputPath(const char *requestedName, char *resolvedPath,
                           size_t resolvedSize) {
  const char *baseName = strrchr(requestedName, '/');
  if (baseName != NULL) {
    baseName++;
  } else {
    baseName = requestedName;
  }

  if (baseName[0] == '\0') {
    baseName = "output.txt";
  }

  return snprintf(resolvedPath, resolvedSize, "outputs/%s", baseName) <
         (int)resolvedSize;
}

int main(int argc, char *argv[]) {
  if (argc > 3) {
    printf("Usage: %s [inputFilePath [outputFileName]]\n", argv[0]);
    return 1;
  }

  if (!ensureOutputDir()) {
    printf("Unable to create/access outputs directory.\n");
    return 1;
  }
  if (!clearDirectoryContents("outputs")) {
    printf("Unable to clear outputs directory.\n");
    return 1;
  }

  char inputPath[512];
  char requestedOutputName[256];

  if (argc >= 2) {
    strncpy(inputPath, argv[1], sizeof(inputPath) - 1);
    inputPath[sizeof(inputPath) - 1] = '\0';
  } else if (!chooseInputInteractively(inputPath, sizeof(inputPath))) {
    printf("Unable to determine input file path.\n");
    return 1;
  }

  if (argc >= 3) {
    strncpy(requestedOutputName, argv[2], sizeof(requestedOutputName) - 1);
    requestedOutputName[sizeof(requestedOutputName) - 1] = '\0';
  } else {
    deriveOutputName(inputPath, requestedOutputName, sizeof(requestedOutputName));
    printf("Using output file name: %s\n", requestedOutputName);
  }

  char outputPath[512];
  if (!buildOutputPath(requestedOutputName, outputPath, sizeof(outputPath))) {
    printf("Output file name is too long.\n");
    return 1;
  }

  grammar G = initializeGrammar();
  FirstFollow ff = computeFirstFollowSet(G);
  table T;
  createParseTable(ff, &T);

  while (1) {
    int choice = -1;
    printf("%s", MAIN_MENU);
    printf("==> ");
    if (scanf("%d", &choice) != 1) {
      printf("Invalid input. Exiting.\n");
      return 1;
    }

    switch (choice) {
    case 0:
      return 0;

    case 1: {
      removeComments(inputPath, outputPath);
      printf("Comment-free source written to %s\n\n", outputPath);
      break;
    }

    case 2: {
      FILE *fp = fopen(inputPath, "r");
      if (fp == NULL) {
        printf("Unable to open input file: %s\n\n", inputPath);
        break;
      }
      printf("==== Lexer Tokens ====\n");
      getStream(fp);
      fclose(fp);
      printf("Total lexical errors: %d\n", getLexicalErrorCount());
      printf("==== Lexer Tokens Complete ====\n\n");
      break;
    }

    case 3: {
      FILE *inputFile = fopen(inputPath, "r");
      FILE *outputFile = fopen(outputPath, "w");
      if (inputFile == NULL || outputFile == NULL) {
        printf("Unable to open input/output file(s).\n\n");
        if (inputFile != NULL)
          fclose(inputFile);
        if (outputFile != NULL)
          fclose(outputFile);
        break;
      }

      printf("==== Parsing... ====\n");
      parseTree *root = parseInputSourceCode(T, ff, G, inputFile);
      printParseTree(root, outputFile);

      char dotPath[600];
      char svgPath[600];
      snprintf(dotPath, sizeof(dotPath), "%s.dot", outputPath);
      snprintf(svgPath, sizeof(svgPath), "%s.svg", outputPath);

      FILE *dotFile = fopen(dotPath, "w");
      if (dotFile != NULL) {
        writeParseTreeDot(root, dotFile);
        fclose(dotFile);
      }

      if (dotFile != NULL) {
        char cmd[1400];
        snprintf(cmd, sizeof(cmd),
                 "command -v dot >/dev/null 2>&1 && dot -Tsvg \"%s\" -o \"%s\"",
                 dotPath, svgPath);
        if (system(cmd) == 0) {
          printf("Parse tree visual files: %s and %s\n", dotPath, svgPath);
        } else {
          printf("Parse tree visual file: %s\n", dotPath);
        }
      }

      fclose(inputFile);
      fclose(outputFile);
      printf("Parse tree written to %s\n\n", outputPath);
      break;
    }

    case 4: {
      FILE *inputFile = fopen(inputPath, "r");
      if (inputFile == NULL) {
        printf("Unable to open input file: %s\n\n", inputPath);
        break;
      }

      clock_t start_time = clock();
      parseTree *root = parseInputSourceCode(T, ff, G, inputFile);
      (void)root;
      clock_t end_time = clock();

      double ticks = (double)(end_time - start_time);
      printf("Parsing complete.\n");
      printf("CPU ticks: %.0f\n", ticks);
      printf("CPU time (seconds): %.6f\n\n", ticks / CLOCKS_PER_SEC);
      fclose(inputFile);
      break;
    }

    default:
      printf("Invalid choice. Try again.\n\n");
      break;
    }
  }
}
