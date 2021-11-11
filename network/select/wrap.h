#ifndef __WRAP_H
#define __WRAP_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

extern int errno;

int Socket(int domain, int type, int protocol);
int Bind(int fd, struct sockaddr_in *addr, socklen_t len);
int Accept(int fd, struct sockaddr_in *addr, socklen_t *addr_len);
int Connect(int fd, struct sockaddr_in *addr, socklen_t len);
int Listen(int fd, int n);
int Readn(int fd, void *buf, size_t n);
int readline(int fd, void *vptr, int maxlen);
#endif