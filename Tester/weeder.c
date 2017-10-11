/*********************************************************************
*   Title:      weeder.c
*   Desc:       This file contains several useful functions when
*               communicating with the 1 WTAIN and 1 WTDIO Weeder
*               board.
*   Engineer:   Riley Culligan
*   Last Mod.:  12/2/2014
*********************************************************************/
//******************     Includes/defines   **************************

#define _SVID_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <string.h>
#include <time.h>
//*****************     Defines & Macros
#define PAUSE_NS        30000000    // 30ms works well for weeder
#define RETURN_IF_ERROR() if(ret)return(ret);
/********************************************************************/
/******************     Typedefs and Constants  *********************/

typedef enum {WTDIO, WTAIN, WTDAC} WEEDER_DEVICE;
typedef struct {
    char dir[32];
    WEEDER_DEVICE type;
    char addr;
    struct termios t;
    int fd;
} WEEDER;
/********************************************************************/
/******************     Function Prototypes     *********************/

int WeederInit(WEEDER *wdr, WEEDER *lastwdr, WEEDER_DEVICE type);
int WeederQuickInit(WEEDER *wdr, )
int WeederCommand(WEEDER *wdr, const char *cmnd, char *resp);
int WeederAsciiToInt(char *resp);
int WeederAsciiToBin(char *resp);
int WeederReadChan(WEEDER *wdr, const char chan);
float WeederReadChanFloat(WEEDER *wdr, const char chan);
int WeederSetIns(WEEDER *wdr, const char *chans);
int WeederSetupAIns(WEEDER *wdr, const char *chans, int mode, int dec);
int WeederSetOuts(WEEDER *wdr, const char *chans);
int WeederSetOutTimed(WEEDER *wdr, const char chan, int time);
int WeederClrOuts(WEEDER *wdr, const char *chans);
int WeederOutVolt(WEEDER *wdr, const char chan, int mvolts);
void WeederClose(WEEDER *wdr);
/********************************************************************/
/******************     Function Descriptions   *********************/

#ifndef weeder_is_included
int main(void)
{
#define basic_test
    //Main is used purely to test this other functions
#ifdef basic_test
    WEEDER wdr, wdr2;
    int e;
    e = WeederInit(&wdr, 0, WTDAC);
    printf("WTDAC found at %s with address %c and ret %i\n", wdr.dir, wdr.addr, e);
    e = WeederInit(&wdr2, &wdr, WTDAC);
    printf("WTDAC found at %s with address %c and ret %i\n", wdr2.dir, wdr2.addr, e);
    WeederOutVolt(&wdr, 'A', 1000);
    WeederOutVolt(&wdr2, 'A', 2000);
#endif
#ifdef debug_battery_tester
    WEEDER wdrin;
    int e;
    float curr;
    char timeBuf[128], *ch, logMsg[128];
    time_t timeNow;
    struct timespec ts;
    ts.tv_sec  = 59;
    ts.tv_nsec = 1000000000-PAUSE_NS;
    //Code Begins
    e = WeederInit(&wdrin, 0, WTAIN);
    printf("WTDIO found at %s with address %c and ret %i\n", wdrin.dir, wdrin.addr, e);
    e = WeederSetupAIns(&wdrin, "C", 3, 4);
    do
    {
        curr = WeederReadChanFloat(&wdrin, 'C')*10;
        printf("%1.4f\n", curr);
        nanosleep(&ts, NULL);
    }
    while (curr > 0.010);
    timeNow = time(NULL);
    ctime_r(&timeNow, timeBuf);
    ch = strchr(timeBuf, '\n');
    *ch = '\0';
    snprintf(logMsg, sizeof(logMsg), "%s\n", timeBuf);
    printf("End at time %s\n", curr, logMsg);
#endif
#ifdef debug_p3tester
    WEEDER wdrio;
    WEEDER wdrin;
    int e, A, B, C, D;
    float Af, Bf, Cf, Df;
    e = WeederInit(&wdrio, 0, WTDIO);
    printf("WTDIO found at %s with address %c and ret %i\n", wdrio.dir, wdrio.addr, e);
    e = WeederInit(&wdrin, 0, WTAIN);
    printf("WTAIN found at %s with address %c and ret %i\n", wdrin.dir, wdrin.addr, e);
    e = WeederSetupAIns(&wdrin, "BD", 1, 3);
    printf("Channels BD changed to mode 1 dec 3, resp is %i\n", e);
    e = WeederSetupAIns(&wdrin, "AC", 3, 4);
    printf("Channels AC changed to mode 3 dec 4, resp is %i\n", e);
    for (e = 0;e < 5; e++)
    {
        A = WeederReadChan(&wdrin, 'A');
        B = WeederReadChan(&wdrin, 'B');
        C = WeederReadChan(&wdrin, 'C');
        D = WeederReadChan(&wdrin, 'D');
        printf("A = %i \tB = %i \tC = %i \tD = %i\n", A, B, C, D);
        sleep(1);
    }
    for (e = 0;e < 5; e++)
    {
        Af = WeederReadChanFloat(&wdrin, 'A');
        Bf = WeederReadChanFloat(&wdrin, 'B');
        Cf = WeederReadChanFloat(&wdrin, 'C');
        Df = WeederReadChanFloat(&wdrin, 'D');
        printf("A = %f \tB = %f \tC = %f \tD = %f\n", Af, Bf, Cf, Df);
        sleep(1);
    }
#endif
#ifdef debug_iron_lung
    WEEDER wdr;
    //char buffer[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i, e;
    e = WeederInit(&wdr, 0, WTDIO);
    printf("WTDIO found at %s with address %c and ret %i\n", wdr.dir, wdr.addr, e);
    e = WeederClrOuts(&wdr, "AB");
    e = WeederSetIns(&wdr, "E");
    //e = WeederSetOutTimed(&wdr, 'A', 337); //return can't be trusted
    //printf("Timed output returns %i\n", e);
    for (e = 0;e <10;e++){
        i = WeederReadChan(&wdr, 'E');
        printf("%i\n", i);
        sleep(1);
    }
#endif
    return (0);
}
#endif

