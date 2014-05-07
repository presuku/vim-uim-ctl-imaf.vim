/* for debug */

#include <unistd.h>
#include <poll.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <uim/uim.h>
#include <uim/uim-helper.h>

#ifndef INFTIM
# define INFTIM -1
#endif

#define RETRY_EINTR(ret, funcall)     \
  do {                                \
    ret = funcall;                    \
  } while (ret == -1 && errno == EINTR)

int
main(int argc, char **argv)
{
  int uim_fd;
  int fds[2][2];
  char buf[2][BUFSIZ];
  ssize_t len[2];
  ssize_t n;
  struct pollfd pfd[2];
  int i;

  uim_fd = uim_helper_init_client_fd(NULL);
  if (uim_fd < 0) {
    fprintf(stderr, "error: uim_helper_init_client_fd()\n");
    return 1;
  }

  fds[0][0] = STDIN_FILENO;
  fds[0][1] = uim_fd;
  fds[1][0] = uim_fd;
  fds[1][1] = STDOUT_FILENO;

  pfd[0].fd = STDIN_FILENO;
  pfd[0].events = POLLIN;
  pfd[1].fd = uim_fd;
  pfd[1].events = POLLIN;

  for (;;) {
    RETRY_EINTR(n, poll(pfd, 2, INFTIM));
    if (n == -1)
      break;
    for (i = 0; i < 2; ++i) {
      if (pfd[i].revents & POLLIN) {
        RETRY_EINTR(n, read(pfd[i].fd, buf[i], sizeof(buf[i])));
        if (n == 0 || n == -1)
          break;
        len[i] = n;
        pfd[i].fd = fds[i][1];
        pfd[i].events = POLLOUT;
      } else if (pfd[i].revents & POLLOUT) {
        RETRY_EINTR(n, write(pfd[i].fd, buf[i], len[i]));
        if (n == 0 || n == -1)
          break;
        len[i] -= n;
        if (len[i] == 0) {
          pfd[i].fd = fds[i][0];
          pfd[i].events = POLLIN;
        } else {
          memmove(buf[i], buf[i] + n, len[i]);
        }
      } else if (pfd[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        goto endloop;
      }
    }
  }
endloop:

  uim_helper_close_client_fd(uim_fd);

  return 0;
}

