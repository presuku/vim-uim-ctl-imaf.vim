#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/un.h>
#include <dlfcn.h>
#include <poll.h>
#include <pthread.h>
#include <unistd.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <uim/uim.h>
#include <uim/uim-helper.h>

#define ACTION_HIRAGANA "action_mozc_hiragana"
#define ACTION_DIRECT   "action_mozc_direct"

#ifndef INFTIM
# define INFTIM -1
#endif

/* #define DEBUG */
#ifdef DEBUG
#define DBG_FPINTF(x, y, z) fprintf(x, y, z)
#else
#define DBG_FPINTF(x, y, z)
#endif

#define RETRY_EINTR(ret, funcall)     \
  do {                                \
    ret = funcall;                    \
  } while (ret == -1 && errno == EINTR)

#define STRLCPY(dst, src, n)          \
  do {                                \
    strncpy(dst, src, n);             \
    dst[n - 1] = '\0';                \
  } while (0)

static void *self;
static int uim_fd = -1;
static pthread_t twatch;
static long long im_on = -1;

static void * uimwatch(void *nouse);
static inline ssize_t xwrite(int fd, const char *buf, size_t size);

int load(const char* dsopath)
{

  int ret;
  struct sockaddr_un server;
  char path[MAXPATHLEN];

  DBG_FPINTF(stderr, "%s, start\n", __func__);
  if (uim_fd >= 0) {
    /* already init-ed. */
    goto end;
  }

  self = dlopen(dsopath, RTLD_LAZY);
  if (self == NULL) {
    /* return strerror(errno); */
    ret = errno;
    goto error;
  }

  /*
   * connect to uim-helper-server.
   * see uim/uim-helper-client.c.
   */
  if (!uim_helper_get_pathname(path, sizeof(path))){
    /* return "error uim_helper_get_pathname()"; */
    ret = -1;
    goto error_dlclose;
  }

  memset(&server, 0, sizeof(server));
  server.sun_family = PF_UNIX;
  STRLCPY(server.sun_path, path, sizeof(server.sun_path));

  uim_fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (uim_fd < 0) {
    /* return "error socket()"; */
    ret = -2;
    goto error_dlclose;
  }

  if (connect(uim_fd, (struct sockaddr *)&server, sizeof(server)) != 0) {
    /* return "cannot connect to uim-helper-server"; */
    ret = -3;
    goto error_shutdown_close;
  }

  if (pthread_create(&twatch, NULL, uimwatch, (void *)NULL) != 0) {
    /* return "error pthread_create()"; */
    ret = -4;
    goto error_shutdown_close;
  }

  DBG_FPINTF(stderr, "%s, end\n", __func__);

end:;
  return 0;
error_shutdown_close:;
  shutdown(uim_fd, SHUT_RDWR);
  close(uim_fd);
error_dlclose:;
  dlclose(self);
error:;
  return ret;
}

int unload(int nouse)
{
  (void)nouse;

  DBG_FPINTF(stderr, "%s, start\n", __func__);
  if (uim_fd < 0) {
    DBG_FPINTF(stderr, "!!!!!!!!!!!!! %s, fd-error\n", __func__);
    return 1;
  }

  shutdown(uim_fd, SHUT_RDWR);
  close(uim_fd);
  pthread_cancel(twatch);
  pthread_join(twatch, NULL);
  dlclose(self);

  DBG_FPINTF(stderr, "%s, end\n", __func__);
  return 0;
}

int is_im_enable(int nouse)
{
  char *astarisk_pos;
  char *hiragana_pos;

  (void)nouse;

  DBG_FPINTF(stderr, "%s, start\n", __func__);
  if (uim_fd < 0) {
    DBG_FPINTF(stderr, "!!!!!!!!!!!!! %s, fd-error\n", __func__);
    return 0;
  }

  DBG_FPINTF(stderr, "  hiragana : %s\n", im_on > 0 ? "on" : "off");
  DBG_FPINTF(stderr, "%s, end\n", __func__);
  return im_on > 0 ? 1 : 0;
}

static inline int uim_send_message(int fd, const char* msg)
{
  xwrite(fd, msg, strlen(msg));
  return 0;
}

static void * uimwatch(void *nouse)
{
  char tmp[BUFSIZ];
  char *buf = strdup("");
  char *p;
  struct pollfd pfd;
  ssize_t n;

  (void)nouse;

  pfd.fd = uim_fd;
  pfd.events = (POLLIN | POLLPRI);

  for (;;) {
    RETRY_EINTR(n, poll(&pfd, 1, INFTIM));
    if (n == -1)
      break;
    if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
      break;
    RETRY_EINTR(n, read(uim_fd, tmp, sizeof(tmp)));
    if (n == 0 || n == -1)
      break;
    if (tmp[0] == 0)
      continue;
    buf = uim_helper_buffer_append(buf, tmp, n);
    while ((p = uim_helper_buffer_get_message(buf)) != NULL) {
      if (strncmp(p, "prop_list_update", sizeof("prop_list_update") - 1) == 0) {
          DBG_FPINTF(stderr, "  %s\n", p);
          astarisk_pos = strstr(p, "*");
          hiragana_pos = strstr(p, ACTION_HIRAGANA);
          im_on = (long long)(astarisk_pos - hiragana_pos);
          free(p);
      }
    }
  }

  return NULL;
}

static inline ssize_t xwrite(int fd, const char *buf, size_t size)
{
  ssize_t n;
  size_t s = 0;
  while (s < size) {
    RETRY_EINTR(n, write(fd, buf + s, size - s));
    if (n == -1)
      return -1;
    s += n;
  }
  return s;
}

int im_set(char *active)
{
  DBG_FPINTF(stderr, "%s, start\n", __func__);
  if (uim_fd < 0) {
    DBG_FPINTF(stderr, "!!!!!!!!!!!!! %s, fd-error\n", __func__);
    return 1;
  }

  DBG_FPINTF(stderr, "  active : %s\n", active);
  if (active[0] == '1') {
    if (im_on < 1) {
      uim_send_message(uim_fd, "prop_activate\n"ACTION_HIRAGANA"\n\n");
    }
  } else {
    if (im_on > 0) {
      uim_send_message(uim_fd, "prop_activate\n"ACTION_DIRECT"\n\n");
    }
  }

  DBG_FPINTF(stderr, "%s, end\n", __func__);
  return 0;
}

/* vim:set sts=2 sw=2 tw=0 et: */