/******************     WeederOutVolt
*   Desc.       Sets specified channel's output to voltage.  WTDAC only
*   Params:     wdr: WEEDER type object info
*               chan: Channel to change voltage.  (eg. 'A')
*               mvolts: Voltage in millivolts (accurate to 10mV) to change to
*   Return:     0 = no error
*               1 = Command not understood (probably serial err)
*               2 = Bad channel given
*               3 = Weeder not WTDAC
*               4 = mvolts out of range
*   Engineers:  Riley Culligan
*   Last mod.   12/22/2014 RTC
******************/
int WeederOutVolt(WEEDER *wdr, const char chan, int mvolts)
{
    char cmnd[10], resp[10], temp[10];
    int ret = 0;
    int i, e;
    int volts = mvolts;
    /** Perform error checks **/
    if (wdr->type != WTDAC)
    {
        perror("Weeder board not WTDAC\n");
        return(3);
    }
    if (!((chan >= 'A') && (chan <= 'D')))
    {
        perror("Bad channel given\n");
        return(2);
    }
    if (!((mvolts >= -10000) & (mvolts <= 10000)))
    {
        perror("Out volts not in range\n");
        return(4);
    }
    /*** Construct command ***/
    cmnd[0] = 'V';
    cmnd[1] = chan;
    /** Convert volts into string **/
    // Round to nearest 10mV
    if (volts%10 >= 5)
    {
        volts += 10;
    }
    volts /= 10;
    for (i = 0; volts; i++)
    {
        temp[i] = (volts%10) + '0';
        volts /= 10;
    }
    e = 2;
    for (i--; i>=0; i--)
    {
        cmnd[e] = temp[i];
        e++;
    }
    cmnd[e] = '\r';
    printf("Temp is %s, Command for WeederOutVolt is %s\n", temp, cmnd);
    ret = WeederCommand(wdr, cmnd, resp);
    return(ret);
}

