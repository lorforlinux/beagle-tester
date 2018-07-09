/*
 * beagle-tester.c
 *
 * based on evtest and fb-test
 *
 * Author: Jason Kridner <jdk@ti.com>
 * Copyright (c) 1999-2000 Vojtech Pavlik
 * Copyright (c) 2009-2011 Red Hat, Inc
 * Copyright (C) 2009-2012 Tomi Valkeinen
 * Copyright (C) 2016-2018 Texas Instruments
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <rc/adc.h>
#include <rc/bmp.h>
#include <rc/mpu.h>
#include <rc/start_stop.h>
#include <rc/time.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>

/*
 * common.h
 *
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 * Copyright (C) 2009-2012 Tomi Valkeinen

 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <linux/fb.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define ASSERT(x) if (!(x)) \
	{ perror("assert(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }
#define FBCTL0(ctl) if (ioctl(fd, ctl))\
	{ perror("fbctl0(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }
#define FBCTL1(ctl, arg1) if (ioctl(fd, ctl, arg1))\
	{ perror("fbctl1(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }

#define IOCTL0(fd, ctl) if (ioctl(fd, ctl))\
	{ perror("ioctl0(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }
#define IOCTL1(fd, ctl, arg1) if (ioctl(fd, ctl, arg1))\
	{ perror("ioctl1(" __FILE__ ":" TOSTRING(__LINE__) "): "); exit(1); }

struct fb_info
{
	int fd;

	void *ptr;

	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
	unsigned bytespp;
};

extern char fontdata_8x8[];

void fb_open(int fb_num, struct fb_info *fb_info);
void fb_update_window(int fd, short x, short y, short w, short h);
void fb_sync_gfx(int fd);
int fb_put_string(struct fb_info *fb_info, int x, int y, const char *s, int maxlen,
		unsigned color, int clear, int clearlen);

#endif

static struct fb_info fb_info;
/* end of common.h */

/*
 * red.h
 *
 * Author: Neeraj Dantu <neeraj.dantu@octavosystems.com>
 * Copyright (C) 2018 Octavo Systems

 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)
 
#define MAP_SIZE							4096UL
#define MAP_MASK							(MAP_SIZE - 1)

unsigned long read_write_mem(int argc, unsigned int address, char type, unsigned long writeval);


#define TEMP_I2C_ADD						0x48
#define TEMP_MFG_ID 						0xfe
#define TEMP_I2C_BUS						0
#define TEMP_ID								1128


int test_tmp480();

#define TPM_I2C_ADD 						0x29
#define TPM_REV_ID							0x4c
#define TPM_I2C_BUS 						0

int test_tpm();

#define CONTROL_BASE_ADDR               	(0x44E10000)
#define BOOTMODE                        	(CONTROL_BASE_ADDR + 0x40)
#define BOOTMODE_SD							(0x00400318)

int check_boot_mode();
int test_imu();
int test_bmp();


#define NOR_READ_ID 						0x9e
struct spi_ioc_transfer xfer[2]; 

int test_nor();

/*
 * red.c
 *
 * Author: Neeraj Dantu <neeraj.dantu@octavosystems.com>
 * Copyright (C) 2018 Octavo Systems

 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

int osd3358_sm_ref_design_tests(){
	
	int ret;
	//Test bootmode
	ret = check_boot_mode();
	if(ret<0) {
		fprintf(stderr, "failed: boot mode test\n");
		beagle_notice("BOOT MODE", "fail");
		return -3;
	}
	else
	{
		beagle_notice("BOOT MODE", "pass");
	}

	
	//Test IMU using driver
	ret = test_imu();
	if(ret<0) {
		fprintf(stderr, "failed: mpu9250 imu\n");
		beagle_notice("IMU", "fail");
		return -3;
	}
	else
	{
		beagle_notice("IMU", "pass");
	}

	// test barometer
	ret = test_bmp();
	if(ret<0) 
	{
		fprintf(stderr, "failed: bmp280 barometer\n");
		beagle_notice("BMP", "fail");
		return -4;
	}
	else
	{
		beagle_notice("BMP", "pass");
	}

	//test temp sensor
	ret = test_tmp480();
	if(ret<0) 
	{
		fprintf(stderr, "failed: tmp480 temp sensor\n");
		beagle_notice("TEMP", "fail");
		return -5;
	}
	else
	{
		beagle_notice("TEMP", "pass");
	}
	
	//test TPM
	ret = test_tpm();
	if(ret<0) {
		fprintf(stderr, "failed: TPM\n");
		beagle_notice("TPM", "fail");
		return -6;
	}
	else
	{
		beagle_notice("TPM", "pass");
	}
	
	
	//Test NOR Flash on SPI0
	ret = test_nor();
	if(ret<0){
		fprintf(stderr, "failed: NOR\n");
		beagle_notice("NOR", "fail");
		return -7;
	}
	else
	{
		beagle_notice("NOR", "pass");
	}
	

	return 0;
}


unsigned long read_write_mem(int argc, unsigned int address, char type, unsigned long writeval) {
    int fd;
    void *map_base, *virt_addr; 
    unsigned long read_result = 0;
    off_t target;
    int access_type = 'w';
    
    if(argc < 1) { exit(1); }

    target = address;

    if(argc > 1)
        access_type = tolower(type);


    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
    fflush(stdout);
    
    /* Map one page */
    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
    if(map_base == (void *) -1) FATAL;
    if (0) { printf("Memory mapped at address %p.\n", map_base); }
    fflush(stdout);
    
    virt_addr = map_base + (target & MAP_MASK);
    
    if(argc <= 2) {
        switch(access_type) {
            case 'b':
                read_result = *((unsigned char *) virt_addr);
                break;
            case 'h':
                read_result = *((unsigned short *) virt_addr);
                break;
            case 'w':
                read_result = *((unsigned long *) virt_addr);
                break;
            default:
                fprintf(stderr, "Illegal data type '%c'.\n", access_type);
                exit(2);
        }


    } else {
        switch(access_type) {
            case 'b':
                *((unsigned char *) virt_addr) = writeval;
                read_result = *((unsigned char *) virt_addr);
                break;
            case 'h':
                *((unsigned short *) virt_addr) = writeval;
                read_result = *((unsigned short *) virt_addr);
                break;
            case 'w':
                *((unsigned long *) virt_addr) = writeval;
                read_result = *((unsigned long *) virt_addr);
                break;
        }

    }
    
    if(munmap(map_base, MAP_SIZE) == -1) FATAL;
    close(fd);
    return read_result;
}


int test_tmp480()
{
	
	uint8_t c = 0xFF;
	int file, ret, i;
	char buf[10] = "";
	uint16_t *data;
	
	file = open("/dev/i2c-0", O_RDWR);
	
	
	if(ioctl(file, I2C_SLAVE, TEMP_I2C_ADD) < 0)
	{
		printf("Address of temp sensor was not set\n");
	}

	ret = write(file, &c, 1);
	if(ret!=1)
	{
		printf("write to i2c bus with temp sensor failed\n");
		return -1;
	}
	
	ret = read(file, buf, 2);
	if(ret!=(2))
	{
		printf("i2c device returned %d bytes\n",ret);
		return -1;
	}
	
	// form words from bytes and put into user's data array
	for(i=0;i<1;i++)
	{
		data[i] = (((uint16_t)buf[0])<<8 | buf[1]); 
	}
	
	if(data[0] != TEMP_ID)
	{
		return -1;
	}
	else
	{
		return 0;
	}  
}


