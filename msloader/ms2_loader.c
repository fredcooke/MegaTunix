/* This program is a VERY quick and dirty hack which will be cleaned up later
 * I wrote it very quickly in order to be able to reflash the MS2 from linux, so
 * things like comport speed, etc... are hardcoded
 * and I'm using blocking I/O when non-blocking would be better, and
 * I can get into situations that require the boot jumper on occasion
 * This program is based on efahl's ms2dl C++ program, but ported to Linux.
 *
 * $Id: ms2_loader.c,v 1.1.2.4 2009/02/23 03:59:08 extace Exp $
 */

#ifndef _POSIX_VDISABLE
#define _POSIX_VDISABLE 0
#endif

#include <gtk/gtk.h>
#include <loader_common.h>
#include <ms2_loader.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define C_WAKE_UP        0x0D
#define C_READ_BYTE      0xA1
#define C_WRITE_BYTE     0xA2
#define C_READ_WORD      0xA3
#define C_WRITE_WORD     0xA4
#define C_READ_NEXT      0xA5
#define C_WRITE_NEXT     0xA6
#define C_READ_BLOCK     0xA7
#define C_WRITE_BLOCK    0xA8
#define MAX_BLOCK        256 // For both writes and reads.
#define C_READ_REGS      0xA9
#define C_WRITE_SP       0xAA
#define C_WRITE_PC       0xAB
#define C_WRITE_IY       0xAC
#define C_WRITE_IX       0xAD
#define C_WRITE_D        0xAE
#define C_WRITE_CCR      0xAF

#define C_GO             0xB1
#define C_TRACE1         0xB2
#define C_HALT           0xB3
#define C_RESET          0xB4
#define C_ERASE_ALL      0xB6
#define C_DEVICE_INFO    0xB7
#define C_ERASE_PAGE     0xB8
#define C_ERASE_EEPROM   0xB9

//--  Error codes  -------------------------------------------------------------

#define E_NONE           0xE0
#define E_COMMAND        0xE1
#define E_NOT_RUN        0xE2
#define E_SP_RANGE       0xE3
#define E_SP_INVALID     0xE4
#define E_READ_ONLY      0xE5
#define E_FACCERR        0xE6
#define E_ACCERR         0xE9

//--  Status codes  ------------------------------------------------------------

#define S_ACTIVE         0x00
#define S_RUNNING        0x01
#define S_HALTED         0x02
#define S_TRACE1         0x04
#define S_COLD_RESET     0x08
#define S_WARM_RESET     0x0C

#define LONG_DELAY 500000  /* 500 ms */

unsigned int total_bytes = 0;

char **fileBuf;
unsigned int count = 0;
int debug = 3;
/* debug levels
0 = Quiet
1 = Some progress
2 = Full progress
3 = + serial comms
4 = + the s19 file as parsed
5 = + comments
*/
gboolean check_status(gint );

void do_ms2_load(int port_fd, int file_fd)
{
	total_bytes = 0;
	count = read_s19(file_fd);
	if (count == 0)
		return;
	enter_boot_mode(port_fd);
	if (!wakeup_S12(port_fd))
		return;
	erase_S12(port_fd);
	send_S12(port_fd,count);
	free_s19(count);
	reset_proc(port_fd);
	output(g_strdup_printf("Wrote %d bytes\n", total_bytes),TRUE);
	return;
}

void ms2_chomp(char *inBuf)
{
	char *s;

	s = strrchr(inBuf, '\n');
	if (s)
		*s = 0;
	s = strrchr(inBuf, '\r');
	if (s)
		*s = 0;
}

gint read_s19 (gint file_fd)
{
	unsigned int lines = 0;
	char *buf = NULL;
	gsize len = 0;
	GIOChannel *chan = NULL;
	GError *err = NULL;


#ifdef __WIN32__
	chan = g_io_channel_win32_new_fd(file_fd);
#else
	chan = g_io_channel_unix_new(file_fd);
#endif
	g_io_channel_set_encoding(chan, NULL,&err);
	count = 0;

	while(G_IO_STATUS_NORMAL == g_io_channel_read_line(chan,&buf,&len,NULL,&err)) {
		if (len > 0)	
			g_free(buf);
		lines++;
	}
	fileBuf = (char **)malloc(lines * sizeof(char *));

	// Go to beginning of file
	g_io_channel_seek_position(chan,0,G_SEEK_SET,&err);
	while(G_IO_STATUS_NORMAL == g_io_channel_read_line(chan,&buf,&len,NULL,&err)) {
		if (debug > 4) {
			output(g_strdup_printf("%s", buf),TRUE);
		}
		ms2_chomp(buf);
		fileBuf[count] = strdup(buf);
		g_free(buf);
		count++;
	}

	return count;
}