/******************     WeederSetIns
*   Desc.       Sets specified channels to inputs (WTDIO ONLY)
*   Params:     *wdr: WEEDER type object info
*               must be in all caps.
*   Return:     0 = no error
*               1 = Command not understood (probably serial err)
*               2 = Bad channel given
*               3 = Weeder not WTDIO
******************/
int WeederSetIns(WEEDER *wdr, const char *chans)
{
    char cmnd[10], resp[10];
    int ret = 0;
    int i = 0;
    // Code Begins
    if (wdr->type != WTDIO)
    {
        ret = 3;
    }
    cmnd[0] = 'I';
    cmnd[2] = '\r';
    for (;chans[i] && !ret;i++)
    {
        cmnd[1] = chans[i];
        ret = WeederCommand(wdr, cmnd, resp);
    }
    return (ret);
}

/******************     WeederSetupAIns()
*   Desc.       Sets up WTAIN input pins to specified mode and/or decimal.
*   Params:     *wdr:   WEEDER type object info
*               chans:  Channels to initialize to mode and/or decimal.
*               mode:   -1 -> don't change the mode of channels.
*                       x  -> change the channels' mode to x.
*               dec:    -1 -> don't change the decimal of channels.
*                       x  -> change the channels' decimal to x.
*   Return:     0 = no error
*               1 = Command not understood (coding error)
*               2 = Bad chans, mode, or dec given
*               3 = Weeder not WTAIN
*   Engineer:   Riley Culligan
*   Last Mod.   12/3/2014
******************/
int WeederSetupAIns(WEEDER *wdr, const char *chans, int mode, int dec)
{
    char cmnd[10], resp[10];
    int ret = 0;
    int i;
    /* Code Begins */
    if (wdr->type != WTAIN)
    {
        ret = 3;
    }
    /* Mode Change */
    if (mode == -1)
    {
        // Don't change mode
    }
    else if ((mode >= 0) && (mode <= 5))
    {
        // Change the channels' mode
        cmnd[0] = 'M';
        cmnd[2] = mode + '0';
        cmnd[3] = '\r';
        cmnd[4] = 0;
        for (i = 0; chans[i] && !ret; i++)
        {
            cmnd[1] = chans[i];
            ret = WeederCommand(wdr, cmnd, resp);
        }
    }
    else
    {
        ret = 2;
    }
    /* Decimal Change */
    if (dec == -1)
    {
        // Don't change decimal
    }
    else if ((dec >= 0) && (dec <= 7))
    {
        // Change the channels' decimal position
        cmnd[0] = 'D';
        cmnd[2] = dec + '0';
        cmnd[3] = '\r';
        cmnd[4] = 0;
        for (i = 0; chans[i] && !ret; i++)
        {
            cmnd[1] = chans[i];
            ret = WeederCommand(wdr, cmnd, resp);
        }
    }
    return (ret);
}

/******************     WeederSetOuts()
*   Desc.       Sets specified channels to output high (WTDIO only)
*   Params      *wdr: WEEDER type object info
*               chans: list of channels to be set high (eg "A", "ABC")
*               must be in all caps
*   Return      0 = no error
*               1 = Command not understood (probably serial err)
*               2 = Bad channel given
*               3 = Weeder not WTDIO
******************/
int WeederSetOuts(WEEDER *wdr, const char *chans)
{
    char cmnd[10], resp[10];
    int ret = 0;
    int i = 0;
    // Code Begins
    if (wdr->type != WTDIO)
    {
        ret = 3;
    }
    cmnd[0] = 'H';
    cmnd[2] = '\r';
    for (;chans[i] && !ret;i++)
    {
        cmnd[1] = chans[i];
        ret = WeederCommand(wdr, cmnd, resp);
    }
    return (ret);
}

/******************     WeederClrOuts()
*   Desc.       Sets specified channels to output low (WTDIO only)
*   Params      *wdr: WEEDER type object info
*               chans: list of channels to be set low (eg "A", "ABC"),
*               must be in all caps
*   Return      0 = no error
*               1 = Command not understood (probably serial err)
*               2 = Bad channel given
*               3 = Weeder not WTDIO
******************/
int WeederClrOuts(WEEDER *wdr, const char *chans)
{
    char cmnd[10], resp[10];
    int ret = 0;
    int i = 0;
    // Code Begins
    if (wdr->type != WTDIO)
    {
        ret = 3;
    }
    cmnd[0] = 'L';
    cmnd[2] = '\r';
    for (;chans[i] && !ret;i++)
    {
        cmnd[1] = chans[i];
        ret = WeederCommand(wdr, cmnd, resp);
    }
    return (ret);
}

