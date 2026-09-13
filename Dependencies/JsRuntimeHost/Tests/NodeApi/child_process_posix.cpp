#include "child_process.h"

#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#if defined(__ANDROID__)
#include <android/api-level.h>
#endif

#if !defined(__ANDROID__)
#include <spawn.h>
#elif (__ANDROID_API__ >= 29)
#include <spawn.h>
#endif

#ifndef VerifyElseExit
#define VerifyElseExit(condition)                                              \
  do {                                                                         \
    if (!(condition)) {                                                        \
      ExitOnError(#condition, nullptr);                                        \
    }                                                                          \
  } while (false)
#endif

#ifndef VerifyElseExitWithCleanup
#define VerifyElseExitWithCleanup(condition, actions_ptr)                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      ExitOnError(#condition, actions_ptr);                                    \
    }                                                                          \
  } while (false)
#endif

#if defined(__ANDROID__) && (__ANDROID_API__ < 29)

namespace node_api_tests {

ProcessResult SpawnSync(std::string_view /*command*/,
                        std::vector<std::string> /*args*/) {
  ProcessResult result{};
  result.status = -1;
  result.std_error = "child_process.spawnSync is not supported on this platform.";
  result.std_output.clear();
  return result;
}

}  // namespace node_api_tests

#else

extern char** environ;

namespace node_api_tests {

namespace {

void DrainPipes(int out_fd, int err_fd, std::string* out, std::string* err);
void ExitOnError(const char* message, posix_spawn_file_actions_t* actions);

}  // namespace

ProcessResult SpawnSync(std::string_view command,
                        std::vector<std::string> args) {
  ProcessResult result{};

  // These int arrays each comprise two file descriptors: { readEnd, writeEnd }.
  int stdout_pipe[2], stderr_pipe[2];
  VerifyElseExit(pipe(stdout_pipe) == 0);
  VerifyElseExit(pipe(stderr_pipe) == 0);

  posix_spawn_file_actions_t actions;
  VerifyElseExit(posix_spawn_file_actions_init(&actions) == 0);

  VerifyElseExitWithCleanup(posix_spawn_file_actions_adddup2(
                                &actions, stdout_pipe[1], STDOUT_FILENO) == 0,
                            &actions);
  VerifyElseExitWithCleanup(posix_spawn_file_actions_adddup2(
                                &actions, stderr_pipe[1], STDERR_FILENO) == 0,
                            &actions);

  VerifyElseExitWithCleanup(
      posix_spawn_file_actions_addclose(&actions, stdout_pipe[0]) == 0,
      &actions);
  VerifyElseExitWithCleanup(
      posix_spawn_file_actions_addclose(&actions, stderr_pipe[0]) == 0,
      &actions);

  std::vector<char*> argv;
  argv.push_back(strdup(std::string(command).c_str()));
  for (const std::string& arg : args) {
    argv.push_back(strdup(arg.c_str()));
  }
  argv.push_back(nullptr);

  pid_t pid;
  VerifyElseExitWithCleanup(
      posix_spawnp(&pid, argv[0], &actions, nullptr, argv.data(), environ) == 0,
      &actions);

  posix_spawn_file_actions_destroy(&actions);

  // Close the write ends of the pipes, then drain both read ends concurrently while the child
  // runs. Waiting first deadlocks as soon as the child writes more than a pipe buffer (16 KiB on
  // macOS, 64 KiB on Linux): it blocks on write while the parent blocks in waitpid.
  close(stdout_pipe[1]);
  close(stderr_pipe[1]);
  DrainPipes(stdout_pipe[0], stderr_pipe[0], &result.std_output, &result.std_error);

  int wait_status;
  pid_t waited_pid;
  do {
    waited_pid = waitpid(pid, &wait_status, 0);
  } while (waited_pid == -1 && errno == EINTR);

  VerifyElseExit(waited_pid == pid);

  if (WIFEXITED(wait_status)) {
    result.status = WEXITSTATUS(wait_status);
  } else if (WIFSIGNALED(wait_status)) {
    result.status = 128 + WTERMSIG(wait_status);
  } else {
    result.status = 1;
  }
  // Close the read ends of the pipes.
  close(stdout_pipe[0]);
  close(stderr_pipe[0]);

  for (char* arg : argv) {
    free(arg);
  }

  return result;
}

namespace {

// Reads out_fd and err_fd to EOF, servicing whichever has data, so neither side can fill its
// pipe buffer and stall the child while the other is idle.
void DrainPipes(int out_fd, int err_fd, std::string* out, std::string* err) {
  struct Sink {
    int fd;
    std::string* target;
    bool open;
  };
  Sink sinks[2] = {{out_fd, out, true}, {err_fd, err, true}};
  char buffer[4096];
  while (sinks[0].open || sinks[1].open) {
    pollfd fds[2];
    Sink* owners[2];
    nfds_t count = 0;
    for (Sink& sink : sinks) {
      if (sink.open) {
        fds[count] = pollfd{sink.fd, POLLIN, 0};
        owners[count] = &sink;
        ++count;
      }
    }
    if (poll(fds, count, -1) < 0) {
      if (errno == EINTR) continue;
      ExitOnError("poll", nullptr);
    }
    for (nfds_t i = 0; i < count; ++i) {
      if (fds[i].revents == 0) continue;  // POLLIN, POLLHUP and POLLERR all mean "read now"
      ssize_t bytesRead = read(fds[i].fd, buffer, sizeof(buffer));
      if (bytesRead > 0) {
        owners[i]->target->append(buffer, static_cast<size_t>(bytesRead));
      } else if (bytesRead == 0) {
        owners[i]->open = false;
      } else if (errno != EINTR && errno != EAGAIN) {
        ExitOnError("read", nullptr);
      }
    }
  }
}

// Format a readable error message, print it to console, and exit from the
// application.
void ExitOnError(const char* message, posix_spawn_file_actions_t* actions) {
  int err = errno;
  const char* err_msg = strerror(err);

  fprintf(stderr, "%s failed with error %d: %s\n", message, err, err_msg);

  if (actions != nullptr) {
    posix_spawn_file_actions_destroy(actions);
  }

  exit(1);
}

}  // namespace

}  // namespace node_api_tests

#endif  // __ANDROID__
