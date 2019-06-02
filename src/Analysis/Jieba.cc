#include "Jieba.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <spawn.h>
#include <unistd.h>
#include <errno.h>

#include <cstdint>

const char *Jieba::pythonDaemonCode = "import sys\nimport jieba\njieba.setLogLevel(20)\njieba.initialize()\ntry: \n while True:\n  l = list(jieba.cut(bytes(map(int, input().split(' '))).decode('utf-8')))\n  sys.stdout.buffer.write(len(l).to_bytes(4, byteorder='little'))\n  for w in l:\n   b = w.encode('utf-8')\n   sys.stdout.buffer.write(len(b).to_bytes(4, byteorder='little') + b)\n sys.stdout.buffer.flush()\nexcept:\n pass";

#define ENSURE(posixCall) (((posixCall) == 0) || (perror(#posixCall), false))
#define ENSURE_EQ(posixCall, expectedReturnValue) (((posixCall) == expectedReturnValue) || (perror(#posixCall), false))

Jieba::Jieba() {
    int pipeStdin[2], pipeStdout[2];
    ENSURE(pipe(pipeStdin));
    ENSURE(pipe(pipeStdout));

    fdWriteStdin = pipeStdin[1];
    fdReadStdout = pipeStdout[0];

    posix_spawn_file_actions_t action;
    posix_spawn_file_actions_init(&action);
    posix_spawn_file_actions_addclose(&action, pipeStdin[1]);
    posix_spawn_file_actions_addclose(&action, pipeStdout[0]);
    posix_spawn_file_actions_adddup2(&action, pipeStdin[0], STDIN_FILENO);
    posix_spawn_file_actions_adddup2(&action, pipeStdout[1], STDOUT_FILENO);
    posix_spawn_file_actions_addclose(&action, pipeStdin[0]);
    posix_spawn_file_actions_addclose(&action, pipeStdout[1]);

    const char *args[] = { "python3", "-c", pythonDaemonCode, NULL };
    ENSURE(posix_spawnp(&pid, args[0], &action, NULL, (char *const*)args, NULL));

    ENSURE(close(pipeStdin[0]));
    ENSURE(close(pipeStdout[1]));
}

Jieba::~Jieba() {
    ENSURE(kill(pid, SIGTERM));

    int status;
    waitpid(pid, &status, 0);

    ENSURE(close(fdWriteStdin));
    ENSURE(close(fdReadStdout));
}

std::vector<std::string> Jieba::cut(const std::string &stringToCut) {
    if (stringToCut.empty()) return std::vector<std::string>();

    std::string writeBuffer;
    for (size_t i = 0; i < stringToCut.length(); i++) {
        writeBuffer.append(std::to_string((unsigned char)stringToCut[i]));
        writeBuffer.append(i == stringToCut.length() - 1 ? "\n" : " ");
    }
    ENSURE_EQ(write(fdWriteStdin, writeBuffer.data(), writeBuffer.length()), writeBuffer.length());

    uint32_t listLength;
    ENSURE_EQ(read(fdReadStdout, &listLength, sizeof(listLength)), sizeof(listLength));

    std::vector<std::string> result(listLength);
    for (std::string &word : result) {
        uint32_t wordLength;
        ENSURE_EQ(read(fdReadStdout, &wordLength, sizeof(wordLength)), sizeof(wordLength));
        word.resize(wordLength);
        ENSURE_EQ(read(fdReadStdout, word.data(), wordLength), wordLength);
    }

    return result;
}