/******************     WeederSetOutTimed()
*   Desc.       Sets specified channels to output high (WTDIO only)
*               for a specified time.
*   Params      *wdr: WEEDER type object info
*               chan: channel to be set high (eg 'A')
*                   must be in all caps
*               time: length of time in ms the port will be on.
*                   must be at least PAUSE_NS long.
*   Return      0 = no error
*               1 = Command not understood (probably serial err)
*               2 = Bad channel given
*               3 = Weeder not WTDIO
*               4 = Time < PAUSE_NS
*   Engineer:   Riley Culligan
*   Last Mod.   11/24/2014
******************/
int WeederSetOutTimed(WEEDER *wdr, const char chan, int time)
{
    char cmnd[10];
    int ret = 0;
    long timeval = time - PAUSE_NS/1000000;
    struct timespec ts;
    ts.tv_sec = 0;
    // Code Begins
    if (wdr->type != WTDIO)
    {
        ret = 3;
    }
    else if (time < PAUSE_NS/1000000)
    {
        ret = 4;
    }
    else
    {
        // Format time values
        while (timeval>1000)
        {
            ts.tv_sec++;
            timeval -= 1000;
        }
        ts.tv_nsec = timeval*1000000;
        cmnd[0] = chan;
        cmnd[1] = 0;
        printf("%s set high for ms #%i\n", cmnd, time);
        ret = WeederSetOuts(wdr, cmnd);
        if (!ret)
        {
            nanosleep(&ts, NULL);
        }
        ret = WeederClrOuts(wdr, cmnd);
    }
    return (ret);
}

/******************     WeederReadChan()
*   Desc.       Reads the weeders' designated chanel, or all of them
*   Params:     *wdr: WEEDER type object info
*               Chan:   'A'-'N' = specific channel to read
*                       0       = read ALL channels (WTDIO only)
*                       1       = read channels 'A'-'H' (WTDIO only)
*   Return:     Int value of channel(s) (channel 'A' is LSB)
*               0 could mean error
*   Engineer:   Riley Culligan
*   Last Mod.   12/4/2014 RTC
******************/
int WeederReadChan(WEEDER *wdr, const char chan)
{
    char resp[20];
    char cmnd[20];
    int ret = 1;    //set to 1 to stop if bad chan given
    int i;
    //Code Begins
    // Guarantee string null termination, since one isn't returned from wdr
    for (i = 0;i < 20; i++)
    {
        resp[i] = 0;
    }
    // Fill cmnd block depending wdr board type and channel
    cmnd[0] = 'R';
    if (wdr->type==WTDIO && chan==0)
    {
        //read ALL channels on WTDIO
        cmnd[1] = '\r';
        cmnd[2] = 0;
    } else if (wdr->type==WTDIO && chan==1)
    {
        //read first 8 channels on WTDIO
        cmnd[1] = 'P';
        cmnd[2] = '\r';
        cmnd[3] = 0;
    } else if (wdr->type==WTDIO && (chan>='A' && chan<='N'))
    {
        cmnd[1] = chan;
        cmnd[2] = '\r';
        cmnd[3] = 0;
    } else if (wdr->type==WTAIN && (chan>='A' && chan<='D'))
    {
        cmnd[1] = chan;
        cmnd[2] = '\r';
        cmnd[3] = 0;
    } else
    {
        // Incorrect channel entry
        ret = 0;
    }
    if (ret)
    {
        WeederCommand(wdr, cmnd, resp);
        if (wdr->type == WTDIO)
        {
            ret = WeederAsciiToBin(resp);
        } else
        {
            ret = WeederAsciiToInt(resp);
        }
    }
    return (ret);    
}

