/*********************************************************************
*   Title:      weeder.c
*   Desc:       This file contains several useful functions when
*               communicating with the 1 WTAIN and 1 WTDIO Weeder
*               board.
*   Engineer:   Riley Culligan
*   Last Mod.:  11/6/2014
*********************************************************************/
/********************************************************************/
/******************     Defines and variables   *********************/
/********************************************************************/

#define weeder_is_included
/********************************************************************/
/******************     Typedefs and Constants  *********************/
/********************************************************************/

typedef enum {WTDIO, WTAIN, WTDAC} WEEDER_DEVICE;
typedef struct {
    char dir[14];
    WEEDER_DEVICE type;
    char addr;
    struct termios t;
} WEEDER;
/********************************************************************/
/******************     Function Externs    *************************/
/********************************************************************/

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
int WeederOutVolt(WEEDER *wdr, const char chan, int mvolts);

/******************     WeederReadChan()
*   Desc.       Reads the weeders' designated chanel, or all of them
*   Params:     *wdr: WEEDER type object info
*   Chan:       'A'-'N' = specific channel to read
*               0       = read ALL channels (WTDIO only)
*               1       = read channels 'A'-'H' (WTDIO only)
*   Return:     Int value of channel(s) (channel 'A' is LSB)
*               0 could mean error
*   Engineer:   Riley Culligan
*   Last Mod.   11/11/2014 RTC
******************/
int WeederReadChan(WEEDER *wdr, const char chan);

/******************     WeederAsciiToBin()
*   Desc.       Parses through a Weeder response, and returns the
*               binary value.  Used after an IO read.
*   Param:      *resp: pointer to the response to be converted
*   Return:     int value of the converted number
*   Engineer    Riley Culligan
*   Last Mod.   11/12/2014 RTC
******************/
int WeederAsciiToBin(char *resp);

/******************     WeederAsciiToInt()
*   Desc.       Parses through a Weeder respons, and returns the
*               numerical value.  Used after an ADC read.
*   Param:      *resp: pointer to the response to be converted
*   Return:     int value of the converted number, multiplied by
*               10^x so that it may remain an int
*   Engineer    Riley Culligan
*   Last Mod.   10/27/2014 RTC
******************/
int WeederAsciiToInt(char *resp);

/******************     WeederReadChanFloat
*   Desc.       Parses through a Weeder response, and returns the
*               numerical value as float.  Used for WTAIN only.
*   Param:      *wdr: WEEDER type object info
*               chan: channel to read float value from
*   Return:     float value of the converted number
*   Engineer    Riley Culligan
*   Last Mod.   12/4/2014 RTC
******************/
float WeederReadChanFloat(WEEDER *wdr, const char chan);

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
int WeederCommand(WEEDER *wdr, const char *cmnd, char *resp);

/******************     WeederInit()
*   Desc:       This function searches through the /dev/ directory
*               to find the which ports the weeder boards are on
*   Params:     wdr: Weeder board object info
*   Returns:    0->no error, 1->WTAIN not found, 2->WTDIO not found
                3->WTAIN and WTDIO not found
*   Engineer:   Riley Culligan
*   Last Mod.:  10/23/2014 RTC
******************/
int WeederInit(WEEDER *wdr, WEEDER *lastwdr, WEEDER_DEVICE type);

/******************     WeederSetIns
*   Desc.       Sets specified channels to inputs (WTDIO ONLY)
*   Params:     *wdr: WEEDER type object info
*   Chans:      List of channels to be set to an input (eg "ABC"),
*               must be in all caps.
*   Return:     0 = no error
*               1 = Command not understood (probably serial err)
*               2 = Bad channel given
*               3 = Weeder not WTDIO
******************/
int WeederSetIns(WEEDER *wdr, const char *chans);

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
int WeederSetOuts(WEEDER *wdr, const char *chans);

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
int WeederClrOuts(WEEDER *wdr, const char *chans);

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
int WeederSetOutTimed(WEEDER *wdr, char chan, int time);

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
int WeederSetupAIns(WEEDER *wdr, const char *chans, int mode, int dec);

/******************     WeederClose()
*   Desc:       This function closes and unlocks thefile descriptor in case of
*               an emergency closure.
*   Params:     wdr: WEEDER type object info
*   Returns:    none
*   Engineer:   Riley Culligan
*   Last Mod.:  12/23/2014 RTC
******************/
void WeederClose(WEEDER *wdr);