void free_s19(count)
{
	unsigned int i;

	for (i = 0; i < count; i++) {
		free(fileBuf[i]);
	}

	free(fileBuf);
}

unsigned int extract_number(char *data, unsigned int nBytes)
{
	char number[128];

	strncpy(number, data, nBytes);
	number[nBytes] = 0;

	return strtol(number, NULL, 16);
}

unsigned char cs(unsigned int l)
{
	return ((l >> 24) & 0xff) + ((l >> 16) & 0xff) + ((l >> 8) & 0xff) + ((l >> 0) & 0xff);
}

unsigned char extract_data(char *data, unsigned int nBytes, unsigned char checksum, unsigned char *binary)
{
	unsigned int i;

	for (i = 0; i < nBytes; i++) {
		unsigned int n = extract_number(data +i*2, 2);
		binary[i] = (char)n;
		checksum += n;
	}

	return checksum;
}

gboolean wakeup_S12(gint port_fd)
{
	unsigned char c;
	unsigned char prompt;
	gint res = 0;
	int i;

	c = C_WAKE_UP;

#ifndef __WIN32__
	fcntl(port_fd, F_SETFL, O_NDELAY);
#endif

	for (i = 0; i < 10; i++) {
		prompt = 0;
		output("Attempting Wakeup...\n",FALSE);
		res = write(port_fd, &c, 1);
		if (res != 1)
			output(g_strdup_printf("wakeup_S12(): SHORT WRITE! %i of 1\n", res),TRUE);

		if (debug >= 4) {
			output(g_strdup_printf("TX: %02x\n", c),TRUE);
		}

		g_usleep(LONG_DELAY);
		if (check_status(port_fd))
			break;
	}

#ifndef __WIN32__
	fcntl(port_fd, F_SETFL, 0);
#endif

	if (i == 10) {
		output("Could not wake up processor, try jumpering\nthe boot jumper and power cycling the ECU...\n",FALSE);
		return FALSE;
	}
	return TRUE;
}
   