/******************     WeederReadChanFloat
*   Desc.       Parses through a Weeder response, and returns the
*               numerical value as float.  Used for WTAIN only.
*   Param:      *wdr: WEEDER type object info
*               chan: channel to read float value from
*   Return:     float value of the converted number
*   Engineer    Riley Culligan
*   Last Mod.   12/4/2014 RTC
******************/
float WeederReadChanFloat(WEEDER *wdr, const char chan)
{
    float ret = 0.;
    char cmnd[10], resp[20];
    int i, digit;
    int neg_flg = 0;
    int dec_cnt = 0;
    int dec_flg = 0;
    //Code Begins
    // Guarantee string null termination, since one isn't returned from wdr
    for (i = 0;i < 20; i++)
    {
        resp[i] = 0;
    }
    // make sure WTDIO, then fill out cmnd
    if (wdr->type == WTDIO)
    {
        return (0.);
    }
    cmnd[0] = 'R';
    cmnd[1] = chan;
    cmnd[2] = '\r';
    cmnd[3] = 0;
    i = WeederCommand(wdr, cmnd, resp);
    if (i) return (0.);
    for (i = 0; resp[i]; i++)
    {
        if (dec_flg)
        {
            // Count decimal places to later divide by
            dec_cnt++;
        }
        if (resp[i] == '.')
        {
            // Throw flag to begin counting decimal places
            dec_flg = 1;
        }
        else if (resp[i] == '-')
        {
            neg_flg = 1;
        }
        else if ((resp[i] >= '0') && (resp[i] <= '9'))
        {
            digit = resp[i];
            digit -= '0';
            ret *= 10;
            ret += (float)digit;
        }
        else
        {
            // if letters or other non-numbers, ignore
        }
    }
    while (dec_cnt)
    {
        ret /= 10;
        dec_cnt--;
    }
    if (neg_flg)
    {
        ret *= -1.;
    }
    return (ret);
}

/******************     WeederAsciiToBin()
*   Desc.       Parses through a Weeder response, and returns the
*               binary value.  Used after an IO read.
*   Param:      *resp: pointer to the response to be converted
*   Return:     int value of the converted number
*   Engineer    Riley Culligan
*   Last Mod.   11/12/2014 RTC
******************/
int WeederAsciiToBin(char *resp)
{
    int ret = 0; 
    int i, digit;
    //Code Begins
    i = 2;
    if (resp[i] == 'L')
    {
        ret = 0;
    } else if (resp[i] == 'H') 
    {
        ret = 1;
    } else 
    {
        for (;resp[i] != 0;i++)
        {
            //Convert ascii to hexadecimal, then add to return value
            digit = resp[i];
            if (digit>='0' && digit<='9')
            {
                digit -= '0';
            } else 
            {
                digit -= 'A';
                digit += 10;
            }
            ret *= 0xf;
            ret += digit;
            printf("creating ret %i", ret);
        }
    }
    return (ret);
}

/******************     WeederAsciiToInt()
*   Desc.       Parses through a Weeder respons, and returns the
*               numerical value.  Used after an ADC read.
*   Param:      *resp: pointer to the response to be converted
*   Return:     int value of the converted number, multiplied by
*               10^x so that it may remain an int
*   Engineer    Riley Culligan
*   Last Mod.   10/27/2014 RTC
******************/
int WeederAsciiToInt(char *resp)
{
    int ret = 0; 
    int neg_flg = 0;
    int i, digit;
    //Code Begins
    //Find the first digit
    i = 1;
    for (;(resp[i] != 0) && (resp[i] != 1);i++)
    {
        //Convert ascii to decimal, then add to return value
        if (resp[i] == '-')
        {
            neg_flg = 1;
        }
        else if (resp[i] != '.')
        {
            digit = resp[i];
            digit -= '0';
            ret *= 10;
            ret += digit;
        }
    }
    if (neg_flg)
    {
        ret *= -1;
    }
    return (ret);
}

