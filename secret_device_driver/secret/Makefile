# Makefile for the secretkeeper driver.
PROG=	secretkeeper
SRCS=	secretkeeper.c

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-ldriver -lsys

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