gboolean check_status(gint port_fd)
{
	unsigned char errorCode = 0;
	unsigned char statusCode = 0;
	unsigned char prompt = 0;
	unsigned char c[3] = {-1, -1, -1};
	gint res = 0;
	gboolean retval = TRUE;

	res = read(port_fd, &c, 3);
	if (res != 3 )
	{
		output(g_strdup_printf("check_status(), SHORT READ! wanted 3, got %i\n",res),TRUE);
		res = read(port_fd, &c, 3);
		if (res != 3 )
			output(g_strdup_printf("check_status(), SHORT READ! (2nd try) wanted 3, got %i\n",res),TRUE);
	}
	errorCode = c[0];
	statusCode = c[1];
	prompt = c[2];
	if (debug >= 4) {
		output(g_strdup_printf("RX: %02x %02x %02xn", c[0], c[1], c[2]),TRUE);
	}

	switch(errorCode) 
	{
		case E_NONE:
			retval = TRUE;
			break;
		case E_COMMAND: 
			retval = FALSE;
			output(g_strdup_printf("Command not recognized, e=0x%02x\n",errorCode),TRUE);
			break;
		case E_NOT_RUN: 
			retval = FALSE;
			output(g_strdup_printf("Command not allowed in run mode, e=0x%02x\n",errorCode),TRUE);
			break;
		case E_SP_RANGE:
			retval = FALSE;
			output(g_strdup_printf("Stack pointer out of range, e=0x%02x\n",errorCode),TRUE);
			break;
		case E_SP_INVALID:
			retval = FALSE;
			output(g_strdup_printf("Stack pointer invalid, e=0x%02x\n",errorCode),TRUE);
			break;
		case E_READ_ONLY:
			retval = FALSE;
			output(g_strdup_printf("Attempt to write read-only memory, e=0x%02x\n",errorCode),TRUE);
			break;
		case E_FACCERR:
			retval = FALSE;
			output(g_strdup_printf("Access error when writing FLASH/EEPROM memory, e=0x%02x\n",errorCode),TRUE);
			break;
		case E_ACCERR:
			retval = FALSE;
			output(g_strdup_printf("Memory protection violation writing EEPROM memory, e=0x%02x\n",errorCode),TRUE);
			break;
		default:
			retval = FALSE;
			output(g_strdup_printf("Error code 02x%x\n", errorCode),TRUE);
			break;
	}

	switch(statusCode) 
	{
		case S_ACTIVE:
			retval = TRUE;
			break;
		case S_RUNNING:
			retval = FALSE;
			output(g_strdup_printf("Program running, download failed, s=0x%02x\n",statusCode),TRUE);
			break;
		case S_HALTED:
			retval = FALSE;
			output(g_strdup_printf("Program halted, download failed, s=0x%02x\n",statusCode),TRUE);
			break;
		case S_TRACE1:
			retval = FALSE;
			output(g_strdup_printf("Program trace detected, download failed, s=0x%02x\n",statusCode),TRUE);
			break;
		case S_COLD_RESET:
			retval = FALSE;
			output(g_strdup_printf("Cold reset detected, something grossly wrong, s=0x%02x\n",statusCode),TRUE);
			break;
		case S_WARM_RESET:
			retval = FALSE;
			output(g_strdup_printf("Warm reset detected, something grossly wrong, s=0x%02x\n",statusCode),TRUE);
			break;
		default:
			retval = FALSE;
			output(g_strdup_printf("Unknown status received, s=0x%02x\n",statusCode),TRUE);
			break;

	}

	if (prompt != '>') {
		output(g_strdup_printf("Prompt was expected to be \">\" but returned as %c\n", prompt),TRUE);
	} 
	else
		retval = TRUE;
	return retval;
}

void sendPPAGE(gint port_fd, unsigned int a, unsigned char erasing)
{
	unsigned char c;
	static unsigned char page = 0;
	unsigned char command [4];
	gint res = 0;

	a = 0xFFFF & (a >> 16);

	if (a != page) {
		page = a & 0xFF;
		output(g_strdup_printf("Setting page to 0x%02x:\n", page),TRUE);
		command[0] = C_WRITE_BYTE;
		command[1] = 0x00;
		command[2] = 0x30;
		command[3] = page;
		res = write(port_fd, command, 4);
		if (res != 4)
			output(g_strdup_printf("sendPPAGE(): SHORT WRITE %i of 4",res),TRUE);
		if (debug >= 4) {
			output(g_strdup_printf("TX: %02x %02x %02x %02x\n", command[0], command[1], command[2], command[3] ),TRUE);
		}
		g_usleep(LONG_DELAY);
		check_status(port_fd);

		c = C_ERASE_PAGE;
		if (erasing) {
			output(g_strdup_printf("Erasing page 0x%02x:\n", page),TRUE);
			res = write(port_fd, &c, 1);
			if (res != 1)
				output(g_strdup_printf("sendPPAGE():(erasing) SHORT WRITE %i of 1",res),TRUE);
			g_usleep(5*LONG_DELAY);
			output(g_strdup_printf("Page 0x%02x erased...\n",page),TRUE);
			check_status(port_fd);
		}

	}
}

void send_block(gint port_fd, unsigned int a, unsigned char *b, unsigned int n)
{
	unsigned char command[4];
	unsigned char i;
	gint res = 0;

	if ((a < 0x400)) {
		if (debug) {
			output(g_strdup_printf("Skipping block %04x\n", a),TRUE);
		}
		return;
	}

	command[0] = C_WRITE_BLOCK;
	command[1] = 0;
	command[2] = 0;
	command[3] = 0;

	sendPPAGE(port_fd,a, 1);

	command[1] = 0xFF & (a >> 8);
	command[2] = 0xFF & a;
	command[3] = n - 1;

	if (debug >= 4) {
		output(g_strdup_printf("TX: %02x %02x %02x %02x\n", command[0], command[1], command[2], command[3] ),TRUE);
	}

	res = write(port_fd, command, 4);
	if (res != 4)
		output(g_strdup_printf("send_block(): SHORT WRITE %i of 4",res),TRUE);
	if (debug >= 4) {
		output( "TX:",FALSE);
		for (i = 0 ; i < n ; i++) {
			output(g_strdup_printf(" %02x", b[i] ),TRUE);
		}
		output( "\n",FALSE);
	}
	res = write(port_fd, b, n);
	if (res != n)
		output(g_strdup_printf("send_block(): SHORT WRITE %i of %i",res,n),TRUE);
	total_bytes += n;
	check_status(port_fd);
}