/******************     WeederCommand()
*   Desc:       This function gives a command to the given weeder board,
*               and gives back the result.
*   Params:     *wdr: the WEEDER type object info
*               brd: chooses board to command, either WTDIO or WTAIN 
*               *cmnd: The command given to the board
*               *resp: Pointer to a buffer for the response
*   Returns:    0-> no error, 1-> command not understood, 2-> error
*   Engineer:   Riley Culligan
*   Last Mod.   10/23/2014 RTC
******************/
int WeederCommand(WEEDER *wdr, const char *cmnd, char *resp)
{
    char brd_cmnd[10];
    int e, ret;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = PAUSE_NS;
    // Code begin
    brd_cmnd[0] = wdr->addr;
    brd_cmnd[1] = 0;
    strcat(brd_cmnd, cmnd);
    //printf("Command is %s\n", brd_cmnd);//DEBUG
    wdr->fd = open(wdr->dir, O_RDWR|O_NOCTTY|O_NDELAY); 
    if (wdr->fd < 0)
    {
        perror("error opening board");
    }
    e = flock(wdr->fd, LOCK_EX);
    tcsetattr(wdr->fd, TCSAFLUSH, &(wdr->t));
    e = write(wdr->fd, brd_cmnd, sizeof(brd_cmnd));
    nanosleep(&ts, NULL);    //pause for ts ms
    if (e<0) 
    {
        perror("error writing to board");
        ret = 2;
    } else 
    {
        e = read(wdr->fd, resp, 20);
        if (e<0)
        {
            perror("error reading from board");
            ret = 2;
        } else if (strchr(resp, '?'))  
        {
            printf("reason for fail was %d for %s\n", strncmp(resp,cmnd,2), resp);//debug
            //If the response =/= command, weeder didn't understand
            ret = 1;
        } else {
            // Successfull write and read
            ret = 0;
        }
    }
    close(wdr->fd);
    wdr->fd = 0;
    return (ret);
}

/******************     WeederClose()
*   Desc:       This function closes and unlocks thefile descriptor in case of
*               an emergency closure.
*   Params:     wdr: WEEDER type object info
*   Returns:    none
*   Engineer:   Riley Culligan
*   Last Mod.:  12/23/2014 RTC
******************/
void WeederClose(WEEDER *wdr)
{
    if (wdr->fd > 0)
    {
        flock(wdr->fd, LOCK_UN);
        close(wdr->fd);
    }
}

