# Example makefile for CPE 464
#

CC = gcc
CFLAGS = -g -Wall -Werror
OS = $(shell uname -s)
PROC = $(shell uname -p)
EXEC_SUFFIX=$(OS)-$(PROC)

ifeq ("$(OS)", "SunOS")
	OSLIB=-L/opt/csw/lib -R/opt/csw/lib -lsocket -lnsl
	OSINC=-I/opt/csw/include
	OSDEF=-DSOLARIS
else
ifeq ("$(OS)", "Darwin")
	OSLIB=
	OSINC=
	OSDEF=-DDARWIN
else
	OSLIB=
	OSINC=
	OSDEF=-DLINUX
endif
endif

all: trace-$(EXEC_SUFFIX)

trace-$(EXEC_SUFFIX): trace.c
	$(CC) $(CFLAGS) $(OSINC) $(OSLIB) $(OSDEF) -o $@ trace.c checksum.c smartalloc.c -lpcap

handin: README
	handin bellardo 464_p1 README smartalloc.c smartalloc.h checksum.c checksum.h trace.c trace.h Makefile

clean:
	rm -rf trace-* trace-*.dSYM

ArpTest: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/ArpTest.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/ArpTest.pcap > CPE464_p1_files/ArpTest.out.txt
	diff CPE464_p1_files/ArpTest.out.txt CPE464_p1_files/ArpTestSample.out.txt

PingTest: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/PingTest.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/PingTest.pcap > CPE464_p1_files/PingTest.out.txt
	diff CPE464_p1_files/PingTest.out.txt CPE464_p1_files/PingTestSample.out.txt

UDPfile: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/UDPfile.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/UDPfile.pcap > CPE464_p1_files/UDPfile.out.txt
	diff CPE464_p1_files/UDPfile.out.txt CPE464_p1_files/UDPfileSample.out.txt

IP_bad_checksum: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/IP_bad_checksum.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/IP_bad_checksum.pcap > CPE464_p1_files/IP_bad_checksum.out.txt
	diff CPE464_p1_files/IP_bad_checksum.out.txt CPE464_p1_files/IP_bad_checksumSample.out.txt

largeMix: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/largeMix.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/largeMix.pcap > CPE464_p1_files/largeMix.out.txt
	diff CPE464_p1_files/largeMix.out.txt CPE464_p1_files/largeMixSample.out.txt

largeMix2: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/largeMix2.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/largeMix2.pcap > CPE464_p1_files/largeMix2.out.txt
	diff CPE464_p1_files/largeMix2.out.txt CPE464_p1_files/largeMix2Sample.out.txt

smallTCP: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/smallTCP.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/smallTCP.pcap > CPE464_p1_files/smallTCP.out.txt
	diff CPE464_p1_files/smallTCP.out.txt CPE464_p1_files/smallTCPSample.out.txt

TCP_bad_checksum: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/TCP_bad_checksum.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/TCP_bad_checksum.pcap > CPE464_p1_files/TCP_bad_checksum.out.txt
	diff CPE464_p1_files/TCP_bad_checksum.out.txt CPE464_p1_files/TCP_bad_checksumSample.out.txt

Unknown_pdu_1: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/brendens_packets1.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/brendens_packets1.pcap > CPE464_p1_files/brendens_packets1.out.txt
	diff CPE464_p1_files/brendens_packets1.out.txt CPE464_p1_files/brendens_packets1Sample.out.txt

Unknown_pdu_2: trace-$(EXEC_SUFFIX)
	rm -f CPE464_p1_files/brendens_packets2.out.txt
	./trace-$(EXEC_SUFFIX) CPE464_p1_files/brendens_packets2.pcap > CPE464_p1_files/brendens_packets2.out.txt
	diff CPE464_p1_files/brendens_packets2.out.txt CPE464_p1_files/brendens_packets2Sample.out.txt
 