void erase_S12(gint port_fd)
{
	unsigned char c;
	gint res = 0;

	if (debug) {
		output( "Erasing main flash!\n",FALSE);
	}
	c = C_ERASE_ALL;
	res = write(port_fd, &c, 1);
	if (res != 1)
		output(g_strdup_printf("erase_S12(): SHORT WRITE %i of 1",res),TRUE);
	output( "Main Flash Erased...\n",FALSE);
	g_usleep(5*LONG_DELAY);
	check_status(port_fd);
}
void send_S12(gint port_fd, gint count)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned char checksum = 0;
	unsigned char *thisRec = NULL;
	unsigned char recsum = 0;
	unsigned char difsum = 0;
	unsigned int size = 0;
	unsigned int dataSize = 0;
	unsigned int addr = 0;
	unsigned int addrSize = 0;
	unsigned int nBlocks = 0;

	for (i = 0; i < count; i++) {
		switch (fileBuf[i][1]) {
			case '0':
				addrSize = 4;
				break;
			case '1':
				addrSize = 4;
				break;
			case '2':
				addrSize = 6;
				break;
			case '3':
				addrSize = 8;
				break;
			case '7':
				addrSize   = 8;
				break;
			case '8':
				addrSize   = 6;
				break;
			case '9':
				addrSize   = 4;
				break;
		}
		size = extract_number(fileBuf[i]+2, 2);
		dataSize = size - addrSize/2 -1;
		addr = extract_number(fileBuf[i]+4, addrSize);
		checksum = cs(addr) + size;

		thisRec = (unsigned char *)malloc(dataSize + 1);
		checksum = extract_data(fileBuf[i]+4+addrSize, dataSize, checksum, thisRec);

		recsum = extract_number(fileBuf[i]+4+addrSize+dataSize*2, 2);
		difsum = ~(recsum + checksum);

		if (difsum != 0) {
			output(g_strdup_printf("Invalid checksum, found 0x%02x, expected %#04x\n", recsum,(unsigned char)~checksum),TRUE);

		}

		if (debug >= 3) {
			output(g_strdup_printf("Sending record %d:%6x\n", i, addr),TRUE);
		}
		if (addr >= 0x8000 && addr <= 0xBFFF) addr -= 0x4000;

		nBlocks = (dataSize - 1) / MAX_BLOCK + 1;
		for (j = 0; dataSize > 0; j++) {
			unsigned int nn = dataSize > MAX_BLOCK ? MAX_BLOCK : dataSize;
			unsigned char *thisRecPtr = thisRec;
			if (addr < 0x8000 && addr+nn > 0x8000) {
				nn = 0x8000 - addr;
			}

			send_block(port_fd,addr, thisRecPtr, nn);
			dataSize -= nn;
			thisRecPtr += nn;
			addr += nn;
			if (addr >= 0x8000 && addr <= 0xBFFF) addr += 0x4000;
		}

		free(thisRec);
	}
}

void enter_boot_mode(gint port_fd)
{
	gint res = 0;
	res = write(port_fd, "!!!SafetyFirst", 14);
	if (res != 14)
		output(g_strdup_printf("enter_boot_mode() SHORT WRITE, sent %i of 14\n",res),TRUE);
	flush_serial(port_fd, BOTH);
	g_usleep(LONG_DELAY);
}

void reset_proc(gint port_fd)
{
	gint res = 0;
	unsigned char command = C_RESET;

	res = write(port_fd, &command, 1);
	if (res != 1)
		output(g_strdup_printf("reset_proc() SHORT WRITE, sent %i of 1\n",res),TRUE);
}