/******************     WeederInit()
*   Desc:       This function searches through the /dev/ directory
*               to find the which ports the weeder boards are on
*   Params:     wdr: Weeder board object info
*   Returns:    0->no error, 1->WTAIN not found, 2->WTDIO not found
                3->WTAIN and WTDIO not found
*   Engineer:   Riley Culligan, Kevin KleinOsowski
*   Last Mod.:  10/23/2014 RTC
******************/
int WeederInit(WEEDER *wdr, WEEDER *lastwdr, WEEDER_DEVICE type)
{
    struct dirent **namelist;
    struct timespec ts;
    int ctrlBits, i;
    int ret = 3;
    char buf[256];
    char readbuf[256];
    char namebuf[256];
    char addr = 'A';
    wdr->type = type;
    ts.tv_sec = 0;
    ts.tv_nsec = PAUSE_NS;
    i = scandir("/dev/", &namelist, 0, alphasort);
    if (i<0) 
    {
        perror("scandir");
    }
    //Initialize serial port
    //Set baud rate to 9600 bps
    cfsetispeed(&(wdr->t), B9600);
    cfsetospeed(&(wdr->t), B9600);
    //Setup control modes
    wdr->t.c_cflag &= ~(HUPCL | PARENB | CSTOPB);
    wdr->t.c_cflag &= ~(CSIZE);
    wdr->t.c_cflag |= CS8;
    wdr->t.c_cflag |= (CLOCAL | CREAD);
    //Setup local modes
    wdr->t.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //Setup input modes
    wdr->t.c_iflag &= ~(IXON | IXOFF | IXANY);
    wdr->t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP);
    //wdr->t.c_iflag |= ICRNL;
    //Setup output flags
    wdr->t.c_oflag &= ~OPOST;
    if (lastwdr)
    {
    printf("DEBUG: looking for %s\n", lastwdr->dir);
        addr = lastwdr->addr + 1;
        do
        {
            i--;
            strcpy(buf, "/dev/");
            printf("DEBUG: checking %s\n", namelist[i]->d_name);
            strcat(buf, namelist[i]->d_name);
        } while (strcmp(buf,lastwdr->dir) && i);
        printf("Lastwdr called, look in %s at address %c\n", buf, addr);
        i++;
    }
    while (i-- && ret) 
    {
        strcpy(buf, "/dev/");
        strcat(buf, namelist[i]->d_name);
        if (!strstr(buf, "ttyS"))
        {
            // Check to see if it's a ttyS serial port
            continue;
            printf("Port %s not ttyS\n", buf);
        }
        wdr->fd =open(buf, O_RDWR|O_NOCTTY|O_NDELAY); 
        if (wdr->fd<0) {
            // Not a port
            continue;
        } 
        else if (-1 == ioctl(wdr->fd, TIOCMGET, &ctrlBits)) 
        {
            // Not a serial port
            close(wdr->fd);
            continue;
        } else 
        {
            // Is a serial port
        }
        //Apply configs
        tcsetattr(wdr->fd, TCSAFLUSH, &(wdr->t));
        // Write and read from serial ports to test for weeder boards    
        switch (type) 
        {
        case WTAIN:
            strcpy(namebuf, " MA\r");
            for (; addr <= 'D'; addr++)
            {
                // Since addr can only be A-P,a-p, wrap from P->a
                if (addr == 'Q') 
                {
                    addr = 'a';
                }
                namebuf[0] = addr;
                if (-1 == write(wdr->fd, namebuf, 4)) 
                {
                    perror("Error in write to serial port");
                }
                nanosleep(&ts, NULL);   //Pause for ts ms
                if (-1 == read(wdr->fd, &readbuf[0], 256)) 
                {
                    perror("Error in read from serial port");
                }
                if (
                    (readbuf[0] == addr) && 
                    (readbuf[1] == 'M') && 
                    (readbuf[2] == 'A'))
                {
                    wdr->addr = addr;
                    strcpy(wdr->dir, buf);
                    ret = 0;
                    printf("WTAIN succesfully discovered\n");
                    addr = 'D'; //end the loop
                }
            }
            break;
        case WTDIO:
            strcpy(namebuf, " LA\r");
            for (; addr <= 'D'; addr++)
            {
//                tcflush(wdr->fd, TCIFLUSH);  //Flush serial input buffer
                // Since addr can only be A-P,a-p, upper case->lower
                if (addr == 'Q') 
                {
                    addr = 'a';
                }
                namebuf[0] = addr;
                if (-1 == write(wdr->fd, namebuf, 4)) 
                {
                    perror("Error in write to serial port");
                }
                nanosleep(&ts, NULL);   //Pause 
                if (-1 == read(wdr->fd, &readbuf[0], 256)) 
                {
                    perror("Error in read from serial port");
                }
                if (
                    (readbuf[0] == addr) && 
                    (readbuf[1] == 'L') && 
                    (readbuf[2] == 'A'))
                {
                    wdr->addr = addr;
                    strcpy(wdr->dir, buf);
                    ret = 0;
                    printf("WTDIO succesfully discovered at %s\n", wdr->dir);
                    addr = 'D'; //end the loop
                }
            }
            break;
        case WTDAC:
            strcpy(namebuf, " PA2\r");
            for (; addr <= 'D'; addr++)
            {
                // Since addr can only be A-P,a-p, wrap from P->a
                if (addr == 'Q') 
                {
                    addr = 'a';
                }
                namebuf[0] = addr;
                if (-1 == write(wdr->fd, namebuf, 5)) 
                {
                    perror("Error in write to serial port");
                }
                nanosleep(&ts, NULL);   //Pause for ts ms
                if (-1 == read(wdr->fd, &readbuf[0], 256)) 
                {
                    perror("Error in read from serial port");
                }
                if (
                    (readbuf[0] == addr) && 
                    (readbuf[1] == 'P') && 
                    (readbuf[2] == 'A'))
                {
                    wdr->addr = addr;
                    strcpy(wdr->dir, buf);
                    ret = 0;
                    printf("WTDAC succesfully discovered\n");
                    addr = 'D'; //end the loop
                }
            }
            break;
        }
        addr = 'A';
        close(wdr->fd);
        wdr->fd = 0;
        tcflush(wdr->fd, TCIFLUSH);
    }
    return (ret);
}