int test_tpm()
{
	FILE *fp;
	char buff[10];
	char string[] = "0xc6";
	
	system("touch /var/tpmtest.txt");
	system("i2cget -f -y 0 0x29 0x4c c > /var/tpmtest.txt");
	fp = fopen("/var/tpmtest.txt", "r");
	fscanf(fp, "%s", buff);
	fclose(fp);
	system("rm /var/tpmtest.txt");
	
	if(strcmp(buff,string) == 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int check_boot_mode() {

    unsigned long temp = 0;
    temp = read_write_mem(2, BOOTMODE, 'w', 0x00000000);
    if(temp == BOOTMODE_SD)
    {
    	return 1;
    }
    else
    {
    	return -1;
    }
}

int test_imu()
{
	FILE *fp;
	char buff[10];
	char string[] = "0x71";
	
	system("touch /var/imutest.txt");
	system("i2cget -f -y 0 0x68 0x75 > /var/imutest.txt");
	fp = fopen("/var/imutest.txt", "r");
	fscanf(fp, "%s", buff);
	fclose(fp);
	system("rm /var/imutest.txt");
	
	if(strcmp(buff,string) == 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}


int test_bmp()
{
	FILE *fp;
	char buff[10];
	char string[] = "0x58";
	
	system("touch /var/bmptest.txt");
	system("i2cget -f -y 0 0x76 0xD0 > /var/bmptest.txt");
	fp = fopen("/var/bmptest.txt", "r");
	fscanf(fp, "%s", buff);
	fclose(fp);
	system("rm /var/bmptest.txt");
	
	if(strcmp(buff,string) == 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int test_nor()
{
	int spi_speed_hz, slave, ret, fd, tx_bytes, i, mode_proper, bits, rx_bytes;
	mode_proper = SPI_MODE_0;
	bits = 8;
	rx_bytes = 8;
	spi_speed_hz = 10000;
	char command[8];
	command[0] = 0x90;
	command[1] = 0x00;
	command[2] = 0x00;
	command[3] = 0x00;
	command[4] = 0x00;
	command[5] = 0x00;
	command[6] = 0x00;
	command[7] = 0x00;

	char *rx_id = malloc(8*sizeof(char));
	slave = 1;
	spi_speed_hz = 10000;
	tx_bytes = 4;
	
	int fdsp;
	//Setting spi mode
	
	system("config-pin P9_17 spi_cs");
	system("config-pin P9_18 spi");
	system("config-pin P9_21 spi");
	system("config-pin P9_22 spi_sclk");
	
	fdsp = open("/dev/spidev1.0", O_RDWR);
	ret = ioctl(fdsp, SPI_IOC_WR_MODE);
	if(ioctl(fdsp, SPI_IOC_WR_MODE, &mode_proper)<0){
		printf("can't set spi mode");
		close(fdsp);
		return -1;
	}
	if(ioctl(fdsp, SPI_IOC_RD_MODE, &mode_proper)<0){
		printf("can't get spi mode");
		close(fdsp);
		return -1;
	}
	if(ioctl(fdsp, SPI_IOC_WR_BITS_PER_WORD, &bits)<0){
		printf("can't set bits per word");
		close(fdsp);
		return -1;
	}
	if(ioctl(fdsp, SPI_IOC_RD_BITS_PER_WORD, &bits)<0){
		printf("can't get bits per word");
		close(fdsp);
		return -1;
	}
	if(ioctl(fdsp, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed_hz)<0){
		printf("can't set max speed hz");
		close(fdsp);
		return -1;
	}
	if(ioctl(fdsp, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed_hz)<0){
		printf("can't get max speed hz");
		close(fdsp);
		return -1;
	}
	
	//Filling in the xfer struct
	xfer[0].cs_change = 1;
	xfer[0].delay_usecs = 0;
	xfer[0].speed_hz = spi_speed_hz;
	xfer[0].bits_per_word = bits;
	xfer[0].rx_buf = 0;
	xfer[0].tx_buf = (unsigned long) command;
	xfer[0].len = 8;
	
	ret = ioctl(fdsp, SPI_IOC_MESSAGE(1), &xfer[0]);
	if(ret<0){
		printf("ERROR: SPI_IOC_MESSAGE_FAILED %x\n", ret);
	}
	
	xfer[1].cs_change = 1;
	xfer[1].delay_usecs = 0;
	xfer[1].speed_hz = spi_speed_hz;
	xfer[1].bits_per_word = bits;
	xfer[1].rx_buf = (unsigned long) rx_id;
	xfer[1].tx_buf = 0;
	xfer[1].len = rx_bytes;
	// receive
	ret=ioctl(fdsp, SPI_IOC_MESSAGE(1), &xfer[1]);
	if(ret<0){
		printf("ERROR: SPI read command failed\n");
		return -1;
	}
	
	if (*rx_id == 0x01){
		return 1;
	}
	else{
		return -1;
	}
}



/*
 * test.c
 *
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 * Copyright (C) 2009-2012 Tomi Valkeinen

 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

static void draw_pixel(struct fb_info *fb_info, int x, int y, unsigned color)
{
	void *fbmem;

	fbmem = fb_info->ptr;
	if (fb_info->var.bits_per_pixel == 8) {
		unsigned char *p;

		fbmem += fb_info->fix.line_length * y;

		p = fbmem;

		p += x;

		*p = color;
	} else if (fb_info->var.bits_per_pixel == 16) {
		unsigned short c;
		unsigned r = (color >> 16) & 0xff;
		unsigned g = (color >> 8) & 0xff;
		unsigned b = (color >> 0) & 0xff;
		unsigned short *p;

		r = r * 32 / 256;
		g = g * 64 / 256;
		b = b * 32 / 256;

		c = (r << 11) | (g << 5) | (b << 0);

		fbmem += fb_info->fix.line_length * y;

		p = fbmem;

		p += x;

		*p = c;
	} else if (fb_info->var.bits_per_pixel == 24) {
		 unsigned char *p;

		p = (unsigned char *)fbmem + fb_info->fix.line_length * y + 3 * x;
		*p++ = color;
		*p++ = color >> 8;
		*p = color >> 16;
	} else {
		unsigned int *p;

		fbmem += fb_info->fix.line_length * y;

		p = fbmem;

		p += x;

		*p = color;
	}
}

static void fill_screen(struct fb_info *fb_info)
{
	unsigned x, y;
	unsigned h = fb_info->var.yres;
	unsigned w = fb_info->var.xres;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			if (x < 20 && y < 20)
				draw_pixel(fb_info, x, y, 0xffffff);
			else if (x < 20 && (y > 20 && y < h - 20))
				draw_pixel(fb_info, x, y, 0xff);
			else if (y < 20 && (x > 20 && x < w - 20))
				draw_pixel(fb_info, x, y, 0xff00);
			else if (x > w - 20 && (y > 20 && y < h - 20))
				draw_pixel(fb_info, x, y, 0xff0000);
			else if (y > h - 20 && (x > 20 && x < w - 20))
				draw_pixel(fb_info, x, y, 0xffff00);
			else if (x == 20 || x == w - 20 ||
					y == 20 || y == h - 20)
				draw_pixel(fb_info, x, y, 0xffffff);
			else if (x == y || w - x == h - y)
				draw_pixel(fb_info, x, y, 0xff00ff);
			else if (w - x == y || x == h - y)
				draw_pixel(fb_info, x, y, 0x00ffff);
			else if (x > 20 && y > 20 && x < w - 20 && y < h - 20) {
				int t = x * 3 / w;
				unsigned r = 0, g = 0, b = 0;
				unsigned c;
				if (fb_info->var.bits_per_pixel == 16) {
					if (t == 0)
						b = (y % 32) * 256 / 32;
					else if (t == 1)
						g = (y % 64) * 256 / 64;
					else if (t == 2)
						r = (y % 32) * 256 / 32;
				} else {
					if (t == 0)
						b = (y % 256);
					else if (t == 1)
						g = (y % 256);
					else if (t == 2)
						r = (y % 256);
				}
				c = (r << 16) | (g << 8) | (b << 0);
				draw_pixel(fb_info, x, y, c);
			} else {
				draw_pixel(fb_info, x, y, 0);
			}
		}

	}

	fb_put_string(fb_info, w / 3 * 2, 30, "RED", 3, 0xffffff, 1, 3);
	fb_put_string(fb_info, w / 3, 30, "GREEN", 5, 0xffffff, 1, 5);
	fb_put_string(fb_info, 20, 30, "BLUE", 4, 0xffffff, 1, 4);
}

void fill_screen_solid(struct fb_info *fb_info, unsigned int color)
{

	unsigned x, y;
	unsigned h = fb_info->var.yres;
	unsigned w = fb_info->var.xres;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++)
			draw_pixel(fb_info, x, y, color);
	}
}

static void do_fill_screen(struct fb_info *fb_info, int pattern)
{

	switch (pattern) {
	case 1:
		fill_screen_solid(fb_info, 0xff0000);
		break;
	case 2:
		fill_screen_solid(fb_info, 0x00ff00);
		break;
	case 3:
		fill_screen_solid(fb_info, 0x0000ff);
		break;
	case 4:
		fill_screen_solid(fb_info, 0xffffff);
		break;
	case 0:
	default:
		fill_screen(fb_info);
		break;
	}
}


/*
 * common.c
 *
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 * Copyright (C) 2009-2012 Tomi Valkeinen

 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

void fb_open(int fb_num, struct fb_info *fb_info)
{
	char str[64];
	int fd,tty;

	tty = open("/dev/tty1", O_RDWR);

	if(ioctl(tty, KDSETMODE, KD_GRAPHICS) == -1)
		fprintf(stderr, "Failed to set graphics mode on tty1\n");

	sprintf(str, "/dev/fb%d", fb_num);
	fd = open(str, O_RDWR);

	ASSERT(fd >= 0);

	fb_info->fd = fd;
	IOCTL1(fd, FBIOGET_VSCREENINFO, &fb_info->var);
	IOCTL1(fd, FBIOGET_FSCREENINFO, &fb_info->fix);

	fprintf(stderr, "fb res %dx%d virtual %dx%d, line_len %d, bpp %d\n",
			fb_info->var.xres, fb_info->var.yres,
			fb_info->var.xres_virtual, fb_info->var.yres_virtual,
			fb_info->fix.line_length, fb_info->var.bits_per_pixel);
	fflush(stderr);

	void *ptr = mmap(0,
			fb_info->var.yres_virtual * fb_info->fix.line_length,
			PROT_WRITE | PROT_READ,
			MAP_SHARED, fd, 0);

	ASSERT(ptr != MAP_FAILED);

	fb_info->ptr = ptr;
}

static void fb_clear_area(struct fb_info *fb_info, int x, int y, int w, int h)
{
	int i = 0;
	int loc;
	char *fbuffer = (char *)fb_info->ptr;
	struct fb_var_screeninfo *var = &fb_info->var;
	struct fb_fix_screeninfo *fix = &fb_info->fix;

	for (i = 0; i < h; i++) {
		loc = (x + var->xoffset) * (var->bits_per_pixel / 8)
			+ (y + i + var->yoffset) * fix->line_length;
		memset(fbuffer + loc, 0, w * var->bits_per_pixel / 8);
	}
}

static void fb_put_char(struct fb_info *fb_info, int x, int y, char c,
		unsigned color)
{
	int i, j, bits, loc;
	unsigned char *p8;
	unsigned short *p16;
	unsigned int *p32;
	struct fb_var_screeninfo *var = &fb_info->var;
	struct fb_fix_screeninfo *fix = &fb_info->fix;

	unsigned short c16;
	unsigned r = (color >> 16) & 0xff;
	unsigned g = (color >> 8) & 0xff;
	unsigned b = (color >> 0) & 0xff;

	r = r * 32 / 256;
	g = g * 64 / 256;
	b = b * 32 / 256;

	c16 = (r << 11) | (g << 5) | (b << 0);

	for (i = 0; i < 8; i++) {
		bits = fontdata_8x8[8 * c + i];
		for (j = 0; j < 8; j++) {
			loc = (x + j + var->xoffset) * (var->bits_per_pixel / 8)
				+ (y + i + var->yoffset) * fix->line_length;
			if (loc >= 0 && loc < (int)(fix->smem_len) &&
					((bits >> (7 - j)) & 1)) {
				switch (var->bits_per_pixel) {
				case 8:
					p8 =  fb_info->ptr + loc;
					*p8 = color;
				case 16:
					p16 = fb_info->ptr + loc;
					*p16 = c16;
					break;
				case 24:
				case 32:
					p32 = fb_info->ptr + loc;
					*p32 = color;
					break;
				}
			}
		}
	}
}

int fb_put_string(struct fb_info *fb_info, int x, int y, const char *s, int maxlen,
		unsigned color, int clear, int clearlen)
{
	int i;
	int w = 0;

	if (clear)
		fb_clear_area(fb_info, x, y, clearlen * 8, 8);

	for (i = 0; i < (int)strlen(s) && i < maxlen; i++) {
		fb_put_char(fb_info, (x + 8 * i), y, s[i], color);
		w += 8;
	}

	return w;
}
/* end of fb-test.c */

/*
 * font_8x8.c
 *
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 * Copyright (C) 2009-2012 Tomi Valkeinen

 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**********************************************/
/*                                            */
/*       Font file generated by cpi2fnt       */
/*                                            */
/**********************************************/

char fontdata_8x8[] = {

	/* 0 0x00 '^@' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 1 0x01 '^A' */
	0x7e, /* 01111110 */
	0x81, /* 10000001 */
	0xa5, /* 10100101 */
	0x81, /* 10000001 */
	0xbd, /* 10111101 */
	0x99, /* 10011001 */
	0x81, /* 10000001 */
	0x7e, /* 01111110 */

	/* 2 0x02 '^B' */
	0x7e, /* 01111110 */
	0xff, /* 11111111 */
	0xdb, /* 11011011 */
	0xff, /* 11111111 */
	0xc3, /* 11000011 */
	0xe7, /* 11100111 */
	0xff, /* 11111111 */
	0x7e, /* 01111110 */

	/* 3 0x03 '^C' */
	0x6c, /* 01101100 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0x7c, /* 01111100 */
	0x38, /* 00111000 */
	0x10, /* 00010000 */
	0x00, /* 00000000 */

	/* 4 0x04 '^D' */
	0x10, /* 00010000 */
	0x38, /* 00111000 */
	0x7c, /* 01111100 */
	0xfe, /* 11111110 */
	0x7c, /* 01111100 */
	0x38, /* 00111000 */
	0x10, /* 00010000 */
	0x00, /* 00000000 */

	/* 5 0x05 '^E' */
	0x38, /* 00111000 */
	0x7c, /* 01111100 */
	0x38, /* 00111000 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0xd6, /* 11010110 */
	0x10, /* 00010000 */
	0x38, /* 00111000 */

	/* 6 0x06 '^F' */
	0x10, /* 00010000 */
	0x38, /* 00111000 */
	0x7c, /* 01111100 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0x7c, /* 01111100 */
	0x10, /* 00010000 */
	0x38, /* 00111000 */

	/* 7 0x07 '^G' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 8 0x08 '^H' */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xe7, /* 11100111 */
	0xc3, /* 11000011 */
	0xc3, /* 11000011 */
	0xe7, /* 11100111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */

	/* 9 0x09 '^I' */
	0x00, /* 00000000 */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x42, /* 01000010 */
	0x42, /* 01000010 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 10 0x0a '^J' */
	0xff, /* 11111111 */
	0xc3, /* 11000011 */
	0x99, /* 10011001 */
	0xbd, /* 10111101 */
	0xbd, /* 10111101 */
	0x99, /* 10011001 */
	0xc3, /* 11000011 */
	0xff, /* 11111111 */

	/* 11 0x0b '^K' */
	0x0f, /* 00001111 */
	0x07, /* 00000111 */
	0x0f, /* 00001111 */
	0x7d, /* 01111101 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x78, /* 01111000 */

	/* 12 0x0c '^L' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */

	/* 13 0x0d '^M' */
	0x3f, /* 00111111 */
	0x33, /* 00110011 */
	0x3f, /* 00111111 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x70, /* 01110000 */
	0xf0, /* 11110000 */
	0xe0, /* 11100000 */

	/* 14 0x0e '^N' */
	0x7f, /* 01111111 */
	0x63, /* 01100011 */
	0x7f, /* 01111111 */
	0x63, /* 01100011 */
	0x63, /* 01100011 */
	0x67, /* 01100111 */
	0xe6, /* 11100110 */
	0xc0, /* 11000000 */

	/* 15 0x0f '^O' */
	0x18, /* 00011000 */
	0xdb, /* 11011011 */
	0x3c, /* 00111100 */
	0xe7, /* 11100111 */
	0xe7, /* 11100111 */
	0x3c, /* 00111100 */
	0xdb, /* 11011011 */
	0x18, /* 00011000 */

	/* 16 0x10 '^P' */
	0x80, /* 10000000 */
	0xe0, /* 11100000 */
	0xf8, /* 11111000 */
	0xfe, /* 11111110 */
	0xf8, /* 11111000 */
	0xe0, /* 11100000 */
	0x80, /* 10000000 */
	0x00, /* 00000000 */

	/* 17 0x11 '^Q' */
	0x02, /* 00000010 */
	0x0e, /* 00001110 */
	0x3e, /* 00111110 */
	0xfe, /* 11111110 */
	0x3e, /* 00111110 */
	0x0e, /* 00001110 */
	0x02, /* 00000010 */
	0x00, /* 00000000 */

	/* 18 0x12 '^R' */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */

	/* 19 0x13 '^S' */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */

	/* 20 0x14 '^T' */
	0x7f, /* 01111111 */
	0xdb, /* 11011011 */
	0xdb, /* 11011011 */
	0x7b, /* 01111011 */
	0x1b, /* 00011011 */
	0x1b, /* 00011011 */
	0x1b, /* 00011011 */
	0x00, /* 00000000 */

	/* 21 0x15 '^U' */
	0x3e, /* 00111110 */
	0x61, /* 01100001 */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x86, /* 10000110 */
	0x7c, /* 01111100 */

	/* 22 0x16 '^V' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x7e, /* 01111110 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 23 0x17 '^W' */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */

	/* 24 0x18 '^X' */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 25 0x19 '^Y' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 26 0x1a '^Z' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0xfe, /* 11111110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 27 0x1b '^[' */
	0x00, /* 00000000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0xfe, /* 11111110 */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 28 0x1c '^\' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 29 0x1d '^]' */
	0x00, /* 00000000 */
	0x24, /* 00100100 */
	0x66, /* 01100110 */
	0xff, /* 11111111 */
	0x66, /* 01100110 */
	0x24, /* 00100100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 30 0x1e '^^' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 31 0x1f '^_' */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0x7e, /* 01111110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 32 0x20 ' ' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 33 0x21 '!' */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 34 0x22 '"' */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x24, /* 00100100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 35 0x23 '#' */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x00, /* 00000000 */

	/* 36 0x24 '$' */
	0x18, /* 00011000 */
	0x3e, /* 00111110 */
	0x60, /* 01100000 */
	0x3c, /* 00111100 */
	0x06, /* 00000110 */
	0x7c, /* 01111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 37 0x25 '%' */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xcc, /* 11001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x66, /* 01100110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 38 0x26 '&' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 39 0x27 ''' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 40 0x28 '(' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x00, /* 00000000 */

	/* 41 0x29 ')' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */

	/* 42 0x2a '*' */
	0x00, /* 00000000 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0xff, /* 11111111 */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 43 0x2b '+' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 44 0x2c ',' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */

	/* 45 0x2d '-' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 46 0x2e '.' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 47 0x2f '/' */
	0x06, /* 00000110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0xc0, /* 11000000 */
	0x80, /* 10000000 */
	0x00, /* 00000000 */

	/* 48 0x30 '0' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xd6, /* 11010110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 49 0x31 '1' */
	0x18, /* 00011000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 50 0x32 '2' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0x06, /* 00000110 */
	0x1c, /* 00011100 */
	0x30, /* 00110000 */
	0x66, /* 01100110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 51 0x33 '3' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0x06, /* 00000110 */
	0x3c, /* 00111100 */
	0x06, /* 00000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 52 0x34 '4' */
	0x1c, /* 00011100 */
	0x3c, /* 00111100 */
	0x6c, /* 01101100 */
	0xcc, /* 11001100 */
	0xfe, /* 11111110 */
	0x0c, /* 00001100 */
	0x1e, /* 00011110 */
	0x00, /* 00000000 */

	/* 53 0x35 '5' */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xfc, /* 11111100 */
	0x06, /* 00000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 54 0x36 '6' */
	0x38, /* 00111000 */
	0x60, /* 01100000 */
	0xc0, /* 11000000 */
	0xfc, /* 11111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 55 0x37 '7' */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */

	/* 56 0x38 '8' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 57 0x39 '9' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7e, /* 01111110 */
	0x06, /* 00000110 */
	0x0c, /* 00001100 */
	0x78, /* 01111000 */
	0x00, /* 00000000 */

	/* 58 0x3a ':' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 59 0x3b ';' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */

	/* 60 0x3c '<' */
	0x06, /* 00000110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x06, /* 00000110 */
	0x00, /* 00000000 */

	/* 61 0x3d '=' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 62 0x3e '>' */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0x00, /* 00000000 */

	/* 63 0x3f '?' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 64 0x40 '@' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xde, /* 11011110 */
	0xde, /* 11011110 */
	0xde, /* 11011110 */
	0xc0, /* 11000000 */
	0x78, /* 01111000 */
	0x00, /* 00000000 */

	/* 65 0x41 'A' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 66 0x42 'B' */
	0xfc, /* 11111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0xfc, /* 11111100 */
	0x00, /* 00000000 */

	/* 67 0x43 'C' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 68 0x44 'D' */
	0xf8, /* 11111000 */
	0x6c, /* 01101100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x6c, /* 01101100 */
	0xf8, /* 11111000 */
	0x00, /* 00000000 */

	/* 69 0x45 'E' */
	0xfe, /* 11111110 */
	0x62, /* 01100010 */
	0x68, /* 01101000 */
	0x78, /* 01111000 */
	0x68, /* 01101000 */
	0x62, /* 01100010 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 70 0x46 'F' */
	0xfe, /* 11111110 */
	0x62, /* 01100010 */
	0x68, /* 01101000 */
	0x78, /* 01111000 */
	0x68, /* 01101000 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */
	0x00, /* 00000000 */

	/* 71 0x47 'G' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xce, /* 11001110 */
	0x66, /* 01100110 */
	0x3a, /* 00111010 */
	0x00, /* 00000000 */

	/* 72 0x48 'H' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 73 0x49 'I' */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 74 0x4a 'J' */
	0x1e, /* 00011110 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x78, /* 01111000 */
	0x00, /* 00000000 */

	/* 75 0x4b 'K' */
	0xe6, /* 11100110 */
	0x66, /* 01100110 */
	0x6c, /* 01101100 */
	0x78, /* 01111000 */
	0x6c, /* 01101100 */
	0x66, /* 01100110 */
	0xe6, /* 11100110 */
	0x00, /* 00000000 */

	/* 76 0x4c 'L' */
	0xf0, /* 11110000 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0x62, /* 01100010 */
	0x66, /* 01100110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 77 0x4d 'M' */
	0xc6, /* 11000110 */
	0xee, /* 11101110 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0xd6, /* 11010110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 78 0x4e 'N' */
	0xc6, /* 11000110 */
	0xe6, /* 11100110 */
	0xf6, /* 11110110 */
	0xde, /* 11011110 */
	0xce, /* 11001110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 79 0x4f 'O' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 80 0x50 'P' */
	0xfc, /* 11111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */
	0x00, /* 00000000 */

	/* 81 0x51 'Q' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xce, /* 11001110 */
	0x7c, /* 01111100 */
	0x0e, /* 00001110 */

	/* 82 0x52 'R' */
	0xfc, /* 11111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0x6c, /* 01101100 */
	0x66, /* 01100110 */
	0xe6, /* 11100110 */
	0x00, /* 00000000 */

	/* 83 0x53 'S' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 84 0x54 'T' */
	0x7e, /* 01111110 */
	0x7e, /* 01111110 */
	0x5a, /* 01011010 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 85 0x55 'U' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 86 0x56 'V' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 87 0x57 'W' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xd6, /* 11010110 */
	0xd6, /* 11010110 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0x00, /* 00000000 */

	/* 88 0x58 'X' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 89 0x59 'Y' */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 90 0x5a 'Z' */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0x8c, /* 10001100 */
	0x18, /* 00011000 */
	0x32, /* 00110010 */
	0x66, /* 01100110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 91 0x5b '[' */
	0x3c, /* 00111100 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 92 0x5c '\' */
	0xc0, /* 11000000 */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x06, /* 00000110 */
	0x02, /* 00000010 */
	0x00, /* 00000000 */

	/* 93 0x5d ']' */
	0x3c, /* 00111100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 94 0x5e '^' */
	0x10, /* 00010000 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 95 0x5f '_' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */

	/* 96 0x60 '`' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 97 0x61 'a' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 98 0x62 'b' */
	0xe0, /* 11100000 */
	0x60, /* 01100000 */
	0x7c, /* 01111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */

	/* 99 0x63 'c' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc0, /* 11000000 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 100 0x64 'd' */
	0x1c, /* 00011100 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 101 0x65 'e' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 102 0x66 'f' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x60, /* 01100000 */
	0xf8, /* 11111000 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */
	0x00, /* 00000000 */

	/* 103 0x67 'g' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x7c, /* 01111100 */
	0x0c, /* 00001100 */
	0xf8, /* 11111000 */

	/* 104 0x68 'h' */
	0xe0, /* 11100000 */
	0x60, /* 01100000 */
	0x6c, /* 01101100 */
	0x76, /* 01110110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0xe6, /* 11100110 */
	0x00, /* 00000000 */

	/* 105 0x69 'i' */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 106 0x6a 'j' */
	0x06, /* 00000110 */
	0x00, /* 00000000 */
	0x06, /* 00000110 */
	0x06, /* 00000110 */
	0x06, /* 00000110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */

	/* 107 0x6b 'k' */
	0xe0, /* 11100000 */
	0x60, /* 01100000 */
	0x66, /* 01100110 */
	0x6c, /* 01101100 */
	0x78, /* 01111000 */
	0x6c, /* 01101100 */
	0xe6, /* 11100110 */
	0x00, /* 00000000 */

	/* 108 0x6c 'l' */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 109 0x6d 'm' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xec, /* 11101100 */
	0xfe, /* 11111110 */
	0xd6, /* 11010110 */
	0xd6, /* 11010110 */
	0xd6, /* 11010110 */
	0x00, /* 00000000 */

	/* 110 0x6e 'n' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xdc, /* 11011100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */

	/* 111 0x6f 'o' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 112 0x70 'p' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xdc, /* 11011100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */

	/* 113 0x71 'q' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x7c, /* 01111100 */
	0x0c, /* 00001100 */
	0x1e, /* 00011110 */

	/* 114 0x72 'r' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xdc, /* 11011100 */
	0x76, /* 01110110 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */
	0x00, /* 00000000 */

	/* 115 0x73 's' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x06, /* 00000110 */
	0xfc, /* 11111100 */
	0x00, /* 00000000 */

	/* 116 0x74 't' */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0xfc, /* 11111100 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x36, /* 00110110 */
	0x1c, /* 00011100 */
	0x00, /* 00000000 */

	/* 117 0x75 'u' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 118 0x76 'v' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 119 0x77 'w' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xd6, /* 11010110 */
	0xd6, /* 11010110 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0x00, /* 00000000 */

	/* 120 0x78 'x' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 121 0x79 'y' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7e, /* 01111110 */
	0x06, /* 00000110 */
	0xfc, /* 11111100 */

	/* 122 0x7a 'z' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x4c, /* 01001100 */
	0x18, /* 00011000 */
	0x32, /* 00110010 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 123 0x7b '{' */
	0x0e, /* 00001110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x70, /* 01110000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x0e, /* 00001110 */
	0x00, /* 00000000 */

	/* 124 0x7c '|' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 125 0x7d '}' */
	0x70, /* 01110000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x0e, /* 00001110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x70, /* 01110000 */
	0x00, /* 00000000 */

	/* 126 0x7e '~' */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 127 0x7f '' */
	0x00, /* 00000000 */
	0x10, /* 00010000 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 128 0x80 '�' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x0c, /* 00001100 */
	0x78, /* 01111000 */

	/* 129 0x81 '�' */
	0xcc, /* 11001100 */
	0x00, /* 00000000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 130 0x82 '�' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 131 0x83 '�' */
	0x7c, /* 01111100 */
	0x82, /* 10000010 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 132 0x84 '�' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 133 0x85 '�' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 134 0x86 '�' */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 135 0x87 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x7e, /* 01111110 */
	0x0c, /* 00001100 */
	0x38, /* 00111000 */

	/* 136 0x88 '�' */
	0x7c, /* 01111100 */
	0x82, /* 10000010 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 137 0x89 '�' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 138 0x8a '�' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 139 0x8b '�' */
	0x66, /* 01100110 */
	0x00, /* 00000000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 140 0x8c '�' */
	0x7c, /* 01111100 */
	0x82, /* 10000010 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 141 0x8d '�' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 142 0x8e '�' */
	0xc6, /* 11000110 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 143 0x8f '�' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 144 0x90 '�' */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0xf8, /* 11111000 */
	0xc0, /* 11000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 145 0x91 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0xd8, /* 11011000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 146 0x92 '�' */
	0x3e, /* 00111110 */
	0x6c, /* 01101100 */
	0xcc, /* 11001100 */
	0xfe, /* 11111110 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xce, /* 11001110 */
	0x00, /* 00000000 */

	/* 147 0x93 '�' */
	0x7c, /* 01111100 */
	0x82, /* 10000010 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 148 0x94 '�' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 149 0x95 '�' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 150 0x96 '�' */
	0x78, /* 01111000 */
	0x84, /* 10000100 */
	0x00, /* 00000000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 151 0x97 '�' */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 152 0x98 '�' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7e, /* 01111110 */
	0x06, /* 00000110 */
	0xfc, /* 11111100 */

	/* 153 0x99 '�' */
	0xc6, /* 11000110 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 154 0x9a '�' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 155 0x9b '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 156 0x9c '�' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x64, /* 01100100 */
	0xf0, /* 11110000 */
	0x60, /* 01100000 */
	0x66, /* 01100110 */
	0xfc, /* 11111100 */
	0x00, /* 00000000 */

	/* 157 0x9d '�' */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 158 0x9e '�' */
	0xf8, /* 11111000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xfa, /* 11111010 */
	0xc6, /* 11000110 */
	0xcf, /* 11001111 */
	0xc6, /* 11000110 */
	0xc7, /* 11000111 */

	/* 159 0x9f '�' */
	0x0e, /* 00001110 */
	0x1b, /* 00011011 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0xd8, /* 11011000 */
	0x70, /* 01110000 */
	0x00, /* 00000000 */

	/* 160 0xa0 '�' */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 161 0xa1 '�' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 162 0xa2 '�' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 163 0xa3 '�' */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 164 0xa4 '�' */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0xdc, /* 11011100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */

	/* 165 0xa5 '�' */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0xe6, /* 11100110 */
	0xf6, /* 11110110 */
	0xde, /* 11011110 */
	0xce, /* 11001110 */
	0x00, /* 00000000 */

	/* 166 0xa6 '�' */
	0x3c, /* 00111100 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x3e, /* 00111110 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 167 0xa7 '�' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 168 0xa8 '�' */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x63, /* 01100011 */
	0x3e, /* 00111110 */
	0x00, /* 00000000 */

	/* 169 0xa9 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 170 0xaa '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x06, /* 00000110 */
	0x06, /* 00000110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 171 0xab '�' */
	0x63, /* 01100011 */
	0xe6, /* 11100110 */
	0x6c, /* 01101100 */
	0x7e, /* 01111110 */
	0x33, /* 00110011 */
	0x66, /* 01100110 */
	0xcc, /* 11001100 */
	0x0f, /* 00001111 */

	/* 172 0xac '�' */
	0x63, /* 01100011 */
	0xe6, /* 11100110 */
	0x6c, /* 01101100 */
	0x7a, /* 01111010 */
	0x36, /* 00110110 */
	0x6a, /* 01101010 */
	0xdf, /* 11011111 */
	0x06, /* 00000110 */

	/* 173 0xad '�' */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 174 0xae '�' */
	0x00, /* 00000000 */
	0x33, /* 00110011 */
	0x66, /* 01100110 */
	0xcc, /* 11001100 */
	0x66, /* 01100110 */
	0x33, /* 00110011 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 175 0xaf '�' */
	0x00, /* 00000000 */
	0xcc, /* 11001100 */
	0x66, /* 01100110 */
	0x33, /* 00110011 */
	0x66, /* 01100110 */
	0xcc, /* 11001100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 176 0xb0 '�' */
	0x22, /* 00100010 */
	0x88, /* 10001000 */
	0x22, /* 00100010 */
	0x88, /* 10001000 */
	0x22, /* 00100010 */
	0x88, /* 10001000 */
	0x22, /* 00100010 */
	0x88, /* 10001000 */

	/* 177 0xb1 '�' */
	0x55, /* 01010101 */
	0xaa, /* 10101010 */
	0x55, /* 01010101 */
	0xaa, /* 10101010 */
	0x55, /* 01010101 */
	0xaa, /* 10101010 */
	0x55, /* 01010101 */
	0xaa, /* 10101010 */

	/* 178 0xb2 '�' */
	0x77, /* 01110111 */
	0xdd, /* 11011101 */
	0x77, /* 01110111 */
	0xdd, /* 11011101 */
	0x77, /* 01110111 */
	0xdd, /* 11011101 */
	0x77, /* 01110111 */
	0xdd, /* 11011101 */

	/* 179 0xb3 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 180 0xb4 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 181 0xb5 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 182 0xb6 '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf6, /* 11110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 183 0xb7 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 184 0xb8 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 185 0xb9 '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf6, /* 11110110 */
	0x06, /* 00000110 */
	0xf6, /* 11110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 186 0xba '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 187 0xbb '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x06, /* 00000110 */
	0xf6, /* 11110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 188 0xbc '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf6, /* 11110110 */
	0x06, /* 00000110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 189 0xbd '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 190 0xbe '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 191 0xbf '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 192 0xc0 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 193 0xc1 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 194 0xc2 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 195 0xc3 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 196 0xc4 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 197 0xc5 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 198 0xc6 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 199 0xc7 '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x37, /* 00110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 200 0xc8 '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x37, /* 00110111 */
	0x30, /* 00110000 */
	0x3f, /* 00111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 201 0xc9 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x3f, /* 00111111 */
	0x30, /* 00110000 */
	0x37, /* 00110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 202 0xca '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf7, /* 11110111 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 203 0xcb '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0xf7, /* 11110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 204 0xcc '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x37, /* 00110111 */
	0x30, /* 00110000 */
	0x37, /* 00110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 205 0xcd '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 206 0xce '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf7, /* 11110111 */
	0x00, /* 00000000 */
	0xf7, /* 11110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 207 0xcf '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 208 0xd0 '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 209 0xd1 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 210 0xd2 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 211 0xd3 '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x3f, /* 00111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 212 0xd4 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 213 0xd5 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 214 0xd6 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x3f, /* 00111111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 215 0xd7 '�' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xff, /* 11111111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 216 0xd8 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 217 0xd9 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 218 0xda '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 219 0xdb '�' */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */

	/* 220 0xdc '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */

	/* 221 0xdd '�' */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */

	/* 222 0xde '�' */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */

	/* 223 0xdf '�' */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 224 0xe0 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0xc8, /* 11001000 */
	0xdc, /* 11011100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 225 0xe1 '�' */
	0x78, /* 01111000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xd8, /* 11011000 */
	0xcc, /* 11001100 */
	0xc6, /* 11000110 */
	0xcc, /* 11001100 */
	0x00, /* 00000000 */

	/* 226 0xe2 '�' */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x00, /* 00000000 */

	/* 227 0xe3 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x00, /* 00000000 */

	/* 228 0xe4 '�' */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 229 0xe5 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0xd8, /* 11011000 */
	0xd8, /* 11011000 */
	0xd8, /* 11011000 */
	0x70, /* 01110000 */
	0x00, /* 00000000 */

	/* 230 0xe6 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0xc0, /* 11000000 */

	/* 231 0xe7 '�' */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 232 0xe8 '�' */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */

	/* 233 0xe9 '�' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 234 0xea '�' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0xee, /* 11101110 */
	0x00, /* 00000000 */

	/* 235 0xeb '�' */
	0x0e, /* 00001110 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x3e, /* 00111110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 236 0xec '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0xdb, /* 11011011 */
	0xdb, /* 11011011 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 237 0xed '�' */
	0x06, /* 00000110 */
	0x0c, /* 00001100 */
	0x7e, /* 01111110 */
	0xdb, /* 11011011 */
	0xdb, /* 11011011 */
	0x7e, /* 01111110 */
	0x60, /* 01100000 */
	0xc0, /* 11000000 */

	/* 238 0xee '�' */
	0x1e, /* 00011110 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0x7e, /* 01111110 */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x1e, /* 00011110 */
	0x00, /* 00000000 */

	/* 239 0xef '�' */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 240 0xf0 '�' */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 241 0xf1 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 242 0xf2 '�' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 243 0xf3 '�' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 244 0xf4 '�' */
	0x0e, /* 00001110 */
	0x1b, /* 00011011 */
	0x1b, /* 00011011 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 245 0xf5 '�' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xd8, /* 11011000 */
	0xd8, /* 11011000 */
	0x70, /* 01110000 */

	/* 246 0xf6 '�' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 247 0xf7 '�' */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 248 0xf8 '�' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 249 0xf9 '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 250 0xfa '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 251 0xfb '�' */
	0x0f, /* 00001111 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0xec, /* 11101100 */
	0x6c, /* 01101100 */
	0x3c, /* 00111100 */
	0x1c, /* 00011100 */

	/* 252 0xfc '�' */
	0x6c, /* 01101100 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 253 0xfd '�' */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 254 0xfe '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 255 0xff '�' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

};
/* end of font_8x8.c */


/* beagle-tester.c */
#define MODEL_BOARD "TI OMAP3 BeagleBoard"
#define MODEL_XM    "TI OMAP3 BeagleBoard xM"
#define MODEL_X15   "TI AM5728 BeagleBoard-X15"
#define MODEL_BONE  "TI AM335x BeagleBone"
#define MODEL_BLACK "TI AM335x BeagleBone Black"
#define MODEL_WIFI  "TI AM335x BeagleBone Black Wireless"
#define MODEL_BLUE  "TI AM335x BeagleBone Blue"
#define MODEL_OSD3358_BSM_REF "Octavo Systems OSD3358-SM-RED"
#define COLOR_TEXT 0xffffffu
#define COLOR_PASS 0x00ff00u
#define COLOR_FAIL 0xff0000u
#define SCAN_VALUE_REPEAT "BURN-IN"
#define SCAN_VALUE_COLORBAR "COLORBAR"
#define SCAN_VALUE_STOP "STOP"
int fail = 0;
int notice_line = 0;
int display = 1;

void beagle_test(const char *scan_value);
void beagle_notice(const char *test, const char *status);
void do_colorbar();
int blue_specific_tests();
int osd3358_sm_ref_design_tests();
void set_led_trigger(const char * led, const char * mode);
void set_user_leds(int code);

struct cape
{
	const char prefix[5];
	const char id_str[21];
	int eeprom_addr;
	const char name[33];
	int (*test)(const char *scan_value, unsigned id);
};

int test_comms_cape(const char *scan_value, unsigned id);
int test_display18_cape(const char *scan_value, unsigned id);
int test_display50_cape(const char *scan_value, unsigned id);
int test_display70_cape(const char *scan_value, unsigned id);
int test_load_cape(const char *scan_value, unsigned id);
int test_motor_cape(const char *scan_value, unsigned id);
int test_power_cape(const char *scan_value, unsigned id);
int test_proto_cape(const char *scan_value, unsigned id);
int test_relay_cape(const char *scan_value, unsigned id);
int test_robotics_cape(const char *scan_value, unsigned id);
int test_servo_cape(const char *scan_value, unsigned id);
void install_overlay(const char *scan_value, const char *id_str);

/* Per https://github.com/beagleboard/capes/blob/master/README.mediawiki */
static struct cape capes[] = {
	{ "BC00", "BBORG_COMMS", 0x56, "Industrial Comms Cape", test_comms_cape },
	{ "BC01", "BBORG_DISPLAY18", 0x57, "1.8\" Display Cape", test_display18_cape },
	{ "BC02", "BBORG_DISPLAY50", 0x57, "5\" Display Cape", test_display50_cape },
	{ "BC03", "BBORG_DISPLAY70", 0x57, "7\" Display Cape", test_display70_cape },
	{ "BC04", "BBORG_LOAD", 0x54, "Load Driver Cape", test_load_cape },
	{ "BC05", "BBORG_MOTOR", 0x55, "Motor Driver Cape", test_motor_cape },
	{ "BC06", "BBORG_POWER", 0, "Power Supply Cape", test_power_cape },
	{ "BC07", "BBORG_PROTO", 0x54, "Prototyping Cape", test_proto_cape },
	{ "BC08", "BBORG_RELAY", 0x54, "Relay Cape", test_relay_cape },
	{ "BC09", "BBORG_ROBOTICS", 0, "Robotics Cape", test_robotics_cape },
	{ "BC0A", "BBORG_SERVO", 0x55, "Servo Driver Capes", test_servo_cape },
};

/* Per https://github.com/beagleboard/beaglebone-black/wiki/System-Reference-Manual#824-eeprom-data-format */
const char cape_eeprom[88] = {
	0xAA, 0x55, 0x33, 0xEE,		/* header (4) */
	0x41, 0x31,			/* format version (2) */
	0, 0, 0, 0, 0, 0, 0, 0,		/* board name (32)@6 */
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0x30, 0x30, 0x30, 0x30,		/* board version (4)@38 */
	'B', 'e', 'a', 'g', 'l', 'e',	/* manufacturer (16)@42 */
	'B', 'o', 'a', 'r', 'd', '.',
	'o', 'r', 'g', 0,
	0, 0, 0, 0, 0, 0, 0, 0,		/* part number (16)@58 */
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0,				/* number of pins (2)@60 */
	0, 0, 0, 0, 0, 0, 0, 0,		/* serial number (12)@76 */
	0, 0, 0, 0
};

static void do_stop()
{
	rc_set_state(EXITING);
}

int main(int argc, char** argv)
{
	unsigned short barcode_id[4];
	int barcode = open("/dev/input/beagle-barcode", O_RDONLY);
	fd_set rdfs;
	struct input_event ev[256];
	int i, n, rd = 0, run = 0;
	struct timeval timeout;
	char scan_value[32];
	int scan_i = 0;

	fprintf(stderr, "Starting beagle-tester.\n");
	fflush(stderr);

	if (!barcode) {
		fprintf(stderr, "ERROR: valid barcode scanner not found.\n");
		fflush(stderr);
	}

	//FILE *errlog = fopen("/var/log/beagle-tester.log", "w");
	ioctl(barcode, EVIOCGID, barcode_id);
	fprintf(stderr, "Found input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
		barcode_id[ID_BUS], barcode_id[ID_VENDOR], barcode_id[ID_PRODUCT], barcode_id[ID_VERSION]);
	fflush(stderr);

	system("/usr/sbin/beagle-tester-open.sh");
	set_led_trigger("red", "default-on");
	set_led_trigger("green", "default-on");
	set_led_trigger("beaglebone:green:usr0", "default-on");
	set_led_trigger("beaglebone:green:usr1", "default-on");
	set_led_trigger("beaglebone:green:usr2", "default-on");
	set_led_trigger("beaglebone:green:usr3", "default-on");

	if (access("/dev/fb0", W_OK)) {
		fprintf(stderr, "Unable to write to /dev/fb0\n");
		fflush(stderr);
		display = 0;
	} else {
		fb_open(0, &fb_info);
		do_colorbar();
	}

	signal(SIGINT, do_stop);
	signal(SIGTERM, do_stop);

	while (rc_get_state()!=EXITING) {
		FD_ZERO(&rdfs);
		FD_SET(barcode, &rdfs);
		timeout.tv_sec = 0;
		timeout.tv_usec = 4000;
		rd = select(barcode + 1, &rdfs, NULL, NULL, &timeout);
		if (rc_get_state()==EXITING)
			break;
		if (rd > 0) {
			rd = read(barcode, ev, sizeof(ev));
		}

		for (i = 0; i < rd; i += (int)sizeof(struct input_event)) {
			unsigned int type, code, value;

			if ((rd % (int) sizeof(struct input_event)) != 0) {
				//fprintf(stderr, "Data too small: %d\n", rd); fflush(stderr);
				break;
			}

			n = i / (int) sizeof(struct input_event);

			type = ev[n].type;
			code = ev[n].code;
			value = ev[n].value;

			//fprintf(stderr, "Event: %02x %02x %06x\n", type, code, value); fflush(stderr);

			if ((type == 1) && (value == 1)) {
				switch (code) {
				default:
					break;
				case KEY_ENTER:
					scan_value[scan_i] = 0;
					fprintf(stderr, "Got scanned value: %s\n", scan_value);
					fflush(stderr);
					run = 1;
					scan_i = 0;
					fail = 0;
					break;
				case KEY_MINUS:
					scan_value[scan_i] = '-';
					scan_i++;
					break;
				case KEY_0:
					scan_value[scan_i] = '0';
					scan_i++;
					break;
				case KEY_1:
					scan_value[scan_i] = '1';
					scan_i++;
					break;
				case KEY_2:
					scan_value[scan_i] = '2';
					scan_i++;
					break;
				case KEY_3:
					scan_value[scan_i] = '3';
					scan_i++;
					break;
				case KEY_4:
					scan_value[scan_i] = '4';
					scan_i++;
					break;
				case KEY_5:
					scan_value[scan_i] = '5';
					scan_i++;
					break;
				case KEY_6:
					scan_value[scan_i] = '6';
					scan_i++;
					break;
				case KEY_7:
					scan_value[scan_i] = '7';
					scan_i++;
					break;
				case KEY_8:
					scan_value[scan_i] = '8';
					scan_i++;
					break;
				case KEY_9:
					scan_value[scan_i] = '9';
					scan_i++;
					break;
				case KEY_A:
					scan_value[scan_i] = 'A';
					scan_i++;
					break;
				case KEY_B:
					scan_value[scan_i] = 'B';
					scan_i++;
					break;
				case KEY_C:
					scan_value[scan_i] = 'C';
					scan_i++;
					break;
				case KEY_D:
					scan_value[scan_i] = 'D';
					scan_i++;
					break;
				case KEY_E:
					scan_value[scan_i] = 'E';
					scan_i++;
					break;
				case KEY_F:
					scan_value[scan_i] = 'F';
					scan_i++;
					break;
				case KEY_G:
					scan_value[scan_i] = 'G';
					scan_i++;
					break;
				case KEY_H:
					scan_value[scan_i] = 'H';
					scan_i++;
					break;
				case KEY_I:
					scan_value[scan_i] = 'I';
					scan_i++;
					break;
				case KEY_J:
					scan_value[scan_i] = 'J';
					scan_i++;
					break;
				case KEY_K:
					scan_value[scan_i] = 'K';
					scan_i++;
					break;
				case KEY_L:
					scan_value[scan_i] = 'L';
					scan_i++;
					break;
				case KEY_M:
					scan_value[scan_i] = 'M';
					scan_i++;
					break;
				case KEY_N:
					scan_value[scan_i] = 'N';
					scan_i++;
					break;
				case KEY_O:
					scan_value[scan_i] = 'O';
					scan_i++;
					break;
				case KEY_P:
					scan_value[scan_i] = 'P';
					scan_i++;
					break;
				case KEY_Q:
					scan_value[scan_i] = 'Q';
					scan_i++;
					break;
				case KEY_R:
					scan_value[scan_i] = 'R';
					scan_i++;
					break;
				case KEY_S:
					scan_value[scan_i] = 'S';
					scan_i++;
					break;
				case KEY_T:
					scan_value[scan_i] = 'T';
					scan_i++;
					break;
				case KEY_U:
					scan_value[scan_i] = 'U';
					scan_i++;
					break;
				case KEY_V:
					scan_value[scan_i] = 'V';
					scan_i++;
					break;
				case KEY_W:
					scan_value[scan_i] = 'W';
					scan_i++;
					break;
				case KEY_X:
					scan_value[scan_i] = 'X';
					scan_i++;
					break;
				case KEY_Y:
					scan_value[scan_i] = 'Y';
					scan_i++;
					break;
				case KEY_Z:
					scan_value[scan_i] = 'Z';
					scan_i++;
					break;
				}
			}
			//fprintf(stderr, "*"); fflush(stderr);
		}

		if(scan_i == 0 && argc > 1) {
			strcpy(scan_value, argv[argc-1]);
			run = 1;
			argc--;
		}

		if (run == 0) {
			if (display) do_colorbar();
		}
		if (!strcmp(scan_value, SCAN_VALUE_STOP)) {
			rc_set_state(EXITING);
			break;
		} else if (run == 1) {
			if (display) do_fill_screen(&fb_info, 0);
			beagle_test(scan_value);
			fprintf(stderr, "Test fails: %d\n", fail);
			fflush(stderr);
			if (fail > 0) {
				printf("RESULT: \033[41;30;5m FAIL \033[0m\n");
			} else {
 				printf("RESULT: PASS \n");
			}
			if (rc_get_state()==EXITING) {
				run = 0;
				break;	
			} else if (!strcmp(scan_value, SCAN_VALUE_REPEAT)) {
				// pause 4 seconds and run again
				sleep(4);
			} else {
				memset(scan_value, 0, sizeof(scan_value));
				run = 2;
			}
		}
	}

	if (display) do_fill_screen(&fb_info, 4);
	set_user_leds(-1);
	system("/usr/sbin/beagle-tester-close.sh");
	set_led_trigger("red", "none");
	set_led_trigger("green", "none");

	return 0;
}

void beagle_test(const char *scan_value)
{
	int r;
	int fd_sn;
	char str[70];
	char str2[50];
	char wlan0_ap[50];
	char wlan0_host[50];
        int len, off;
	char model[70];
	FILE *fp;
	unsigned x, y;
	unsigned color;

	notice_line = 0;
	beagle_notice("scan", scan_value);

	beagle_notice("tester", "$Id$");

	if(!strncmp(scan_value, "BC", 2)) {
		for(x = 0; x < sizeof(capes) / sizeof(capes[0]); x++) {
			if(!strncmp(scan_value, capes[x].prefix, 4)) {
				fail = capes[x].test(scan_value, x);
				goto done;
			}
		}
	}

	fp = fopen("/proc/device-tree/model", "r");
	fgets(str, sizeof(str), fp);
	fclose(fp);
	strcpy(model, str);
	beagle_notice("model", str);

	fd_sn = open("/sys/bus/i2c/devices/i2c-0/0-0050/0-00500/nvmem", O_RDWR);
	lseek(fd_sn, 0, SEEK_SET);
	r = read(fd_sn, str, 28);
	str[28] = 0;
	beagle_notice("eeprom", str);

	fp = fopen("/etc/dogtag", "r");
	fgets(str, sizeof(str), fp);
	fclose(fp);
        len = strlen(str);
	len--; str[len] = 0; // remove trailing character
        off = (len > 25) ? len-25 : 0;
	beagle_notice("dogtag", &str[off]);

	strcpy(str, "memtester 1M 1 > /dev/null");
	fprintf(stderr, str);
	fprintf(stderr, "\n");
	fflush(stderr);
	r = system(str);
	beagle_notice("memory", r ? "fail" : "pass");

	// if we have WiFi
	if(!strcmp(model, MODEL_WIFI) || !strcmp(model, MODEL_BLUE)) {
		// connect to ap
		system("bb-connect-ap > /tmp/beagle-tester-ap");
		fp = fopen("/tmp/beagle-tester-ap", "r");
		if (fp != NULL) {
			fgets(str2, sizeof(str2)-1, fp);
			str2[25] = 0;
			fclose(fp);
		} else {
			str2[0] = 0;
		}
		beagle_notice("ap", str2);

		// fetch wlan0 address
		system("ip -4 addr show wlan0 | grep inet | awk '{print $2}' | cut -d/ -f1 | tr -d '\n' | tr -d '\r' > /tmp/beagle-tester-wlan0-host");
		fp = fopen("/tmp/beagle-tester-wlan0-host", "r");
		if (fp != NULL) {
			fgets(wlan0_host, sizeof(wlan0_host)-1, fp);
			fclose(fp);
		} else {
			wlan0_host[0] = 0;
		}

		// fetch wlan0 gateway
		system("ip route | grep -E 'wlan0|link' | grep -Ev 'src|default' | awk '{print $1}' > /tmp/beagle-tester-wlan0-gw");
		fp = fopen("/tmp/beagle-tester-wlan0-gw", "r");
		if (fp != NULL) {
			fgets(wlan0_ap, sizeof(wlan0_ap)-1, fp);
			fclose(fp);
		} else {
			wlan0_ap[0] = 0;
		}

		sprintf(str, "ping -c 4 -w 10 -I %s %s",
			wlan0_host, wlan0_ap);
		fprintf(stderr, str);
		fprintf(stderr, "\n");
		fflush(stderr);
		r = system(str);
		fprintf(stderr, "ping returned: %d\n", r);
		beagle_notice("wifi", r ? "fail" : "pass");
	} else { // Ethernet
		fp = popen("ip route get 1.1.1.1 | perl -n -e 'print $1 if /via (.*) dev/'",
			 "r"); // fetch gateway
		if (fp != NULL) {
			fgets(str2, sizeof(str2)-1, fp);
			pclose(fp);
		} else {
			str2[0] = 0;
		}
		sprintf(str, "ping -s 8184 -i 0.01 -q -c 150 -w 2 -I eth0 %s",
			 str2);
		fprintf(stderr, str);
		fprintf(stderr, "\n");
		fflush(stderr);
		r = system(str);
		beagle_notice("ethernet", r ? "fail" : "pass");
	}

	// if BeagleBoard-xM
	if(!strcmp(model, MODEL_XM)) {
		sprintf(str, "ping -s 8184 -i 0.01 -q -c 90 -w 2 -I 192.168.9.2 192.168.9.1");
	} else {
		sprintf(str, "ping -s 8184 -i 0.01 -q -c 150 -w 2 -I 192.168.7.2 192.168.7.1");
	}
	fprintf(stderr, str);
	fprintf(stderr, "\n");
	fflush(stderr);
	r = system(str);
	beagle_notice("usb dev", r ? "fail" : "pass");

	// if BeagleBoard-xM
	if(!strcmp(model, MODEL_XM)) {
		sprintf(str, "timeout 8 hdparm -q -t --direct /dev/sda");
		fprintf(stderr, str);
		fprintf(stderr, "\n");
		fflush(stderr);
		r = system(str);
		beagle_notice("usb 1", r ? "fail" : "pass");

		sprintf(str, "timeout 8 hdparm -q -t --direct /dev/sdb");
		fprintf(stderr, str);
		fprintf(stderr, "\n");
		fflush(stderr);
		r = system(str);
		beagle_notice("usb 2", r ? "fail" : "pass");

		sprintf(str, "timeout 8 hdparm -q -t --direct /dev/sdc");
		fprintf(stderr, str);
		fprintf(stderr, "\n");
		fflush(stderr);
		r = system(str);
		beagle_notice("usb 3", r ? "fail" : "pass");
	}

	// if BeagleBone Blue
	if(!strcmp(model, MODEL_BLUE)) {
		r = blue_specific_tests();
		beagle_notice("sensors", r ? "fail" : "pass");
	}
	
	// If OSD3358-SM-RED
	if(!strcmp(model, MODEL_OSD3358_BSM_REF)){
		r = osd3358_sm_ref_design_tests();
		beagle_notice("OSD3358-SM Reference Design board components", r ? "fail" : "pass");
	}

	// if not xM, didn't fail and we aren't in repeat mode
	if(strcmp(model, MODEL_XM) && 
			!fail && strcmp(scan_value, SCAN_VALUE_REPEAT)) {
		lseek(fd_sn, 0, SEEK_SET);
		r = read(fd_sn, str, 12);
		memcpy(&str[12], scan_value, 16);
		str[28] = 0;
		lseek(fd_sn, 0, SEEK_SET);
		r = write(fd_sn, str, 28);
		lseek(fd_sn, 0, SEEK_SET);
		r = read(fd_sn, str2, 28);
		str2[28] = 0;
		beagle_notice("eeprom", str2);
		fail = strcmp(str, str2) ? 1 : 0;
		beagle_notice("eeprom", fail ? "fail" : "pass");
	}

	close(fd_sn);

done:
	color = fail ? COLOR_FAIL : COLOR_PASS;
	if (display) {
		for (y = fb_info.var.yres/2; y < fb_info.var.yres; y++) {
			for (x = fb_info.var.xres/2; x < fb_info.var.xres; x++)
				draw_pixel(&fb_info, x, y, color);
		}
	}

	if (!strcmp(model, MODEL_BLUE)) {
		if (fail) {
			set_led_trigger("red", "timer");
			set_led_trigger("green", "none");
		} else {
			set_led_trigger("red", "none");
			set_led_trigger("green", "timer");
		}
	}

	if (!fail) {
		set_led_trigger("beaglebone:green:usr0", "default-on");
		set_led_trigger("beaglebone:green:usr1", "default-on");
		set_led_trigger("beaglebone:green:usr2", "default-on");
		set_led_trigger("beaglebone:green:usr3", "default-on");
	}
}

void beagle_notice(const char *test, const char *status)
{
	const char *fmt = "%8.8s: %-25.25s";
	unsigned color = COLOR_TEXT;
	char str[70];

	set_user_leds(notice_line);

	if(!strcmp(status, "fail")) {
		fail++;
		color = COLOR_FAIL;
	}
	sprintf(str, fmt, test, status);
	fprintf(stderr, str);
	fprintf(stderr, "\n");
	fflush(stderr);
	if (display)
		fb_put_string(&fb_info, 20, 50+notice_line*10, str, 70, color, 1, 70);
	notice_line++;
}

void do_colorbar()
{
	static int init = 0;
	static int cur_x = 0, cur_dir = 0;
	int x, y;

	if (!init) {
		if (fb_info.var.xres == 1280 && fb_info.var.bits_per_pixel == 32)
			system("xzcat /usr/share/beagle-tester/itu-r-bt1729-colorbar-1280x1024-32.raw.xz > /dev/fb0");
		else if (fb_info.var.xres == 1024 && fb_info.var.bits_per_pixel == 32)
			system("xzcat /usr/share/beagle-tester/itu-r-bt1729-colorbar-1024x768-32.raw.xz > /dev/fb0");
		else if (fb_info.var.xres == 320)
			system("xzcat /usr/share/beagle-tester/itu-r-bt1729-colorbar-320x240.raw.xz > /dev/fb0");
		else if (fb_info.var.xres == 480)
			system("xzcat /usr/share/beagle-tester/itu-r-bt1729-colorbar-480x272.raw.xz > /dev/fb0");
		else if (fb_info.var.xres == 800)
			system("xzcat /usr/share/beagle-tester/itu-r-bt1729-colorbar-800x600.raw.xz > /dev/fb0");
		else if (fb_info.var.xres == 1024)
			system("xzcat /usr/share/beagle-tester/itu-r-bt1729-colorbar-1024x768.raw.xz > /dev/fb0");
		else if (fb_info.var.xres == 1280)
			system("xzcat /usr/share/beagle-tester/itu-r-bt1729-colorbar-1280x720.raw.xz > /dev/fb0");
		else if (fb_info.var.xres == 1360)
			system("xzcat /usr/share/beagle-tester/itu-r-bt1729-colorbar-1360x768.raw.xz > /dev/fb0");
		else if (fb_info.var.xres == 1920)
			system("xzcat /usr/share/beagle-tester/itu-r-bt1729-colorbar-1920x1080.raw.xz > /dev/fb0");
		else
			system("cat /dev/zero > /dev/fb0");
		init = 1;
	}

	for (x = cur_x; x < cur_x+4; x++)
		for (y = (388*(int)fb_info.var.yres)/480; y < (405*(int)fb_info.var.yres)/480; y++)
			draw_pixel(&fb_info, x+(int)fb_info.var.xres/2, y, 0x000000);

	if (cur_dir == 0) {
		cur_x++;
		if (cur_x >= 56*(int)fb_info.var.xres/400) cur_dir = 1;
	} else {
		cur_x--;
		if (cur_x <= -58*(int)fb_info.var.xres/400) cur_dir = 0;
	}

	for (x = cur_x; x < cur_x+4; x++)
		for (y = (388*(int)fb_info.var.yres)/480; y < (405*(int)fb_info.var.yres)/480; y++)
			draw_pixel(&fb_info, x+(int)fb_info.var.xres/2, y, 0xffffff);
	
	//usleep(4444);
}

int blue_specific_tests() {
	int ret;

	// use defaults for now, except also enable magnetometer.
	float v;
	rc_mpu_data_t data;
	rc_mpu_config_t conf = rc_mpu_default_config();
	conf.i2c_bus = 2;
	conf.gpio_interrupt_pin = 117;
	conf.enable_magnetometer = 1;

	if(rc_adc_init()){
		fprintf(stderr, "ERROR: rc_adc_init() failed to initialize adc\n");
		return -1;
	}

	// check charger by checking for the right voltage on the batt line
	v = rc_adc_batt();
	fprintf(stderr, "battery input/charger voltage: %.2fV\n", v);
	if(v>10.0 || v<6.0) {
		fprintf(stderr, "ERROR: battery input voltage out of spec\n");
		rc_adc_cleanup();
		return -1;
	}

	// make sure 12V DC supply is connected
	v = rc_adc_dc_jack();
	fprintf(stderr, "dc jack input voltage: %.2fV\n", v);
	if(v<10.0) {
		fprintf(stderr, "ERROR: dc jack voltage too low\n");
		rc_adc_cleanup();
		return -2;
	}

	rc_adc_cleanup();

	// test imu
	ret = rc_mpu_initialize_dmp(&data, conf);
	rc_mpu_power_off();
	if(ret<0) {
		fprintf(stderr, "failed: mpu9250 imu\n");
		return -3;
	}

	// test barometer
	ret = rc_bmp_init(BMP_OVERSAMPLE_16,BMP_FILTER_OFF);
	rc_bmp_power_off();
	if(ret<0) {
		fprintf(stderr, "failed: bmp280 barometer\n");
		rc_mpu_power_off();
		return -4;
	}

	//cleanup_cape();
	return 0;
}

void set_led_trigger(const char * led, const char * mode)
{
	int fd;
	char path[100];
	int mode_len;

	sprintf(path, "/sys/class/leds/%s/trigger", led);
	mode_len = strlen(mode);
	fd = open(path, O_WRONLY);
	if(!fd) return;
	write(fd, mode, mode_len);
	close(fd);
}

void set_user_leds(int code)
{
	if (code < 0) {
		set_led_trigger("beaglebone:green:usr0", "heartbeat");
		set_led_trigger("beaglebone:green:usr1", "mmc0");
		//set_led_trigger("beaglebone:green:usr2", "cpu0"); -- seems to no longer exist
		set_led_trigger("beaglebone:green:usr2", "none");
		set_led_trigger("beaglebone:green:usr3", "mmc1");
	} else {
		set_led_trigger("beaglebone:green:usr0", (code & 1) ? "timer" : "none");
		set_led_trigger("beaglebone:green:usr1", (code & 2) ? "timer" : "none");
		set_led_trigger("beaglebone:green:usr2", (code & 4) ? "timer" : "none");
		set_led_trigger("beaglebone:green:usr3", (code & 8) ? "timer" : "none");
	}
}

int test_comms_cape(const char *scan_value, unsigned id)
{
	fail++;
	return(fail);
}

int test_display18_cape(const char *scan_value, unsigned id)
{
	fail++;
	return(fail);
}

int test_display50_cape(const char *scan_value, unsigned id)
{
	fail++;
	return(fail);
}

int test_display70_cape(const char *scan_value, unsigned id)
{
	fail++;
	return(fail);
}

/* BC0400A2yywwnnnnnnnn */
int test_load_cape(const char *scan_value, unsigned id)
{
	int r;
	int fd_sn;
	char str[90];
	char str2[90];

	install_overlay(scan_value, capes[id].id_str);

	fd_sn = open("/sys/bus/i2c/devices/i2c-2/2-0054/2-00540/nvmem", O_RDWR);
	lseek(fd_sn, 0, SEEK_SET);
	r = read(fd_sn, str, 88);
	str[89] = 0;
	beagle_notice("name", &str[6]);

	gpio_out_test("sink1", 7);
	gpio_out_test("sink2", 20);
	gpio_out_test("sink3", 112);
	gpio_out_test("sink4", 115);
	gpio_out_test("sink5", 44);
	gpio_out_test("sink6", 45);
	gpio_out_test("sink7", 47);
	gpio_out_test("sink8", 27);

	memcpy(str, cape_eeprom, 88);
	strcpy(&str[6], capes[id].name);	/* board name */
	memcpy(&str[38], &scan_value[4], 4);	/* board version */
	strcpy(&str[58], capes[id].id_str);	/* part number */
	strncpy(&str[76], &scan_value[8], 16);	/* serial number */
	str[89] = 0;
	lseek(fd_sn, 0, SEEK_SET);
	r = write(fd_sn, str, 88);
	lseek(fd_sn, 0, SEEK_SET);
	r = read(fd_sn, str2, 88);
	str2[89] = 0;
	beagle_notice("name", &str2[6]);
	beagle_notice("ver/mfr", &str2[38]);
	beagle_notice("partno", &str2[58]);
	beagle_notice("serial", &str2[76]);
	fail = memcmp(str, str2, 88) ? 1 : 0;
	beagle_notice("eeprom", fail ? "fail" : "pass");

	close(fd_sn);

	return(fail);
}

/* BC0500A2yywwnnnnnnnn */
int test_motor_cape(const char *scan_value, unsigned id)
{
	int r;
	int fd_sn;
	char str[90];
	char str2[90];
	const char *sleep = "sleep 1";

	install_overlay(scan_value, capes[id].id_str);

	fd_sn = open("/sys/bus/i2c/devices/i2c-2/2-0054/2-00540/nvmem", O_RDWR);
	lseek(fd_sn, 0, SEEK_SET);
	r = read(fd_sn, str, 88);
	str[89] = 0;
	beagle_notice("name", &str[6]);

	/* Export PWMs */
	/* Motor 1 */
	system("echo pwm > /sys/devices/platform/ocp/ocp:P9_16_pinmux/state");
	system("echo 1 > /sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip*/export");
	/* Motor 2 */
	system("echo pwm > /sys/devices/platform/ocp/ocp:P9_14_pinmux/state");
	system("echo 0 > /sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip*/export");
	/* Motor 3 */
	system("echo pwm > /sys/devices/platform/ocp/ocp:P8_13_pinmux/state");
	system("echo 1 > /sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip*/export");
	/* Motor 4 */
	system("echo pwm > /sys/devices/platform/ocp/ocp:P8_19_pinmux/state");
	system("echo 0 > /sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip*/export");
	system(sleep);

	/* Test Motor 1 */
	system("echo 500000 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*1/period");
	system("echo 50000 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*1/duty_cycle");
	system("echo 1 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*1/enable");
	system(sleep);
	system("echo 500000 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*1/duty_cycle");
	system(sleep);
	system("echo 0 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*1/enable");

	/* Test Motor 2 */
	system("echo 500000 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*0/period");
	system("echo 50000 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*0/duty_cycle");
	system("echo 1 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*0/enable");
	system(sleep);
	system("echo 500000 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*0/duty_cycle");
	system(sleep);
	system("echo 0 > /sys/devices/platform/ocp/48302000.*/48302200.*/pwm/pwmchip*/pwm*0/enable");

	/* Test Motor 3 */
	system("echo 500000 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*1/period");
	system("echo 50000 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*1/duty_cycle");
	system("echo 1 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*1/enable");
	system(sleep);
	system("echo 500000 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*1/duty_cycle");
	system(sleep);
	system("echo 0 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*1/enable");

	/* Test Motor 4 */
	system("echo 500000 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*0/period");
	system("echo 50000 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*0/duty_cycle");
	system("echo 1 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*0/enable");
	system(sleep);
	system("echo 500000 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*0/duty_cycle");
	system(sleep);
	system("echo 0 > /sys/devices/platform/ocp/48304000.*/48304200.*/pwm/pwmchip*/pwm*0/enable");

	memcpy(str, cape_eeprom, 88);
	strcpy(&str[6], capes[id].name);	/* board name */
	memcpy(&str[38], &scan_value[4], 4);	/* board version */
	strcpy(&str[58], capes[id].id_str);	/* part number */
	strncpy(&str[76], &scan_value[8], 16);	/* serial number */
	str[89] = 0;
	lseek(fd_sn, 0, SEEK_SET);
	r = write(fd_sn, str, 88);
	lseek(fd_sn, 0, SEEK_SET);
	r = read(fd_sn, str2, 88);
	str2[89] = 0;
	beagle_notice("name", &str2[6]);
	beagle_notice("ver/mfr", &str2[38]);
	beagle_notice("partno", &str2[58]);
	beagle_notice("serial", &str2[76]);
	fail = memcmp(str, str2, 88) ? 1 : 0;
	beagle_notice("eeprom", fail ? "fail" : "pass");

	close(fd_sn);

	return(fail);
}

int test_power_cape(const char *scan_value, unsigned id)
{
	fail++;
	return(fail);
}

int test_proto_cape(const char *scan_value, unsigned id)
{
	fail++;
	return(fail);
}

/* BC0800A2yywwnnnnnnnn */
int test_relay_cape(const char *scan_value, unsigned id)
{
	int r;
	int fd_sn;
	char str[90];
	char str2[90];

	install_overlay(scan_value, capes[id].id_str);

	fd_sn = open("/sys/bus/i2c/devices/i2c-2/2-0054/2-00540/nvmem", O_RDWR);
	lseek(fd_sn, 0, SEEK_SET);
	r = read(fd_sn, str, 88);
	str[89] = 0;
	beagle_notice("name", &str[6]);

	gpio_out_test("relay1", 20);
	gpio_out_test("relay2", 7);
	gpio_out_test("relay3", 112);
	gpio_out_test("relay4", 115);

	//for(r = 0; r < 88; r++) printf("%02x", cape_eeprom[r]); printf("\n");
	memcpy(str, cape_eeprom, 88);
	//for(r = 0; r < 88; r++) printf("%02x", str[r]); printf("\n");
	strcpy(&str[6], capes[id].name);	/* board name */
	//for(r = 0; r < 88; r++) printf("%02x", str[r]); printf("\n");
	memcpy(&str[38], &scan_value[4], 4);	/* board version */
	//for(r = 0; r < 88; r++) printf("%02x", str[r]); printf("\n");
	strcpy(&str[58], capes[id].id_str);	/* part number */
	//for(r = 0; r < 88; r++) printf("%02x", str[r]); printf("\n");
	strncpy(&str[76], &scan_value[8], 16);	/* serial number */
	str[89] = 0;
	//for(r = 0; r < 88; r++) printf("%02x", str[r]); printf("\n");
	lseek(fd_sn, 0, SEEK_SET);
	r = write(fd_sn, str, 88);
	lseek(fd_sn, 0, SEEK_SET);
	r = read(fd_sn, str2, 88);
	str2[89] = 0;
	beagle_notice("name", &str2[6]);
	beagle_notice("ver/mfr", &str2[38]);
	beagle_notice("partno", &str2[58]);
	beagle_notice("serial", &str2[76]);
	//for(r = 0; r < 88; r++) printf("%02x", str[r]); printf("\n");
	//for(r = 0; r < 88; r++) printf("%02x", str2[r]); printf("\n");
	fail = memcmp(str, str2, 88) ? 1 : 0;
	beagle_notice("eeprom", fail ? "fail" : "pass");

	close(fd_sn);

	return(fail);
}

int test_robotics_cape(const char *scan_value, unsigned id)
{
	fail++;
	return(fail);
}

int test_servo_cape(const char *scan_value, unsigned id)
{
	fail++;
	return(fail);
}

void install_overlay(const char *scan_value, const char *id_str)
{
	/* #dtb_overlay=/lib/firmware/<file8>.dtbo */
	const char *cmd = "perl -i.bak -pe 's!^.*dtb_overlay=/lib/firmware/.+\.dtbo.*!dtb_overlay=/lib/firmware/%s.dtbo!;' /boot/uEnv.txt";
	printf("Optional command to force overlay loading:\n");
	printf(cmd, id_str);
	printf("\n");
}

int gpio_out_test(const char *name, unsigned pin)
{
	const char *sleep = "sleep 1";
	const char *pinfile = "/sys/class/gpio/gpio%i";
	const char *export = "echo %i > /sys/class/gpio/export";
	const char *dir_out = "echo out > /sys/class/gpio/gpio%i/direction";
	const char *value_high = "echo 1 > /sys/class/gpio/gpio%i/value";
	const char *value_low = "echo 0 > /sys/class/gpio/gpio%i/value";
	char buffer[50];
	struct stat mystat;

	sprintf(buffer, pinfile, pin);
	if(stat(buffer, &mystat) != 0) {
		sprintf(buffer, export, pin);
		system(buffer);
	}
	sprintf(buffer, dir_out, pin);
	system(buffer);
	sprintf(buffer, value_high, pin);
	system(buffer);
	beagle_notice(name, "on");
	system(sleep);
	sprintf(buffer, value_low, pin);
	system(buffer);
	beagle_notice(name, "off");
	system(sleep);

	return(0);
}

